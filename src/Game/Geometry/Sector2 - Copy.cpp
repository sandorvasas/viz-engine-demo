#include "Sector2.h"
#include "MC_Tables.h"

/*
====================================================================

	vsSector2OTNode

====================================================================
*/
vsSector2OTNode::vsSector2OTNode( void ) {
	visible = false;
}

vsSector2OTNode::~vsSector2OTNode( void ) 
{
	if(nodeType == TYPE_LEAF) {
//		indices.Free();
//		glDeleteBuffers(1, &indexBuffer);			
	}
}

void vsSector2OTNode::SetVoxelData( vsSector2* pSector, 
								    uint x, uint y, uint z,
								    uint w, uint h, uint d )
{
	this->pSector = pSector;
	this->x = x;
	this->y = y;
	this->z = z;
	this->width = w;
	this->height = h;
	this->depth = d;
}


/*
====================================================================

	vsSector2

	Note:
		voxelData Coords: 
		 x : from left to right
		 y : from top to bottom
		 z : from front to back
====================================================================
*/
vsSector2::vsSector2( void )
{
	frustum = NULL;
	shader  = NULL;
	numNodes = 0;
	coordX = coordY = 0;
	voxelData = NULL;
	isolevel = (VOXELDATATYPE) 127;
	vertexBuffer = 0;
	width=height=depth=0;
}

vsSector2::vsSector2( const sector2Args_t& lsp )
{
	vsSector2();
	Generate(lsp);
}

vsSector2::~vsSector2( void )
{
	Destroy();
}

void vsSector2::Destroy( void )
{
	for(uint i=0; i<nodes.Length(); ++i)
		if(nodes[i]) 
			delete nodes[i];

	nodes.Free();
	FreeVoxelData();

	if(vertexBuffer)
		glDeleteBuffers(1, &vertexBuffer);
}

int vsSector2::Generate( const sector2Args_t& lsp )
{
	if(lsp.generateNew) 
		GenerateNew( lsp );

	this->player		= lsp.player;
	this->frustum		= &player->frustum;
	this->shader		= lsp.shader;
	this->gridSize		= lsp.gridSize;
	this->scale_1		= scale / vsVec3(float(width), float(height), float(depth));
	REPEAT(16) 
		textures[i] = lsp.textures[i];


	vertices.Alloc((width /gridSize)*(height/gridSize)*(depth / gridSize)*5);
	realNumVertices=0;
	console->Print("PRE vertlen=%d", vertices.Length());

	CreateGeometryFromVoxels();
	vertices.Resize(realNumVertices);


	if(!vertexBuffer)
		glGenBuffers(1, &vertexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(	GL_ARRAY_BUFFER, 
					vertices.Length()*sizeof_vsVertex, 
					vertices.list, 
					GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	bounds.FromVertices(vertices.list, vertices.Count);
	console->Print("bounds: %s %s", bounds.points[0].ToCharPtr(),
									bounds.points[7].ToCharPtr());

	console->Print("POST vertlen=%d", vertices.Length());
	

	GenerateOctree();

	return VS_OK;
}

void vsSector2::GenerateNew( const sector2Args_t& lsp )
{
	Destroy();

	this->coordX		= lsp.coordX;
	this->coordY		= lsp.coordY;
	this->terrainType	= lsp.terrainType;
	this->width			= lsp.size;
	this->height		= lsp.size;
	this->depth			= lsp.size;
	this->scale			= lsp.scale;
	this->texScale		= lsp.texScale;
	this->offset		= lsp.translation - scale*.5f*0.f; 
	this->LOD			= lsp.LOD; 
		
	AllocVoxelData(lsp.size, lsp.size, lsp.size);

	uint i=0;
	vsSector2* n;
	neighbors.front = neighbors.back = neighbors.left = neighbors.right=0;

	// FRONT NEIGHBOR
	if((n=lsp.neighbors[SECTOR_NEIGHBOR_FRONT])!=NULL)
	{
		neighbors.front = 1;
		REPU2(x, width)
		REPU2(y, height)
		{
			voxelData[x][y][0] = n->voxelData[x][y][depth-1];
		}
		offset = n->offset + vsVec3(0.0f, 0.0f, 0.5f*(n->scale.z+scale.z)); 
	}
	
	// BACK NEIGHBOR
	if((n=lsp.neighbors[SECTOR_NEIGHBOR_BACK])!=NULL)
	{
		neighbors.back = 1;
		REPU2(x, width)
		REPU2(y, height)
		{
			voxelData[x][y][depth-1] = n->voxelData[x][y][0];
		}
		offset = n->offset - vsVec3(0.0f, 0.0f, 0.5f*(n->scale.z+scale.z)); 
	}

	//RIGHT NEIGHBOR
	if((n=lsp.neighbors[SECTOR_NEIGHBOR_RIGHT])!=NULL)
	{
		neighbors.right = 1;
		REPU2(y, height)
		REPU2(z, depth)
		{
			voxelData[0][y][z] = n->voxelData[width-1][y][z];
		}
		offset = n->offset - vsVec3(0.5f*(n->scale.x+scale.x), 0.0f, 0.0f); 
	}

	//LEFT NEIGHBOR
	if((n=lsp.neighbors[SECTOR_NEIGHBOR_LEFT])!=NULL)
	{
		neighbors.left = 1;
		REPU2(y, height)
		REPU2(z, depth)
		{
			voxelData[width-1][y][z] = n->voxelData[0][y][z];
		}
		offset = n->offset + vsVec3(0.5f*(n->scale.x+scale.x), 0.0f, 0.0f); 
	}

	// HEIGHTFIELD GENERATION DEPENDS ON THE TYPE OF THE TERRAIN ********
	switch(lsp.terrainType) {
		case TERRAIN_TYPE_HILLS: GenerateHills();
		break;
	}


}

void vsSector2::Render( void )
{
/*	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof_vsVertex, vertices.list);
	glDrawArrays(GL_TRIANGLES, 0, vertices.Length());
	glDisableClientState(GL_VERTEX_ARRAY);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	*/

	if(!shader) return;

	glLineWidth(1.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shader->Bind(true);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	REPEAT(3)
	{
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		shader->Uniform1i(vsStr("textures[%d]", i).ToCharPtr(), i);

		glClientActiveTexture(GL_TEXTURE0+i);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof_vsVertex, BUFFER_OFFSET(36));
	}


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0,	3, GL_FLOAT, false, sizeof_vsVertex, NULL);
	glVertexAttribPointer(1,	3, GL_FLOAT, true,  sizeof_vsVertex, BUFFER_OFFSET(12));
	glVertexAttribPointer(2,	3, GL_FLOAT, true,	sizeof_vsVertex, BUFFER_OFFSET(24));
//	glVertexAttribPointer(3,	3, GL_FLOAT, false, sizeof_vsVertex, BUFFER_OFFSET(60));
	
	glDrawArrays(GL_TRIANGLES, 0, vertices.Length());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
//	glDisableVertexAttribArray(3);

	
	REPEAT(3)
	{
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, 0);
		glClientActiveTexture(GL_TEXTURE0+i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader->Bind(false);

	RenderNode(0);
}

void vsSector2::RenderVoxelData( void )
{
	glPointSize(10.0f);
	glColor3f(0.0f, 0.0f, 0.8f);
	glBegin(GL_POINTS);
		REPU2(x, width)
		REPU2(y, height)
		REPU2(z, depth)
			if( voxelData[x][y][z] > 0 )
				glVertex3fv((vsVec3( float(x), float(height-y), float(z) ) * (scale / vsVec3(float(width), float(height), float(depth))) +offset).ToFloatPtr());
	glEnd();
}

void vsSector2::AllocVoxelData( uint width, uint height, uint depth )
{
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

void vsSector2::FreeVoxelData( void )
{
	if(voxelData) {
		REPU2(x, width) {
			REPU2(y, height)
				delete [] voxelData[x][y];
			delete [] voxelData[x];
		}
		delete [] voxelData;
	}
}

void vsSector2::GenerateHills( void )
{
	byte h;
	for(uint x=neighbors.left, z, y; x<width-neighbors.right; ++x)
	{
		for(z=neighbors.front*0; z<depth-neighbors.back*0; ++z)
		{
			h = (byte)(height/2);// - (rand() % 4);
			voxelData[x][h][z] = 255;
			for(y=h+1; y<height; ++y)
			{
				voxelData[x][y][z] = 255;
			}
		}
	}
}

void vsSector2::CreateGeometryFromVoxels( void )
{


	uint gridStepX =  gridSize;
	uint gridStepY =  gridSize;
	uint gridStepZ =  gridSize;

	float fx, fy, fz, fx1, fy1, fz1;
	uint x,y,z, x1, y1, z1;

	sector2GridCell_t cell;

	for(x=0; x<width; x+=gridStepX)
	for(y=0; y<height; y+=gridStepY)
	for(z=0; z<depth; z+=gridStepZ)
	{
		x1 = x+gridStepX < width  ? x+gridStepX : x;
		y1 = y+gridStepY < height ? y+gridStepY : y;
		z1 = z+gridStepZ < depth  ? z+gridStepZ : z;

		fx=(float)x; fy=(float)height-y; fz=(float)z;
		fx1=(float)x1; fy1=(float)height-y1; fz1=(float)z1;

		cell.values[0] = voxelData[x][y1][z1];
		cell.vertices[0] = vsVec3(fx, fy1, fz1);

		cell.values[1] = voxelData[x1][y1][z1];
		cell.vertices[1] = vsVec3(fx1, fy1, fz1);

		cell.values[2] = voxelData[x1][y1][z];
		cell.vertices[2] = vsVec3(fx1, fy1, fz);

		cell.values[3] = voxelData[x][y1][z];
		cell.vertices[3] = vsVec3(fx, fy1, fz);

		cell.values[4] = voxelData[x][y][z1];
		cell.vertices[4] = vsVec3(fx, fy, fz1);

		cell.values[5] = voxelData[x1][y][z1];
		cell.vertices[5] = vsVec3(fx1, fy, fz1);

		cell.values[6] = voxelData[x1][y][z];
		cell.vertices[6] = vsVec3(fx1, fy, fz);

		cell.values[7] = voxelData[x][y][z];
		cell.vertices[7] = vsVec3(fx, fy, fz);

		PolygoniseCell( cell );
	}

}

void vsSector2::PolygoniseCell( const sector2GridCell_t& cell )
{
	int cubeIndex = 0;
	int j, k;

	for(j=0, k=1; j<8; ++j, k*=2) 
		if(cell.values[j] < isolevel) cubeIndex |= k;


	if(/*edgeTable[cubeIndex] == 0xFF ||*/ edgeTable[cubeIndex] == 0x00)
		return;

	vsVec3 vert[12];

	for(j=0, k=1; j<12; ++j, k*=2)
		if(edgeTable[cubeIndex] & k) vert[j] = InterpolateMCVertex(cell, j);

	vsVertex v[3];
	for(j=0; j<triVertLenTable[cubeIndex]; j+=3)
	{
		v[0].xyz = vert[ triTable[cubeIndex][j] ] * scale_1 +offset;
		v[1].xyz = vert[ triTable[cubeIndex][j+1] ] * scale_1 +offset;
		v[2].xyz = vert[ triTable[cubeIndex][j+2] ] * scale_1 +offset;
		v[0].CalculateTangent(v[1], v[2]);

		if(j%2) {
			v[0].st = vsVec2(0.0f);
			v[1].st = vsVec2(2.0f, 0.0f);
			v[2].st = vsVec2(2.0f, 2.0f);
		} else 
		{
			v[0].st = vsVec2(0.0f, 2.0f);
			v[1].st = vsVec2(0.0f, 0.0f);
			v[2].st = vsVec2(2.0f, 0.0f);
		}

		v[1].Normal  = v[2].Normal  = v[0].Normal;
		v[1].Tangent = v[2].Tangent = v[0].Tangent;

		vertices[realNumVertices++] = v[0];
		vertices[realNumVertices++] = v[1];
		vertices[realNumVertices++] = v[2];
	}
}

vsVec3 vsSector2::InterpolateMCVertex( const sector2GridCell_t& cell, byte edgeNum )
{
	float v1 = (float)cell.values[ edgeVertices[edgeNum][0] ]; 
	float v2 = (float)cell.values[ edgeVertices[edgeNum][1] ];
	vsVec3 p1 = cell.vertices[ edgeVertices[edgeNum][0] ];
	vsVec3 p2 = cell.vertices[ edgeVertices[edgeNum][1] ];

	return p1 + (float(isolevel) - v1) * ((p2-p1)/(v2-v1));
}


void vsSector2::GenerateOctree( void )
{
	for(uint i=0; i<nodes.Length(); ++i)
		if(nodes[i]) {
			delete nodes[i];
		}
	nodes.Free();

	nodes.Alloc( PrecalculateOctreeNodeCount(0, 0, 0, width-1, height-1, depth-1) );
	console->Print("Precalc nodeCount: %d", nodes.Length());
	for(uint i=0; i<nodes.Length(); ++i)
		nodes[i] = NULL;

	GenerateOctreeNode(0, 0, 0, width-1, height-1, depth-1, -1);
}

uint vsSector2::PrecalculateOctreeNodeCount( int startX, int startY, int startZ, int endX, int endY, int endZ )
{
	if( (endX - startX+1 <= SECTOR_OT_NODE_MIN_WIDTH) && 
		(endY - startY+1 <= SECTOR_OT_NODE_MIN_HEIGHT) &&
		(endZ - startZ+1 <= SECTOR_OT_NODE_MIN_DEPTH) )
	{
		return 1;
	}
	
	int Cx = (startX + endX) / 2;
	int Cy = (startY + endY) / 2;
	int Cz = (startZ + endZ) / 2;

	uint num = 1;
	num += PrecalculateOctreeNodeCount(startX, startY, startZ, Cx, Cy, Cz);
	num += PrecalculateOctreeNodeCount(Cx, startY, startZ, endX, Cy, Cz);
	num += PrecalculateOctreeNodeCount(Cx, Cy, startZ, endX, endY, Cz);
	num += PrecalculateOctreeNodeCount(startX, Cy, startZ, Cx, endY, Cz);
	num += PrecalculateOctreeNodeCount(startX, startY, Cz, Cx, Cy, endZ);
	num += PrecalculateOctreeNodeCount(Cx, startY, Cz, endX, Cy, endZ);
	num += PrecalculateOctreeNodeCount(Cx, Cy, Cz, endX, endY, endZ);
	num += PrecalculateOctreeNodeCount(startX, Cy, Cz, Cx, endY, endZ);

	return num;
}

int vsSector2::GenerateOctreeNode( int startX, int startY, int startZ, int endX, int endY, int endZ, int parentId )
{
	if(parentId==-1) numNodes = 0;

	vsSector2OTNode* node = new vsSector2OTNode();

	node->parentId	= parentId;
	node->Id		= numNodes++;
	node->width		= endX - startX+1;
	node->height	= endY - startY+1;
	node->depth		= endZ - startZ+1;
	
	//------------------------------
	//	Bounding Box
	//------------------------------
	vsVec3 min = offset + vsVec3(float(startX),float(startY), float(startZ)) * scale_1;
	vsVec3 max = offset + vsVec3(float(endX), float(endY), float(endZ)) * scale_1;
	node->bounds.FromMinMax(min, max);

	if( (node->width<=SECTOR_OT_NODE_MIN_WIDTH) && 
		(node->height<=SECTOR_OT_NODE_MIN_HEIGHT) &&
		(node->depth<=SECTOR_OT_NODE_MIN_DEPTH) )
	{
		node->nodeType = TYPE_LEAF;
		node->SetVoxelData(this, startX, startY, startZ, 
							node->width, node->height, node->depth);

		console->Print("LEAF: x:%d y:%d z:%d w:%d h:%d d:%d", startX, startY, startZ, node->width, node->height, node->depth);
							
		this->nodes.list[numNodes-1] = node;
	} else
	{
		node->nodeType = TYPE_NODE;

		this->nodes.list[numNodes-1] = node;

		int Cx = (startX + endX) / 2;
		int Cy = (startY + endY) / 2;
		int Cz = (startZ + endZ) / 2;

		node->childrenId[0] = GenerateOctreeNode(startX, startY, startZ, Cx, Cy, Cz, node->Id);
		node->childrenId[1] = GenerateOctreeNode(Cx, startY, startZ, endX, Cy, Cz, node->Id);
		node->childrenId[2] = GenerateOctreeNode(Cx, Cy, startZ, endX, endY, Cz, node->Id);
		node->childrenId[3] = GenerateOctreeNode(startX, Cy, startZ, Cx, endY, Cz, node->Id);
		node->childrenId[4] = GenerateOctreeNode(startX, startY, Cz, Cx, Cy, endZ, node->Id);
		node->childrenId[5] = GenerateOctreeNode(Cx, startY, Cz, endX, Cy, endZ, node->Id);
		node->childrenId[6] = GenerateOctreeNode(Cx, Cy, Cz, endX, endY, endZ, node->Id);
		node->childrenId[7] = GenerateOctreeNode(startX, Cy, Cz, Cx, endY, endZ, node->Id);
	}

	return( node->Id );
}

void vsSector2::RenderNode( uint nodeId )
{
	vsSector2OTNode *n = nodes[nodeId];

	bool inside;

	if(frustum)
		inside = frustum->IntersectBox(nodes[nodeId]->bounds, 0.002f);
	else
		inside = true;

	if(!inside) return;

	n->visible = true;

	if(n->nodeType == TYPE_NODE)
	{
		for(int i=0; i<8; ++i)
			RenderNode(n->childrenId[i]);
	} else
	//if(n->nodeType == TYPE_LEAF) 
	{
		n->bounds.RenderBox();
		
	} // type == TYPE_LEAF
			
}


void vsSector2::PutBlock( uint x, uint y, uint z )
{
	voxelData[x][height-y][z] = 255;
	CreateGeometryFromVoxels();
}

void vsSector2::RemoveBlock( uint x, uint y, uint z )
{
	voxelData[x][y][z] = 0;
	CreateGeometryFromVoxels();
}

void vsSector2::LocatePoint_Cell( const vsVec3& p, int& x, int& y, int& z )
{
	vsVec3 c = (p - offset) / scale_1;
	x = (int) c.x;
	y = (int) c.y;
	z = (int) c.z;

	if(!(x>0 && y>0 && z>0 && x<(int)width && y<(int)height && z<(int)depth))
	{
		x = y = z = -1;
	}
}

void vsSector2::SaveToFile( vsOutputFile* f )
{
	f->WriteUnsignedInt(&coordX); 
	f->WriteUnsignedInt(&coordY);
	f->WriteByte(&terrainType);
	f->WriteVec3(&bounds.points[0]);
	f->WriteVec3(&bounds.points[7]);
	f->WriteUnsignedInt(&width);
	f->WriteVec3(&scale);
	f->WriteVec3(&offset);
	f->WriteVec3(&texScale);
	REPU2(x, width)
	REPU2(y, height)
	REPU2(z, depth)
	{
		f->WriteByte(&voxelData[x][y][z]);
	}
}

void vsSector2::RestoreFromFile( vsInputFile* f )
{
	Destroy();
	f->ReadUnsignedInt(&coordX); 
	f->ReadUnsignedInt(&coordY);
	f->ReadByte(&terrainType);
	f->ReadVec3(&bounds.points[0]);
	f->ReadVec3(&bounds.points[7]);
	f->ReadUnsignedInt(&width);
	height = depth = width;
	f->ReadVec3(&scale);
	f->ReadVec3(&offset);
	f->ReadVec3(&texScale);	
	bounds.FromMinMax(bounds.points[0], bounds.points[7]);

	AllocVoxelData(width, height, depth);
	REPU2(x, width)
	REPU2(y, height)
	REPU2(z, depth)
	{
		f->ReadByte(&voxelData[x][y][z]);
	}
}