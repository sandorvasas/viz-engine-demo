#ifndef __VS_CHUNK_H__
#define __VS_CHUNK_H__

#include "..\Blocks.h"
#include "..\Physics\Physics.h"
#include "..\..\vsLib2\vsLib2.h"

const int chunkDimX = 16;
const int chunkDimY = 16;
const int chunkDimZ = 16;

const vsVec3i chunkDim = vsVec3i( chunkDimX, chunkDimY, chunkDimZ );
const vsVec3f invChunkDim = 1.0f / chunkDim.ToVec3f();

const float ONE_OVER_EIGHT = 1.0f / 8.0f;

/*
============================
	chunkGridCell_t
============================
*/
typedef struct chunkGridCell_s 
{
	vsVec3ui		coords;
	ushort			values[ 8 ];
	vsVec3f			vertices[ 8 ];
} chunkGridCell_t;

/*
============================
	Chunk Vertex Attribs
============================
*/
enum {
	CVA_POSITION = 0,
	CVA_NORMAL,
	CVA_CELLLIFE,
	CVA_MEGATEXCOORDS,
	CVA_GHOSTTEXCOORDS,
	CVA_VALUE
};


/*
========================================================

	vsChunk

========================================================
*/
class vsChunk {
public:
						vsChunk( void );
						~vsChunk( void );

	void				Generate( const vsVec3i& _chunkCoord, const char* szRestoreFolder );
	void				SaveToFolder( const char* szFolder );
	void				RestoreFromFolder( const char* szFolder, const vsVec3i& coords );
	void				Render( void );

	void				PutBlock( const vsVec3i& toCell, const block_t& block );
	block_t				RemoveBlock( const vsVec3i& block );							// returns blocktype id

	bool				TraceBox( const vsBox& box, vsVec3f& velocity );	// returns new velocity

	void				SetId( int newId );
	void				SetCellLife( const vsVec3i& cell, float life );
	void		/*!*/	ChangeBlockAttributes( const vsVec3i& cell, bool blockType, bool extra, const block_t& block );

	const int&			GetId( void ) const;
	const vsVec3i&		GetChunkCoords( void ) const;
	const vsVec3i&		GetBlockCoords( void ) const;
	const vsBox&		GetBox( void ) const;
	const block_t&		GetBlockOfCell( const vsVec3i& cell ) const;
	const char*			GetBlockTypeNameOfCell( const vsVec3i& cell ) const;
	const float&		GetCellLife( const vsVec3i& cell ) const;

	bool				PointInside( const vsVec3f& point ) const;
	bool				CellInside( const vsVec3i& cell ) const;
	bool				BoxInside( const vsBox& box ) const;
	//bool				StableBlock( const vsVec3i& cell ) const;		// returns true if block != [ air, water, lava, etc ]
	const bool&			Modified( void ) const;
	
private:
	block_t				EdgeGetBlockOfCell( const vsVec3i& cell );
	void				SetupShader( void );
	void				GenerateGeometry( void );
	void				GenerateGeometryForBlock( const vsVec3i& cur, uint& numVertices, uint& numTransparentVertices );
	void				RemoveGeometryOfBlock( const vsVec3i& block, uint& pos, uint& len );
	//void				GetGeometryOfBlock( const vsVec3i& block, 
	void				UpdateBuffers( void );
	void				QueryVoxels( void );
	void				SetupTriangle( int texCoords, const block_t& block, const vsVec3i& cellCoord, vsList<vsVertex>* pVert, uint* numVertices, const chunkGridCell_t& cell,
										int a, int b, int c, const vsVec3f& Normal, const blocktype_t* blocktype );
	void				Event_BlockModified( const vsVec3i& block );
	
private:
	int					id;								// id to octree in vsWorld
	vsBox				box;							// boundaries of chunk
	vsVec3i				chunkCoord;						// chunk space coordinates
	vsVec3i				blockCoord;						// = chunkCoord * [ W H D ]
	block_t				blocks[ chunkDimX ][ chunkDimY ][ chunkDimZ ];

	vsList<vsVertex>	vertices;
	uint				buffer;
	vsList<vsVertex>	transparentVertices;
	uint				transparentBuffer;

	vsShader*			shader;

	vsList<vsModel_MD5*>nonsolidBlocks;

	bool				updateGeometry;
	bool				updateBuffers;
	bool				modified;

	uint				oldVertexCount;
	uint				oldLiquidVertexCount;
	vsList<vsBox>		aabbs;
	vsList<vsVec3f>		ipoints;
};


/* 
=======================================================================
	Minor functions
=======================================================================
*/

VS_INLINE void vsChunk::SetId( int newId ) {
	this->id = newId;
}


VS_INLINE const int& vsChunk::GetId( void ) const {
	return id;
}


VS_INLINE const vsVec3i& vsChunk::GetChunkCoords( void ) const {
	return chunkCoord;
}


VS_INLINE const vsVec3i& vsChunk::GetBlockCoords( void ) const {
	return blockCoord;
}


VS_INLINE const vsBox& vsChunk::GetBox( void ) const {
	return box;
}


VS_INLINE const block_t& vsChunk::GetBlockOfCell( const vsVec3i& cell ) const {
	vsVec3i local = cell - blockCoord;
	return blocks[ local.x ][ local.y ][ local.z ];
}


VS_INLINE const char* vsChunk::GetBlockTypeNameOfCell( const vsVec3i& cell ) const {
	vsVec3i local = cell - blockCoord;
	return blockLibrary->GetBlockType( blocks[ local.x ][ local.y ][ local.z ].id )->name;
}


VS_INLINE bool vsChunk::PointInside( const vsVec3f& point ) const {
	return box.IsPointInside( point );
}


VS_INLINE bool vsChunk::CellInside( const vsVec3i& cell ) const {
	return ( cell >= blockCoord && cell < blockCoord + chunkDim );
}


VS_INLINE bool vsChunk::BoxInside( const vsBox& box ) const {
	return this->box.IsBoxInside( box );
}


VS_INLINE const bool& vsChunk::Modified( void ) const {
	return modified;
}


#endif