#ifndef __VS_SECTOR2_H__
#define __VS_SECTOR2_H__

/*
====================================================


	Voxel-based sector 


====================================================
*/


#include "..\Player.h"
#include "..\..\vsLib2\vsLib2.h"
#include "Grass.h"

using namespace vsLib2;


class vsCollisionManager;
class vsSector2;


/* 
============================================
	(Min width/height of leaf octree nodes) * 2
============================================
*/
#ifndef SECTOR_OT_NODE_MIN_DIM
#define SECTOR_OT_NODE_MIN_DIM
//const vsVec3ui SECTOR_OT_NODE_MIN_DIMENSIONS = vsVec3ui(16);
#endif

// type of voxels
typedef byte VOXELDATATYPE;

/* 
=====================================
	VOXEL TYPE ENUM
=====================================
*/
#ifndef VOXEL_TYPE_ENUM
#define VOXEL_TYPE_ENUM
enum {
	VOXEL_TYPE_AIR = 0,
	VOXEL_TYPE_WATER,
	VOXEL_TYPE_BUSH,
	VOXEL_TYPE_TREE,
	VOXEL_TYPE_FIRE,
	VOXEL_TYPE_LADDER,
	VOXEL_TYPE_FENCE,
	VOXEL_TYPE_TURBINE,
	//==================
	VOXEL_TYPE_ISOLEVEL = 10,
	//==================
	VOXEL_TYPE_GRASS,
	VOXEL_TYPE_DRYGRASS,
	VOXEL_TYPE_SOIL,
	VOXEL_TYPE_ROCK1,
	VOXEL_TYPE_GROUNDROCK1,
	VOXEL_TYPE_COOLEDLAVA,
	VOXEL_TYPE_LAVASTREAM,
	VOXEL_TYPE_SNOW,
	VOXEL_TYPE_IRON,
	VOXEL_TYPE_COPPER,
	VOXEL_TYPE_GOLD,
	VOXEL_TYPE_DIAMOND,
	VOXEL_TYPE_SILVER,
	VOXEL_TYPE_BONE,
	VOXEL_TYPE_ICE,
	VOXEL_TYPE_SAND,
	VOXEL_TYPE_STONE,
	VOXEL_TYPE_CLAY,
	VOXEL_TYPE_TAR,
	VOXEL_TYPE_ROCKOIL,
	VOXEL_TYPE_GRAVEL,
	VOXEL_TYPE_BOX,
	VOXEL_TYPE_BRICK,
	VOXEL_TYPE_TILE1,
	VOXEL_TYPE_TILE2,
	VOXEL_TYPE_GLASS,
	VOXEL_TYPE_WOOD,

	VOXEL_TYPE_INVALID_INDEX = 255
};
#endif

/* 
=====================================
	vsSector2Texture
=====================================
*/
namespace vsSector2Texture 
{
	extern vsVec2f MegaTextureCoordinates[255];

	void SetupMegaTextureCoordinates( void );

	const int	numTexturesPerRow = 8;
	const float megaTextureSize	  = 2048.f;
	const float textureSize		  = 1.0f / float(numTexturesPerRow);
}

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
#ifndef NEIGHBOR_TYPE_ENUM2
#define NEIGHBOR_TYPE_ENUM2
enum {
	S2N_F = 0,	// FRONT
	S2N_R,		// RIGHT
	S2N_B,		// BACK
	S2N_L,		// LEFT
	S2N_UP,		// UPPER
	S2N_LO		// LOWER
};
#endif

/* 
=====================================
	NEIGHBOR ARGUMENTS
=====================================
*/
#ifndef NEIGHBOR_ARGUMENTS2
#define NEIGHBOR_ARGUMENTS2
typedef struct sector2NeighborArgs_s {
	unsigned front		: 1;
	unsigned back		: 1;
	unsigned right		: 1;
	unsigned left		: 1;
	unsigned lower		: 1;
	unsigned upper		: 1;

	sector2NeighborArgs_s( void ) {
		upper=lower=front=back=right=left=0;
	}
} sector2NeighborArgs_t;
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
	TYPE_INTERNAL
} TreeNodeType_t;
#endif


/* 
=====================================
	sector2Vertex_t
=====================================
*/
class sector2Vertex_t : public vsVertex {
public:
	MINIMUM_OPERATORS(sector2Vertex_t);
	float		cellValues[8];
};

/*
=====================================================

	vsSector2OTNode
		OctTree Node class for sectors


	vsVertex member behaviors:
		Color(vec4) PolygoniseCell()...CalculateNormals():

					Color.x = cellCoordOfVertex.x
					Color.y = cellCoordOfVertex.y
					Color.z = cellCoordOfVertex.z
					Color.w = empty

					After that:
					Color.x = megaTextureOffset.x
					Color.y = megaTextureOffset.y
					Color.z = empty
					Color.w = empty

		st(vec2) & reserved(float) holds the triplanar texture blending coords

=====================================================
*/ 
class vsSector2OTNode {
public:
						vsSector2OTNode( void );
						~vsSector2OTNode( void );
	void				SetVoxelData( vsSector2* pSector, const vsVec3ui& start, const vsVec3ui& end );
	bool				IsLeaf( void );
//	void				GenerateGrass( vsGrass* grass );

public:
	int					Id;
	int					parentId;
	TreeNodeType_t		nodeType;
	vsBox				bounds;
	bool				visible;
	vsVec3ui			start;
	vsVec3ui			end;
	vsSector2*			pSector;
	int					childrenId[ 8 ];
	vsList<sector2Vertex_t>	vertices;
	vsList<uint>		indices;
	uint				buffers[2];
	bool				geometryModified;
	//vsGrass*			grass;
};



/*
============================
	sectorArgs_t
============================
*/
typedef struct sector2Args_s
{
	struct {
		vsVec3ui			coord;
		uint				LOD;				// 1: max 2: every 2nd vertex 3: etc.
		uint				size;
		vsShader*			terrainShader;
		vsVec3f				scale;
		vsVec3f				translation;
		vsVec3f				texScale;	
		byte				terrainType;
		bool				generateNew;
		uint				megaTexDiff;
		uint				megaTexNorm;
		vsSector2*			neighbors[6];
		uint				gridSize;
		bool				IsThisTheMainSector;
		vsVec3ui			nodeMinDimensions;
		vsVec3i				voxelOffset;
	}						SECTOR;
	/*struct {
		vsShader*		shader;
		int				density;
		vsVec3f			windVec;
		uint			finTexture;
		vsVec2f			scale;
	}					GRASS;*/
} sector2Args_t;


/*
============================
	sector2GridCell_t
============================
*/
typedef struct sector2GridCell_s 
{
	vsVec3ui		coords;
	VOXELDATATYPE	values[8];
	vsVec3f			vertices[8];
} sector2GridCell_t;


enum {
	S2ATTRIB_POSITION = 0,
	S2ATTRIB_NORMAL = 1,
	S2ATTRIB_TANGENT=2,
	S2ATTRIB_CELLSTARTPOS=3,
	S2ATTRIB_CELLVALUES1=4,
	S2ATTRIB_CELLVALUES2=5
};

/*
====================================================================


	vsSector2


====================================================================
*/
class vsSector2  {

	friend class vsSector2OTNode;
	friend class vsCollisionManager;
	friend class vsConsoleProcessor;
	friend class vsGrass;

public:
							vsSector2( void );
				explicit	vsSector2( const sector2Args_t& lsp );
							~vsSector2( void );

	int						Generate( const sector2Args_t& lsp );
	void					Destroy( void );
	void					Render( void );
	void					RenderVoxelData( void );
	void					RenderTree(  int nodeId );
	int						PutBlock( const vsVec3ui& xyz, VOXELDATATYPE blockType );
	int						RemoveBlock( const vsVec3ui& xyz );
	int						PutMultipleBlocks( vsVec3i coords[], int len, VOXELDATATYPE blockType );
	int						RemoveMultipleBlocks( vsVec3i coords[], int len );
	void					SaveToFile( vsOutputFile* f );
	void					RestoreFromFile( vsInputFile* f, bool keepGridSize=true, uint newGridSize=1 );
	vsVec3i					GetCellOfPoint( const vsVec3f& p );
	vsSector2OTNode*		LocatePlayer( void );				// returns pointer to node in which the player is staying
	vsSector2OTNode*		GetNodeOfPoint( const vsVec3f& point );
	vsSector2OTNode*		GetLeafNodeOfCell( const vsVec3ui& cell, int& side, int resultNum );
	vsVec3f					GetCellDimensions( void );
	vsBox*					GetBoxOfCell( const vsVec3ui& cell );
	VOXELDATATYPE			GetVoxelDataAt( const vsVec3ui& xyz );
	bool					IsValidVoxelIndex( const vsVec3i& xyz );
	bool					IsPointInside( const vsVec3f& point );
	bool					IsModified( void );
	void					SetModificationState( bool isModified );
	void					FetchNeighborsFromList( const vsList<vsSector2*>& list );

private:
	void					NeighborPutBlock( const vsVec3ui& xyz, VOXELDATATYPE blockType );
	void					NeighborRemoveBlock( const vsVec3ui& xyz );
	void					GenerateNew( const sector2Args_t& lsp );
	
	uint					Node_CreateGeometry( vsSector2OTNode* n );
	void					CreateNodeGeometryFromVoxels( vsSector2OTNode* node, uint _gridSize );
	uint					CreateGeometryFromVoxels( 
					vsList<sector2Vertex_t>* vertices,
					vsList<uint>* indices,
					uint* buffers,
					const vsVec3ui& voxelStart, 
					const vsVec3ui& voxelEnd,
					uint _gridSize );

	void					PolygoniseCell( vsList<sector2Vertex_t>* vertices, uint* numVertices, const sector2GridCell_t& cell );
	vsVec3f					InterpolateMCVertex( const sector2GridCell_t& cell, byte edgeNum );
	int						UpdateModifiedGeometry( const vsVec3ui& p );
	void					LocateCellAndCreateGeometry( uint x, uint y, uint z );
	VOXELDATATYPE&			VoxelDataAt( const vsVec3ui& xyz );

	void					GenerateOctree( void );
	uint					PrecalculateOctreeNodeCount( uint sX, uint sY, uint sZ, uint eX, uint eY, uint eZ );
	int						GenerateOctreeNode( uint sX, uint sY, uint sZ, uint eX, uint eY, uint eZ, int parentId );

	void					RenderNode( uint nodeId, bool regenerate=false );
	void					CalculateNormals( vsList<sector2Vertex_t>* vertices, vsList<uint>* indices, uint start, uint end );
	void					AllocVoxelData( uint width, uint height, uint depth );
	void					FreeVoxelData( void );
	void					AllocNodes( uint count );
	void					FreeNodes( void );
	int						GetPointsNode( int nodeId, const vsVec3f& point );		// returns the id of node in which the player is staying
	vsVec3f					TriPlanarTexCoords( const vsVec3f& point );
	float					EncodeMegaTextureCoords( int indexToTextureMap );

	void					GenerateVoxels( int numHills, int randomRange );
	void					GenerateHills( void );

public:
	vsVec3ui				coord;
	vsVec3i					voxelOffset;
	vsBox					bounds;
	int						numVisibleNodes;
	int						numVisibleLeafNodes;
	bool					mainSector;
	vsVec3f					offset;

private:
	vsSector2*				neighbors[6];
	vsShader*				shader;
	vsFrustum*				frustum;
	vsVec3ui				dimensions;
	vsVec3ui				realDimensions;
	VOXELDATATYPE***		voxelData;
	uint					gridSize;
	uint					roughness;	
	uint					LOD;
	uint					megaTexDiff;
	uint					megaTexNorm;
	vsVec3f					texScale;
	vsVec3f					scale;
	vsVec3f					scale_1;
	
	vsList<vsSector2OTNode*>nodes;
	uint					numNodes;
	vsVec3ui				nodeMinDimensions;
	byte					terrainType;
	bool					modified;
	uint					earthLevel;
	bool					nogeometry;
//	grassArgs_t				Grass;			// parentSectorNode property left "empty"
};

#endif