#include "Perlin.h"
#include <time.h>

vsPerlinNoise::vsPerlinNoise( void ) {
	Reseed();
}

vsPerlinNoise::~vsPerlinNoise( void ) {
}

void vsPerlinNoise::Reseed( void ) {
	srand( ( unsigned )time( NULL ) );
	GenerateLookupTables();
}

float vsPerlinNoise::Noise1D( float x ) {
	int gridL, gridR;
	float distL, distR, f, u, v;
		
	x = fabs( x );

	if ( x > 255.0f )
		x = 255.0f - fmod( x, 255.0f );
	
	gridL = ( int ) floorf( x );
	gridR = ( int ) ceilf( x );

	distL = x - ( float ) gridL;
	distR = x - ( float ) gridR;

	f = Curve( distL );

	u = distL * gradientTable1D[ permTable[ gridL ] ];
	v = distR * gradientTable1D[ permTable[ gridR ] ];

	return u * ( 1.0f - f ) + v * f;
}

float vsPerlinNoise::Noise2D( float x, float y ) {
	x = fabs( x );
	y = fabs( y );

	float s, t, u, v;
	int floorX, floorY;

	floorX = ( int ) x;
	floorY = ( int ) y;

	int p1, p2, p3, p4;
	p1 = permTable[ floorX * permTable[ floorY % PERLIN_TABLE_SIZE ]  % PERLIN_TABLE_SIZE ];
	p2 = permTable[ ( floorX + 1 ) * permTable[ floorY % PERLIN_TABLE_SIZE ]  % PERLIN_TABLE_SIZE ];
	p3 = permTable[ floorX * permTable[ ( floorY + 1 ) % PERLIN_TABLE_SIZE ]  % PERLIN_TABLE_SIZE ];
	p4 = permTable[ ( floorX + 1 ) * permTable[ ( floorY + 1 ) % PERLIN_TABLE_SIZE ]  % PERLIN_TABLE_SIZE ];

	float dx0 = x - floorX;
	float dx1 = dx0 - 1.0f;
	float dy0 = y - floorY;
	float dy1 = dy0 - 1.0f;

	s = gradientTable2D[ p1 ][ 0 ] * dx0 + gradientTable2D[ p1 ][ 1 ] * dy0;
	t = gradientTable2D[ p2 ][ 0 ] * dx1 + gradientTable2D[ p2 ][ 1 ] * dy0;
	u = gradientTable2D[ p3 ][ 0 ] * dx0 + gradientTable2D[ p3 ][ 1 ] * dy1;
	v = gradientTable2D[ p4 ][ 0 ] * dx1 + gradientTable2D[ p4 ][ 1 ] * dy1;

	float curveX = Curve( x - floorX );
	float curveY = Curve( y - floorY );

	float int1 = s * ( 1.0f - curveX ) + t * curveX;
	float int2 = u * ( 1.0f - curveX ) + v * curveX;

	return int1 * ( 1.0f - curveY ) + int2 * curveY;
}

float vsPerlinNoise::Noise3D( float x, float y, float z ) {
	( x ); ( y ); ( z );
	return 0.f;
}

void vsPerlinNoise::GenerateLookupTables( void ) {
	REPEAT( PERLIN_TABLE_SIZE ) {
		permTable[ i ] = i;

		gradientTable1D[ i ] = RandomNoiseFloat();

		gradientTable2D[ i ][ 0 ] = RandomNoiseFloat();
		gradientTable2D[ i ][ 1 ] = RandomNoiseFloat();

		gradientTable3D[ i ][ 0 ] = RandomNoiseFloat();
		gradientTable3D[ i ][ 1 ] = RandomNoiseFloat();
		gradientTable3D[ i ][ 2 ] = RandomNoiseFloat();
	}

	REPEAT( PERLIN_TABLE_SIZE ) 
		vsSystem::Swap( permTable[ i ], permTable[ rand() % PERLIN_TABLE_SIZE ] );
}

float vsPerlinNoise::RandomNoiseFloat( void ) {
	return ( float )( rand() % ( 2 * PERLIN_TABLE_SIZE ) - PERLIN_TABLE_SIZE ) / PERLIN_TABLE_SIZE;
}

void vsPerlinNoise::SaveToFile( vsOutputFile * f )
{
	REPEAT( PERLIN_TABLE_SIZE ) {
		f->WriteFloat( &gradientTable1D[ i ] );
		f->WriteFloat( &gradientTable2D[ i ][ 0 ] );
		f->WriteFloat( &gradientTable2D[ i ][ 1 ] );

		f->WriteFloat( &gradientTable3D[ i ][ 0 ] );
		f->WriteFloat( &gradientTable3D[ i ][ 1 ] );
		f->WriteFloat( &gradientTable3D[ i ][ 2 ] );
		f->WriteUnsignedInt( &permTable[ i ] );
	}
}

void vsPerlinNoise::RestoreFromFile( vsInputFile * f )
{
	REPEAT( PERLIN_TABLE_SIZE ) {
		f->ReadFloat( &gradientTable1D[ i ] );
		f->ReadFloat( &gradientTable2D[ i ][ 0 ] );
		f->ReadFloat( &gradientTable2D[ i ][ 1 ] );

		f->ReadFloat( &gradientTable3D[ i ][ 0 ] );
		f->ReadFloat( &gradientTable3D[ i ][ 1 ] );
		f->ReadFloat( &gradientTable3D[ i ][ 2 ] );
		f->ReadUnsignedInt( &permTable[ i ] );
	}
}