#ifndef __VS_SECTOR2_H__
#define __VS_SECTOR2_H__

/*
====================================================


	Voxel-based sector 


====================================================
*/

#include "..\Player.h"
#include "..\..\vsLib2\vsLib2.h"
#include "..\GameObject.h"
#include "..\Item.h"
#include "Grass.h"
#include "..\Blocks.h"

using namespace vsLib2;

struct Item_t;
class vsPhysicsManager;
class vsSector2;


/* 
============================================
	( Min width/height of leaf octree nodes ) * 2
============================================
*/
#ifndef SECTOR_OT_NODE_MIN_DIM
#define SECTOR_OT_NODE_MIN_DIM
//const vsVec3ui SECTOR_OT_NODE_MIN_DIMENSIONS = vsVec3ui( 16 );
#endif

/* 
=====================================
	vsSector2Texture
=====================================
*/
namespace vsSector2Texture 
{
	const int	numTexturesPerRow = 8;
	const float megaTextureSize	  = 2048.f;
	const float textureSize		  = 1.0f / float( numTexturesPerRow );
}


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
	MINIMUM_OPERATORS( sector2Vertex_t );
};

/*
============================
	sector2GridCell_t
============================
*/
typedef struct sector2GridCell_s 
{
	vsVec3ui		coords;
	ushort			values[ 8 ];
	vsVec3f			vertices[ 8 ];
} sector2GridCell_t;


enum {
	S2ATTRIB_POSITION = 0,
	S2ATTRIB_NORMAL,
	S2ATTRIB_TANGENT,
	S2ATTRIB_MEGATEXCOORDS,
	S2ATTRIB_GHOSTTEXCOORDS
};

/*
=====================================================

	vsSector2OTNode
		OctTree Node class for sectors

=====================================================
*/ 
enum nodeGeomState_t {
	NGS_NO_GEOMETRY = 0,
	NGS_GEOMETRY_OK,
	NGS_VEGETATION_OK,
	NGS_FULL_GEOMETRY
};

class vsSector2OTNode {
	
	friend class vsVegetation;

public:
							vsSector2OTNode( void );
							~vsSector2OTNode( void );
	
	void					Setup( int id, int parentId, const vsBox& bounds, bool isLeaf, vsSector2* pSector, 
								   byte gridSize, const vsVec3ui& voxelStart, const vsVec3ui& voxelEnd );
	const bool&				IsLeaf( void ) const;

public:
	int						Id;
	int						parentId;
	bool					isLeaf;
	int						childrenId[ 8 ];
	vsBox					bounds;
	byte					gridSize;
	vsVec3f					cellScale;
	bool					visible;
	vsVec3ui				start;
	vsVec3ui				end;
	vsSector2*				parentSector;
	uint					query;
	int						lastQueryVisible;

	//	geometry data
	uint					buffers[2];
	vsList<sector2Vertex_t>	vertices;
	//vsList<sector2Vertex_t> transparentVertices;
	vsList<uint>			indices;

	// default vegetation items ( trees, bushes, etc )
	vsList<Item_t*>			vegetation;
	vsGrass					grass;

	vsList<vsModel_MD5*>	models;

	// geometry updates
	nodeGeomState_t			geometryState;
	bool					geometryModified;
	bool					bufferUpdateNeeded;
	uint					oldVertexCount;
	uint					oldIndexCount;
};


/*
===================================
	IsLeaf
===================================
*/
VS_INLINE
const bool& vsSector2OTNode::IsLeaf( void ) const
{
	return( isLeaf );
}


/*
============================
	sectorArgs_t
============================
*/
typedef struct sector2Args_s
{
	struct {
		vsVec3i				coord;
		uint				LOD;				// 1: max 2: every 2nd vertex 3: etc.
		uint				size;
		vsShader*			terrainShader;
		vsVec3f				scale;
		vsVec3f				translation;
		vsVec3f				texScale;	
		byte				terrainType;
		bool				generateNew;
		bool				physicsTest;
		uint				megaTexDiff;
		uint				megaTexNorm;
		vsSector2*			neighbors[6];
		uint				gridSize;
		bool				IsThisTheMainSector;
		vsVec3ui			nodeMinDimensions;
		vsVec3i				voxelOffset;
	}						SECTOR;
	struct {
		vsShader*		shader;
		int				density;
		vsVec3f			windVec;
		uint			finTexture;
		vsVec2f			scale;
	}					GRASS;

} sector2Args_t;


/*
====================================================================


	vsSector2


====================================================================
*/
//	Sector Memsafe Function
/*
#define SMF_BEGIN { LockMemory();
#define SMF_END ReleaseMemoryLock(); }
#define SMF_RETURN( type, return_value ) type _smfret = return_value; LeaveCriticalSection();  return _smfret; }
*/
class vsGameObject;
class vsSector2 : public vsGameObject {

	friend class vsSector2OTNode;

public:
							vsSector2( void );
				explicit	vsSector2( const sector2Args_t& lsp );
							~vsSector2( void );

	int						Generate( const sector2Args_t& lsp );
	void					Destroy( void );
	void					Render( void );
	void					RenderVoxelData( void );
	int						PutBlock( const vsVec3ui& xyz, ushort blockType );
	ushort			RemoveBlock( const vsVec3ui& xyz );
	int						PutMultipleBlocks( vsVec3i coords[], int len, ushort blockType );
	int						RemoveMultipleBlocks( vsVec3i coords[], int len );
	void					SaveToFile( char* saveFolder );
	void					RestoreFromFile( char* restoreFolder, const vsVec3i& coords, bool keepGridSize = true, uint newGridSize = 1 );
	void					SetModificationState( bool isModified );
	void					FetchNeighborsFromList( const vsList<vsSector2*>& list );
	void					UpdateBuffers( void );
	void					Refresh( bool mainSector, bool physicsTest, bool generated );		// NULLs all neighbors
	
	const vsVec3f&			GetCellDimensions( void ) const;
	vsSector2OTNode*		GetNodeOfPlayer( void );											// returns pointer to node in which the player is staying
	vsVec3i					GetLocalCellOfPoint( const vsVec3f& p );
	vsVec3ui				GetLocalCellOfAbsoluteCell( const vsVec3i& c );
	vsSector2OTNode*		GetNodeOfPoint( const vsVec3f& point );
	vsSector2OTNode*		GetLeafNodeOfCell( const vsVec3ui& cell, int& side, int resultNum );
	void					GetBoxOfCell( const vsVec3i& cell, vsBox* box );
	const ushort&			GetVoxelDataOfLocalCell( const vsVec3i& xyz ) const;
	const vsVec3i&			GetCoords( void ) const;
	const vsVec3i&			GetVoxelOffset( void ) const;
	const vsBox&			GetBounds( void ) const;
	const vsVec3f&			GetOffset( void ) const;
	const bool&				IsMainSector( void ) const;
	const bool&				IsGenerated( void ) const;
	bool					IsValidVoxelIndex( const vsVec3i& xyz );
	bool					IsAbsoluteCellInside( const vsVec3i& xyz );
	bool					IsPointInside( const vsVec3f& point );
	const bool&				IsModified( void ) const;
	void					Reset( void );
	const vsSector2OTNode*	GetNode( int i ) const;
	const uint&				GetNodeCount( void ) const;

private:
	void					NeighborPutBlock( const vsVec3ui& xyz, ushort blockType );
	void					NeighborRemoveBlock( const vsVec3ui& xyz );
	void					GenerateNew( const sector2Args_t& lsp );
	int						UpdateModifiedGeometry( const vsVec3ui& p );
	ushort&					VoxelDataAt( const vsVec3ui& xyz );
	void					SetupTriangle( int texCoords, ushort val, vsList<sector2Vertex_t>* pVert, uint* numVertices, const sector2GridCell_t& cell,
											int a, int b, int c, const vsVec3f& Normal, const blocktype_t* blocktype );

	void					GenerateOctree( void );
	uint					PrecalculateOctreeNodeCount( uint sX, uint sY, uint sZ, uint eX, uint eY, uint eZ );
	int						GenerateOctreeNode( uint sX, uint sY, uint sZ, uint eX, uint eY, uint eZ, int parentId );

	void					AllocVoxelData( uint width, uint height, uint depth );
	void					FreeVoxelData( void );
	void					AllocNodes( uint count );
	void					FreeNodes( void );
	int						GetPointsNode( int nodeId, const vsVec3f& point );		// returns the id of node in which the player is staying

	void					GenerateVoxels( void );

	uint					Node_CreateGeometry( int id, bool keepGridSize=true, byte newGridSize=1 );
	void					Node_UpdateBuffers( int id, uint oldVertexCount=0, uint oldIndexCount=0 );
	void					Node_UpdateVisibility( int id );
	void					Node_Render( int id );
	void					Node_RenderBasis( int id );
	void					Node_RenderTree( int id );

private:
	bool					generated;
	vsVec3i					coord;
	vsVec3i					voxelOffset;
	vsBox					bounds;
	bool					mainSector;
	vsVec3f					offset;


	int						numVisibleNodes;
	int						numVisibleLeafNodes;

	vsSector2*				neighbors[6];
	vsShader*				shader;
	vsVec3ui				dimensions;
	vsVec3ui				realDimensions;
	ushort***				voxelData;
	uint					gridSize;
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
	grassArgs_t				Grass;			// parentSectorNode property left "empty"

	bool					newSector;
};


/* 
=====================================
	IsGenerated
=====================================
*/
VS_INLINE
const bool& vsSector2::IsGenerated( void ) const {
	return generated;
}

/* 
=====================================
	Reset
=====================================
*/
VS_INLINE
void vsSector2::Reset( void ) {
	generated = false;
}

/* 
=====================================
	GetCoords
=====================================
*/
VS_INLINE
const vsVec3i& vsSector2::GetCoords( void ) const {
	return coord;
}

/* 
=====================================
	GetVoxelOffset
=====================================
*/
VS_INLINE
const vsVec3i& vsSector2::GetVoxelOffset( void ) const {
	return voxelOffset;
}

/* 
=====================================
	GetBounds
=====================================
*/
VS_INLINE
const vsBox& vsSector2::GetBounds( void ) const {
	return bounds;
}

/* 
=====================================
	GetOffset
=====================================
*/
VS_INLINE
const vsVec3f& vsSector2::GetOffset( void ) const {
	return offset;
}

/* 
=====================================
	IsMainSector
=====================================
*/
VS_INLINE
const bool& vsSector2::IsMainSector( void ) const {
 return mainSector;
}

/*
===================================
	GetCellDimensions
		Returns scale / ( dimensions-1 )
===================================
*/
VS_INLINE
const vsVec3f& vsSector2::GetCellDimensions( void ) const {
	return scale_1;
}


/*
===================================
	IsValidVoxelIndex
		Returns wheter xyz is valid for using it
		as an index for voxeldata
===================================
*/
VS_INLINE
bool vsSector2::IsValidVoxelIndex( const vsVec3i& xyz ) {
	return( xyz >= vsVec3i( 0 ) && xyz < dimensions.ToVec3i() );
}

/*
===================================
	IsAbsoluteCellInside
===================================
*/
VS_INLINE
bool vsSector2::IsAbsoluteCellInside( const vsVec3i& xyz ) {
	return ( xyz >= voxelOffset && xyz < voxelOffset + realDimensions.ToVec3i() );
}

/*
===================================
	IsPointInside
		Is a point inside the sectors bounds?
===================================
*/
VS_INLINE
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
VS_INLINE
const bool& vsSector2::IsModified( void ) const
{
	return modified;
}

/*
===================================
	SetModificationState
		Sets wether the landscape has been modified or not
===================================
*/
VS_INLINE
void vsSector2::SetModificationState( bool isModified )
{
	modified = isModified;
}



/*
===================================
	GetBoxOfCell
		Returns the BOUNDING BOX of a given VOXEL COORDINATE
===================================
*/
VS_INLINE 
void vsSector2::GetBoxOfCell( const vsVec3i& cell, vsBox* box )
{
	vsVec3i c = cell;
	vsVec3f a = offset + ( c.ToVec3f() - vsVec3f( 0.5f ) ) * scale_1;
	vsVec3f b = offset + ( c.ToVec3f() + vsVec3f( 0.5f ) ) * scale_1;

	*box = vsBox( a, b );
}

/*
===================================
	GetVoxelDataOfLocalCell()
		Public function for getting voxel value at "xyz"
		BEWARE: NO LIMIT CHECK!
===================================
*/
VS_INLINE
const ushort& vsSector2::GetVoxelDataOfLocalCell( const vsVec3i& xyz ) const {
	return voxelData[xyz.x][xyz.y][xyz.z];
}


/*
===================================
	GetLocalCellOfAbsoluteCell
===================================
*/
VS_INLINE
vsVec3ui vsSector2::GetLocalCellOfAbsoluteCell( const vsVec3i& c ) {
	return ( c - voxelOffset ).ToVec3ui();
}

/*
===================================
	GetNodeCount
===================================
*/
VS_INLINE
const uint& vsSector2::GetNodeCount( void ) const {
	return nodes.Count;
}

/*
===================================
	GetNode
===================================
*/
VS_INLINE
const vsSector2OTNode* vsSector2::GetNode( int i ) const {
	return nodes[ i ];
}

#endif