#include "Sector2.h"
#include "MC_Tables.h"
#include "..\WorldGenerator.h"
#include "..\..\System\VarStorage.h"
#include "..\Physics\PhysicsManager.h"
#include <time.h>

using namespace vsSector2Texture;


/*
====================================================================

	vsSector2OTNode

====================================================================
*/
vsSector2OTNode::vsSector2OTNode( void ) {
	visible = false;
	buffers[0] = 0;
	buffers[1] = 0;
	bufferUpdateNeeded = false;
	//grass = NULL;
	geometryState = NGS_NO_GEOMETRY;
	geometryModified = false;
	isLeaf = false;
	lastQueryVisible=0;
	oldVertexCount = oldIndexCount = 0;
	vegetation = NULL;
}

/*
===================================
	~vsSector2OTNode
===================================
*/
vsSector2OTNode::~vsSector2OTNode( void ) 
{
	if ( isLeaf ) {
		if ( vertices.Length() > 0 )
		{
			vertices.Free();
			indices.Free();
			if ( buffers[ 0 ] != 0 )
				glDeleteBuffers( 2, buffers );

			glDeleteQueries( 1, &query );
		}
		
		vegetation.Free();

		REPU( models.Length() ) {
			modelManager->DeleteMesh( models[ i ] );
		}
		models.Free();

		//if ( grass ) delete grass;
	}
}

/*
===================================
	Setup
===================================
*/
void vsSector2OTNode::Setup( int id, int parentId, 
							 const vsBox& bounds, 
							 bool isLeaf, vsSector2* pSector, 
							 byte gridSize, 
							 const vsVec3ui& voxelStart,
							 const vsVec3ui& voxelEnd )
{
	this->Id = id;
	this->parentId = parentId;
	this->isLeaf = isLeaf;
	this->parentSector = pSector;
	this->start = voxelStart;
	this->end = voxelEnd;
	this->gridSize = gridSize;
	this->cellScale = pSector->scale_1 * gridSize;
	this->bounds = bounds;
}



/*
====================================================================

	vsSector2

	Note:
		voxelData Coords: 
		 x : from left to right
		 y : from top to bottom
		 z : from front to back
	Not sure about that though.
====================================================================
*/
vsSector2::vsSector2( void )
{
	shader  = NULL;
	numNodes = 0;
	voxelData = NULL;
	numVisibleNodes = 0;
	numVisibleLeafNodes = 0;
	mainSector = false;
	REPEAT( 6 )
		neighbors[i] = NULL;

	newSector = true;
	generated = false;

	SetPhysics( physicsManager->New( PHYSICS_SECTOR, false ) );
	( ( vsPhysics_Sector* )physics )->SetSectorPtr( this );
}

/*
===================================
	vsSector2()
===================================
*/
vsSector2::vsSector2( const sector2Args_t& lsp )
{
	vsSector2();
	Generate( lsp );
}

/*
===================================
	~vsSector2()
===================================
*/
vsSector2::~vsSector2( void )
{
	Destroy();
}

/*
===================================
	AllocNodes
===================================
*/
void vsSector2::AllocNodes( uint count )
{
	FreeNodes();
	nodes.Alloc( count );
	for ( uint i = 0; i < count; ++i )
		nodes[i] = NULL;
}

/*
===================================
	FreeNodes
===================================
*/
void vsSector2::FreeNodes( void )
{
	nodes.DeletePointerElements();
	nodes.Free();
}

/*
===================================
	Destroy
===================================
*/
void vsSector2::Destroy( void )
{
	FreeNodes();
	FreeVoxelData();
}

/*
===================================
	Generate
===================================
*/
int vsSector2::Generate( const sector2Args_t& lsp )
{
	generated = false;
	nogeometry = true;

	REPEAT( 6 )
		neighbors[ i ] = NULL;

	this->nogeometry		= !true;
	this->newSector			= lsp.SECTOR.generateNew;
	this->shader			= lsp.SECTOR.terrainShader;
	this->mainSector		= lsp.SECTOR.IsThisTheMainSector;
	this->nodeMinDimensions = lsp.SECTOR.nodeMinDimensions;
	this->megaTexDiff		= lsp.SECTOR.megaTexDiff;
	this->megaTexNorm		= lsp.SECTOR.megaTexNorm;
	

	this->Grass.density		= lsp.GRASS.density;
	this->Grass.scale		= lsp.GRASS.scale;
	this->Grass.shader		= lsp.GRASS.shader;
	this->Grass.texture		= lsp.GRASS.finTexture;
	this->Grass.wind		= lsp.GRASS.windVec;

	if ( newSector ) 
		GenerateNew( lsp );

	this->bounds.FromMinMax( offset, offset + scale );	
	
	SetModificationState( false );

	shader->BindAttrib( S2ATTRIB_POSITION,		"in_Position" );
	shader->BindAttrib( S2ATTRIB_NORMAL,			"in_Normal" );
	shader->BindAttrib( S2ATTRIB_TANGENT,		"in_Tangent" );
	shader->BindAttrib( S2ATTRIB_MEGATEXCOORDS,	"in_MegaTexCoords" );
	shader->BindAttrib( S2ATTRIB_GHOSTTEXCOORDS,	"in_GhostTexCoords" );



	generated = true;

	physicsManager->SetTesting( physics, lsp.SECTOR.physicsTest  );

	return VS_OK;
}

/*
===================================
	GenerateNew
		Generates whole new terrain and voxel data
		NO LOADING FROM FILE!
===================================
*/
void vsSector2::GenerateNew( const sector2Args_t& lsp )
{
	Destroy();

	this->coord				= lsp.SECTOR.coord;
	this->terrainType		= lsp.SECTOR.terrainType;
	this->scale				= lsp.SECTOR.scale;
	this->texScale			= lsp.SECTOR.texScale;
	this->offset			= lsp.SECTOR.translation - scale*0.5f*0.f; 
	this->LOD				= lsp.SECTOR.LOD; 
	this->gridSize			= lsp.SECTOR.gridSize;
	this->realDimensions	= vsVec3ui( lsp.SECTOR.size );
	this->dimensions		= realDimensions / gridSize;	
	this->scale_1			= scale / ( dimensions - 1 ).ToVec3f();
	AllocVoxelData( dimensions.x, dimensions.y, dimensions.z );

	vsSector2* n;

	voxelOffset = coord * ( realDimensions-1 ).ToVec3i();
	offset = coord.ToVec3f() * scale;

	GenerateVoxels();

	GenerateOctree();
}

/*
===================================
	Render
===================================
*/
void vsSector2::Render( void )
{
	if ( !shader || nogeometry || !generated ) {
		return;
	}

	if ( !player->GetFrustum()->IntersectBox( bounds, 0.02f ) )
		return;

	numVisibleNodes = numVisibleLeafNodes = 0;
	
	
	glLineWidth( 1.0f );
	
	shader->Bind( true );
	
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, megaTexDiff );
	shader->Uniform1i( "in_MegaTextureDiffuse", 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, megaTexNorm );
	shader->Uniform1i( "in_MegaTextureNormal", 1 );
	
	shader->Uniform3v( "in_CellScale", this->scale_1 );

	glEnableVertexAttribArray( S2ATTRIB_POSITION );
	glEnableVertexAttribArray( S2ATTRIB_NORMAL );
	glEnableVertexAttribArray( S2ATTRIB_TANGENT );
	glEnableVertexAttribArray( S2ATTRIB_MEGATEXCOORDS );
	glEnableVertexAttribArray( S2ATTRIB_GHOSTTEXCOORDS );

	Node_Render( 0 );
	
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glDisableVertexAttribArray( S2ATTRIB_POSITION );
	glDisableVertexAttribArray( S2ATTRIB_NORMAL );
	glDisableVertexAttribArray( S2ATTRIB_TANGENT );
	glDisableVertexAttribArray( S2ATTRIB_MEGATEXCOORDS );
	glDisableVertexAttribArray( S2ATTRIB_GHOSTTEXCOORDS );
	
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );
		
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	shader->Bind( false );

	if ( varStorage.Bool["rendertree"] )
		Node_RenderTree( 0 );
	
	/*for ( uint i = 0; i < nodes.Length(); i++ )
		if ( nodes[ i ]->IsLeaf() && nodes[ i ]->visible ) 
		{
			REPU2( j, nodes[ i ]->vegetation.Length() )
					nodes[ i ]->vegetation[ j ]->Render();

			//if ( !varStorage.Bool["nograss"] ) 
				//nodes[ i ]->grass.Render();
		}*/

	for ( uint i = 0; i < nodes.Length(); i++ )
		if ( nodes[ i ]->IsLeaf() && nodes[ i ]->visible ) 
		{
			REPU2( j, nodes[ i ]->models.Length() )
					nodes[ i ]->models[ j ]->RenderAnimated();
		}
}

/*
===================================
	RenderVoxelData
===================================
*/
void vsSector2::RenderVoxelData( void )
{
	vsVec3f p;
	glPointSize( 10.0f );
	glColor3f( 0.0f, 0.0f, 0.8f );
	glBegin( GL_POINTS );
		REPU2( x, dimensions.x )
		REPU2( y, dimensions.y )
		REPU2( z, dimensions.z )
			if ( voxelData[x][y][z] > 0 )
			{
				p = vsVec3ui( x, y, z ).ToVec3f() * scale_1 + offset;
				glVertex3fv( p.ToTypePtr() );
			}
	glEnd();
}

/*
===================================
	AllocVoxelData
===================================
*/
void vsSector2::AllocVoxelData( uint width, uint height, uint depth )
{
	//dimensions = vsVec3ui( width, height, depth );
	FreeVoxelData();

	// Allocate memory for voxels
	voxelData = new ushort**[width];
	REPU2( x, width ) {
		voxelData[x] = new ushort*[height];
		REPU2( y, height ) {
			voxelData[x][y] = new ushort[depth];
			ZeroMemory( voxelData[x][y], depth );
		}
	}
}

/*
===================================
	FreeVoxelData
===================================
*/
void vsSector2::FreeVoxelData( void )
{
	if ( voxelData ) {
		REPU2( x, dimensions.x ) {
			REPU2( y, dimensions.y )
				delete [] voxelData[ x ][ y ];
			delete [] voxelData[ x ];
		}
		delete [] voxelData;
		voxelData = NULL;
	}
}

/*
===================================
	GenerateVoxels
===================================
*/
void vsSector2::GenerateVoxels( void )
{
	nogeometry = false;
	REPU2( x, dimensions.x )
	REPU2( y, dimensions.y )
	REPU2( z, dimensions.z )
		voxelData[ x ][ y ][ z ] = worldGenerator->TerrainAt( 
								voxelOffset.x + x * gridSize, 
								voxelOffset.y + y * gridSize,
								voxelOffset.z + z * gridSize ).id;

}


/*
===================================
	GenerateOctree
===================================
*/
void vsSector2::GenerateOctree( void )
{
	AllocNodes( PrecalculateOctreeNodeCount( 0, 0, 0, dimensions.x, dimensions.y, dimensions.z ) );
	GenerateOctreeNode( 0, 0, 0, dimensions.x, dimensions.y, dimensions.z, -1 );
}

/*
===================================
	PrecalculateOctreeNodeCount
		Helper function for octree generation
===================================
*/
uint vsSector2::PrecalculateOctreeNodeCount( uint sX, uint sY, uint sZ, uint eX, uint eY, uint eZ )
{
	vsVec3ui start	= vsVec3ui( sX, sY, sZ );
	vsVec3ui end	= vsVec3ui( eX, eY, eZ );

	vsVec3ui dim = end - start;
	int depth = 0;
	while( dim > nodeMinDimensions )
	{
		depth++;
		dim /= 2;
	}

	uint num = 0;
	for ( float i = 0; i <= ( float )depth; i++ )
		num += ( uint )powf( 8.0f, i );	

	return num;
}

/*
===================================
	GenereateOctreeNode
		Recursive function for generating octree nodes
===================================
*/
int vsSector2::GenerateOctreeNode( uint sX, uint sY, uint sZ, uint eX, uint eY, uint eZ, int parentId )
{
	if ( parentId == -1 ) 
		numNodes = 0;

	vsSector2OTNode* node = new vsSector2OTNode();

	vsVec3ui start = vsVec3ui( sX, sY, sZ );
	vsVec3ui end   = vsVec3ui( eX, eY, eZ );
	vsVec3ui dim   = end  - start;

	int id = numNodes++;

	//------------------------------
	//	Bounding Box
	//------------------------------
	vsBox bb;
	vsVec3f min = offset + start.ToVec3f()	* scale_1;
	vsVec3f max = offset + end.ToVec3f()	* scale_1;
	bb.FromMinMax( min, max );

	if ( dim <= nodeMinDimensions )
	{
		vsVec3ui end1 = end + 1;
		end1.Clamp( NULLVEC3UI, dimensions-1 );

		nodes[ id ] = node;
		node->Setup( id, parentId, bb, true, this, ( byte )gridSize, start, end1 );
		
		Node_CreateGeometry( id );

	} else {
		nodes[ id ] = node;
		node->Setup( id, parentId, bb, false, this, ( byte )gridSize, vsVec3ui(), vsVec3ui( end ) );	

		vsVec3ui C = ( start + end ) / 2;

		node->childrenId[ 0 ] = GenerateOctreeNode( start.x,	start.y,start.z,C.x,	C.y,	C.z,	node->Id );
		node->childrenId[ 1 ] = GenerateOctreeNode( C.x,		start.y,start.z,end.x,	C.y,	C.z,	node->Id );
		node->childrenId[ 2 ] = GenerateOctreeNode( C.x,		C.y,	start.z,end.x,	end.y,	C.z,	node->Id );
		node->childrenId[ 3 ] = GenerateOctreeNode( start.x,	C.y,	start.z,C.x,	end.y,	C.z,	node->Id );
		node->childrenId[ 4 ] = GenerateOctreeNode( start.x,	start.y,C.z,	C.x,	C.y,	end.z,	node->Id );
		node->childrenId[ 5 ] = GenerateOctreeNode( C.x,		start.y,C.z,	end.x,	C.y,	end.z,	node->Id );
		node->childrenId[ 6 ] = GenerateOctreeNode( C.x,		C.y,	C.z,	end.x,	end.y,	end.z,	node->Id );
		node->childrenId[ 7 ] = GenerateOctreeNode( start.x,	C.y,	C.z,	C.x,	end.y,	end.z,	node->Id );
	}
	
	return ( node->Id );
}

/*
===================================
	CreateGeometry
		"Marching cubes" algorithm main function
===================================
*/
uint vsSector2::Node_CreateGeometry( int id, bool keepGridSize, byte newGridSize )
{
	vsSector2OTNode* n = nodes[ id ];

	if ( !n->isLeaf )
		return 0;

	if ( !keepGridSize ) ( keepGridSize )/* do stuff */ ;

	n->oldVertexCount = n->vertices.Length();
	n->oldIndexCount = n->vertices.Length();
	n->vertices.Free();
	n->indices.Free();
	
	REPU( n->models.Length() ) {
		modelManager->DeleteMesh( n->models[ i ] );
	}
	n->models.Free();


	vsVec3ui			gridStep = vsVec3ui( 1 );
	vsVec3f				curf, nextf;
	vsVec3ui			cur, next;
	sector2GridCell_t	cell;
	uint				numVertices = 0;
	ushort		values[ 7 ];
	bool				allEqual;
	vsVec3f				N;
	vsVec3i				wcur;
	MD5ModelArgs_t		margs;
	vsVec3ui nv = ( n->end - n->start ); // /gridSize;
	vsList<sector2Vertex_t> tmpVertices;
	

	tmpVertices.Alloc( nv.x * nv.y * nv.z * 5 );

	for ( cur.x = n->start.x; cur.x < n->end.x; cur.x += gridStep.x )
	for ( cur.y = n->start.y; cur.y < n->end.y; cur.y += gridStep.y )
	for ( cur.z = n->start.z; cur.z < n->end.z; cur.z += gridStep.z )
	{
		if ( ( values[ 0 ] = VoxelDataAt( cur ) ) == 0 ) 
			continue;

		next = cur + gridStep;
		next.Clamp( NULLVEC3UI, dimensions-1 );
		curf = cur.ToVec3f();
		nextf = next.ToVec3f();
		wcur = voxelOffset + cur.ToVec3i();
		
		if ( cur.x > 0 ) values[ 1 ] = VoxelDataAt( vsVec3ui( cur.x - 1, cur.y, cur.z ) ); 
		else values[ 1 ] = worldGenerator->TerrainAt( wcur.x - 1, wcur.y, wcur.z ).id;

		if ( cur.x < dimensions.x-1 ) values[ 2 ] = VoxelDataAt( vsVec3ui( cur.x + 1, cur.y, cur.z ) ); 
		else values[ 2 ] = worldGenerator->TerrainAt( wcur.x + 1, wcur.y, wcur.z ).id;


		if ( cur.y > 0 ) values[ 3 ] = VoxelDataAt( vsVec3ui( cur.x, cur.y - 1, cur.z ) ); 
		else values[ 3 ] = worldGenerator->TerrainAt( wcur.x, wcur.y - 1, wcur.z ).id;

		if ( cur.y < dimensions.y-1 ) values[ 4 ] = VoxelDataAt( vsVec3ui( cur.x, cur.y + 1, cur.z ) ); 
		else values[ 4 ] = worldGenerator->TerrainAt( wcur.x, wcur.y + 1, wcur.z ).id;


		if ( cur.z > 0 ) values[ 5 ] = VoxelDataAt( vsVec3ui( cur.x, cur.y, cur.z - 1 ) ); 
		else values[ 5 ] = worldGenerator->TerrainAt( wcur.x, wcur.y, wcur.z - 1 ).id;

		if ( cur.z < dimensions.z-1 ) values[ 6 ] = VoxelDataAt( vsVec3ui( cur.x, cur.y, cur.z + 1 ) ); 
		else values[ 6 ] = worldGenerator->TerrainAt( wcur.x, wcur.y, wcur.z + 1 ).id;
		
		allEqual = true;
		for ( int i = 1; i < 7; i++ )
			if ( values[ 0 ] != values[ i ] )
				allEqual = false;

		if ( allEqual ) 
			continue;

		cell.vertices[ 0 ] = ( curf + vsVec3f( -0.5f, -0.5f, -0.5f ) ) * n->cellScale + offset;
		cell.vertices[ 1 ] = ( curf + vsVec3f(  0.5f, -0.5f, -0.5f ) ) * n->cellScale + offset;
		cell.vertices[ 2 ] = ( curf + vsVec3f(  0.5f,  0.5f, -0.5f ) ) * n->cellScale + offset;
		cell.vertices[ 3 ] = ( curf + vsVec3f( -0.5f,  0.5f, -0.5f ) ) * n->cellScale + offset;

		cell.vertices[ 4 ] = ( curf + vsVec3f( -0.5f,  0.5f,  0.5f ) ) * n->cellScale + offset;
		cell.vertices[ 5 ] = ( curf + vsVec3f( -0.5f, -0.5f,  0.5f ) ) * n->cellScale + offset;
		cell.vertices[ 6 ] = ( curf + vsVec3f(  0.5f, -0.5f,  0.5f ) ) * n->cellScale + offset;
		cell.vertices[ 7 ] = ( curf + vsVec3f(  0.5f,  0.5f,  0.5f ) ) * n->cellScale + offset;

		const blocktype_t* block = blockLibrary->GetBlockType( values[ 0 ] );

		// 5 6 0 0 6 1
		// 4 7 3 3 7 2
		// 3 2 0 0 2 1
		// 4 7 5 5 7 6
		// vertex connection dir (xy): -+ ++ --   -- ++ +-
		if ( ( block->flags & BLOCK_FLAG_NONSOLID ) == 0 ) {
			if ( ( block->flags & BLOCK_FLAG_LIQUID ) == 0 ) {

				REPEAT( 6 )
					if ( blockLibrary->GetBlockType( values[ i + 1 ] )->flags & BLOCK_FLAG_LIQUID )
						values[ i + 1 ] = 0;


#pragma region SOLID, NONLIQUID
				// X negative
				if ( values[ 0 ] && !values[ 1 ] ) {
					N = vsVec3f( -1.0f, 0.0f, 0.0f );
					SetupTriangle( 1, values[ 0 ], &tmpVertices, &numVertices, cell, 5, 4, 0, N, block );
					SetupTriangle( 2, values[ 0 ], &tmpVertices, &numVertices, cell, 0, 4, 3, N, block );
				}

				// X positive
				if ( values[ 0 ] && !values[ 2 ] ) {
					N = vsVec3f( 1.0f, 0.0f, 0.0f );
					SetupTriangle( 1, values[ 0 ], &tmpVertices, &numVertices, cell, 6, 7, 1, N, block );
					SetupTriangle( 2, values[ 0 ], &tmpVertices, &numVertices, cell, 1, 7, 2, N, block );
				}

				// Y negative
				if ( values[ 0 ] && !values[ 3 ] ) {
					N = vsVec3f( 0.0f, -1.0f, 0.0f );
					SetupTriangle( 1, values[ 0 ], &tmpVertices, &numVertices, cell, 5, 6, 0, N, block );
					SetupTriangle( 2, values[ 0 ], &tmpVertices, &numVertices, cell, 0, 6, 1, N, block );

				}

				// Y positive
				if ( values[ 0 ] && !values[ 4 ] ) {
					N = vsVec3f( 0.0f, 1.0f, 0.0f );
					SetupTriangle( 1, values[ 0 ], &tmpVertices, &numVertices, cell, 4, 7, 3, N, block );
					SetupTriangle( 2, values[ 0 ], &tmpVertices, &numVertices, cell, 3, 7, 2, N, block );

				}

				// Z negative
				if (  values[ 0 ] && !values[ 5 ] ) {
					N = vsVec3f( 0.0f, 0.0f, -1.0f );
					SetupTriangle( 1, values[ 0 ], &tmpVertices, &numVertices, cell, 1, 2, 0, N, block );
					SetupTriangle( 2, values[ 0 ], &tmpVertices, &numVertices, cell, 0, 2, 3, N, block );

				}

				// Z positive
				if (  values[ 0 ] && !values[ 6 ] ) {
					N = vsVec3f( 0.0f, 0.0f, 1.0f );
					SetupTriangle( 1, values[ 0 ], &tmpVertices, &numVertices, cell, 6, 7, 5, N, block );
					SetupTriangle( 2, values[ 0 ], &tmpVertices, &numVertices, cell, 5, 7, 4, N, block );
				}
#pragma endregion
			} else {	// if liquid
				// Y positive
				if ( values[ 0 ] && !values[ 4 ] ) {
					N = vsVec3f( 0.0f, 1.0f, 0.0f );
					SetupTriangle( 1, values[ 0 ], &tmpVertices, &numVertices, cell, 4, 7, 3, N, block );
					SetupTriangle( 2, values[ 0 ], &tmpVertices, &numVertices, cell, 3, 7, 2, N, block );
				}
			}
		} else { // if non-solid
			if ( block->modelargs != NULL ) {
				n->models.Expand();

				margs = *block->modelargs;
				margs.offset = cell.vertices[ 0 ] + 0.5f * n->cellScale;
				n->models[ n->models.Length()-1 ] = modelManager->LoadMesh( margs );

				if ( strlen( block->animFileName ) > 0 ) {
					n->models[ n->models.Length()-1 ]->AddAnimation( modelManager->LoadAnim( block->animFileName ) );
					n->models[ n->models.Length()-1 ]->SetAnimation( 0, ANIM_PLAY_REPEAT );
				}
			} // eof modelargs != NULL
		} // eof non-solid
	} // eof for
	
	if ( numVertices > 0 ) 
	{
		vsVec3f bbMin, bbMax;
		vsVec3f* v;
		bbMin = bbMax = tmpVertices[0].xyz;
		n->vertices.Alloc( numVertices );
		n->indices.Alloc( numVertices );
		
		// Remove duplicated vertices
		bool found;
		uint realNumVertices=0;
		for ( uint i=0,j; i<numVertices; ++i )
		{
			found=false;
			//for ( j=realNumVertices-1; j>0; --j ) -- maybe faster
			/*for ( j=0; j<realNumVertices; ++j )
			{
				if ( n->vertices[j] == tmpVertices[i] ) {
					n->indices[i] = j;
					found=true;
					break;
				}
			}*/

			if ( !found ) {
				n->vertices[realNumVertices] = tmpVertices[i];
				n->indices[i] = realNumVertices;

				v = &tmpVertices[i].xyz;
				if ( v->x > bbMax.x ) bbMax.x = v->x;
				if ( v->y > bbMax.y ) bbMax.y = v->y;
				if ( v->z > bbMax.z ) bbMax.z = v->z;
				if ( v->x < bbMin.x ) bbMin.x = v->x;
				if ( v->y < bbMin.y ) bbMin.y = v->y;
				if ( v->z < bbMin.z ) bbMin.z = v->z;

				++realNumVertices;
			}
		}

		numVertices = realNumVertices;
		n->vertices.Resize( numVertices );

		n->bounds.FromMinMax( bbMin, bbMax );
		
		//Node_CalculateNormals( n->Id );	

		n->bufferUpdateNeeded = true;

		if ( n->geometryState == NGS_NO_GEOMETRY )
			n->geometryState = NGS_GEOMETRY_OK;
	}

	tmpVertices.Free();

	n->geometryModified = false;
	//n->bufferUpdateNeeded = true;

	return numVertices;
}

/*
===================================
	SetupTriangle
===================================
*/
void vsSector2::SetupTriangle( int texCoords, ushort val, vsList<sector2Vertex_t>* pVert, uint* numVertices, const sector2GridCell_t& cell,
							   int a, int b, int c, const vsVec3f& Normal, const blocktype_t* blocktype )
{
	sector2Vertex_t v1, v2, v3;
	
	v1.xyz = cell.vertices[ a ];
	v2.xyz = cell.vertices[ b ];
	v3.xyz = cell.vertices[ c ];
	
	

	v1.Normal = v2.Normal = v3.Normal = Normal;
	if ( Normal.x != 0.0f ) {
		v1.Tangent = v2.Tangent = v3.Tangent = Normal.GetOrthoXY();
		v1.st = v2.st = v3.st = blocktype->tcSide.ToVec2f();
	}
	else
	if ( Normal.y != 0.0f ) {
		v1.Tangent = v2.Tangent = v3.Tangent = Normal.GetOrthoYZ();
		if ( Normal.y > 0.0f )
			v1.st = v2.st = v3.st = blocktype->tcTop.ToVec2f();
		else
			v1.st = v2.st = v3.st = blocktype->tcBottom.ToVec2f();
	}
	else
	if ( Normal.z != 0.0f ) {
		v1.Tangent = v2.Tangent = v3.Tangent = Normal.GetOrthoXZ();
		v1.st = v2.st = v3.st = blocktype->tcSide.ToVec2f();
	}
		
	// in_GhostTexCoords ( 3 bytes each )
	if ( texCoords == 1 ) {
		v1.reserved[0] = 1;
		v1.reserved[1] = 1;

		v2.reserved[0] = 1;
		v2.reserved[1] = 0;

		v3.reserved[0] = 0;
		v3.reserved[1] = 1;
	} else {
		v1.reserved[0] = 0;
		v1.reserved[1] = 1;

		v2.reserved[0] = 1;
		v2.reserved[1] = 0;

		v3.reserved[0] = 0;
		v3.reserved[1] = 0;
	}
	/*	if ( texCoords == 1 ) {
		v1.reserved[0] = 0;
		v1.reserved[1] = 1;

		v2.reserved[0] = 1;
		v2.reserved[1] = 1;

		v3.reserved[0] = 0;
		v3.reserved[1] = 0;
	} else {
		v1.reserved[0] = 0;
		v1.reserved[1] = 0;

		v2.reserved[0] = 1;
		v2.reserved[1] = 1;

		v3.reserved[0] = 1;
		v3.reserved[1] = 0;
	}*/
	/*
	ushort * s;
	s = reinterpret_cast< ushort* >( v1.reserved[ 2 ] );
	*s = val;

	s = reinterpret_cast< ushort* >( v2.reserved[ 2 ] );
	*s = val;

	s = reinterpret_cast< ushort* >( v3.reserved[ 2 ] );
	*s = val;*/

	( *pVert )[ ( *numVertices )++ ] = v1;
	( *pVert )[ ( *numVertices )++ ] = v2;
	( *pVert )[ ( *numVertices )++ ] = v3;
}

/*
===================================
	Node_UpdateBuffers
===================================
*/
void vsSector2::Node_UpdateBuffers( int id, uint oldVertexCount, uint oldIndexCount )
{
	vsSector2OTNode* n = nodes[id];

	if ( n->geometryState == NGS_GEOMETRY_OK ) {		
		/*if ( newSector ) {
			// vegetation
			REPU( n->vegetation.Length() )
				itemManager->DeleteItem( n->vegetation[ i ] );
			n->vegetation.Free();

			vegetator->GenerateForNode( n );
		}

		//grass
		if ( !varStorage.Bool["nograss"] ) {
			n->grass.Init( Grass );
			n->grass.Generate( n, Grass.density );
		}
		*/
		n->geometryState = NGS_VEGETATION_OK;
	}

	if ( n->buffers[0] == 0 ) {
		glGenBuffers( 2, n->buffers );
		glGenQueries( 1, &n->query );
		n->geometryState = NGS_FULL_GEOMETRY;
	}
		
	glBindBuffer( GL_ARRAY_BUFFER, n->buffers[0] );
	if ( oldVertexCount < n->vertices.Length() )
		glBufferData( GL_ARRAY_BUFFER, 
						n->vertices.Length()*sizeof( sector2Vertex_t ), 
						n->vertices.list, 
						GL_DYNAMIC_DRAW );
	else
		glBufferSubData( GL_ARRAY_BUFFER, 
						0,
						n->vertices.Length()*sizeof( sector2Vertex_t ),
						n->vertices.list );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, n->buffers[1] );
	if ( oldIndexCount < n->indices.Length() )
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
						n->indices.Length()*sizeof( uint ), 
						n->indices.list, 
						GL_DYNAMIC_DRAW );
	else
		glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 
						0,
						n->indices.Length()*sizeof( uint ),
						n->indices.list );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	n->bufferUpdateNeeded = false;
}


/*
===================================
	Node_UpdateVisibility
===================================
*/
void vsSector2::Node_UpdateVisibility( int id )
{
	vsSector2OTNode* n = nodes[id];

	if ( !( n->visible = player->GetFrustum()->IntersectBox( bounds, 0.2f ) ) )
		return;

	//if ( id == 0 ) {
		glBeginQuery( GL_SAMPLES_PASSED, n->query );
//	}


//	if ( id == 0 ) {
		glEndQuery( GL_SAMPLES_PASSED );
	//}
}

/*
===================================
	Render()
===================================
*/
void vsSector2::Node_Render( int id )
{
	if ( !generated ) 
		return;

#define VAP( attrib, size, offset ) \
	glVertexAttribPointer( attrib, size, GL_FLOAT, false, sizeof( sector2Vertex_t ), BUFFER_OFFSET( offset ) )

	vsSector2OTNode* n = nodes[id];

	if ( !n )
		return;

	if ( !( n->visible = player->GetFrustum()->IntersectBox( bounds, 0.001f ) ) )
		return;

//	if ( !n->visible )
	//	return;

	if ( n->isLeaf )
	{
		if ( n->geometryModified )
			Node_CreateGeometry( n->Id );
		if ( n->bufferUpdateNeeded )
			Node_UpdateBuffers( n->Id, n->oldVertexCount, n->oldIndexCount );

		if ( !( n->visible = ( n->vertices.Length() > 0 ) ) )
			return;

		if ( varStorage.Bool["wireframe"] ) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}

	/*	glGetQueryObjectiv( n->query, GL_QUERY_RESULT, &n->lastQueryVisible );	

		glColorMask( 0, 0, 0, 0 );
		glDepthMask( 0 );
		glBeginQuery( GL_SAMPLES_PASSED, n->query );
			n->bounds.RenderBox( false );
		glEndQuery( GL_SAMPLES_PASSED );
		glColorMask( 1, 1, 1, 1 );
		glDepthMask( 1 );	

		if ( !( n->visible=( n->lastQueryVisible>0 ) ) )
			return;
			*/
		numVisibleLeafNodes++;

		glBindBuffer( GL_ARRAY_BUFFER, n->buffers[ 0 ] );


		VAP( S2ATTRIB_POSITION,		3, sector2Vertex_t::Offset_xyz );
		
		VAP( S2ATTRIB_NORMAL,		3, sector2Vertex_t::Offset_Normal );
	
		VAP( S2ATTRIB_TANGENT,		3, sector2Vertex_t::Offset_Tangent );

		VAP( S2ATTRIB_MEGATEXCOORDS,	2, sector2Vertex_t::Offset_st );

		glVertexAttribPointer( S2ATTRIB_GHOSTTEXCOORDS, 2, GL_UNSIGNED_BYTE, false, sizeof( sector2Vertex_t ), BUFFER_OFFSET( sector2Vertex_t::Offset_reserved ) );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, n->buffers[1] );

		glDrawElements( GL_TRIANGLES, n->indices.Length(), GL_UNSIGNED_INT, NULL );

		if ( varStorage.Bool["rendernormals"] ) {
			shader->Bind( false );
			Node_RenderBasis( n->Id );
			shader->Bind( true );
		}

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	} else {
		for ( int i=0; i<8; ++i )
			Node_Render( n->childrenId[i] );
	} 
}

/*
===================================
	RenderBasis )
===================================
*/
void vsSector2::Node_RenderBasis( int id )
{
	vsSector2OTNode* n= nodes[id];
	glLineWidth( 4.0f );
	glBegin( GL_LINES );
	REPU( n->vertices.Length() )
	{
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex3fv( n->vertices[i].xyz.ToTypePtr() );
		glColor3f( 1.0f, 1.0f, 0.0f );
		glVertex3fv( ( n->vertices[i].xyz+n->vertices[i].Normal ).ToTypePtr() );

	//	glColor3f( 0.0f, 1.0f, 0.0f );
	//	glVertex3fv( vertices[i].xyz.ToTypePtr() );
	//	glVertex3fv( ( vertices[i].xyz+vertices[i].Tangent ).ToTypePtr() );
	}
	glEnd();	
}

/*
===================================
	RenderTree()
===================================
*/
void vsSector2::Node_RenderTree( int id )
{
	vsSector2OTNode* n = nodes[id];
	//if ( !( n->visible = player->GetFrustum()->IntersectBox( bounds, 0.2f ) ) )
		//return;

	if ( n->IsLeaf() /*&& n->visible*/ )
	{
		n->bounds.RenderBox( true );
	} else {
		for ( int i=0; i<8; ++i )
			Node_RenderTree( n->childrenId[i] );
	} 
}

/*
===================================
	PutBlock
		Return: 1 if block has been put
			    0 if not.
===================================
*/
int vsSector2::PutBlock( const vsVec3ui& xyz, ushort blockType )
{
	if ( VoxelDataAt( xyz ) == 0 )
	{
		//console->Print( "PutBlock" );
		VoxelDataAt( xyz ) = blockType;
		SetModificationState( true );
		UpdateModifiedGeometry( xyz );

		vsVec3ui realxyz = xyz * gridSize;
		vsSector2* n = neighbors[S2N_L];
		if ( xyz.x == 0 && n )
		{
			n->NeighborPutBlock( vsVec3ui( n->realDimensions.x-1,
										  realxyz.y, 
										  realxyz.z ), 
								 blockType );
		}

		n = neighbors[S2N_R];
		if ( xyz.x == dimensions.x-1 && n )
		{
			n->NeighborPutBlock( vsVec3ui( 0,
										  realxyz.y, 
										  realxyz.z ), 
								 blockType );
		}

		n = neighbors[S2N_F];
		if ( xyz.z == 0 && n )
		{
			n->NeighborPutBlock( vsVec3ui( realxyz.x, 
										  realxyz.y,
										  n->realDimensions.z-1 ), 
								blockType );
		}

		n = neighbors[S2N_B];
		if ( xyz.z == dimensions.z-1 && n )
		{
			n->NeighborPutBlock( vsVec3ui( realxyz.x, 
										  realxyz.y,
										  0 ), 
								blockType );
		}

		n = neighbors[S2N_LO];
		if ( xyz.y == 0 && n )
		{
			n->NeighborPutBlock( vsVec3ui( realxyz.x, 
										  n->realDimensions.y-1, 
										  realxyz.z ), 
								blockType );
		}

		n = neighbors[S2N_UP];
		if ( xyz.y == dimensions.y-1 && n )
		{
			n->NeighborPutBlock( vsVec3ui( realxyz.x, 
										  0, 
										  realxyz.z ), 
								blockType );
		}

		return 1;
	}

	return 0;
}

/*
===================================
	RemoveBlock
		Return: 1 if block has been removed
			    0 if not.
===================================
*/
ushort vsSector2::RemoveBlock( const vsVec3ui& xyz )
{
	ushort voxel = VoxelDataAt( xyz );

	if ( voxel != 0 )
	{
		SetModificationState( true );
		VoxelDataAt( xyz ) = 0;
		UpdateModifiedGeometry( xyz );
				
		vsVec3ui realxyz = xyz * gridSize;
		vsSector2* n = neighbors[S2N_L];
		if ( xyz.x == 0 && n )
		{
			n->NeighborRemoveBlock( vsVec3ui( n->realDimensions.x-1,
										  realxyz.y, 
										  realxyz.z ) );
		}

		n = neighbors[S2N_R];
		if ( xyz.x == dimensions.x-1 && n )
		{
			n->NeighborRemoveBlock( vsVec3ui( 0,
										  realxyz.y, 
										  realxyz.z ) );
		}

		n = neighbors[S2N_F];
		if ( xyz.z == 0 && n )
		{
			n->NeighborRemoveBlock( vsVec3ui( realxyz.x, 
										  realxyz.y,
										  n->realDimensions.z-1 ) );
		}

		n = neighbors[S2N_B];
		if ( xyz.z == dimensions.z-1 && n )
		{
			n->NeighborRemoveBlock( vsVec3ui( realxyz.x, 
										  realxyz.y,
										  0 ) );
		}

		n = neighbors[S2N_LO];
		if ( xyz.y == 0 && n )
		{
			n->NeighborRemoveBlock( vsVec3ui( realxyz.x, 
										  n->realDimensions.y-1, 
										  realxyz.z ) );
		}

		n = neighbors[S2N_UP];
		if ( xyz.y == dimensions.y-1 && n )
		{
			n->NeighborRemoveBlock( vsVec3ui( realxyz.x, 
										  0, 
										  realxyz.z ) );
		}

		return voxel;
	}

	return 0;
}

/*
===================================
	PutMultipleBlocks
		Return: 1 if blocks have been put
			    0 if not.
===================================
*/
int vsSector2::PutMultipleBlocks( vsVec3i coords[], int len, ushort blockType )
{
	for ( int i = 0; i < len; ++i )
	{
		if ( IsValidVoxelIndex( coords[ i ] ) ) 
			PutBlock( coords[ i ].ToVec3ui(), blockType );
	}

	return 1;
}

/*
===================================
	RemoveMultipleBlocks
		Return: 1 if blocks have been removed
			    0 if not.
===================================
*/
int vsSector2::RemoveMultipleBlocks( vsVec3i coords[], int len )
{
	for ( int i = 0; i < len; ++i )
	{
		if ( IsValidVoxelIndex( coords[ i ] ) ) 
			RemoveBlock( coords[ i ].ToVec3ui() );
	}

	return 1;
}

/*
===================================
	NeighborPutBlock
		This function is called by the neighbor when 
		a neighboring voxel is updated.
		xyz is in REAL VOXEL COORDS
===================================
*/
void vsSector2::NeighborPutBlock( const vsVec3ui& xyz, ushort blockType )
{
	vsVec3f fxyz = xyz.ToVec3f();
	vsVec3f transformedXYZ = fxyz / ( float )gridSize;

	// xyz is not visible at our precision
	if ( fxyz != transformedXYZ.Floor() )
		return;

	vsVec3ui ijk = transformedXYZ.ToVec3ui();
	if ( VoxelDataAt( ijk ) == 0 )
	{
		VoxelDataAt( ijk ) = blockType;
		SetModificationState( true );
		UpdateModifiedGeometry( ijk );
	}
}

/*
===================================
	NeighborRemoveBlock
		This function is called by the neighbor when 
		a neighboring voxel is updated.
		xyz is in REAL VOXEL COORDS
===================================
*/
void vsSector2::NeighborRemoveBlock( const vsVec3ui& xyz )
{
	vsVec3f fxyz = xyz.ToVec3f();
	vsVec3f transformedXYZ = fxyz / ( float )gridSize;

	// xyz is not visible at our precision
	if ( fxyz != transformedXYZ.Floor() )
		return;

	vsVec3ui ijk = transformedXYZ.ToVec3ui();
	if ( VoxelDataAt( ijk ) != 0 )
	{
		SetModificationState( true );
		VoxelDataAt( ijk ) = 0;
		UpdateModifiedGeometry( ijk );
	}

}
/*
===================================
	UpdateModifiedGeometry
		Recreates geometry for the node in which the given
		VOXEL COORDINATE is in
===================================
*/
int vsSector2::UpdateModifiedGeometry( const vsVec3ui& p )
{
	int side;
	vsSector2OTNode *n;
	
	n = GetLeafNodeOfCell( p, side, 1 );

	if ( n == NULL ) 
		return -1;

	if ( n->geometryModified ) 
		return -1;

	n->geometryModified = true;
	
	for ( int i = -1; i < 2; ++i )
	for ( int j = -1; j < 2; ++j )
	for ( int k = -1; k < 2; ++k )
		UpdateModifiedGeometry( p + vsVec3ui( i * 2, j * 2, k * 2 ) );

	return -1;
}


/*
===================================
	VoxelDataAt
===================================
*/
ushort& vsSector2::VoxelDataAt( const vsVec3ui& xyz )
{
#define VoxelDataAt_NoCheck
#ifndef VoxelDataAt_NoCheck
	static ushort err = VOXEL_TYPE_INVALID_INDEX;

	if ( xyz<dimensions && xyz>=vsVec3ui( 0 ) )
		return voxelData[xyz.x][xyz.y][xyz.z];
	else
		return err;
#else
	return voxelData[ xyz.x ][ xyz.y ][ xyz.z ];
#endif
}

/*
===================================
	GetLocalCellOfPoint
		Returns the VOXEL coordinates of a point in space
===================================
*/
vsVec3i vsSector2::GetLocalCellOfPoint( const vsVec3f& p )
{
	vsVec3ui c = ( ( p - offset + 0.5f * scale_1 ).Abs() / scale_1 ).ToVec3ui().Abs();
	
	if ( c >= NULLVEC3UI && c < dimensions )
	{
		return c.ToVec3i();
	}

	return vsVec3i( -1 );	
}

/*
===================================
	GetLeafNodeOfCell
		Returns the NODE of a given VOXEL coordinate 
		in which it is located

		resultNum: [hun] annyiadik találattal térjen vissza
===================================
*/
vsSector2OTNode* vsSector2::GetLeafNodeOfCell( const vsVec3ui& cell, int& side, int resultNum )
{
	if ( cell < dimensions )
	{
		int k=0;
		for ( uint i = 0; i < nodes.Length(); ++i )
			if ( nodes[ i ]->IsLeaf() ) {
				if ( nodes[ i ]->start <= cell &&
					nodes[ i ]->end >= cell ) {
					if ( resultNum == ++k ) {

						return nodes[i];
					}
				}
			} 
	}
	
	return NULL;
}

/*
===================================
	GetPointsNode
		Helper function for GetNodeOfPlayer() & GetNodeOfPoint
===================================
*/
int vsSector2::GetPointsNode( int nodeId, const vsVec3f& point )
{
	if ( nodeId<0 || nodes.list == NULL ) 
		return -1;

	int i,j;

	if ( nodes[nodeId]->bounds.IsPointInside( point ) )
	{
		if ( nodes[nodeId]->IsLeaf() )
			return nodeId;
		else 
		{
			i=-1; j=0;
			while( j<8 )
			{
				i = GetPointsNode( nodes[nodeId]->childrenId[j++], point );

				if ( i!=-1 )
					return i;
			}			
		}
	}

	return -1;
}

/*
===================================
	GetNodeOfPlayer
		Returns the NODE in which the PLAYER is in
===================================
*/
vsSector2OTNode* vsSector2::GetNodeOfPlayer( void )
{
	int i = GetPointsNode( 0, player->GetPhysics()->GetDestination() );
	if ( i == -1 )
		return NULL;

	return nodes[ i ];
}

/*
===================================
	GetNodeOfPoint
		Returns the NODE in which the given POINT is in
===================================
*/
vsSector2OTNode* vsSector2::GetNodeOfPoint( const vsVec3f& point )
{
	int i = GetPointsNode( 0, point );
	if ( i == -1 )
		return NULL;

	return nodes[ i ];
}


/*
===================================
	UpdateBuffers
===================================
*/
void vsSector2::UpdateBuffers( void )
{
	REPU( nodes.Length() )
		if ( nodes[ i ]->IsLeaf() )
			Node_UpdateBuffers( i );
}

/*
===================================
	Refresh
		[ neighbors = NULL ]
===================================
*/
void vsSector2::Refresh( bool mainSector, bool physicsTest, bool generated )
{
	physicsManager->SetTesting( physics, physicsTest );

	this->mainSector = mainSector;
	this->generated = generated;
	
	REPU2( k, 6 ) 
		neighbors[ k ] = NULL;
}

/*
===================================
	FetchNeighborsFromList
===================================
*/
void vsSector2::FetchNeighborsFromList( const vsList<vsSector2*>& list )
{
	REPU( 6 ) neighbors[i] = NULL;
	vsVec3i dc;
	REPU( list.Length() )
	{
		dc = list.list[i]->coord - coord;
		if ( dc == vsVec3i( 0,-1, 0 ) )
			neighbors[S2N_LO] = list.list[i];
		else
		if ( dc == vsVec3i( 0, 1, 0 ) )
			neighbors[S2N_UP] = list.list[i];
		else
		if ( dc == vsVec3i( -1, 0, 0 ) )
			neighbors[S2N_L] = list.list[i];
		else
		if ( dc == vsVec3i( 1, 0, 0 ) )
			neighbors[S2N_R] = list.list[i];
		else
		if ( dc == vsVec3i( 0, 0,-1 ) )
			neighbors[S2N_F] = list.list[i];
		else
		if ( dc == vsVec3i( 0, 0, 1 ) )
			neighbors[S2N_B] = list.list[i];
	}
}

/*
===================================
	SaveToFile
===================================
*/
void vsSector2::SaveToFile( char* saveFolder )
{	
	if ( saveFolder )
	{
		char szFileName[128];
		ZeroMemory( szFileName, 128 );
		sprintf_s<128>( szFileName, "%s/%d_%d_%d.vws", saveFolder, coord.x, coord.y, coord.z );

		vsOutputFile f;
		if ( f.Open( szFileName, "wb+" ) != VS_OK ) 
		{
			console->Print( CMT_ERROR, "sector::SaveToFile() FAILED" );
			return;
		}

		f.WriteInt( &coord.x ); 
		f.WriteInt( &coord.y );
		f.WriteInt( &coord.z );
		f.WriteInt( &voxelOffset.x ); 
		f.WriteInt( &voxelOffset.y );
		f.WriteInt( &voxelOffset.z );
		f.WriteUnsignedInt( &gridSize );
		f.WriteVec3( &bounds.points[0] );
		f.WriteVec3( &bounds.points[7] );
		f.WriteUnsignedInt( &realDimensions.x );
		f.WriteVec3( &scale );
		f.WriteVec3( &offset );
		f.WriteVec3( &texScale );

		REPU2( x, dimensions.x )
		REPU2( y, dimensions.y )
		REPU2( z, dimensions.z )
		{
			f.WriteUnsignedShort( &voxelData[x][y][z] );
		}
	
		f.Close();
	}
}

/*
===================================
	RestoreFromFile
===================================
*/
void vsSector2::RestoreFromFile( char* restoreFolder, const vsVec3i& coords, bool keepGridSize, uint newGridSize )
{
	char fname[ 128 ];
	sprintf_s< 128 >( fname, "%s/%d_%d_%d.vws", restoreFolder, coords.x, coords.y, coords.z );
	
	vsInputFile f;
	if ( f.Open( fname, "rb" ) != VS_OK ) 
	{
		console->Print( CMT_ERROR, "Failed to restore sector from file" );
		return;
	}

	Destroy();
	f.ReadInt( &coord.x ); 
	f.ReadInt( &coord.y );
	f.ReadInt( &coord.z );
	f.ReadInt( &voxelOffset.x ); 
	f.ReadInt( &voxelOffset.y );
	f.ReadInt( &voxelOffset.z );
	f.ReadUnsignedInt( &gridSize );
	f.ReadVec3( &bounds.points[ 0 ] );
	f.ReadVec3( &bounds.points[ 7 ] );
	f.ReadUnsignedInt( &realDimensions.x );
	f.ReadVec3( &scale );
	f.ReadVec3( &offset );
	f.ReadVec3( &texScale );	

	bounds.FromMinMax( bounds.points[ 0 ], bounds.points[ 7 ] );
	realDimensions = vsVec3ui( realDimensions.x );

	if ( !keepGridSize && gridSize != newGridSize ) 
	{
		byte tmpByte;
		uint q;

		dimensions = realDimensions / newGridSize;
		AllocVoxelData( dimensions.x, dimensions.y, dimensions.z );

		// file contains it more detailed
		// but we want to load it at lower precision
		if ( newGridSize > gridSize ) 
		{	
			q = newGridSize / gridSize;

			REPU2 ( x, dimensions.x )
			REPU2 ( y, dimensions.y )
			REPU2 ( z, dimensions.z ) {
				f.ReadUnsignedShort( &voxelData[ x ][ y ][ z ] );
				REPU( q - 1 )
					f.ReadByte( &tmpByte );
			}
		} else 
		// file contains it less detailed
		{	
			q = gridSize / newGridSize;

			for ( uint y, z, x = 0; x < dimensions.x; x += q )
			for ( y = 0; y < dimensions.y; y += q )
			for ( z = 0; z < dimensions.z; z += q )
				f.ReadUnsignedShort( &voxelData[ x ][ y ][ z ] );

			// generate missing voxels
			for ( uint z, y, x = 0; x < dimensions.x; x++ )
			for ( y = 0; y < dimensions.y; y++ )
			for ( z = 0; z < dimensions.z; z++ )
				if ( !( ( x % q == 0 ) && ( y % q == 0 ) && ( z % q == 0 ) ) )
					voxelData[ x ][ y ][ z ] = worldGenerator->TerrainAt( voxelOffset.x+x, voxelOffset.y+y, voxelOffset.z+z ).id;
		}

		gridSize = newGridSize;
	} else {
		dimensions = realDimensions / gridSize;
		AllocVoxelData( dimensions.x, dimensions.y, dimensions.z );

		REPU2 ( x, dimensions.x )
		REPU2 ( y, dimensions.y )
		REPU2 ( z, dimensions.z ) 
			f.ReadUnsignedShort( &voxelData[ x ][ y ][ z ] );
	}

	this->scale_1 = scale / ( dimensions - 1 ).ToVec3f();
	GenerateOctree();

	f.Close();
}