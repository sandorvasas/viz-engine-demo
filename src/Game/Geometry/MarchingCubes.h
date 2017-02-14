#ifndef __VS_MARCHING_CUBES_H__
#define __VS_MARCHING_CUBES_H__


#include "..\..\vsLib2\vsLib2.h"
using namespace vsLib2;


typedef struct marchingCubesGridCell_s {
	vsVec3f		vertices[8];
	byte		values[8];
} marchingCubesGridCell_t;


typedef struct marchingCubesArgs_s {
	float		isolevel;
	uint		gridSize;
	uint		voxelDataSize;
	vsShader*	shader;
} marchingCubesArgs_t;



class vsMarchingCubes {
public:
					vsMarchingCubes( void );
					~vsMarchingCubes( void );

	void			Generate( const marchingCubesArgs_t& a );
	void			GenerateSampleVoxelData( void );
	void			GenerateIsosurface( void );

	void			RenderVoxelData( void );
	void			Render( void );

private:
	int				Polygonize( const marchingCubesGridCell_t& cell );
	vsVec3f			InterpolateVertex( const marchingCubesGridCell_t& c, int edgeIndex );

public:
	float			isolevel;
	uint			size;
	uint			gridSize;
	byte***			voxelData;
	uint			numVertices;
	vsList<vsVertex>vertices;
	vsList<uint>	indices;
	vsShader*		shader;
};

#endif