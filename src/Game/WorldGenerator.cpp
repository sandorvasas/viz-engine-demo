#include "WorldGenerator.h"
#include <ctime>
#include "Game.h"
#include "Blocks.h"

vsWorldGenerator* worldGenerator;

/*
===================================
	vsWorldGenerator
===================================
*/
vsWorldGenerator::vsWorldGenerator( void )
{
	GenerateRandomList();
}

/*
===================================
	~vsWorldGenerator
===================================
*/
vsWorldGenerator::~vsWorldGenerator( void )
{
}

/*
===================================
	SetParameters
===================================
*/
void vsWorldGenerator::SetParameters( const worldParams_t& p )
{
	this->params = p;
}

/*
===================================
	RestoreFromFile
===================================
*/
void vsWorldGenerator::RestoreFromFile( vsInputFile* f )
{
	terrainArctic.RestoreFromFile( f );
	terrainTaiga.RestoreFromFile( f );
	terrainGrassland.RestoreFromFile( f );
	terrainDesert.RestoreFromFile( f );
	terrainForest.RestoreFromFile( f );
	terrainSavanna.RestoreFromFile( f );
	biome.RestoreFromFile( f );
}

/*
===================================
	SaveToFile
===================================
*/
void vsWorldGenerator::SaveToFile( vsOutputFile * f )
{
	terrainArctic.SaveToFile( f );
	terrainTaiga.SaveToFile( f );
	terrainGrassland.SaveToFile( f );
	terrainDesert.SaveToFile( f );
	terrainForest.SaveToFile( f );
	terrainSavanna.SaveToFile( f );
	biome.SaveToFile( f );
}

block_t vsWorldGenerator::TerrainAt( int x, int y, int z ) {
	float fx = float( x ) * 0.0005f;
	float fz = float( z ) * 0.0005f;

	// Biome ---------------------------------------------------------------

	float temperature = 1.0f;

	temperature -= biome.Noise2D( fx, fz );

	temperature *= 60.0f;
	temperature -= 40.0f;
	Clamp( temperature, -29.0f, 60.0f );

	if ( 30.0f < temperature && temperature <= 60.0f )
		return Biome_Desert( x, y, z );

	if ( 10.0f < temperature && temperature <= 40.0f )
		return Biome_Grassland( x, y, z );

	if ( -30.0f < temperature && temperature <= 10.0f )
		return Biome_Taiga( x, y, z );
	
	return block_t( 0, 0 );

}

/*
===================================
	GenerateRandomList
===================================
*/
void vsWorldGenerator::GenerateRandomList( void ) 
{
	srand( ( uint )::time( NULL ) );

	REPU2 ( j, 4 ) {
		randomList[ j ].Alloc( 50 );

		REPU ( randomList[ j ].Length() ) 
			randomList[ j ][ i ] = ( uint )( rand() % 100 );
	}
}



/*
===================================
	TerrainAt
===================================
*/
block_t vsWorldGenerator::TerrainAt( const vsVec3i& coords )
{
	return TerrainAt( coords.x, coords.y, coords.z );
}


/*
===================================
	Biome_Arctic
===================================
*/
block_t vsWorldGenerator::Biome_Arctic( int x, int y, int z )
{
	(x);(y);(z);
	return block_t( 0,0 );
}

/*
===================================
	Biome_Snow
===================================
*/
block_t vsWorldGenerator::Biome_Taiga( int x, int y, int z )
{
	float fx = float( x ) * 0.04f;
	float fz = float( z ) * 0.04f;

	float f = terrainTaiga.Noise2D( fx, 0.5f * fz );

	int F = int( 5.0f + f * 3.0f );

	if ( y > F )
		return block_t( 0,0 );

	if ( y > F )
		if ( y <= 0 )
			return block_t( blockLibrary->GetBlockTypeId( "ice" ), 0 );
		else
			return block_t( 0,0 );

	if ( y == F )
		return block_t( blockLibrary->GetBlockTypeId( "snow" ), 0 );

	if ( y < F )
		return block_t( blockLibrary->GetBlockTypeId( "soil" ), 0 );

	return block_t( 0,0 );
}

/*
===================================
	Biome_Hills
===================================
*/
block_t vsWorldGenerator::Biome_Grassland( int x, int y, int z )
{
	float fx = float( x ) * 0.04f;
	float fz = float( z ) * 0.04f;

	float f = terrainGrassland.Noise2D( fx, fz );

	for ( float i = 1.0f, j = 1.0f; i < 5.0f; i++, j += 0.5f ) {
		f += terrainGrassland.Noise2D( i * fx * 0.3f, j * fz * 0.3f );
	}

	int F = int( 5.0f + f * 7.0f * ( terrainGrassland.Noise2D( fx*0.1f, fz*0.1f ) + 2.0f ) );
	
	//int put_pumpkin = ( bool )( randomList[ 0 ][ ( x * z + y ) % 50 ] % 5 );

	if ( y > F )
		if ( y <= 0 )
			return block_t( blockLibrary->GetBlockTypeId( "still_water" ), 0 );
		else
		/*	if ( y == F + 1 && put_pumpkin )
				return block_t( blockLibrary->GetBlockTypeId( "pumpkin" ), BLOCK_DIR_NORTH );
			else*/
				return block_t( 0,0 );

	if ( y == F )
		return block_t( blockLibrary->GetBlockTypeId( "grass" ), 0 );

	if ( y < F )
		return block_t( blockLibrary->GetBlockTypeId( "soil" ), 0 );

	return block_t( 0,0 );
}

/*
===================================
	Biome_Mountains
===================================
*/
block_t vsWorldGenerator::Biome_Mountains( int x, int y, int z )
{(x);(y);(z);
	return block_t( 0,0 );
}

/*
===================================
	Biome_Desert
===================================
*/
block_t vsWorldGenerator::Biome_Desert( int x, int y, int z )
{
	float fx = float( x ) * 0.03f;
	float fz = float( z ) * 0.03f;

	float f = terrainDesert.Noise2D( fx, fz );

	int F = int( 5.0f + f * 4.0f );

	if ( y > F )
		return block_t( 0,0 );

	if ( y <= F  && y > F - 30 )
		return block_t( blockLibrary->GetBlockTypeId( "sand" ), 0 );

	if ( y <= F - 30 )
		return block_t( blockLibrary->GetBlockTypeId( "soil" ), 0 );

	return block_t( 0,0 );
}

/*
===================================
	Biome_Forest
===================================
*/
block_t vsWorldGenerator::Biome_Forest( int x, int y, int z )
{(x);(y);(z);
	return block_t( 0,0 );
}

/*
===================================
	Biome_Savanna
===================================
*/
block_t vsWorldGenerator::Biome_Savanna( int x, int y, int z )
{(x);(y);(z);
	return block_t( 0,0 );
}

/*
===================================
	Plant
===================================
*/
void vsWorldGenerator::Plant( block_t* chunkBlocks[ chunkDimX ][ chunkDimY ][ chunkDimZ ] ) 
{
/*	int times = randomList[ 0 ][ 6 ];

	REPEAT( times )
	{
		for ( int y = chunkDimY - 1; y > 0; y-- ) {
			if ( blockLibrary->GetBlockType( ( *chunkBlocks )[ x ][ y ][ z ] )->flags & BLOCK_FLAG_LIQUID )
				break;

			if ( *chunkBlocks )[ x ][ y ][ z ] != 0 ) {
				//if ( x > 3 
			}
		}
	}*/
}