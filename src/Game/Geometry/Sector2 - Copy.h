#ifndef __VS_SECTOR2_H__
#define __VS_SECTOR2_H__

/*
====================================================


	Voxel-based sector 


====================================================
*/


#include "..\Player.h"
#include "..\..\vsLib2\vsLib2.h"
using namespace vsLib2;


class vsCollisionManager;
class vsSector2;


/* 
============================================
	(Min width/height of leaf octree nodes) * 2
============================================
*/
#ifndef SECTOR_OT_NODE_MIN_DIMENSIONS
#define SECTOR_OT_NODE_MIN_DIMENSIONS
const uint SECTOR_OT_NODE_MIN_WIDTH = 16;
const uint SECTOR_OT_NODE_MIN_HEIGHT = 16;
const uint SECTOR_OT_NODE_MIN_DEPTH = 16;
#endif

typedef byte VOXELDATATYPE;
/* 
=====================================
	VOXEL TYPE ENUM
=====================================
*/
#ifndef VOXEL_TYPE_ENUM
#define VOXEL_TYPE_ENUM
enum {
	VOXEL_TYPE_GRASS = 0,
	VOXEL_TYPE_SOLE,
	VOXEL_TYPE_BLACK_SOLE,
	VOXEL_TYPE_SNOW
};
#endif

/* 
=====================================
	TERRAIN TYPE ENUM
=====================================
*/
#ifndef TERRAIN_TYPE_ENUM
#define TERRAIN_TYPE_ENUM
enum {
	TERRAIN_TYPE_HILLS = 0,
	TERRAIN_TYPE_SANDDESERT,
	//TERRAIN_TYPE_ROCKDESERT,
	TERRAIN_TYPE_TUNDRA,
	TERRAIN_TYPE_ARCTIC,
	TERRAIN_TYPE_MOUNTAINS,
	TERRAIN_TYPE_SAVANNA
};
#endif

/* 
=====================================
	NEIGHBOR TYPE ENUM
=====================================
*/
#ifndef NEIGHBOR_TYPE_ENUM
#define NEIGHBOR_TYPE_ENUM
enum {
	SECTOR_NEIGHBOR_FRONT = 0,
	SECTOR_NEIGHBOR_RIGHT,
	SECTOR_NEIGHBOR_BACK,
	SECTOR_NEIGHBOR_LEFT
};
#endif

/* 
=====================================
	NEIGHBOR ARGUMENTS
=====================================
*/
#ifndef NEIGHBOR_ARGUMENTS
#define NEIGHBOR_ARGUMENTS
typedef struct sectorNeighborArgs_s {
	unsigned front	: 1;
	unsigned back	: 1;
	unsigned right	: 1;
	unsigned left	: 1;

	sectorNeighborArgs_s( void ) {
		front = back = right = left = 0;
	}
} sectorNeighborArgs_t;
#endif
/* 
=====================================
	QUADTREE NODE TYPE ENUM
=====================================
*/
#ifndef TREE_NODE_TYPE_ENUM
#define TREE_NODE_TYPE_ENUM
typedef enum {
	TYPE_LEAF = 0,
	TYPE_NODE
} TreeNodeType_t;
#endif

/*
============================
	vsSector2OTNode
		OctTree Node class for sectors
============================
*/ 
class vsSector2OTNode {
public:
					vsSector2OTNode( void );
					~vsSector2OTNode( void );

	void			SetVoxelData( vsSector2* pSector, 
								   uint x, uint y, uint z,
								   uint w, uint h, uint d );
public:
	int				Id;
	int				parentId;
	
	uint			x, y, z;
	uint			width;
	uint			height;
	uint			depth;
	vsSector2*		pSector;

	int				childrenId[ 8 ];
	TreeNodeType_t	nodeType;	
	vsList<vsVertex>vertices;
//	uint			numVertices;
	uint			vertexBuffer;
	vsBox			bounds;
	bool			visible;
};


/*
============================
	sectorArgs_t
============================
*/
typedef struct sector2Args_s
{
	uint			coordX, coordY;
	uint			LOD;				// 1: max 2: every 2nd vertex 3: etc.
	uint			size;
	vsShader*		shader;
	vsVec3			scale;
	vsVec3			translation;
	vsVec3			texScale;	
	vsPlayer*		player;
	byte			terrainType;
	bool			generateNew;
	uint			textures[16];
	vsSector2*		neighbors[4];
	uint			gridSize;
} sector2Args_t;


typedef struct sector2GridCell_s 
{
	VOXELDATATYPE	values[8];
	vsVec3			vertices[8];
} sector2GridCell_t;

/*
====================================================================

	vsSector2

====================================================================
*/

class vsSector2  {

	friend class vsCollisionManager;

public:
							vsSector2( void );
				explicit	vsSector2( const sector2Args_t& lsp );
							~vsSector2( void );

	int						Generate( const sector2Args_t& lsp );
	void					Destroy( void );
	void					Render( void );
	void					RenderVoxelData( void );
	void					PutBlock( uint x, uint y, uint z );
	void					RemoveBlock( uint x, uint y, uint z );
	void					LocatePoint_Cell( const vsVec3& p, int& x, int& y, int& z );
	void					SaveToFile( vsOutputFile* f );
	void					RestoreFromFile( vsInputFile* f );
//	vsSector2OTNode*		LocatePlayer( void );				// returns pointer to node in which the player is staying

private:
	void					GenerateNew( const sector2Args_t& lsp );
	void					AllocVoxelData( uint width, uint height, uint depth );
	void					FreeVoxelData( void );
	void					CreateGeometryFromVoxels( void );
	void					PolygoniseCell( const sector2GridCell_t& cell );
	vsVec3					InterpolateMCVertex( const sector2GridCell_t& cell, byte edgeNum );

	void					GenerateOctree( void );
	uint					PrecalculateOctreeNodeCount( int startX, int startY, int startZ, int endX, int endY, int endZ );
	int						GenerateOctreeNode( int startX, int startY, int startZ, int endX, int endY, int endZ, int parentId );

	void					RenderNode( uint nodeId );
//	void					CalculateNormals( void );
//	void					Smooth( void );
//	int						GetPlayersNode( int nodeId );		// returns the id of node in which the player is staying

//	void					GenerateGrass( uint density );
	void					GenerateHills( void );
/*	void					GenerateSavanna( void );
	void					GenerateSendDesert( void );
	void					GenerateTundra( void );
	void					GenerateMountains( void );
	void					GenerateArctic( void );
	*/
public:
//	 friend static void		DEBUG_GetVisibleNodes( const int& a );

public:
	uint					coordX, coordY;
	vsBox					bounds;

private:
	vsPlayer*				player;
	uint					width;
	uint					height;
	uint					depth;
	VOXELDATATYPE***		voxelData;
	VOXELDATATYPE			isolevel;
	uint					gridSize;
	uint					roughness;	
	uint					LOD;

	uint					textures[16];
	vsShader*				shader;
	vsVec3					texScale;
	vsVec3					scale;
	vsVec3					scale_1;

public:	vsVec3				offset;	//fixme private
	vsFrustum*				frustum;
	vsList<vsVertex>		vertices;
	uint					realNumVertices;
	uint					vertexBuffer;
	vsList<vsSector2OTNode*>nodes;
	uint					numNodes;
	byte					terrainType;
	sectorNeighborArgs_t	neighbors;
};

#endif