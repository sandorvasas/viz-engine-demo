#include "Sector2.h"
#include "MC_Tables.h"
#include "..\WorldGenerator.h"
#include "..\..\System\VarStorage.h"
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
	//grass = NULL;
	geometryModified = false;
}

/*
===================================
	~vsSector2OTNode
===================================
*/
vsSector2OTNode::~vsSector2OTNode( void ) 
{
	if(nodeType == TYPE_LEAF) {
		if(vertices.Length() > 0)
		{
			vertices.Free();
			indices.Free();
			if(buffers[0] != 0)
				glDeleteBuffers(2, buffers);
		}
		//if(grass) delete grass;
	}
}

/*
===================================
	SetVoxelData
===================================
*/
void vsSector2OTNode::SetVoxelData( vsSector2* pSector, 
								    const vsVec3ui& start,
									const vsVec3ui& end )
{
	this->pSector = pSector;
	this->start = start;
	this->end = end;
}

/*
===================================
	IsLeaf
===================================
*/
bool vsSector2OTNode::IsLeaf( void )
{
	return( nodeType == TYPE_LEAF );
}

/*
===================================
	GenerateGrass
===================================

void vsSector2OTNode::GenerateGrass( void )
{
	if(!IsLeaf()) return;

	/*if(!grass) {
		pSector->Grass.parentSectorNode = this;
		grass = new vsGrass(pSector->Grass);
	}

	//grass->Generate();
}
*/
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
	frustum = NULL;
	shader  = NULL;
	numNodes = 0;
	//coord = NULLVEC3UI;
	voxelData = NULL;
//	mainVertexBuffer = 0;
	//dimensions = NULLVEC3UI;
	numVisibleNodes = 0;
	numVisibleLeafNodes = 0;
	mainSector = false;
	REPEAT(6)
		neighbors[i] = NULL;
}

/*
===================================
	vsSector2()
===================================
*/
vsSector2::vsSector2( const sector2Args_t& lsp )
{
	vsSector2();
	Generate(lsp);
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
	for(uint i=0; i<count; ++i)
		nodes[i] = NULL;
}

/*
===================================
	FreeNodes
===================================
*/
void vsSector2::FreeNodes( void )
{
	for(uint i=0; i<nodes.Length(); ++i)
		if(nodes[i] != NULL) 
			delete nodes[i];

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

	//if(mainVertexBuffer)
	//	glDeleteBuffers(1, &mainVertexBuffer);
}

/*
===================================
	Generate
===================================
*/
int vsSector2::Generate( const sector2Args_t& lsp )
{
	//Destroy();
	this->nogeometry = true;
	REPEAT(6)
		neighbors[i] = NULL;
	this->nogeometry	= !true;

	if(lsp.SECTOR.generateNew)  {
		this->gridSize		= lsp.SECTOR.gridSize;
		this->realDimensions= vsVec3ui(lsp.SECTOR.size);
		this->dimensions	= realDimensions / gridSize;
		//AllocVoxelData(dimensions.x, dimensions.y, dimensions.z);
		GenerateNew( lsp );
//	} else {
	}
		

	this->frustum		= &player->frustum;
	this->shader		= lsp.SECTOR.terrainShader;
	this->scale_1		= scale / (dimensions-1).ToVec3f();
	this->mainSector	= lsp.SECTOR.IsThisTheMainSector;
	this->nodeMinDimensions = lsp.SECTOR.nodeMinDimensions;
	this->megaTexDiff	= lsp.SECTOR.megaTexDiff;
	this->megaTexNorm	= lsp.SECTOR.megaTexNorm;

	this->bounds.FromMinMax( offset, offset+scale );
	SetModificationState( false );

	//if(!this->mainSector)
		//gridSize = 2;

/*	this->Grass.density = lsp.GRASS.density;
	this->Grass.scale	= lsp.GRASS.scale;
	this->Grass.player	= lsp.player;
	this->Grass.shader	= lsp.GRASS.shader;
	this->Grass.texture = lsp.GRASS.finTexture;
	this->Grass.wind	= lsp.GRASS.windVec;*/

	
	GenerateOctree();

	//CreateGeometryFromVoxels(&vertices, &mainVertexBuffer, true, 0, 1,
		//					  vsVec3ui(0), dimensions);

	shader->BindAttrib(S2ATTRIB_POSITION, "in_Position");
	shader->BindAttrib(S2ATTRIB_NORMAL,	  "in_Normal");
	shader->BindAttrib(S2ATTRIB_TANGENT,  "in_Tangent");
	shader->BindAttrib(S2ATTRIB_CELLSTARTPOS, "in_CellStartPos");
	shader->BindAttrib(S2ATTRIB_CELLVALUES1, "in_CellValues1");
	shader->BindAttrib(S2ATTRIB_CELLVALUES2, "in_CellValues2");

	SetupMegaTextureCoordinates();

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

	this->coord			= lsp.SECTOR.coord;
	this->terrainType	= lsp.SECTOR.terrainType;
	this->scale			= lsp.SECTOR.scale;
	this->texScale		= lsp.SECTOR.texScale;
	this->offset		= lsp.SECTOR.translation - scale*0.5f*0.f; 
	this->LOD			= lsp.SECTOR.LOD; 
		
	AllocVoxelData(dimensions.x, dimensions.y, dimensions.z);

	vsSector2* n;

	voxelOffset = NULLVEC3I;
	// FRONT NEIGHBOR
	if((n=lsp.SECTOR.neighbors[S2N_F])!=NULL)
	{
		neighbors[S2N_F] = n;
		/*REPU2(x, dimensions.x)
		REPU2(y, dimensions.y)
		{
			voxelData[x][y][0] = n->voxelData[x][y][dimensions.z-1];
		}
		*/
		offset = n->offset + vsVec3f(0.0f, 0.0f, 0.5f * (n->scale.z+scale.z)); 
		voxelOffset = n->voxelOffset + vsVec3i(0, 0, n->realDimensions.z-n->gridSize);
	}
	
	// BACK NEIGHBOR
	if((n=lsp.SECTOR.neighbors[S2N_B])!=NULL)
	{
		neighbors[S2N_B] = n;
		/*
		REPU2(x, dimensions.x)
		REPU2(y, dimensions.y)
		{
			voxelData[x][y][dimensions.z-1] = n->voxelData[x][y][0];
		}
		*/
		offset = n->offset - vsVec3f(0.0f, 0.0f, 0.5f * (n->scale.z+scale.z)); 
		voxelOffset = n->voxelOffset - vsVec3i(0, 0, n->realDimensions.z-n->gridSize);
	}

	//RIGHT NEIGHBOR
	if((n=lsp.SECTOR.neighbors[S2N_R])!=NULL)
	{
		neighbors[S2N_R] = n;
		/*
		REPU2(y, dimensions.y)
		REPU2(z, dimensions.z)
		{
			voxelData[dimensions.x-1][y][z] = n->voxelData[0][y][z];
		}
		*/
		offset = n->offset - vsVec3f(0.5f * (n->scale.x+scale.x), 0.0f, 0.0f); 
		voxelOffset = n->voxelOffset - vsVec3i(n->realDimensions.x-n->gridSize, 0, 0 );
	}

	//LEFT NEIGHBOR
	if((n=lsp.SECTOR.neighbors[S2N_L])!=NULL)
	{
		neighbors[S2N_L] = n;
		/*
		REPU2(y, dimensions.y)
		REPU2(z, dimensions.z)
		{
			voxelData[0][y][z] = n->voxelData[dimensions.x-1][y][z];
		}
		*/
		offset = n->offset + vsVec3f(0.5f * (n->scale.x+scale.x), 0.0f, 0.0f); 
		voxelOffset = n->voxelOffset + vsVec3i(n->realDimensions.x-n->gridSize, 0,0 );
	}
	
	//LOWER NEIGHBOR
	if((n=lsp.SECTOR.neighbors[S2N_LO])!=NULL)
	{
		neighbors[S2N_LO] = n;
		/*
		REPU2(x, dimensions.x)
		REPU2(z, dimensions.z)
		{
			voxelData[x][0][z] = n->voxelData[x][dimensions.y-1][z];
		}
		*/
		offset = n->offset + vsVec3f(0.0f, 0.5f * (n->scale.y+scale.y), 0.0f); 
		voxelOffset = n->voxelOffset + vsVec3i(0, n->realDimensions.y-n->gridSize, 0 );
	}

	//UPPER NEIGHBOR
	if((n=lsp.SECTOR.neighbors[S2N_UP])!=NULL)
	{
		neighbors[S2N_UP] = n;
		/*
		REPU2(x, dimensions.x)
		REPU2(z, dimensions.z)
		{
			voxelData[x][dimensions.y-1][z] = n->voxelData[x][0][z];
		}
		*/
		offset = n->offset - vsVec3f(0.0f, 0.5f * (n->scale.y+scale.y), 0.0f); 
		voxelOffset = n->voxelOffset - vsVec3i(0, n->realDimensions.y-n->gridSize, 0 );
	}
	//console->Print("voxelOffset: %s", voxelOffset.ToCharPtr());
	switch(lsp.SECTOR.terrainType) {
		case TERRAIN_TYPE_HILLS: GenerateHills();
		break;

		case 2: GenerateVoxels(1, 1);
		break;
	}


}

/*
===================================
	Render
===================================
*/
void vsSector2::Render( void )
{
	if(!shader || nogeometry) return;

	//if(!player->frustum.IntersectBox(bounds, 0.02f))
	//	return;

	numVisibleNodes = numVisibleLeafNodes = 0;
	
	
	glLineWidth(1.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if(varStorage.Bool["rendertree"])
			RenderTree(0);

	shader->Bind(true);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, megaTexDiff);

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	shader->Uniform1i("in_MegaTextureDiffuse", 0);
	shader->Uniform3v("in_CellScale", this->scale_1);

	glEnableVertexAttribArray(S2ATTRIB_POSITION);
	glEnableVertexAttribArray(S2ATTRIB_NORMAL);
	glEnableVertexAttribArray(S2ATTRIB_TANGENT);
	glEnableVertexAttribArray(S2ATTRIB_CELLSTARTPOS);
	glEnableVertexAttribArray(S2ATTRIB_CELLVALUES1);
	glEnableVertexAttribArray(S2ATTRIB_CELLVALUES2);

	RenderNode(0, player->origin != player->GetDestination());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(S2ATTRIB_POSITION);
	glDisableVertexAttribArray(S2ATTRIB_NORMAL);
	glDisableVertexAttribArray(S2ATTRIB_TANGENT);
	glDisableVertexAttribArray(S2ATTRIB_CELLSTARTPOS);
	glDisableVertexAttribArray(S2ATTRIB_CELLVALUES1);
	glDisableVertexAttribArray(S2ATTRIB_CELLVALUES2);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
		
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader->Bind(false);
}

/*
===================================
	RenderNode
===================================
*/
void vsSector2::RenderNode( uint nodeId, bool regenerate )
{
	vsSector2OTNode *n = nodes[nodeId];

	bool inside;
	
	if(frustum)
		inside = frustum->IntersectBox(nodes[nodeId]->bounds, 0.2f);
	else
		inside = true;

	n->visible = inside;

	if(!inside) return;
	numVisibleNodes++;

	if(!n->IsLeaf())
	{
		for(int i=0; i<8; ++i)
			RenderNode(n->childrenId[i], regenerate);
	} else
	//if(n->nodeType == TYPE_LEAF) 
	{
		numVisibleLeafNodes++;

		uint gs = gridSize;

		if(n->geometryModified) {
			//Node_CreateGeometry(n);
			CreateNodeGeometryFromVoxels(n, gridSize);
			n->geometryModified = false;
		}

		if(n->vertices.Length() == 0) 
			return;

		glBindBuffer(GL_ARRAY_BUFFER, n->buffers[0]);

		glVertexAttribPointer(S2ATTRIB_POSITION, 3, GL_FLOAT, false, sizeof(sector2Vertex_t), BUFFER_OFFSET(sector2Vertex_t::Offset_xyz));
		glVertexAttribPointer(S2ATTRIB_NORMAL,	 3, GL_FLOAT, false, sizeof(sector2Vertex_t), BUFFER_OFFSET(sector2Vertex_t::Offset_Normal));
		glVertexAttribPointer(S2ATTRIB_TANGENT,  3, GL_FLOAT, false, sizeof(sector2Vertex_t), BUFFER_OFFSET(sector2Vertex_t::Offset_Tangent));
		glVertexAttribPointer(S2ATTRIB_CELLSTARTPOS, 3, GL_FLOAT, false, sizeof(sector2Vertex_t), BUFFER_OFFSET(sector2Vertex_t::Offset_Color));
		glVertexAttribPointer(S2ATTRIB_CELLVALUES1, 4, GL_FLOAT, false, sizeof(sector2Vertex_t), BUFFER_OFFSET(64));
		glVertexAttribPointer(S2ATTRIB_CELLVALUES2, 4, GL_FLOAT, false, sizeof(sector2Vertex_t), BUFFER_OFFSET(80));
		glTexCoordPointer(3, GL_FLOAT, sizeof(sector2Vertex_t), BUFFER_OFFSET(sector2Vertex_t::Offset_st));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, n->buffers[1]);

		glDrawElements(GL_TRIANGLES, n->indices.Length(), GL_UNSIGNED_INT, NULL);


		//if(n->grass) n->grass->Render();


		/*
		=====================================
			Normals & Tangents
		=====================================
		*/
		if(varStorage.Bool["rendernormals"]) {
			shader->Bind(false);
			glLineWidth(4.0f);
			glBegin(GL_LINES);
			REPU(n->vertices.Length())
			{
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3fv(n->vertices[i].xyz.ToTypePtr());
				glColor3f(1.0f, 1.0f, 0.0f);
				glVertex3fv((n->vertices[i].xyz+n->vertices[i].Normal).ToTypePtr());

			//	glColor3f(0.0f, 1.0f, 0.0f);
			//	glVertex3fv(n->vertices[i].xyz.ToTypePtr());
			//	glVertex3fv((n->vertices[i].xyz+n->vertices[i].Tangent).ToTypePtr());
			}
			glEnd();
			shader->Bind(true);
		}
	
	} // type == TYPE_LEAF
			
}

/*
===================================
	RenderTree
===================================
*/
void vsSector2::RenderTree( int nodeId )
{
	vsSector2OTNode *n = nodes[nodeId];  
	
	if(!n->IsLeaf())
	{
		for(int i=0; i<8; ++i)
			RenderTree(n->childrenId[i]);
	} else
		n->bounds.RenderBox(true);
}
/*
===================================
	CalculateNormals
===================================
*/
void vsSector2::CalculateNormals(	vsList<sector2Vertex_t>* vertices, 
									vsList<uint>* indices,
									uint start, 
									uint end  )
{
#define VERTIND(k) (*vertices)[(*indices)[k]]
	vsList<vsVec3f> normals;
	vsList<vsVec3f> tangents;
	uint len = (end-start) / 3;
	uint i, j, k, l;
	normals.Alloc( len );
	tangents.Alloc( len );
	
	
	vsVec3f p[3], vec[2], pvec[2], N, T;
	vsVec2f t[3], tvec[2];

	// warning! iterates through all vertices!
	for(i = start; i<end; ++i)
	{
		VERTIND(i).Normal =
		VERTIND(i).Tangent = NULLVEC3F;
	}

	// Calculate surface N & T
	for(i = start, j = 0; i < end; i+=3, ++j)
	{ 
		VERTIND(i).CalculateTangent(VERTIND(i+1), VERTIND(i+2));

		normals[j] = VERTIND(i).Normal;
		tangents[j] = VERTIND(i).Tangent;
		VERTIND(i).Normal = VERTIND(i).Tangent = NULLVEC3F;
						 
		/*
		p[0] = (*vertices)[(*indices)[i]].xyz;
		p[1] = (*vertices)[(*indices)[i+1]].xyz;
		p[2] = (*vertices)[(*indices)[i+2]].xyz;
		vec[0] = p[0] - p[1];
		vec[1] = p[0] - p[2];
		N = -(vec[0].Cross(vec[1])).Normalize();

		pvec[0] = vec[0] - N*N.Dot(vec[0]);
		pvec[1] = vec[1] - N*N.Dot(vec[1]);

		t[0] = (*vertices)[(*indices)[i]].st;
		t[1] = (*vertices)[(*indices)[i+1]].st;
		t[2] = (*vertices)[(*indices)[i+2]].st;

		tvec[0] = t[0] - t[1];
		tvec[1] = t[0] - t[2];

		if( tvec[0].y + tvec[1].x > tvec[0].x + tvec[1].y)
		{
			tvec[0].x = -tvec[0].x;
			tvec[1].x = -tvec[1].x;
		}

		T = pvec[0] * tvec[1].x - pvec[1] * tvec[0].x;
		T.Normalize();
		normals[j] = N;
		tangents[j] = T;
		*/
				
	}
	
	// Calculate per-vertex N & T
	l=0;
	for(i = start, k; i < end; ++i)
	{
		VERTIND(i).Normal  -= normals[ l ];
		VERTIND(i).Tangent -= tangents[ l ];
		/*for(k = start; k < end; ++k)
		{
			if((*vertices)[(*indices)[i]].xyz == (*vertices)[(*indices)[k]].xyz
				&& i != k )
				
			{		
				VERTIND(k).Normal  += -normals[ l ];
				VERTIND(k).Tangent += -tangents[ l ];
				VERTIND(i).Normal  += -normals[ l ];
				VERTIND(i).Tangent += -tangents[ l ];
			}
		}*/
		if(i%3==0) l+=1;
	}

	vsVec3f tpt;
	sector2Vertex_t* v;
	for(i = start; i < end; ++i)
	{
		v = &(VERTIND(i));
		v->Normal.Normalize();
		v->Tangent.Normalize();
				
		tpt = TriPlanarTexCoords( v->xyz );

		// in_TexCoord ( MegaTexture UV Coords )
		v->st.x = tpt.x;
		v->st.y = tpt.y;
		v->reserved = tpt.z;
		
		/*v[1].st.x = tpt[1].x;
		v[1].st.y = tpt[1].y;
		v[1].reserved = tpt[1].z;

		v[2].st.x = tpt[2].x;
		v[2].st.y = tpt[2].y;
		v[2].reserved = tpt[2].z;*/
	}

	normals.Free();
	tangents.Free();

#undef VERTIND
}

/*
===================================
	RenderVoxelData
===================================
*/
void vsSector2::RenderVoxelData( void )
{
	vsVec3f p;
	glPointSize(10.0f);
	glColor3f(0.0f, 0.0f, 0.8f);
	glBegin(GL_POINTS);
		REPU2(x, dimensions.x)
		REPU2(y, dimensions.y)
		REPU2(z, dimensions.z)
			if( voxelData[x][y][z] > 0 )
			{
				p = vsVec3ui(x, y, z).ToVec3f() * scale_1 + offset;
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
	//dimensions = vsVec3ui(width, height, depth);
	FreeVoxelData();

	// Allocate memory for voxels
	voxelData = new byte**[width];
	REPU2(x, width) {
		voxelData[x] = new byte*[height];
		REPU2(y, height) {
			voxelData[x][y] = new byte[depth];
			ZeroMemory(voxelData[x][y], depth);
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
	if(voxelData) {
		REPU2(x, dimensions.x) {
			REPU2(y, dimensions.y)
				delete [] voxelData[x][y];
			delete [] voxelData[x];
		}
		delete [] voxelData;
		voxelData=NULL;
	}
}

/*
===================================
	GenerateVoxel
===================================
*/
void vsSector2::GenerateVoxels( int numHills, int randomRange )
{
	(numHills);
	(randomRange);
	srand( (uint)::time(NULL) );
	/*
	console->Print("%s [earthLevel=%d]", coord.ToCharPtr(), this->earthLevel);
	//console->Print(" earthLevel: ", this->earthLevel);

	//fixme EARTH LEVEL
	if(coord.y < 1502) {
		//REPU(dimensions.x)
		//REPU2(j, dimensions.y)
		//REPU2(k, dimensions.z)
		 //voxelData[i][j][k] = VOXEL_TYPE_SOIL;
		nogeometry = true;
		console->Print("<earthLevel");
		return;
	} else//fixme EARTH LEVEL
	if(coord.y > 1502) {
		//REPU(dimensions.x)
		//REPU2(j, dimensions.y)
		//REPU2(k, dimensions.z)
		//voxelData[i][j][k] = VOXEL_TYPE_AIR;

		nogeometry = true;
		console->Print(">earthLevel");
		return;
	} else
		console->Print("=earthLevel");

	uint nl = (int)(neighbors[S2N_L] != NULL);
	uint nf = (int)(neighbors[S2N_F] != NULL);
	uint nr = (int)(neighbors[S2N_R] != NULL);
	uint nb = (int)(neighbors[S2N_B] != NULL);
	//fixme: extend to S2N_LO and S2N_UP
	nogeometry = false;
	for(uint x=nl, z, h, y; x<dimensions.x-nr; ++x)
	{
		for(z=nf; z<dimensions.z-nb; ++z)
		{
			h = byte(float(dimensions.y/2) + float(sinf(x*0.2f)*cosf(z*0.1f)+1.0f) * float(dimensions.y/4.f))-1;
			voxelData[x][h][z] = VOXEL_TYPE_GRASS;
			for(y=0; y<h/3; y++)
			{
				voxelData[x][y][z] = VOXEL_TYPE_SOIL;
			}

			for(y=h/3; y<h/3*2; y++)
			{
				voxelData[x][y][z] = VOXEL_TYPE_SAND;
			}

			for(y=h/3*2; y<h; y++)
			{
				voxelData[x][y][z] = VOXEL_TYPE_COOLEDLAVA;
			}
		}
	}*/
	nogeometry = false;
	REPU2(x, dimensions.x)
	REPU2(y, dimensions.y)
	REPU2(z, dimensions.z)
		voxelData[x][y][z] = worldGenerator->GetVoxelAtCoords(
								voxelOffset.x+x*gridSize, 
								voxelOffset.y+y*gridSize,
								voxelOffset.z+z*gridSize );

}

/*
===================================
	GenerateHills
===================================
*/
void vsSector2::GenerateHills( void )
{
	uint h;
	//fixme EARTH LEVEL
	if(coord.y < 1502) {
		//REPU(dimensions.x)
		//REPU2(j, dimensions.y)
		//REPU2(k, dimensions.z)
		 //voxelData[i][j][k] = VOXEL_TYPE_SOIL;
		nogeometry = true;
		console->Print("<earthLevel");
		return;
	} else//fixme EARTH LEVEL
	if(coord.y > 1502) {
		//REPU(dimensions.x)
		//REPU2(j, dimensions.y)
		//REPU2(k, dimensions.z)
		//voxelData[i][j][k] = VOXEL_TYPE_AIR;

		nogeometry = true;
		console->Print(">earthLevel");
		return;
	} else
		console->Print("=earthLevel");

	bool nl = neighbors[S2N_L] == NULL;
	bool nf = neighbors[S2N_F] == NULL;
	bool nr = neighbors[S2N_R] == NULL;
	bool nb = neighbors[S2N_B] == NULL;
	//fixme: extend to S2N_LO and S2N_UP
	nogeometry = false;
	for(uint x=nl, z, h, y; x<dimensions.x-nr; ++x)
	{
		for(z=nf; z<dimensions.z-nb; ++z)
		{
			h = (byte)(dimensions.y/2);// - (rand() % 4);
			voxelData[x][h][z] = VOXEL_TYPE_GRASS;
			for(y=0; y<h/4; y++)
			{
				voxelData[x][y][z] = VOXEL_TYPE_COOLEDLAVA;
			}

			for(y=h/4; y<h/4*2; y++)
			{
				voxelData[x][y][z] = VOXEL_TYPE_STONE;
			}

			for(y=h/4*2; y<h/4*3; y++)
			{
				voxelData[x][y][z] = VOXEL_TYPE_SAND;
			}

			for(y=h/4*3; y<h; y++)
			{
				voxelData[x][y][z] = VOXEL_TYPE_SOIL;
			}
		}
	}

}

/*
===================================
	CreateGeometryFromVoxels
		"Marching Cubes" algorithm main function
		Returns number of vertices
===================================
*/
uint vsSector2::Node_CreateGeometry(  vsSector2OTNode* n )
{
	n->vertices.Free();
	n->indices.Free();
	
	vsVec3ui			gridStep = vsVec3ui(1);
	vsVec3f				curf, nextf;
	vsVec3ui			cur, next;
	sector2GridCell_t	cell;
	uint				numVertices = 0;

	vsVec3ui nv = (n->end-n->start);// / _gridSize;
	vsList<sector2Vertex_t> tmpVertices;
	tmpVertices.Alloc(nv.x * nv.y * nv.z * 5);

	vsVec3ui end = n->end;
	//end.Clamp(NULLVEC3UI, dimensions);

	for(cur.x=n->start.x; cur.x<end.x; cur.x+=gridStep.x)
	for(cur.y=n->start.y; cur.y<end.y; cur.y+=gridStep.y)
	for(cur.z=n->start.z; cur.z<end.z; cur.z+=gridStep.z)
	{
		next = cur + gridStep;
		next.Clamp(NULLVEC3UI, dimensions-1);
		curf = cur.ToVec3f();
		

		nextf = next.ToVec3f();

		cell.values[0] = voxelData[cur.x][next.y][next.z];
		cell.values[1] = voxelData[next.x][next.y][next.z];
		cell.values[2] = voxelData[next.x][next.y][cur.z];
		cell.values[3] = voxelData[cur.x][next.y][cur.z];
		cell.values[4] = voxelData[cur.x][cur.y][next.z];
		cell.values[5] = voxelData[next.x][cur.y][next.z];
		cell.values[6] = voxelData[next.x][cur.y][cur.z];
		cell.values[7] = voxelData[cur.x][cur.y][cur.z];

		cell.vertices[0] = vsVec3f(curf.x, nextf.y, nextf.z);
		cell.vertices[1] = nextf;
		cell.vertices[2] = vsVec3f(nextf.x, nextf.y, curf.z);
		cell.vertices[3] = vsVec3f(curf.x, nextf.y, curf.z);
		cell.vertices[4] = vsVec3f(curf.x, curf.y, nextf.z);
		cell.vertices[5] = vsVec3f(nextf.x, curf.y, nextf.z);
		cell.vertices[6] = vsVec3f(nextf.x, curf.y, curf.z);
		cell.vertices[7] = curf;

		//REPEAT2(xi, 8)
		//	cell.vertices[xi] *= float(_gridSize);
		cell.coords = cur;
		PolygoniseCell( &tmpVertices, &numVertices, cell );
	}

	if(numVertices > 0) 
	{
		nogeometry = false;
		n->vertices.Alloc(numVertices);
		n->indices.Alloc(numVertices);
		
		// Remove duplicated vertices
		bool found;
		uint realNumVertices=0;
		for(uint i=0,j; i<numVertices; ++i)
		{
			//console->Print("v%d c:%s", i, tmpVertices[i].Color.ToCharPtr());
			found=false;
			//for(j=realNumVertices-1; j>0; --j) -- maybe faster
			for(j=0; j<realNumVertices; ++j)
			{
				if(n->vertices[j] == tmpVertices[i]) {
					n->indices[i] = j;
					found=true;
					break;
				}
			}

			if(!found) {
				n->vertices[realNumVertices] = tmpVertices[i];
				n->indices[i] = realNumVertices;

				//if(realNumVertices<20)
					//console->Print(tmpVertices[i].Color.ToCharPtr());
				++realNumVertices;
			}
		}

		numVertices = realNumVertices;
		n->vertices.Resize(numVertices);
		
		CalculateNormals(&n->vertices, &n->indices, 0, n->indices.Length());
	
		if(n->buffers[0] == 0) {
			glGenBuffers(2, n->buffers);
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, n->buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, 
						n->vertices.Length()*sizeof(sector2Vertex_t), 
						n->vertices.list, 
						GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, n->buffers[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
						n->indices.Length()*sizeof(uint), 
						n->indices.list, 
						GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		
	}

	tmpVertices.Free();

	return numVertices;
}


/*
===================================
	CreateGeometryFromVoxels
		"Marching Cubes" algorithm main function
		Returns number of vertices
===================================
*/
uint vsSector2::CreateGeometryFromVoxels( 
					vsList<sector2Vertex_t>* vertices,
					vsList<uint>* indices,
					uint* buffers,
					const vsVec3ui& voxelStart, 
					const vsVec3ui& voxelEnd,
					uint _gridSize )
{
	vertices->Free();
	indices->Free();


	vsVec3ui			gridStep = vsVec3ui(1);
	vsVec3f				curf, nextf;
	vsVec3ui			cur, next;
	sector2GridCell_t	cell;
	uint				numVertices = 0;

	vsVec3ui nv = (voxelEnd-voxelStart);// / _gridSize;
	vsList<sector2Vertex_t> tmpVertices;
	tmpVertices.Alloc(nv.x * nv.y * nv.z * 5);

	vsSector2* n;
	vsVec3ui end = voxelEnd;
	//end.Clamp(NULLVEC3UI, dimensions);

	for(cur.x=voxelStart.x; cur.x<end.x; cur.x+=gridStep.x)
	for(cur.y=voxelStart.y; cur.y<end.y; cur.y+=gridStep.y)
	for(cur.z=voxelStart.z; cur.z<end.z; cur.z+=gridStep.z)
	{
		next = cur + gridStep;
		next.Clamp(NULLVEC3UI, dimensions-1);
		curf = cur.ToVec3f();

		//console->Print("clamped");
		cell.values[0] = voxelData[cur.x][next.y][next.z];
		cell.values[1] = voxelData[next.x][next.y][next.z];
		cell.values[2] = voxelData[next.x][next.y][cur.z];
		cell.values[3] = voxelData[cur.x][next.y][cur.z];
		cell.values[4] = voxelData[cur.x][cur.y][next.z];
		cell.values[5] = voxelData[next.x][cur.y][next.z];
		cell.values[6] = voxelData[next.x][cur.y][cur.z];
		cell.values[7] = voxelData[cur.x][cur.y][cur.z];

		nextf = next.ToVec3f();
		cell.vertices[0] = vsVec3f(curf.x, nextf.y, nextf.z);
		cell.vertices[1] = nextf;
		cell.vertices[2] = vsVec3f(nextf.x, nextf.y, curf.z);
		cell.vertices[3] = vsVec3f(curf.x, nextf.y, curf.z);
		cell.vertices[4] = vsVec3f(curf.x, curf.y, nextf.z);
		cell.vertices[5] = vsVec3f(nextf.x, curf.y, nextf.z);
		cell.vertices[6] = vsVec3f(nextf.x, curf.y, curf.z);
		cell.vertices[7] = curf;

		//REPEAT2(xi, 8)
		//	cell.vertices[xi] *= float(_gridSize);
		cell.coords = cur;
		PolygoniseCell( &tmpVertices, &numVertices, cell );
	}
	/*
	vsSector2* n;
	if(voxelStart.x==0 && neighbors[S2N_L]) {
		console->Print("creating startX=0");
		n = neighbors[S2N_L];
		if(n->voxelData != NULL)
		{
			next.x = voxelStart.x;
			cur.x = -1;
			for(cur.y=voxelStart.y; cur.y<voxelEnd.y; cur.y+=gridStep.y)
			for(cur.z=voxelStart.z; cur.z<voxelEnd.z; cur.z+=gridStep.z)
			{
				next.y = cur.y + gridStep.y;
				next.z = cur.z + gridStep.z;
				next.Clamp(NULLVEC3UI, dimensions-1);

				curf = cur.ToVec3f();
				nextf = curf + gridStep.ToVec3f();
		
				cell.values[0] = n->voxelData[n->dimensions.x-1][next.y][next.z];
				cell.vertices[0] = vsVec3f(curf.x, nextf.y, nextf.z);

				cell.values[1] = voxelData[next.x][next.y][next.z];
				cell.vertices[1] = nextf;

				cell.values[2] = voxelData[next.x][next.y][cur.z];
				cell.vertices[2] = vsVec3f(nextf.x, nextf.y, curf.z);

				cell.values[3] = n->voxelData[n->dimensions.x-1][next.y][cur.z];
				cell.vertices[3] = vsVec3f(curf.x, nextf.y, curf.z);

				cell.values[4] = n->voxelData[n->dimensions.x-1][cur.y][next.z];
				cell.vertices[4] = vsVec3f(curf.x, curf.y, nextf.z);

				cell.values[5] = voxelData[next.x][cur.y][next.z];
				cell.vertices[5] = vsVec3f(nextf.x, curf.y, nextf.z);

				cell.values[6] = voxelData[next.x][cur.y][cur.z];
				cell.vertices[6] = vsVec3f(nextf.x, curf.y, curf.z);

				cell.values[7] = n->voxelData[n->dimensions.x-1][cur.y][cur.z];
				cell.vertices[7] = curf;

				//REPEAT2(xi, 8)
				//	cell.vertices[xi] *= float(_gridSize);
				cell.coords = cur;
				PolygoniseCell( &tmpVertices, &numVertices, cell );
			}
		}
	}*/

	if(numVertices > 0) 
	{
		nogeometry = false;
		vertices->Alloc(numVertices);
		indices->Alloc(numVertices);
		
		// Remove duplicated vertices
		bool found;
		uint realNumVertices=0;
		for(uint i=0,j; i<numVertices; ++i)
		{
			//console->Print("v%d c:%s", i, tmpVertices[i].Color.ToCharPtr());
			found=false;
			//for(j=realNumVertices-1; j>0; --j) -- maybe faster
			for(j=0; j<realNumVertices; ++j)
			{
				if((*vertices)[j] == tmpVertices[i]) {
					(*indices)[i] = j;
					found=true;
					break;
				}
			}

			if(!found) {
				(*vertices)[realNumVertices] = tmpVertices[i];
				(*indices)[i] = realNumVertices;

				//if(realNumVertices<20)
					//console->Print(tmpVertices[i].Color.ToCharPtr());
				++realNumVertices;
			}
		}

		numVertices = realNumVertices;
		vertices->Resize(numVertices);
		
		CalculateNormals(vertices, indices, 0, indices->Length());
	
		if(buffers[0] == 0) {
			glGenBuffers(2, buffers);
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, 
						vertices->Length()*sizeof(sector2Vertex_t), 
						vertices->list, 
						GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
						indices->Length()*sizeof(uint), 
						indices->list, 
						GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		
	}

	tmpVertices.Free();

	return numVertices;
}

/*
===================================
	CreateNodeGeometryFromVoxels
===================================
*/
void vsSector2::CreateNodeGeometryFromVoxels( vsSector2OTNode* node, uint _gridSize )
{
	CreateGeometryFromVoxels( &node->vertices,
							  &node->indices,
							  node->buffers,
							  node->start, 
							  node->end,
							  _gridSize);
	//if(mainSector) node->GenerateGrass();
}

/*
===================================
	PolygoniseCell
		"Marching Cubes" function for generating
		geometry in a cell
===================================
*/
void vsSector2::PolygoniseCell( vsList<sector2Vertex_t>* vertices, uint* numVertices, const sector2GridCell_t& cell )
{
	int cubeIndex = 0;
	int k; 
	byte j;

	for(j=0, k=1; j<8; ++j, k*=2) 
		if(cell.values[j] < VOXEL_TYPE_ISOLEVEL) cubeIndex |= k;

	if(edgeTable[cubeIndex] == 0xFF || edgeTable[cubeIndex] == 0)
		return;

	vsVec3f vert[12];


	for(j=0, k=1; j<12; ++j, k*=2)
		if(edgeTable[cubeIndex] & k)
			vert[j] = InterpolateMCVertex(cell, j);

	sector2Vertex_t v;

	for(int j=0; j<triVertLenTable[cubeIndex]; ++j)
	{

		v.xyz = vert[triTable[cubeIndex][j]] * scale_1 + offset;

		v.Color = (cell.vertices[7] * scale_1 + offset).ToVec4f();
		for(int gh=0; gh<8; ++gh)
			v.cellValues[gh] = EncodeMegaTextureCoords(cell.values[gh]);
		

		(*vertices)[(*numVertices)++] = v;

	}
}

/*
===================================
	InterpolateMCVertex
		"Marching Cubes" function for interpolating
		vetices inside a cell
===================================
*/
vsVec3f vsSector2::InterpolateMCVertex( const sector2GridCell_t& cell, byte edgeNum )
{
	float v1 = (float)cell.values[ edgeVertices[edgeNum][0] ]; 
	float v2 = (float)cell.values[ edgeVertices[edgeNum][1] ];
	vsVec3f p1 = cell.vertices[ edgeVertices[edgeNum][0] ];
	vsVec3f p2 = cell.vertices[ edgeVertices[edgeNum][1] ];

	return p1 + (float(VOXEL_TYPE_ISOLEVEL) - v1) * ((p2-p1)/(v2-v1));
}

/*
===================================
	GenerateOctree
===================================
*/
vsVec3f vsSector2::TriPlanarTexCoords( const vsVec3f& point )
{
	/*vsVec3f OffsetRelativePoint = point - offset;

	vsVec3f tc = (OffsetRelativePoint / scale);
	tc.y = 1.0f-tc.y;*/
	return point;//tc * textureSize;
}

/*
===================================
	EncodeMegaTextureCoords
===================================
*/
float vsSector2::EncodeMegaTextureCoords( int indexToTextureMap )
{
	if(indexToTextureMap == VOXEL_TYPE_AIR)
		return -1.0f;

	vsVec2f uv = vsSector2Texture::MegaTextureCoordinates[indexToTextureMap];

	return uv.y * float(numTexturesPerRow) + uv.x;
}

/*
===================================
	GenerateOctree
===================================
*/
void vsSector2::GenerateOctree( void )
{

	AllocNodes( PrecalculateOctreeNodeCount(0, 0, 0, dimensions.x, dimensions.y, dimensions.z));
	//console->Print("Precalc nodeCount: %d", nodes.Length());

	GenerateOctreeNode(0, 0, 0, dimensions.x, dimensions.y, dimensions.z,-1);

	//console->Print("numNodes: %d | nodes.Length: %d", numNodes, nodes.Length());
}

/*
===================================
	PrecalculateOctreeNodeCount
		Helper function for octree generation
===================================
*/
uint vsSector2::PrecalculateOctreeNodeCount( uint sX, uint sY, uint sZ, uint eX, uint eY, uint eZ )
{
	vsVec3ui start = vsVec3ui(sX, sY, sZ);
	vsVec3ui end = vsVec3ui(eX, eY, eZ);

	vsVec3ui dim = end-start;
	int depth = 0;
	while( dim > nodeMinDimensions )
	{
		depth++;
		dim /= 2;
	}

	uint num=0;
	for(float i=0; i<=(float)depth; i++)
		num += (uint)powf(8.0f, i);	

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
	if(parentId==-1) numNodes = 0;

	vsSector2OTNode* node = new vsSector2OTNode();

	vsVec3ui start = vsVec3ui(sX, sY, sZ);
	vsVec3ui end   = vsVec3ui(eX, eY, eZ);
	vsVec3ui dim   = end  - start;

	node->parentId	 = parentId;
	node->Id		 = numNodes++;
	//node->start = vs
	//node->end		 = end;
	
	//------------------------------
	//	Bounding Box
	//------------------------------
	vsVec3f min = offset + start.ToVec3f() * scale_1;
	vsVec3f max = offset + end.ToVec3f() * scale_1;
	node->bounds.FromMinMax(min, max);

	if( dim <= nodeMinDimensions)
	{
		node->nodeType = TYPE_LEAF;

		vsVec3ui end1 = end+1;
		end1.Clamp(NULLVEC3UI, dimensions-1);

		node->SetVoxelData(this, start, end1);
		//CreateNodeGeometryFromVoxels(node, gridSize);
		this->nodes.list[numNodes-1] = node;

		Node_CreateGeometry(node);
		//console->Print("LEAF: start: %s dim: %s c: %d", start.ToCharPtr(), node->dimensions.ToCharPtr(), node->vertices.Length());
	} else
	{
		node->nodeType = TYPE_INTERNAL;	
		
		this->nodes.list[numNodes-1] = node;

		vsVec3ui C = (start + end) / 2;

		node->childrenId[0]=GenerateOctreeNode(	start.x,	start.y,start.z,C.x,	C.y,	C.z,	node->Id);
		node->childrenId[1]=GenerateOctreeNode(	C.x,		start.y,start.z,end.x,	C.y,	C.z,	node->Id);
		node->childrenId[2]=GenerateOctreeNode(	C.x,		C.y,	start.z,end.x,	end.y,	C.z,	node->Id);
		node->childrenId[3]=GenerateOctreeNode(	start.x,	C.y,	start.z,C.x,	end.y,	C.z,	node->Id);
		node->childrenId[4]=GenerateOctreeNode(	start.x,	start.y,C.z,	C.x,	C.y,	end.z,	node->Id);
		node->childrenId[5]=GenerateOctreeNode(	C.x,		start.y,C.z,	end.x,	C.y,	end.z,	node->Id);
		node->childrenId[6]=GenerateOctreeNode(	C.x,		C.y,	C.z,	end.x,	end.y,	end.z,	node->Id);
		node->childrenId[7]=GenerateOctreeNode(	start.x,	C.y,	C.z,	C.x,	end.y,	end.z,	node->Id);
	}
	
	return( node->Id );
}

/*
===================================
	PutBlock
		Return: 1 if block has been put
			    0 if not.
===================================
*/
int vsSector2::PutBlock( const vsVec3ui& xyz, VOXELDATATYPE blockType )
{
	if(VoxelDataAt(xyz) == VOXEL_TYPE_AIR)
	{
		//console->Print("PutBlock");
		VoxelDataAt(xyz) = blockType;
		SetModificationState( true );
		UpdateModifiedGeometry( xyz );

		vsVec3ui realxyz = xyz * gridSize;
		vsSector2* n = neighbors[S2N_L];
		if(xyz.x == 0 && n)
		{
			n->NeighborPutBlock( vsVec3ui(n->realDimensions.x-1,
										  realxyz.y, 
										  realxyz.z), 
								 blockType );
		}

		n = neighbors[S2N_R];
		if(xyz.x == dimensions.x-1 && n)
		{
			n->NeighborPutBlock( vsVec3ui(0,
										  realxyz.y, 
										  realxyz.z), 
								 blockType );
		}

		n = neighbors[S2N_F];
		if(xyz.z == 0 && n)
		{
			n->NeighborPutBlock( vsVec3ui(realxyz.x, 
										  realxyz.y,
										  n->realDimensions.z-1), 
								blockType );
		}

		n = neighbors[S2N_B];
		if(xyz.z == dimensions.z-1 && n)
		{
			n->NeighborPutBlock( vsVec3ui(realxyz.x, 
										  realxyz.y,
										  0), 
								blockType );
		}

		n = neighbors[S2N_LO];
		if(xyz.y == 0 && n)
		{
			n->NeighborPutBlock( vsVec3ui(realxyz.x, 
										  n->realDimensions.y-1, 
										  realxyz.z), 
								blockType );
		}

		n = neighbors[S2N_UP];
		if(xyz.y == dimensions.y-1 && n)
		{
			n->NeighborPutBlock( vsVec3ui(realxyz.x, 
										  0, 
										  realxyz.z), 
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
int vsSector2::RemoveBlock( const vsVec3ui& xyz )
{
	if(VoxelDataAt(xyz) != VOXEL_TYPE_AIR)
	{
		//console->Print("RemoveBlock");
		SetModificationState( true );
		VoxelDataAt(xyz) = VOXEL_TYPE_AIR;
		UpdateModifiedGeometry( xyz );
				
		vsVec3ui realxyz = xyz * gridSize;
		vsSector2* n = neighbors[S2N_L];
		if(xyz.x == 0 && n)
		{
			n->NeighborRemoveBlock( vsVec3ui(n->realDimensions.x-1,
										  realxyz.y, 
										  realxyz.z));
		}

		n = neighbors[S2N_R];
		if(xyz.x == dimensions.x-1 && n)
		{
			n->NeighborRemoveBlock( vsVec3ui(0,
										  realxyz.y, 
										  realxyz.z));
		}

		n = neighbors[S2N_F];
		if(xyz.z == 0 && n)
		{
			n->NeighborRemoveBlock( vsVec3ui(realxyz.x, 
										  realxyz.y,
										  n->realDimensions.z-1));
		}

		n = neighbors[S2N_B];
		if(xyz.z == dimensions.z-1 && n)
		{
			n->NeighborRemoveBlock( vsVec3ui(realxyz.x, 
										  realxyz.y,
										  0));
		}

		n = neighbors[S2N_LO];
		if(xyz.y == 0 && n)
		{
			n->NeighborRemoveBlock( vsVec3ui(realxyz.x, 
										  n->realDimensions.y-1, 
										  realxyz.z));
		}

		n = neighbors[S2N_UP];
		if(xyz.y == dimensions.y-1 && n)
		{
			n->NeighborRemoveBlock( vsVec3ui(realxyz.x, 
										  0, 
										  realxyz.z));
		}
		return 1;
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
int vsSector2::PutMultipleBlocks( vsVec3i coords[], int len, VOXELDATATYPE blockType )
{
	for(int i=0; i<len; ++i)
	{
		if(IsValidVoxelIndex(coords[i])) 
			PutBlock(coords[i].ToVec3ui(), blockType);
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
	for(int i=0; i<len; ++i)
	{
		if(IsValidVoxelIndex(coords[i])) 
			RemoveBlock(coords[i].ToVec3ui());
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
void vsSector2::NeighborPutBlock( const vsVec3ui& xyz, VOXELDATATYPE blockType )
{
	console->Print("neighborPutBlock");
	vsVec3f fxyz = xyz.ToVec3f();
	vsVec3f transformedXYZ = fxyz / (float)gridSize;

	// xyz is not visible at our precision
	if( fxyz != transformedXYZ.Floor() )
		return;

	vsVec3ui ijk = transformedXYZ.ToVec3ui();
	if(VoxelDataAt(ijk) == VOXEL_TYPE_AIR)
	{
		VoxelDataAt(ijk) = blockType;
		SetModificationState( true );
		UpdateModifiedGeometry(ijk);
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
	console->Print("neighborRemoveBlock");
	vsVec3f fxyz = xyz.ToVec3f();
	vsVec3f transformedXYZ = fxyz / (float)gridSize;

	// xyz is not visible at our precision
	if( fxyz != transformedXYZ.Floor() )
		return;

	vsVec3ui ijk = transformedXYZ.ToVec3ui();
	if(VoxelDataAt(ijk) != VOXEL_TYPE_AIR)
	{
		SetModificationState( true );
		VoxelDataAt(ijk) = VOXEL_TYPE_AIR;
		UpdateModifiedGeometry(ijk);
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
	
	n = GetLeafNodeOfCell(p, side, 1);
	if(n == NULL) return -1;
	if(n->geometryModified) return -1;

	n->geometryModified = true;
	
	for(int i=-1; i<2; ++i)
	for(int j=-1; j<2; ++j)
	for(int k=-1; k<2; ++k)
		UpdateModifiedGeometry( p + vsVec3ui( i*2, j*2, k*2 ));
		

/*	if(side &  1 ) UpdateModifiedGeometry(vsVec3ui( p.x-1, p.y, p.z ));
	if(side &  2 ) UpdateModifiedGeometry(vsVec3ui( p.x, p.y-1, p.z ));
	if(side &  4 ) UpdateModifiedGeometry(vsVec3ui( p.x, p.y, p.z-1 ));
	if(side &  8 ) UpdateModifiedGeometry(vsVec3ui( p.x+1, p.y, p.z ));
	if(side & 16 ) UpdateModifiedGeometry(vsVec3ui( p.x, p.y+1, p.z ));
	if(side & 32 ) UpdateModifiedGeometry(vsVec3ui( p.x, p.y, p.z+1 ));
	*/

/*	
	if(side & 1) { // -2 X X
		LocateCellAndCreateGeometry(p.x-2, p.y, p.z);

		if(side & 2) { // -2 -2 X
			LocateCellAndCreateGeometry(p.x-2, p.y-2, p.z);

			if(side & 4) LocateCellAndCreateGeometry(p.x-2, p.y-2, p.z-2);
			else 
			if(side & 32)LocateCellAndCreateGeometry(p.x-2, p.y-2, p.z+2);
		} //else
		if(side & 16) { // -2 +2 X
			LocateCellAndCreateGeometry(p.x-2, p.y+2, p.z);

			if(side & 4) LocateCellAndCreateGeometry(p.x-2, p.y+2, p.z-2);
			else 
			if(side & 32)LocateCellAndCreateGeometry(p.x-2, p.y+2, p.z+2);
		} //else // -2 0 X
		{ 
			if(side & 4) LocateCellAndCreateGeometry(p.x-2, p.y, p.z-2);
			else
			if(side & 32) LocateCellAndCreateGeometry(p.x-2, p.y, p.z+2);			
		}	
	}

	if(side & 8) { // +2 X X
		LocateCellAndCreateGeometry(p.x+2, p.y, p.z);

		if(side & 2) { // +2 -2 X
			LocateCellAndCreateGeometry(p.x+2, p.y-2, p.z);

			if(side & 4) LocateCellAndCreateGeometry(p.x+2, p.y-2, p.z-2);
			else 
			if(side & 32)LocateCellAndCreateGeometry(p.x+2, p.y-2, p.z+2);
		} //else
		if(side & 16) { // +2 +2 X
			LocateCellAndCreateGeometry(p.x+2, p.y+2, p.z);

			if(side & 4) LocateCellAndCreateGeometry(p.x+2, p.y+2, p.z-2);
			else 
			if(side & 32)LocateCellAndCreateGeometry(p.x+2, p.y+2, p.z+2);
		} //else // +2 0 X
		{ 
			if(side & 4) LocateCellAndCreateGeometry(p.x+2, p.y, p.z-2);
			else
			if(side & 32) LocateCellAndCreateGeometry(p.x+2, p.y, p.z+2);			
		}	
	}

	//if(side & 8) { // 0 X X
		//LocateCellAndCreateGeometry(p.x, p.y, p.z);

		if(side & 2) { // 0 -2 X
			LocateCellAndCreateGeometry(p.x, p.y-2, p.z);

			if(side & 4) LocateCellAndCreateGeometry(p.x, p.y-2, p.z-2);
			else 
			if(side & 32)LocateCellAndCreateGeometry(p.x, p.y-2, p.z+2);
		} //else
		if(side & 16) { // 0 +2 X
			LocateCellAndCreateGeometry(p.x, p.y+2, p.z);

			if(side & 4) LocateCellAndCreateGeometry(p.x, p.y+2, p.z-2);
			else 
			if(side & 32)LocateCellAndCreateGeometry(p.x, p.y+2, p.z+2);
		} //else // 0 0 X
		{ 
			if(side & 4) LocateCellAndCreateGeometry(p.x, p.y, p.z-2);
			else
			if(side & 32) LocateCellAndCreateGeometry(p.x, p.y, p.z+2);			
		}	
	//}
	/*
	if(side & 2) {
		LocateCellAndCreateGeometry(p.x, p.y-1, p.z);
	}
	if(side & 4) {
		LocateCellAndCreateGeometry(p.x-1, p.y, p.z);
		if(side & 8) {
			LocateCellAndCreateGeometry(p.x+1, p.y, p.z-1);
		}
	}
	if(side & 8) {
		LocateCellAndCreateGeometry(p.x+1, p.y, p.z);
		if(side & 32) LocateCellAndCreateGeometry(p.x+1, p.y, p.z+1);
	}
	if(side & 16) {
		LocateCellAndCreateGeometry(p.x, p.y+1, p.z);
	}
	if(side & 32) {
		LocateCellAndCreateGeometry(p.x, p.y, p.z+1);
	}*/

	return -1;
}
/*
===================================
	LocateCellAndCreateGeometry
		Helper function to shorten parts of UpdateModifiedGeometry()
===================================
*/
void vsSector2::LocateCellAndCreateGeometry( uint x, uint y, uint z )
{
	int s;
	vsSector2OTNode* n = GetLeafNodeOfCell( vsVec3ui(x,y,z), s, 1);
	if(n)
		n->geometryModified = true;
}

/*
===================================
	VoxelDataAt
===================================
*/
VOXELDATATYPE& vsSector2::VoxelDataAt( const vsVec3ui& xyz )
{
#define VoxelDataAt_NoCheck
#ifndef VoxelDataAt_NoCheck
	static VOXELDATATYPE err = VOXEL_TYPE_INVALID_INDEX;

	if(xyz<dimensions && xyz>=vsVec3ui(0))
		return voxelData[xyz.x][xyz.y][xyz.z];
	else
		return err;
#else
	return voxelData[xyz.x][xyz.y][xyz.z];
#endif
}

/*
===================================
	GetCellOfPoint
		Returns the VOXEL coordinates of a point in space
===================================
*/
vsVec3i vsSector2::GetCellOfPoint( const vsVec3f& p )
{
	vsVec3ui c = ((p - offset).Abs() / scale_1).ToVec3ui().Abs();
	
	if( c >= NULLVEC3UI && c < dimensions )
	{
		return c.ToVec3i();
	}

	return vsVec3i(-1);	
}

/*
===================================
	GetLeafNodeOfCell
		Returns the NODE of a given VOXEL coordinate 
		in which it is located
===================================
*/
vsSector2OTNode* vsSector2::GetLeafNodeOfCell( const vsVec3ui& cell, int& side, int resultNum )
{
	if(cell<dimensions)
	{
		int k=0;
		for(uint i=0; i<nodes.Length(); ++i)
			if(nodes[i]->IsLeaf()) {
				if( nodes[i]->start <= cell &&
					nodes[i]->end >= cell ) {
					if(resultNum == ++k) {
					
						/*side = 0;
						if(nodes[i]->start.x == cell.x) side |= 1;
						if(nodes[i]->start.y == cell.y) side |= 2;
						if(nodes[i]->start.z == cell.z) side |= 4;
						if(nodes[i]->end.x == cell.x)	side |= 8;
						if(nodes[i]->end.y == cell.y)	side |= 16;
						if(nodes[i]->end.z == cell.z)	side |= 32;
						*/
					
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
		Helper function for LocatePlayer() & GetNodeOfPoint
===================================
*/
int vsSector2::GetPointsNode( int nodeId, const vsVec3f& point )
{
	if(nodeId<0 || nodes.list == NULL) 
		return -1;

	int i,j;

	if(nodes[nodeId]->bounds.IsPointInside(point))
	{
		if(nodes[nodeId]->IsLeaf())
			return nodeId;
		else 
		{
			i=-1; j=0;
			while(j<8)
			{
				i = GetPointsNode(nodes[nodeId]->childrenId[j++], point);

				if(i!=-1)
					return i;
			}			
		}
	}

	return -1;
}

/*
===================================
	LocatePlayer
		Returns the NODE in which the PLAYER is in
===================================
*/
vsSector2OTNode* vsSector2::LocatePlayer( void )
{
	int i = GetPointsNode(0, player->GetDestination());
	if( i == -1 )
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
	int i = GetPointsNode(0, point);
	if( i == -1 )
		return NULL;

	return nodes[ i ];
}

/*
===================================
	IsValidVoxelIndex
		Returns wheter xyz is valid for using it
		as an index for voxeldata
===================================
*/
bool vsSector2::IsValidVoxelIndex( const vsVec3i& xyz )
{
	return( xyz >= vsVec3i(0) && xyz < dimensions.ToVec3i());
}

/*
===================================
	IsPointInside
		Is a point inside the sectors bounds?
===================================
*/
bool vsSector2::IsPointInside( const vsVec3f& point ) 
{
	return bounds.IsPointInside( point );
}

/*
===================================
	IsModified
		Returns TRUE if landscape has been modified
===================================
*/
bool vsSector2::IsModified( void )
{
	return modified;
}

/*
===================================
	SetModificationState
		Sets wether the landscape has been modified or not
===================================
*/
void vsSector2::SetModificationState( bool isModified )
{
	modified = isModified;
}

/*
===================================
	GetCellDimensions
		Returns scale / (dimensions-1)
===================================
*/
vsVec3f vsSector2::GetCellDimensions( void )
{
	return scale_1;
}

/*
===================================
	GetBoxOfCell
		Returns the BOUNDING BOX of a given VOXEL COORDINATE
===================================
*/
vsBox* vsSector2::GetBoxOfCell( const vsVec3ui& cell )
{
	vsVec3f a = offset + ((vsVec3ui)cell).ToVec3f()*scale_1;

	return new vsBox(a, a+scale_1);

}

/*
===================================
	GetVoxelDataAt()
		Public function for getting voxel value at "xyz"
		BEWARE: NO LIMIT CHECK!
===================================
*/
VOXELDATATYPE vsSector2::GetVoxelDataAt( const vsVec3ui& xyz )
{
	return voxelData[xyz.x][xyz.y][xyz.z];
}

/*
===================================
	FetchNeighborsFromList
===================================
*/
void vsSector2::FetchNeighborsFromList( const vsList<vsSector2*>& list )
{
	REPU(6) neighbors[i] = NULL;
	vsVec3i dc;
	REPU(list.Length())
	{
		dc = list.list[i]->coord.ToVec3i() - coord.ToVec3i();
		if(dc == vsVec3i( 0,-1, 0))
			neighbors[S2N_LO] = list.list[i];
		else
		if(dc == vsVec3i( 0, 1, 0))
			neighbors[S2N_UP] = list.list[i];
		else
		if(dc == vsVec3i(-1, 0, 0))
			neighbors[S2N_L] = list.list[i];
		else
		if(dc == vsVec3i( 1, 0, 0))
			neighbors[S2N_R] = list.list[i];
		else
		if(dc == vsVec3i( 0, 0,-1))
			neighbors[S2N_F] = list.list[i];
		else
		if(dc == vsVec3i( 0, 0, 1))
			neighbors[S2N_B] = list.list[i];
	}
}

/*
===================================
	SaveToFile
===================================
*/
void vsSector2::SaveToFile( vsOutputFile* f )
{
	f->WriteUnsignedInt(&coord.x); 
	f->WriteUnsignedInt(&coord.y);
	f->WriteUnsignedInt(&coord.z);
	f->WriteInt(&voxelOffset.x); 
	f->WriteInt(&voxelOffset.y);
	f->WriteInt(&voxelOffset.z);
	f->WriteUnsignedInt(&gridSize);
	f->WriteByte(&terrainType);
	f->WriteVec3(&bounds.points[0]);
	f->WriteVec3(&bounds.points[7]);
	f->WriteUnsignedInt(&realDimensions.x);
	f->WriteVec3(&scale);
	f->WriteVec3(&offset);
	f->WriteVec3(&texScale);

	REPU2(x, dimensions.x)
	REPU2(y, dimensions.y)
	REPU2(z, dimensions.z)
	{
		f->WriteByte(&voxelData[x][y][z]);
	}
}

/*
===================================
	RestoreFromFile
===================================
*/
void vsSector2::RestoreFromFile( vsInputFile* f, bool keepGridSize, uint newGridSize )
{
	Destroy();
	f->ReadUnsignedInt(&coord.x); 
	f->ReadUnsignedInt(&coord.y);
	f->ReadUnsignedInt(&coord.z);
	f->ReadInt(&voxelOffset.x); 
	f->ReadInt(&voxelOffset.y);
	f->ReadInt(&voxelOffset.z);
	f->ReadUnsignedInt(&gridSize);
	f->ReadByte(&terrainType);
	f->ReadVec3(&bounds.points[0]);
	f->ReadVec3(&bounds.points[7]);
	f->ReadUnsignedInt(&realDimensions.x);
	f->ReadVec3(&scale);
	f->ReadVec3(&offset);
	f->ReadVec3(&texScale);	

	bounds.FromMinMax(bounds.points[0], bounds.points[7]);
	realDimensions = vsVec3ui(realDimensions.x);

	if(!keepGridSize) 
	{
		byte tmpByte;
		uint q;

		dimensions = realDimensions / newGridSize;
		AllocVoxelData(dimensions.x, dimensions.y, dimensions.z);

		// file contains it more detailed
		// but we want to load it at lower precision
		if(newGridSize >= gridSize) 
		{	
			q = newGridSize / gridSize;

			REPU2(x, dimensions.x)
			REPU2(y, dimensions.y)
			REPU2(z, dimensions.z) {
				f->ReadByte(&voxelData[x][y][z]);
				REPU(q-1)
					f->ReadByte(&tmpByte);
			}
		} else 
		// file contains it less detailed
		{	
			q = gridSize / newGridSize;

			for(uint x=0; x<dimensions.x; x+=q)
			for(uint y=0; y<dimensions.y; y+=q)
			for(uint z=0; z<dimensions.z; z+=q)
				f->ReadByte(&voxelData[x][y][z]);

			// generate missing voxels
			for(uint x=0; x<dimensions.x; x++)
			for(uint y=0; y<dimensions.y; y++)
			for(uint z=0; z<dimensions.z; z++)
				if(!((x%q==0) && (y%q==0) && (z%q==0)))
					voxelData[x][y][z] = worldGenerator->GetVoxelAtCoords(x,y,z);
		}

		gridSize = newGridSize;
	} else {
		dimensions = realDimensions / gridSize;
		AllocVoxelData(dimensions.x, dimensions.y, dimensions.z);

		REPU2(x, dimensions.x)
		REPU2(y, dimensions.y)
		REPU2(z, dimensions.z) 
			f->ReadByte(&voxelData[x][y][z]);
	}
}