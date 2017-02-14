#ifndef __VS_WORLD_H__
#define __VS_WORLD_H__

#include "Geometry\Skydome.h"
#include "Geometry\Grass.h"
#include "Geometry\Model_MD5.h"
#include "ParticleSystem.h"
#include "AI\AI.h"
#include "Fx\FX.h"
#include "..\System\Threading.h"
#include "Geometry\Chunk.h"

class vsGrass;
class vsPlayer;


struct chunkOffBox_t {
	vsVec3i	offset;
	vsBox	box;
	bool	loaded;

	EMPTY_TRUCTORS( chunkOffBox_t );
	MINIMUM_OPERATORS( chunkOffBox_t );
	
	// operators for sorting
	bool operator > ( const chunkOffBox_t& r ) {
		return ( offset.LengthSqr() > r.offset.LengthSqr() );
	}

	bool operator < ( const chunkOffBox_t& r ) {
		return ( offset.LengthSqr() < r.offset.LengthSqr() );
	}
};

/*
===============================
	chunkRequest_t
===============================
*/
struct chunkRequest_t {
	EMPTY_TRUCTORS( chunkRequest_t );
	MINIMUM_OPERATORS( chunkRequest_t );

	bool		main;
	vsVec3i		coord;

	chunkRequest_t( bool _main, vsVec3i _coord ) : 
		coord( _coord ), 
		main( _main ) {};
};


enum viewDistance_t {
	VIEWDIST_TINY = 3,
	VIEWDIST_SHORT = 5,
	VIEWDIST_NORMAL = 9,
	VIEWDIST_FAR = 17,
	VIEWDIST_REALISTIC = 27
};

enum collisionInfo_t {
	COLLISION_INFO_NOT_COLLIDED = 0x00000000,
	COLLISION_INFO_COLLIDED		= 0x00000001,
	COLLISION_INFO_ONGROUND		= 0x00000010
};

/*
====================================================================

	vsWorld

====================================================================
*/
class vsWorld {

	friend class vsConsoleProcessor;

public:
								vsWorld( void );
								~vsWorld( void );

	void						Initialize( void );
	void						RestoreFromFolder( char* szFolderName );
	void						SaveToFolder( char* szFolderName );
	void						SpawnPlayer( void );
	void						SetZone( const vsVec3i& zone );
	void						Update( void );
	void						Render( void );
	uint						MapAt( const vsVec3i& coords );
	FILE*						OpenMapAt( const vsVec3i& coords );

	void						CollideWithWorld( void );
	byte						TraceBox( const vsBox& box, vsVec3f& velocity );
	
	void						LoadChunk( vsChunk * chunk, const vsVec3i& coords );	// generates or restores chunk from file
	int							FindInactiveChunk( const vsVec3i& chunkCoord );
	void						SortChunks( void );

	void						SetViewDistance( const viewDistance_t& vd );
	void						SetCellLife( const vsVec3i& cell, float life );
	float						GetCellLife( const vsVec3i& cell );
	block_t						GetBlockOfCell( const vsVec3i& cell ) const;
	vsVec3i						GetCellOfPoint( const vsVec3f& point );
	vsVec3i						GetChunkCoordOfPoint( const vsVec3f& point );
	vsChunk*					GetChunkOfPoint( const vsVec3f& point );
	vsChunk*					GetChunkOfCell( const vsVec3i& cell );
	vsChunk*					GetCurrentChunk( void );
	vsChunk*					GetChunk( int chunkId );
	vsChunk*					GetChunk( const vsVec3i& chunkCoord );
	void						GetBoxOfCell( const vsVec3i& cell, vsBox* box );

	float						GetDayTime( void ) const		{ return dayTime; };
	vsVec3f						GetSunPosition( void )			{ return sky.GetSunPosition( dayTime ); };
	
	// Chunk loader thread -----------------------------------------------------------------
	void						ChunkLoaderProc( void );
	static uint WINAPI			GhostChunkLoaderThreadProc( void* );

private:
	vsVec3i						curChunkCoords;			
	uint						numChunks;
	vsList<vsChunk*>			inactiveChunks;
	vsList<vsChunk*>			chunks;	
	int							chunkViewDist;
	uint						curChunkIndex;

	char*						saveFolder;
	bool						playerSpawned;

	// NPCs
	vsList<vsMonster*>			monsters;

	float						dayTime;								// in hours, but really doesn't matter
	vsVec3f						gravity;

	// Chunk loading -------------------------------------------------
	bool						eofChunkLoaderThread;
	HANDLE						chunkLoaderThread;

	bool						allChunksLoaded;
	vsList< chunkOffBox_t >		chunkOffBoxes;

	struct {
		bool					available;
		vsList<chunkRequest_t>	chunkLoad;
		vsList<chunkRequest_t>	chunkUpdate;
	}	request;

public:
	vsParticleSystem*			particleSystem;

	vsVec3f						lightOffset;
	vsVec3f						lightPos;

	vsLensFlare					fx_lensFlare;
	vsAtmosphere				sky;
	vsSharedSection				ssChunks;
};


/*
===========================
	GetBoxOfCell
===========================
*/
VS_INLINE
void vsWorld::GetBoxOfCell( const vsVec3i& cell, vsBox* box ) {
	box->FromMinMax( cell.ToVec3f(), ( cell + 1 ).ToVec3f() );
}

/*
===========================
	GetChunkOfCell
===========================
*/
VS_INLINE
vsChunk* vsWorld::GetChunkOfCell( const vsVec3i& cell )
{
	if ( ssChunks.TryEnter() ) {
		REPU ( chunks.Length() ) {
			if ( chunks[ i ]->CellInside( cell ) ) {
					ssChunks.Leave();
					return chunks[ i ];
				}
		}
		ssChunks.Leave();
	}
	
	return NULL;
}

/*
===========================
	GetChunk
===========================
*/
VS_INLINE
vsChunk* vsWorld::GetChunk( int chunkId )
{
	return chunks[ chunkId ];
}

/*
===========================
	GetCurrentChunk
===========================
*/
VS_INLINE
vsChunk* vsWorld::GetCurrentChunk( void )
{
	return chunks[ curChunkIndex ];
}

/*
===========================
	GetChunkOfPoint
===========================
*/
VS_INLINE
vsChunk* vsWorld::GetChunkOfPoint( const vsVec3f& point )
{
	if ( ssChunks.TryEnter() ) {
		REPU ( chunks.Length() ) 
			if ( chunks[ i ]->PointInside( point ) ) {
				ssChunks.Leave();
				return chunks[ i ];
			}

		ssChunks.Leave();
	}

	return NULL;
}

/*
===========================
	GetChunkCoordOfPoint
===========================
*/
VS_INLINE
vsVec3i vsWorld::GetChunkCoordOfPoint( const vsVec3f& point )
{
	return ( point * invChunkDim ).Floor().ToVec3i();
}

/*
===========================
	GetCellOfPoint
===========================
*/
VS_INLINE
vsVec3i vsWorld::GetCellOfPoint( const vsVec3f& point )
{
	vsVec3i ret;
	if ( point.x < 0.0f ) 
			ret.x = ( int ) ( point.x - 0.5f );
	else if ( point.x > 0.0f ) ret.x = ( int ) ( point.x + 0.5f );
	else ret.x = 0;

	if ( point.y < 0.0f ) 
			ret.y = ( int ) ( point.y - 0.5f );
	else  if ( point.y > 0.0f ) ret.y = ( int ) ( point.y + 0.5f);
	else ret.y = 0;

	if ( point.z < 0.0f ) 
			ret.z = ( int ) ( point.z - 0.5f );
	else  if ( point.z > 0.0f )ret.z = ( int ) ( point.z + 0.5f );
	else ret.z = 0;

	return ret;
}


/*
===========================
	FindInactiveChunk
===========================
*/
VS_INLINE
int vsWorld::FindInactiveChunk( const vsVec3i& coord )
{
	REPU2( j, inactiveChunks.Length() )
	{
		if ( inactiveChunks[ j ] )
			if ( inactiveChunks[ j ]->GetChunkCoords() == coord )
				return j;		
	}

	return -1;
}


#endif