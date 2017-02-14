#include "Chunk.h"
#include "..\WorldGenerator.h"
#include "..\..\System\VarStorage.h"
#include "..\Game.h"

using namespace vsLib2;
using namespace vsMath;

/* 
=====================================
	vsChunk
=====================================
*/
vsChunk::vsChunk( void ) {
	modified		= false;
	updateBuffers	= false;
	updateGeometry	= false;

	shader	= NULL;
	id		= 0;

	buffer				= 0;
	transparentBuffer		= 0;

	oldVertexCount		= 0;
	oldLiquidVertexCount= 0;
}

/* 
=====================================
	~vsChunk
=====================================
*/
vsChunk::~vsChunk( void ) {
	aabbs.Free();
	vertices.Free();
	transparentVertices.Free();

	if ( buffer ) 
		glDeleteBuffers( 1, &buffer );
	if ( transparentBuffer ) 
		glDeleteBuffers( 1, &transparentBuffer );
	
	REPU( nonsolidBlocks.Length() )
		modelManager->DeleteMesh( nonsolidBlocks[ i ] );

	nonsolidBlocks.Free();
}

/* 
=====================================
	Generate
		_chunkCoord:	chunk space coordinate of chunk
		szRestoreFolder: if not NULL, chunk is restored from the folder
						 if NULL, new chunk is generated
=====================================
*/
void vsChunk::Generate( const vsVec3i& _chunkCoord, const char* szRestoreFolder  ) {
	chunkCoord = _chunkCoord;
	blockCoord = chunkCoord * chunkDim;
	box.FromMinMax( blockCoord.ToVec3f(), ( blockCoord + chunkDim ).ToVec3f() );

	SetupShader();

	bool generateNew = true;

	if ( szRestoreFolder ) {
		generateNew = false;

		char fname[ 128 ];
		sprintf_s< 128 >( fname, "%s/%d_%d_%d.vws", 
			szRestoreFolder, 
			_chunkCoord.x, 
			_chunkCoord.y, 
			_chunkCoord.z );
	
		vsInputFile f;
		if ( f.Open( fname, "rb" ) != VS_OK ) 
		{
			console->Print( CMT_ERROR, "Failed to restore chunk -> Generating new" );
			generateNew = true;
		}

		REPEAT2( x, chunkDimX )
		REPEAT2( y, chunkDimY )
		REPEAT2( z, chunkDimZ ) {
		 f.ReadUnsignedShort( &blocks[ x ][ y ][ z ].id );
		 f.ReadByte( &blocks[ x ][ y ][ z ].extra );
		}

		f.Close();
	}

	if ( generateNew )
		QueryVoxels();

	GenerateGeometry();
}

/* 
=====================================
	SaveToFolder
=====================================
*/
void vsChunk::SaveToFolder( const char* szFolder ) {
	char fname[ 128 ];
	sprintf_s< 128 >( fname, "%s/%d_%d_%d.vws", szFolder, chunkCoord.x, chunkCoord.y, chunkCoord.z );
	
	vsOutputFile f;
	if ( f.Open( fname, "wb+" ) != VS_OK ) 
	{
		console->Print( CMT_ERROR, "Failed to save chunk" );
		return;
	}

	REPEAT2( x, chunkDimX )
	REPEAT2( y, chunkDimY )
	REPEAT2( z, chunkDimZ ) {
	 f.WriteUnsignedShort( &blocks[ x ][ y ][ z ].id );
	 f.WriteByte( &blocks[ x ][ y ][ z ].extra );
	}
	f.Close();
}

/* 
=====================================
	Render
=====================================
*/
void vsChunk::Render( void ) {

#define VAP( attrib, size, offset ) \
	glVertexAttribPointer( attrib, size, GL_FLOAT, false, sizeof( vsVertex ), BUFFER_OFFSET( offset ) )

	if ( !shader || ( !vertices.Length() && !transparentVertices.Length() ) )
		return;

	if ( !player->GetFrustum()->IntersectBox( box, 0.02f ) )
		return;

	if ( updateGeometry )
		GenerateGeometry();

	if ( updateBuffers )
		UpdateBuffers();

	if ( varStorage.Bool["wireframe"] ) {
		glLineWidth( 1.0f );
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}

	shader->Bind( true );
	
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, blockLibrary->GetTextureAtlas() );
	shader->Uniform1i( "in_MegaTextureDiffuse", 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, blockLibrary->GetTextureAtlasBump() );
	shader->Uniform1i( "in_MegaTextureNormal", 1 );
	
	glEnableVertexAttribArray( CVA_POSITION );
	glEnableVertexAttribArray( CVA_NORMAL );
	glEnableVertexAttribArray( CVA_CELLLIFE );
	glEnableVertexAttribArray( CVA_MEGATEXCOORDS );
	glEnableVertexAttribArray( CVA_GHOSTTEXCOORDS );
	glEnableVertexAttribArray( CVA_VALUE );

	// Vertices of Solid Blocks
	glBindBuffer( GL_ARRAY_BUFFER, buffer );

	VAP( CVA_POSITION,		3, vsVertex::Offset_xyz );	
	VAP( CVA_NORMAL,		3, vsVertex::Offset_Normal );
	VAP( CVA_CELLLIFE,		1, vsVertex::Offset_Color + 3 * sizeof( float ) );
	VAP( CVA_MEGATEXCOORDS, 2, vsVertex::Offset_st );
	glVertexAttribPointer( CVA_GHOSTTEXCOORDS, 2, GL_UNSIGNED_BYTE, false, sizeof( vsVertex ), BUFFER_OFFSET( vsVertex::Offset_reserved ) );
	glVertexAttribPointer( CVA_VALUE, 1, GL_UNSIGNED_SHORT, false, sizeof( vsVertex ), BUFFER_OFFSET( vsVertex::Offset_reserved + 2 ) );

	glDrawArrays( GL_TRIANGLES, 0, vertices.Length() );

	if ( transparentVertices.Length() ) {
		// Vertices of Liquid blocks
		glBindBuffer( GL_ARRAY_BUFFER, transparentBuffer );

		VAP( CVA_POSITION,		3, vsVertex::Offset_xyz );	
		VAP( CVA_NORMAL,		3, vsVertex::Offset_Normal );
		VAP( CVA_CELLLIFE,		1, vsVertex::Offset_Color + 12 );
		VAP( CVA_MEGATEXCOORDS, 2, vsVertex::Offset_st );
		glVertexAttribPointer( CVA_GHOSTTEXCOORDS, 2, GL_UNSIGNED_BYTE, false, sizeof( vsVertex ), BUFFER_OFFSET( vsVertex::Offset_reserved ) );
		glVertexAttribPointer( CVA_VALUE, 1, GL_UNSIGNED_SHORT, false, sizeof( vsVertex ), BUFFER_OFFSET( vsVertex::Offset_reserved + 2 ) );
		
		glDrawArrays( GL_TRIANGLES, 0, transparentVertices.Length() );
	}

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glDisableVertexAttribArray( CVA_POSITION );
	glDisableVertexAttribArray( CVA_NORMAL );
	glDisableVertexAttribArray( CVA_CELLLIFE );
	glDisableVertexAttribArray( CVA_MEGATEXCOORDS );
	glDisableVertexAttribArray( CVA_GHOSTTEXCOORDS );
	glDisableVertexAttribArray( CVA_VALUE );
	
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );
		
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	shader->Bind( false );

	if ( varStorage.Bool["rendertree"] )
		box.RenderBox( true );

	
	glDisable( GL_DEPTH_TEST );
	REPU( aabbs.Length() ) {
		aabbs[ i ].SetRenderColor( vsVec4f( 0.0f, 0.0f, 0.6f, 0.5f ) );	
		aabbs[ i ].RenderBox( false );
	}

	glEnable( GL_DEPTH_TEST );

	glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
	glPointSize( 10.0f );
	glBegin( GL_POINTS );
	REPU( ipoints.Length() ) 
		glVertex3fv( ipoints[ i ].ToTypePtr() );
	glEnd();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	/*
	ushort soil = blockLibrary->GetBlockTypeId( "soil" );
	ushort grass = blockLibrary->GetBlockTypeId( "grass" );
	REPEAT2( x, chunkDimX )
	REPEAT2( y, chunkDimY )
	REPEAT2( z, chunkDimZ ) {
		if ( blocks[ x ][ y ][ z ] == soil && y < chunkDimY - 1 ) 
			if ( blocks[ x ][ y + 1 ][ z ] == 0 ) {
				blockExtra[ x ][ y ][ z ]++;
				if ( blockExtra[ x ][ y ][ z ] > 200 )
					blocks[ x ][ y ][ z ] = grass;
			}
	}*/
}

/* 
=====================================
	PutBlock
=====================================
*/
void vsChunk::PutBlock( const vsVec3i& toCell, const block_t& block ) {
	vsVec3i indx = toCell - blockCoord;
	ushort still_water = blockLibrary->GetBlockTypeId( "still_water" );
	int not_stable = 0;
	/*
	if ( indx.x > 0 )
		if ( blocks[ indx.x - 1 ][ indx.y ][ indx.z ].id == 0 ||
			 blocks[ indx.x - 1 ][ indx.y ][ indx.z ].id == still_water )
			  not_stable++;
	if ( indx.y > 0 )
		if ( blocks[ indx.x ][ indx.y - 1 ][ indx.z ].id == 0 ||
			 blocks[ indx.x ][ indx.y - 1 ][ indx.z ].id == still_water )
			  not_stable++;
	if ( indx.z > 0 )
		if ( blocks[ indx.x ][ indx.y ][ indx.z - 1 ].id == 0 ||
			 blocks[ indx.x ][ indx.y ][ indx.z - 1 ].id == still_water )
			  not_stable++;
	if ( indx.x < chunkDim.x - 1 )
		if ( blocks[ indx.x + 1 ][ indx.y ][ indx.z ].id == 0 ||
			 blocks[ indx.x + 1 ][ indx.y ][ indx.z ].id == still_water )
			  not_stable++;
	if ( indx.y < chunkDim.y - 1)
		if ( blocks[ indx.x ][ indx.y + 1 ][ indx.z ].id == 0 ||
			 blocks[ indx.x ][ indx.y + 1 ][ indx.z ].id == still_water )
			  not_stable++;
	if ( indx.z < chunkDim.z - 1)
		if ( blocks[ indx.x ][ indx.y ][ indx.z + 1 ].id == 0 ||
			 blocks[ indx.x ][ indx.y ][ indx.z + 1 ].id == still_water )
			  not_stable++;

	if ( not_stable == 6 )
		return;*/

	blocks[ indx.x ][ indx.y ][ indx.z ] = block;

	Event_BlockModified( toCell );
}

/* 
=====================================
	RemoveBlock
=====================================
*/
block_t vsChunk::RemoveBlock( const vsVec3i& block ) {
	vsVec3i indx = block - blockCoord;
	block_t val = blocks[ indx.x ][ indx.y ][ indx.z ];
	const blocktype_t * blocktype = blockLibrary->GetBlockType( blocks[ indx.x ][ indx.y ][ indx.z ].id );

	// Remove block --------------------------------------------------------------------------------------
	blocks[ indx.x ][ indx.y ][ indx.z ].id = 0;
	blocks[ indx.x ][ indx.y ][ indx.z ].extra = 0;

	// spawn dropped item
	if ( rand() % 100 < blocktype->dropChance ) 
		itemManager->SpawnItem( blocktype->dropItemName, block.ToVec3f() + 0.5f );

	Event_BlockModified( block );

	return val;
}

/* 
=====================================
	Event_BlockModified
=====================================
*/
void vsChunk::Event_BlockModified( const vsVec3i& block ) {
	modified = true; 

	vsChunk* neighbor;
	if ( block.x == blockCoord.x ) {
		neighbor = game->world->GetChunkOfCell( vsVec3i( block.x - 2, block.y, block.z ) );
		if ( neighbor ) {
			neighbor->updateGeometry = true;
			neighbor->updateBuffers = true;
		}
	}

	if ( block.y == blockCoord.y ) {
		neighbor = game->world->GetChunkOfCell( vsVec3i( block.x, block.y - 2, block.z ) );
		if ( neighbor ) {
			neighbor->updateGeometry = true;
			neighbor->updateBuffers = true;
		}
	}

	if ( block.z == blockCoord.z ) {
		neighbor = game->world->GetChunkOfCell( vsVec3i( block.x, block.y, block.z - 2 ) );
		if ( neighbor ) {
			neighbor->updateGeometry = true;
			neighbor->updateBuffers = true;
		}
	}

	vsVec3i max = blockCoord + chunkDim;
	if ( block.x == max.x - 1 ) {
		neighbor = game->world->GetChunkOfCell( vsVec3i( block.x + 2, block.y, block.z ) );
		if ( neighbor ) {
			neighbor->updateGeometry = true;
			neighbor->updateBuffers = true;
		}
	}

	if ( block.y == max.y - 1 ) {
		neighbor = game->world->GetChunkOfCell( vsVec3i( block.x, block.y + 2, block.z ) );
		if ( neighbor ) {
			neighbor->updateGeometry = true;
			neighbor->updateBuffers = true;
		}
	}

	if ( block.z == max.z - 1 ) {
		neighbor = game->world->GetChunkOfCell( vsVec3i( block.x, block.y, block.z + 2 ) );
		if ( neighbor ) {
			neighbor->updateGeometry = true;
			neighbor->updateBuffers = true;
		}
	}
	
	updateGeometry = true;
	updateBuffers = true;
}

/* 
=====================================
	ChangeBlockAttributes
=====================================
*/
void vsChunk::ChangeBlockAttributes( const vsVec3i& cell, bool blockType, bool extra, const block_t& block ) {

}

/* 
=====================================
	TraceBox
=====================================
*/
bool vsChunk::TraceBox( const vsBox& box, vsVec3f& velocity ) {
	if ( !vertices.Length() )
		return false;

	struct traceBoxInfo_t {
		float	timeOfIntersection;		// time of first intersection
		vsVec3f	nearPt;					// point of first intersection
		vsBox	box;					// bounding box
		vsVec3f	modifiedVelocity;

		MINIMUM_OPERATORS( traceBoxInfo_t );
		EMPTY_TRUCTORS( traceBoxInfo_t );
	};

	vsList<traceBoxInfo_t> boxes;
	vsVec3f origin = box.GetCenter();
	vsVec3f extents = box.GetExtents();

	aabbs.Free();

	vsVec3i cell;
	int x, y, z;
	for ( x = -1; x < 2; x++ )
	for ( y = -1; y < 2; y++ )
	for ( z = -1; z < 2; z++ ) {
		cell = origin.ToVec3i() + vsVec3i( x, y, z );
		if ( CellInside( cell ) ) 
			if ( GetBlockOfCell( cell ).id != 0 /*&& 
				 GetBlockOfCell( cell ).id != blockLibrary->GetBlockTypeId( "still_water" )*/ ) {
				boxes.Expand();
				game->world->GetBoxOfCell( cell, &boxes[ boxes.Length() - 1 ].box );
				boxes[ boxes.Length() - 1 ].timeOfIntersection = ( boxes[ boxes.Length() - 1 ].box.GetCenter() - box.GetCenter() ).Length();
			}
	}
	if ( ! boxes.Length() )
		return false;
	/*
		float t, dummy;
	REPU( boxes.Length() ) {
		boxes[ i ].timeOfIntersection = 100.0f;
		REPEAT2( j, 8 ) {
			if ( boxes[ i ].box.IntersectRay( box.points[ j ], velocity, t, dummy ) )
				if ( t < boxes[ i ].timeOfIntersection && t >= 0.0f && t <= 1.0f ) {
					boxes[ i ].timeOfIntersection = t;
					boxes[ i ].nearPt = box.points[ j ] + velocity * t;
				}
		}
	}*/

	bool swapped;
	do {
		swapped = false;
		REPU( boxes.Length() - 1 )
			if ( boxes[ i ].timeOfIntersection > boxes[ i + 1 ].timeOfIntersection )  {
				vsSystem::Swap( boxes[ i ], boxes[ i + 1 ] );
				swapped = true;
			}
	} while ( swapped );

	bool collided = false;
	vsBox destBox = box + velocity;
	vsVec3f minVelocity = velocity;
	REPU( boxes.Length() ) {
		boxes[ i ].modifiedVelocity = velocity;
		if ( boxes[ i ].box.IsBoxInside( destBox ) ) 
		{
			if ( velocity.x < 0.0f && destBox.points[ 0 ].x < boxes[ i ].box.points[ 7 ].x )
				boxes[ i ].modifiedVelocity.x = boxes[ i ].box.points[ 7 ].x - box.points[ 0 ].x;

			if ( velocity.x > 0.0f && destBox.points[ 7 ].x > boxes[ i ].box.points[ 0 ].x )
				boxes[ i ].modifiedVelocity.x = boxes[ i ].box.points[ 0 ].x - box.points[ 7 ].x;

			if ( velocity.y < 0.0f && destBox.points[ 0 ].y < boxes[ i ].box.points[ 7 ].y )
				boxes[ i ].modifiedVelocity.y = boxes[ i ].box.points[ 7 ].y - box.points[ 0 ].y;

			if ( velocity.y > 0.0f && destBox.points[ 7 ].y > boxes[ i ].box.points[ 0 ].y )
				boxes[ i ].modifiedVelocity.y = boxes[ i ].box.points[ 0 ].y - box.points[ 7 ].y;

			if ( velocity.z < 0.0f && destBox.points[ 0 ].z < boxes[ i ].box.points[ 7 ].z )
				boxes[ i ].modifiedVelocity.z = boxes[ i ].box.points[ 7 ].z - box.points[ 0 ].z;

			if ( velocity.z > 0.0f && destBox.points[ 7 ].z > boxes[ i ].box.points[ 0 ].z )
				boxes[ i ].modifiedVelocity.z = boxes[ i ].box.points[ 0 ].z - box.points[ 7 ].z;

			if ( Abs( boxes[ i ].modifiedVelocity.x ) < Abs( minVelocity.x ) ) {
				minVelocity.x = boxes[ i ].modifiedVelocity.x;
				collided = true;
			}

			if ( Abs( boxes[ i ].modifiedVelocity.y ) < Abs( minVelocity.y ) ) {
				minVelocity.y = boxes[ i ].modifiedVelocity.y;
				collided = true;
			}

			if ( Abs( boxes[ i ].modifiedVelocity.z ) < Abs( minVelocity.z ) ) {
				minVelocity.z = boxes[ i ].modifiedVelocity.z;
				collided = true;
			}

			velocity = minVelocity;
			destBox = box + velocity;
		}
	}
	

	velocity = minVelocity;

	return collided;

	/*
	-------------------------------------- good solution start -----------------------------
	struct traceBoxInfo_t {
		float	timeOfIntersection;		// time of first intersection
		vsVec3f	nearPt;		// point of first intersection
		vsBox	box;		// bounding box

		MINIMUM_OPERATORS( traceBoxInfo_t );
		EMPTY_TRUCTORS( traceBoxInfo_t );
	};

	int		box_to_test;
	vsList<traceBoxInfo_t> boxes;
	vsVec3f origin = box.GetCenter();
	vsVec3f extents = box.GetExtents();

	aabbs.Free();

	vsVec3i cell;
	int x, y, z;
	for ( x = -1; x < 2; x++ )
	for ( y = -1; y < 2; y++ )
	for ( z = -1; z < 2; z++ ) {
		cell = origin.ToVec3i() + vsVec3i( x, y, z );
		if ( CellInside( cell ) ) 
			if ( GetBlockOfCell( cell ).id != 0 && 
				 GetBlockOfCell( cell ).id != blockLibrary->GetBlockTypeId( "still_water" ) ) {
				boxes.Expand();
				game->world->GetBoxOfCell( cell, &boxes[ boxes.Length() - 1 ].box );
			}
	}
	if ( ! boxes.Length() )
		return;

	int num_coll = 0;
//while ( true )  
{
	float t, dummy;
	REPU( boxes.Length() ) {
		boxes[ i ].timeOfIntersection = 100.0f;
		REPEAT2( j, 8 ) {
			if ( boxes[ i ].box.IntersectRay( box.points[ j ], velocity, t, dummy ) )
				if ( t < boxes[ i ].timeOfIntersection && t >= 0.0f && t <= 1.0f ) {
					boxes[ i ].timeOfIntersection = t;
					boxes[ i ].nearPt = box.points[ j ] + velocity * t;
				}
		}
	}

	bool swapped;
	do {
		swapped = false;
		REPU( boxes.Length() - 1 )
			if ( boxes[ i ].timeOfIntersection > boxes[ i + 1 ].timeOfIntersection )  {
				vsSystem::Swap( boxes[ i ], boxes[ i + 1 ] );
				swapped = true;
			}
	} while ( swapped );

	//vsVec3f v2 = velocity * timeOfIntersection;

	int face;
	vsVec3f v2;
	if ( boxes[ 0 ].timeOfIntersection <= 1.0f ) {
		aabbs.Append( boxes[ 0 ].box );

		face = boxes[ 0 ].box.GetFaceOfSurfacePoint( boxes[ 0 ].nearPt );
		v2 = velocity * boxes[ 0 ].timeOfIntersection * 0.98f;
		if ( face == BOX_FACE_X_NEG || face == BOX_FACE_X_POS ) velocity.x = v2.x;
		if ( face == BOX_FACE_Y_NEG || face == BOX_FACE_Y_POS ) velocity.y = v2.y;
		if ( face == BOX_FACE_Z_NEG || face == BOX_FACE_Z_POS ) velocity.z = v2.z;
		//TraceBox( box, velocity );
	//	if ( num_coll > 50 ) 
	//		break;
		//else
		//	num_coll++;
	} //else;
		//break;
}
---------------------------- good solution endof
*/
	/*
	float t;
	float timeOfIntersection = 100.0f;
	int closestBoxIndex = -1;
	int face; 
	enum { F_X=1, F_Y, F_Z };
	REPU( boxes.Length() ) {
		if ( ! ( box + velocity ).IsBoxInside( boxes[ i ] ) )
			continue;

		if ( velocity.x != 0.0f ) {
			if ( velocity.x > 0.0f )
				t = ( boxes[ i ].points[ 0 ].x - box.points[ 7 ].x ) / velocity.x;
			else
				t = ( boxes[ i ].points[ 7 ].x - box.points[ 0 ].x ) / velocity.x;
			if ( t < timeOfIntersection && t >= 0.0f && t < 1.0f ) {
				timeOfIntersection = t;
				closestBoxIndex = i;
				face = F_X;
			}
		}
		
		if ( velocity.y != 0.0f ) {
			if ( velocity.y > 0.0f )
				t = ( boxes[ i ].points[ 0 ].y - box.points[ 7 ].y ) / velocity.y;
			else
				t = ( boxes[ i ].points[ 7 ].y - box.points[ 0 ].y ) / velocity.y;
			if ( t < timeOfIntersection && t >= 0.0f && t < 1.0f ) {
				timeOfIntersection = t;
				closestBoxIndex = i;
				face = F_Y;
			}
		}

		if ( velocity.z != 0.0f ) {
			if ( velocity.z > 0.0f )
				t = ( boxes[ i ].points[ 0 ].z - box.points[ 7 ].z ) / velocity.z;
			else
				t = ( boxes[ i ].points[ 7 ].z - box.points[ 0 ].z ) / velocity.z;
			if ( t < timeOfIntersection && t >= 0.0f && t < 1.0f ) {
				timeOfIntersection = t;
				closestBoxIndex = i;
				face = F_Z;
			}
		}
	}
	
	if ( closestBoxIndex > -1 ) {
		aabbs.Free();
		aabbs.Alloc( 1 );
		aabbs[ 0 ] = boxes[ closestBoxIndex ];
		aabbs[ 0 ].SetRenderColor( vsColors::Purple );
		
		vsVec3f overlap = boxes[ closestBoxIndex ].OverlapBox( box );
		vsVec3f dest = box.GetCenter();
		if ( face == F_X ) {
			if ( velocity.x < 0.0f ) dest.x = boxes[ closestBoxIndex ].points[ 7 ].x + box.GetExtents().x;
			if ( velocity.x > 0.0f ) dest.x = boxes[ closestBoxIndex ].points[ 0 ].x - box.GetExtents().x;
			velocity.x -= overlap.x * Sign( velocity.x ); //dest.x - box.GetCenter().x;
		}

		if ( face == F_Y ) {
			if ( velocity.y < 0.0f ) dest.y = boxes[ closestBoxIndex ].points[ 7 ].y + box.GetExtents().y;
			if ( velocity.y > 0.0f ) dest.y = boxes[ closestBoxIndex ].points[ 0 ].y - box.GetExtents().y;
			velocity.y -= overlap.y * Sign( velocity.y );
		}

		if ( face == F_Z ) {
			if ( velocity.z < 0.0f ) dest.z = boxes[ closestBoxIndex ].points[ 7 ].z + box.GetExtents().z;
			if ( velocity.z > 0.0f ) dest.z = boxes[ closestBoxIndex ].points[ 0 ].z - box.GetExtents().z;
			velocity.z -= overlap.z * Sign( velocity.z );
		}
	}

	/*
	vsBox destBox;
	vsVec3f dest;
	destBox = box + velocity.x;
	REPU( boxes.Length() ) {
		
		if ( boxes[ i ].IsBoxInside( destBox ) )
		{ // collision happened
			if ( velocity.x < 0.0f ) dest.x = boxes[ i ].points[ 7 ].x + destBox.GetExtents().x;
			if ( velocity.x > 0.0f ) dest.x = boxes[ i ].points[ 0 ].x - destBox.GetExtents().x;
			velocity.x = dest.x - box.GetCenter().x;
		}
	}
	destBox = box + velocity.y;
	REPU( boxes.Length() ) {
		
		if ( boxes[ i ].IsBoxInside( destBox ) ) 
		{ // collision happened
			if ( velocity.y < 0.0f ) dest.y = boxes[ i ].points[ 7 ].y + destBox.GetExtents().y;
			if ( velocity.y > 0.0f ) dest.y = boxes[ i ].points[ 0 ].y - destBox.GetExtents().y;
			velocity.y = dest.y - box.GetCenter().y;
		}
	}
	destBox = box + velocity.z;
	REPU( boxes.Length() ) {
		
		if ( boxes[ i ].IsBoxInside( destBox ) ) 
		{ // collision happened
			if ( velocity.z < 0.0f ) dest.z = boxes[ i ].points[ 7 ].z + destBox.GetExtents().z;
			if ( velocity.z > 0.0f ) dest.z = boxes[ i ].points[ 0 ].z - destBox.GetExtents().z;
			velocity.z = dest.z - box.GetCenter().z;
		}
	}
		/*
	if ( velocity.x > 0.0f ) {
		start.x = box.points[ 0 ].x;
		end.x = box.points[ 7 ].x + velocity.x;
	} else {
		start.x = box.points[ 7 ].x;
		end.x = box.points[ 0 ].x + velocity.x;
	}

	if ( velocity.y > 0.0f ) {
		start.y = box.points[ 0 ].y;
		end.y = box.points[ 7 ].y + velocity.y;
	} else {
		start.y = box.points[ 7 ].y;
		end.y = box.points[ 0 ].y + velocity.y;
	}

	if ( velocity.z > 0.0f ) {
		start.z = box.points[ 0 ].z;
		end.z = box.points[ 7 ].z + velocity.z;
	} else {
		start.z = box.points[ 7 ].z;
		end.z = box.points[ 0 ].z + velocity.z;
	}

	if ( start.x > end.x ) vsSystem::Swap( start.x, end.x );
	if ( start.y > end.y ) vsSystem::Swap( start.y, end.y );
	if ( start.z > end.z ) vsSystem::Swap( start.z, end.z );
	*/
	/*
	if ( !vertices.Length() )
		return;

	vsVec3i cell;
	vsBox destBox = box + velocity;
	vsBox cellBox;

	vsVec3f orig1 = box.GetCenter();
	vsVec3f ext1 = box.GetExtents();
	vsVec3f orig2 = destBox.GetCenter();
	vsVec3f ext2 = destBox.GetExtents();
	float	fa = -1000.f;
	vsVec3f biggestOverlap;
	vsList<float> overlaps;
	enum { x, y, z } dir;

	aabbs.Free();

	// search for intersected boxes ----------------------------------------------------------------
	for ( int x = ( int ) destBox.points[ 0 ].x; x < ( int ) destBox.points[ 7 ].x + 1; x++ )
	for ( int y = ( int ) destBox.points[ 0 ].y; y < ( int ) destBox.points[ 7 ].y + 1; y++ )
	for ( int z = ( int ) destBox.points[ 0 ].z; z < ( int ) destBox.points[ 7 ].z + 1; z++ ) {
		cell = vsVec3i( x, y, z );
		if ( CellInside( cell ) ) 
			if ( GetBlockOfCell( cell ).id != 0 && 
				 GetBlockOfCell( cell ).id != blockLibrary->GetBlockTypeId( "still_water" ) ) {
				aabbs.Expand();
				game->world->GetBoxOfCell( cell, &aabbs[ aabbs.Length() - 1 ] );
			}
	}

	if ( !aabbs.Length() )
		return;

	overlaps.Alloc( aabbs.Length() );

	int boxIndex = -1;
	vsVec3f orig3, ext3, u;
	vsVec3f overlap;
	bool swapped;
	int face;
	vsVec3f nearPt, farPt;
	bool intersected;
	vsVec3f deltaVelocity;

	// calculate overlaps ----------------------------------------------------------------
	REPU( aabbs.Length() ) {
			//orig2 = ( box + velocity ).GetCenter();
			//orig3 = aabbs[ i ].GetCenter();
			//ext3  = aabbs[ i ].GetExtents();

			if ( aabbs[ i ].IntersectRay( orig1, velocity * 20.0f, nearPt, farPt ) ) //ext2 + ext3 - ( orig3 - orig2 ).Abs();
			{
				overlaps[ i ] = ( nearPt - orig1 ).Length();

			}
	}

	do {
		swapped = false;
		REPU( aabbs.Length()-1 ) {
			if ( overlaps[ i ] > overlaps[ i + 1 ] )  {
				swapped = true;

				vsSystem::Swap( overlaps[ i ], overlaps[ i + 1 ] );
				vsSystem::Swap( aabbs[ i ], aabbs[ i + 1 ] );
			}
		}
	} while ( swapped );

	ipoints.Free();
	
	REPEAT( aabbs.Length() ) {
		intersected = aabbs[ i ].IntersectRay( orig1, velocity * 20.0f, nearPt, farPt );
		if ( intersected ) {
			ipoints.Append( nearPt );
			ipoints.Append( farPt );

			aabbs[ i ].SetRenderColor( vsColors::Purple );

			face = aabbs[ boxIndex ].GetFaceOfSurfacePoint( nearPt );

			orig2 = ( box + velocity ).GetCenter();
			orig3 = aabbs[ i ].GetCenter();
			ext3  = aabbs[ i ].GetExtents();

			overlap = ext2 + ext3 - ( orig3 - orig2 ).Abs();

			deltaVelocity = NULLVEC3F;
			if ( face == BOX_FACE_X_NEG || face == BOX_FACE_X_POS )
				deltaVelocity.x = overlap.x * Sign( velocity.x );
			else
			if ( face == BOX_FACE_Y_NEG || face == BOX_FACE_Y_POS )
				deltaVelocity.y = overlap.y * Sign( velocity.y );
			else
			if ( face == BOX_FACE_Z_NEG || face == BOX_FACE_Z_POS )
				deltaVelocity.z = overlap.z * Sign( velocity.z );

			velocity -= deltaVelocity;
			orig1 -= deltaVelocity;
		}
	}

	// sort boxes to biggest-to-smallest overlap
	/*
	do {
		swapped = false;
		REPU( aabbs.Length()-1 ) {
			if ( overlaps[ i ] < overlaps[ i + 1 ] )  {
				swapped = true;

				vsSystem::Swap( overlaps[ i ], overlaps[ i + 1 ] );
				vsSystem::Swap( aabbs[ i ], aabbs[ i + 1 ] );
			}
		}
	} while ( swapped );

	float bok;
	vsVec3f nearI, farI;
	int face;
	REPU( aabbs.Length() ) {
		aabbs[ i ].SetRenderColor( vsColors::Purple );

		orig2 = ( box + velocity ).GetCenter();
		orig3 = aabbs[ i ].GetCenter();
		ext3  = aabbs[ i ].GetExtents();

		overlap = ext2 + ext3 - ( orig3 - orig2 ).Abs();
		biggestOverlap = overlap;
		boxIndex = i;

		aabbs[ boxIndex ].SetRenderColor( vsColors::Blue );

		if ( aabbs[ boxIndex ].IntersectRay( orig1, 20.0f * velocity, nearI, farI ) ) {
			face = aabbs[ boxIndex ].GetFaceOfSurfacePoint( nearI );

			if ( face == BOX_FACE_X_NEG || face == BOX_FACE_X_POS )
				velocity.x -= biggestOverlap.x * Sign( velocity.x );
			else
			if ( face == BOX_FACE_Y_NEG || face == BOX_FACE_Y_POS )
				velocity.y -= biggestOverlap.y * Sign( velocity.y );
			else
			if ( face == BOX_FACE_Z_NEG || face == BOX_FACE_Z_POS )
				velocity.z -= biggestOverlap.z * Sign( velocity.z );
		}
	}*/

}

/* 
=====================================
	GenerateGeometry
=====================================
*/
void vsChunk::GenerateGeometry( void ) {

	oldVertexCount = vertices.Length();
	vertices.Free();

	oldLiquidVertexCount = transparentVertices.Length();
	transparentVertices.Free();
	
	REPU( nonsolidBlocks.Length() ) {
		modelManager->DeleteMesh( nonsolidBlocks[ i ] );
	}
	nonsolidBlocks.Free();
	aabbs.Free();
	
	vsVec3i				cur, max;
	max = blockCoord + chunkDim;
	uint				numVertices = 0;
	uint				numTransparentVertices = 0;
	int					i;
	
	vertices.Alloc( chunkDim.x * chunkDim.y * chunkDim.z * 5 );
	transparentVertices.Alloc( vertices.Length() );

	for ( cur.x = blockCoord.x; cur.x < max.x; cur.x++ )
	for ( cur.y = blockCoord.y; cur.y < max.y; cur.y++ )
	for ( cur.z = blockCoord.z; cur.z < max.z; cur.z++ )
	{
		GenerateGeometryForBlock( cur, numVertices, numTransparentVertices );
	} // eof for
	
	vertices.Resize( numVertices );
	transparentVertices.Resize( numTransparentVertices );

	if ( numVertices > 0 || numTransparentVertices > 0 )
		updateBuffers = true;
	
	updateGeometry = false;
}

/* 
=====================================
	GenerateGeometryForBlock
=====================================
*/
void vsChunk::GenerateGeometryForBlock( const vsVec3i& cur, uint& numVertices, uint& numTransparentVertices ) {

	
	block_t				values[ 7 ];
	
	if ( ( values[ 0 ] = GetBlockOfCell( cur ) ).id == 0 ) 
		return;

	MD5ModelArgs_t		margs;
	bool				allEqual;
	vsVec3f				N, curf;
	chunkGridCell_t		cell;
	const blocktype_t*	block; 
	vsVec3f				blockCoordf = blockCoord.ToVec3f();
	vsVec3i				max;
	vsChunk*			neighbor;
	vsVec3i				neighborCell;
	vsVec3f				d[ 6 ];
	vsList<vsVertex>*	ptrVertices;
	uint*				ptrNumVertices;

	max = blockCoord + chunkDim;
	curf = cur.ToVec3f();
		
	values[ 1 ] = EdgeGetBlockOfCell( vsVec3i( cur.x - 1, cur.y, cur.z ) );
	values[ 2 ] = EdgeGetBlockOfCell( vsVec3i( cur.x + 1, cur.y, cur.z ) );
	values[ 3 ] = EdgeGetBlockOfCell( vsVec3i( cur.x, cur.y - 1, cur.z ) );
	values[ 4 ] = EdgeGetBlockOfCell( vsVec3i( cur.x, cur.y + 1, cur.z ) );
	values[ 5 ] = EdgeGetBlockOfCell( vsVec3i( cur.x, cur.y, cur.z - 1 ) );
	values[ 6 ] = EdgeGetBlockOfCell( vsVec3i( cur.x, cur.y, cur.z + 1 ) );
			
	allEqual = true;
	for ( int i = 1; i < 7; i++ )
		if ( values[ 0 ] != values[ i ] )
			allEqual = false;

	if ( allEqual ) 
		return;

	block = blockLibrary->GetBlockType( values[ 0 ] );

	if ( !strcmp( block->name, "cactus_inner" ) ||
		 !strcmp( block->name, "cactus_top" ) ) 
	{
		d[ 1 ] = vsVec3f( 0.00f, 0.0f, 0.0f );
		d[ 2 ] = vsVec3f( 0.00f, 0.0f, 0.0f );
		d[ 3 ] = vsVec3f( 0.0f, 0.0f,-0.00f );
		d[ 4 ] = vsVec3f( 0.0f, 0.0f, 0.00f );
	} else {
		REPEAT( 8 )
			d[ i ] = vsVec3f( 0.0f );
	}
	cell.vertices[ 0 ] = curf + d[ 0 ];
	cell.vertices[ 1 ] = curf + vsVec3f(  1.0f,  0.0f, 0.0f ) + d[ 1 ];
	cell.vertices[ 2 ] = curf + vsVec3f(  1.0f,  1.0f, 0.0f ) + d[ 2 ];
	cell.vertices[ 3 ] = curf + vsVec3f(  0.0f,  1.0f, 0.0f ) + d[ 3 ];

	cell.vertices[ 4 ] = curf + vsVec3f(  0.0f,  1.0f,  1.0f ) + d[ 4 ];
	cell.vertices[ 5 ] = curf + vsVec3f(  0.0f,  0.0f,  1.0f ) + d[ 5 ];
	cell.vertices[ 6 ] = curf + vsVec3f(  1.0f,  0.0f,  1.0f ) + d[ 6 ];
	cell.vertices[ 7 ] = curf + vsVec3f(  1.0f,  1.0f,  1.0f ) + d[ 7 ];
	
	// 5 6 0 0 6 1
	// 4 7 3 3 7 2
	// 3 2 0 0 2 1
	// 4 7 5 5 7 6
	// vertex connection dir (xy): -+ ++ --   -- ++ +-
	if ( ( block->flags & BLOCK_FLAG_NONSOLID ) == 0 ) {

		if ( block->flags & BLOCK_FLAG_TRANSPARENT ) {
			ptrVertices = &transparentVertices;
			ptrNumVertices = &numTransparentVertices;
		} else {
			ptrVertices = &vertices;
			ptrNumVertices = &numVertices;
		}

	//	if ( ( block->flags & BLOCK_FLAG_LIQUID ) == 0 ) {

			// dont count liquid around if this isn't liquid
			if ( !( blockLibrary->GetBlockType( values[ 0 ] )->flags & BLOCK_FLAG_TRANSPARENT ) )
				for ( int i = 1; i < 7; i++ )
					if ( blockLibrary->GetBlockType( values[ i ] )->flags & BLOCK_FLAG_TRANSPARENT )
						values[ i ].id = 0;

			// X negative
			if ( values[ 0 ].id && !values[ 1 ].id ) {
				N = vsVec3f( -1.0f, 0.0f, 0.0f );
				SetupTriangle( 1, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 5, 4, 0, N, block );
				SetupTriangle( 2, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 0, 4, 3, N, block );
			}

			// X positive
			if ( values[ 0 ].id && !values[ 2 ].id ) {
				N = vsVec3f( 1.0f, 0.0f, 0.0f );
				SetupTriangle( 1, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 6, 7, 1, N, block );
				SetupTriangle( 2, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 1, 7, 2, N, block );
			}

			// Y negative
			if ( values[ 0 ].id && !values[ 3 ].id ) {
				N = vsVec3f( 0.0f, -1.0f, 0.0f );
				SetupTriangle( 1, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 5, 6, 0, N, block );
				SetupTriangle( 2, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 0, 6, 1, N, block );
			}

			// Y positive
			if ( values[ 0 ].id && !values[ 4 ].id ) {
				N = vsVec3f( 0.0f, 1.0f, 0.0f );
				SetupTriangle( 1, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 4, 7, 3, N, block );
				SetupTriangle( 2, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 3, 7, 2, N, block );
			}

			// Z negative
			if (  values[ 0 ].id && !values[ 5 ].id ) {
				N = vsVec3f( 0.0f, 0.0f, -1.0f );
				SetupTriangle( 1, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 1, 2, 0, N, block );
				SetupTriangle( 2, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 0, 2, 3, N, block );
			}

			// Z positive
			if ( values[ 0 ].id && !values[ 6 ].id ) {
				N = vsVec3f( 0.0f, 0.0f, 1.0f );
				SetupTriangle( 1, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 6, 7, 5, N, block );
				SetupTriangle( 2, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 5, 7, 4, N, block );
			}
		/*} /*else {	
			// if liquid ------------------------------------------------------------------------------------
			// Y positive
			if ( values[ 0 ] && !values[ 4 ] ) {
				N = vsVec3f( 0.0f, 1.0f, 0.0f );
				SetupTriangle( 1, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 4, 7, 3, N, block );
				SetupTriangle( 2, values[ 0 ], cur, ptrVertices, ptrNumVertices, cell, 3, 7, 2, N, block );
			}
		}*/
	} else { // if non-solid
		if ( block->modelargs != NULL ) {
			nonsolidBlocks.Expand();

			margs = *block->modelargs;
			margs.offset = cell.vertices[ 0 ] + 0.5f;
			nonsolidBlocks[ nonsolidBlocks.Length()-1 ] = modelManager->LoadMesh( margs );

			if ( strlen( block->animFileName ) > 0 ) {
				nonsolidBlocks[ nonsolidBlocks.Length()-1 ]->AddAnimation( modelManager->LoadAnim( block->animFileName ) );
				nonsolidBlocks[ nonsolidBlocks.Length()-1 ]->SetAnimation( 0, ANIM_PLAY_REPEAT );
			}
		} // eof modelargs != NULL
	} // eof non-solid
}

/* 
=====================================
	EdgeGetBlockTypeIdOfCell
=====================================
*/
block_t vsChunk::EdgeGetBlockOfCell( const vsVec3i& cell ) {
	
	if ( CellInside( cell ) ) 
		return GetBlockOfCell( cell ); 
	else {
		vsChunk* neighbor = game->world->GetChunkOfCell( cell );
		if ( neighbor ) {
			return neighbor->GetBlockOfCell( cell );
		} else {
			return worldGenerator->TerrainAt( cell );
		}
	}
}

/* 
=====================================
	RemoveGeometryOfBlock
=====================================
*/
void vsChunk::RemoveGeometryOfBlock( const vsVec3i& block, uint& pos, uint& len  ) {

}

/* 
=====================================
	QueryVoxels
=====================================
*/
void vsChunk::QueryVoxels( void ) {
	REPEAT2( x, chunkDimX )
	REPEAT2( y, chunkDimY )
	REPEAT2( z, chunkDimZ ) {
		blocks[ x ][ y ][ z ] = worldGenerator->TerrainAt( blockCoord.x + x, blockCoord.y + y, blockCoord.z + z );
	}
}

/* 
=====================================
	SetupTriangle
=====================================
*/
void vsChunk::SetupTriangle( int texCoords, 
							 const block_t& block, 
							 const vsVec3i& cellCoord,
							 vsList<vsVertex>* pVert, 
							 uint* numVertices, 
							 const chunkGridCell_t& cell,
							 int a, int b, int c, 
							 const vsVec3f& Normal, 
							 const blocktype_t* blocktype ) {
	vsVertex v1, v2, v3;
	
	v1.xyz = cell.vertices[ a ];
	v2.xyz = cell.vertices[ b ];
	v3.xyz = cell.vertices[ c ];
	
	// ambient lighting
	v1.Tangent.x = v2.Tangent.x = v3.Tangent.x = ( float ) 0.9f;
	// tangent y szabad
	// tangent z szabad
	v1.Normal = v2.Normal = v3.Normal = Normal;
	if ( Normal.x != 0.0f ) {
		if ( ( Normal.x > 0.0f && block.extra == BLOCK_DIR_EAST ) ||
			 ( Normal.x < 0.0f && block.extra == BLOCK_DIR_WEST ) ) 
			v1.st = v2.st = v3.st = blocktype->tcFront.ToVec2f();
		else
			v1.st = v2.st = v3.st = blocktype->tcSide.ToVec2f();
	}
	else
	if ( Normal.y != 0.0f ) {		
		if ( Normal.y > 0.0f )
			v1.st = v2.st = v3.st = blocktype->tcTop.ToVec2f();
		else
			v1.st = v2.st = v3.st = blocktype->tcBottom.ToVec2f();
	}
	else
	if ( Normal.z != 0.0f ) {
		if ( ( Normal.z > 0.0f && block.extra == BLOCK_DIR_SOUTH ) ||
			 ( Normal.z < 0.0f && block.extra == BLOCK_DIR_NORTH ) ) 
			v1.st = v2.st = v3.st = blocktype->tcFront.ToVec2f();
		else
			v1.st = v2.st = v3.st = blocktype->tcSide.ToVec2f();
	}

	v1.Color = v2.Color = v3.Color = cellCoord.ToVec4f();
	v1.Color.w = v2.Color.w = v3.Color.w = 1.0f;	// stores life of cell in %
		
	// in_GhostTexCoords ( 3 bytes each )
	if ( texCoords == 1 ) {
		v1.reserved[ 0 ] = 1;
		v1.reserved[ 1 ] = 1;

		v2.reserved[ 0 ] = 1;
		v2.reserved[ 1 ] = 0;

		v3.reserved[ 0 ] = 0;
		v3.reserved[ 1 ] = 1;
	} else {
		v1.reserved[ 0 ] = 0;
		v1.reserved[ 1 ] = 1;

		v2.reserved[ 0 ] = 1;
		v2.reserved[ 1 ] = 0;

		v3.reserved[ 0 ] = 0;
		v3.reserved[ 1 ] = 0;
	}

	v1.reserved[ 2 ] = v2.reserved[ 2 ] = v3.reserved[ 2 ] = ( block.id >> 8 ) & 0xFF;
	v1.reserved[ 3 ] = v2.reserved[ 3 ] = v3.reserved[ 3 ] = block.id & 0xFF;


	( *pVert )[ ( *numVertices )++ ] = v1;
	( *pVert )[ ( *numVertices )++ ] = v2;
	( *pVert )[ ( *numVertices )++ ] = v3;
}

/*
===================================
	UpdateBuffers
===================================
*/
void vsChunk::UpdateBuffers( void ) {

	if ( vertices.Length() ) {
		if ( !buffer )
			glGenBuffers( 1, &buffer );
		
		glBindBuffer( GL_ARRAY_BUFFER, buffer );
		if ( oldVertexCount < vertices.Length() )
			glBufferData( GL_ARRAY_BUFFER, 
							vertices.Length()*sizeof( vsVertex ), 
							vertices.list, 
							GL_DYNAMIC_DRAW );
		else
			glBufferSubData( GL_ARRAY_BUFFER, 
							0,
							vertices.Length()*sizeof( vsVertex ),
							vertices.list );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	// LIQUID -------------------------------------------------------------
	if ( transparentVertices.Length() ) {
		if ( !transparentBuffer )
			glGenBuffers( 1, &transparentBuffer );

		glBindBuffer( GL_ARRAY_BUFFER, transparentBuffer );
		if ( oldLiquidVertexCount < transparentVertices.Length() )
			glBufferData( GL_ARRAY_BUFFER, 
							transparentVertices.Length()*sizeof( vsVertex ), 
							transparentVertices.list, 
							GL_DYNAMIC_DRAW );
		else
			glBufferSubData( GL_ARRAY_BUFFER, 
							0,
							transparentVertices.Length()*sizeof( vsVertex ),
							transparentVertices.list );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
	
	updateBuffers = false;
}

/*
===================================
	SetupShader
===================================
*/
void vsChunk::SetupShader( void ) {
	shader = renderSystem->GetShaderByName( "shader_landscape" );

	shader->BindAttrib( CVA_POSITION,		"in_Position" );
	shader->BindAttrib( CVA_NORMAL,			"in_Normal" );
	shader->BindAttrib( CVA_CELLLIFE,		"in_CellLife" );
	shader->BindAttrib( CVA_MEGATEXCOORDS,	"in_MegaTexCoords" );
	shader->BindAttrib( CVA_GHOSTTEXCOORDS,	"in_GhostTexCoords" );
	shader->BindAttrib( CVA_VALUE,			"in_Value" );

	shader->Bind( true );
	shader->Uniform1f( "in_ValWater", ( float ) blockLibrary->GetBlockTypeId( "still_water" ) );
	shader->Bind( false );
}

/*
===================================
	SetCellLife
===================================
*/
void vsChunk::SetCellLife( const vsVec3i& cell, float life ) {
	int found = -1;
	vsVec3f fcell = cell.ToVec3f();

	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	vsVertex * vert = ( vsVertex * ) glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

	REPU( vertices.Length() )
		if ( vertices[ i ].Color.x == fcell.x && 
			 vertices[ i ].Color.y == fcell.y && 
			 vertices[ i ].Color.z == fcell.z ) 
		{
			found = ( int ) i;
			vertices[ i ].Color.w = life;
			vert[ found ].Color.w = life;
		}
	glUnmapBuffer( GL_ARRAY_BUFFER );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	if ( found == -1 ) {	
		glBindBuffer( GL_ARRAY_BUFFER, transparentBuffer );
		vsVertex * vert = ( vsVertex * ) glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

		REPU( transparentVertices.Length() )
			if ( transparentVertices[ i ].Color.x == fcell.x && 
				 transparentVertices[ i ].Color.y == fcell.y && 
				 transparentVertices[ i ].Color.z == fcell.z ) 
			{
				found = ( int ) i;
				transparentVertices[ i ].Color.w = life;
				vert[ found ].Color.w = life;
			}
		glUnmapBuffer( GL_ARRAY_BUFFER );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
}

/*
===================================
	GetCellLife
===================================
*/
const float& vsChunk::GetCellLife( const vsVec3i& cell ) const {
	static float err_value = 0.0f;
	int found = -1;
	vsVec3f fcell = cell.ToVec3f();

	REPU( vertices.Length() )
		if ( vertices[ i ].Color.x == fcell.x && 
			 vertices[ i ].Color.y == fcell.y && 
			 vertices[ i ].Color.z == fcell.z ) 
		{
			return vertices[ i ].Color.w;
		}

	REPU( transparentVertices.Length() ) 
		if ( transparentVertices[ i ].Color.x == fcell.x && 
			transparentVertices[ i ].Color.y == fcell.y && 
			transparentVertices[ i ].Color.z == fcell.z ) 
		{
			return transparentVertices[ i ].Color.w;
		}

	return err_value;
}