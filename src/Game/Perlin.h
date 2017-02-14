#ifndef __VS_PERLIN_H__
#define __VS_PERLIN_H__


#include "..\vsLib2\vsLib2.h"

const int PERLIN_TABLE_SIZE = 512;

class vsPerlinNoise {
public:
					vsPerlinNoise( void );
					~vsPerlinNoise( void );

	void			Reseed( void );
	float			Noise1D( float x );
	float			Noise2D( float x, float y );
	float			Noise3D( float x, float y, float z );

	void			SaveToFile( vsOutputFile * f );
	void			RestoreFromFile( vsInputFile * f );

private:
	float			Curve( float t );
	void			GenerateLookupTables( void );
	float			RandomNoiseFloat( void );

private:
	float			gradientTable1D[ PERLIN_TABLE_SIZE ];
	float			gradientTable2D[ PERLIN_TABLE_SIZE ][ 2 ];
	float			gradientTable3D[ PERLIN_TABLE_SIZE ][ 3 ];
	unsigned		permTable[ PERLIN_TABLE_SIZE ];
};


VS_INLINE float vsPerlinNoise::Curve( float t ) {
	return ( 3.0f - 2.0f * t ) * t * t;
}

#endif