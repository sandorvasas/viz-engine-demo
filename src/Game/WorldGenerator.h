#ifndef __VS_WORLD_GENERATOR_H__
#define __VS_WORLD_GENERATOR_H__

#include "..\vsLib2\vsLib2.h"
#include "Perlin.h"
#include "Geometry\Chunk.h"

/*
===================================
	worldParams_t
===================================
*/
struct worldParams_t {
	MINIMUM_OPERATORS( worldParams_t );

	int		seaLevel;
};

/*
===================================
	terrainTypes enum
===================================
*/
enum terrainTypes {
	TERRAIN_ARCTIC = 0,
	TERRAIN_SNOW,
	TERRAIN_HILLS,
	TERRAIN_MOUNTAINS,
	TERRAIN_DESERT,
	TERRAIN_FOREST,
	TERRAIN_SAVANNA,
	TERRAIN_WATER,
	TERRAIN_ICYWATER,
	TERRAIN_MEADOWS
};

/*
======================================================================


	vsWorldGenerator


======================================================================
*/
class vsWorldGenerator {
public:
	vsWorldGenerator( void );
	~vsWorldGenerator( void );

	void			SetParameters( const worldParams_t& p );
	block_t			TerrainAt( const vsVec3i& coords );
	block_t			TerrainAt( int x, int y, int z );

	void			Plant( block_t* chunkBlocks[ chunkDimX ][ chunkDimY ][ chunkDimZ ] );

	void			SaveToFile( vsOutputFile * f );
	void			RestoreFromFile( vsInputFile * f );

private:
	block_t	Biome_Arctic( int x, int y, int z );
	block_t	Biome_Taiga( int x, int y, int z );
	block_t	Biome_Grassland( int x, int y, int z );
	block_t	Biome_Mountains( int x, int y, int z );
	block_t	Biome_Desert( int x, int y, int z );
	block_t	Biome_Forest( int x, int y, int z );
	block_t	Biome_Savanna( int x, int y, int z );

	// Level 1: scant .. Level 5: tons of ores
	block_t	Underground_RawMaterial_Level1( int x, int y, int z );
	block_t	Underground_RawMaterial_Level2( int x, int y, int z );
	block_t	Underground_RawMaterial_Level3( int x, int y, int z );
	block_t	Underground_RawMaterial_Level4( int x, int y, int z );
	block_t	Underground_RawMaterial_Level5( int x, int y, int z );

	void	GenerateRandomList( void );

private:
	vsPerlinNoise	terrainArctic;
	vsPerlinNoise	terrainTaiga;
	vsPerlinNoise	terrainGrassland;
	vsPerlinNoise	terrainDesert;
	vsPerlinNoise	terrainForest;
	vsPerlinNoise	terrainSavanna;
	vsPerlinNoise	biome;
	worldParams_t	params;
	vsList<uint>	randomList[4];
	int				randomNum;
};

extern vsWorldGenerator* worldGenerator;

#endif