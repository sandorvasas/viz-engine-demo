#ifndef __VS_WORLD_H__
#define __VS_WORLD_H__

#include "Geometry\Skydome.h"
#include "Geometry\Sector.h"


class vsPlayer;

typedef struct worldArgs_s {
	uint				nSectorSize;
	vsVec3				v3SectorTranslation;
	vsVec3				v3SectorScale;
	vsVec3				v3SectorTexScale;
	byte				eTerrainType;
	vsPlayer*			pPlayer;
	bool				bNewWorld;
} worldArgs_t;

// SECTORMAP DIMENSIONS
const byte MAP_W = 33;
const byte MAP_H = 33;

//=============================
//		WORLD
//=============================
class vsWorld {
public:
	vsWorld( void );
	~vsWorld( void );

	void				Initialize( const worldArgs_t& args );
	vsSector*			RestoreSectorFromFile( byte x, byte y );
	void				RestoreFromFile( vsInputFile* f, vsPlayer* player );
	void				SaveToFile( vsOutputFile* f );
	uint				SaveSectorToFile( byte renderSectorIndex ); // returns offset

	void				SpawnPlayer( void );

	vsSector*			LoadSector( byte x, byte y, bool forRendering=false );
	void				SetZone( byte x, byte y );
	void				UpdatePhysics( void );
	void				Render( void );
	void				LocatePlayer( void );
private:
	void				GenNewRenderSector( byte index, byte x, byte y );
	void				LoadNewRenderSector( byte index, byte x, byte y );
	void				GenLoadSector( byte index, byte x, byte y );

private:
	uint				offsetmap[MAP_W][MAP_H];
	byte				curX, curY;			//sector coords
	uint				numSectors;
	byte*				storage;
	vsSector			renderSectors[9];	//fl, ff, fr, ll, cur, rr, bl, bb, br
	sectorArgs_t		sectorArgs;
	char*				saveFileName;
	vsPlayer*			player;
	vsSkyDome			sky;
	vsList<vsShader*>	shaders;
	float				viewDistance; 
	bool				rs_used[9];	// stores which renderSectors are used to avoid deleting them
};

#endif