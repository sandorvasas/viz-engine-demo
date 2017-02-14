#ifndef __SKYDOME_H__
#define __SKYDOME_H__


#include "..\..\vsLib2\vsLib2.h"

using namespace vsMath;


struct sunVert_t {
	vsVec3f pos;
	vsVec2f	st;
};
/*
============================

	vsSun

============================
*/
class vsSun {

	friend class vsAtmosphere;

public:
	vsSun( void );
	~vsSun( void ) {};

	void			Init( float distanceFromPlayer, float size, int numRays, vsShader* shader,
						  uint sunTex, uint rayTex );
	void			Render( float dayTime );

private:
	void			CreateSunGeometry( void );

private:
	vsList<sunVert_t>baseVertices;
	vsList<sunVert_t>vertices;
	float			size;
	vsVec3f			basePosition;
	int				numRays;
	vsShader*		shader;
	uint			sunTex;
	uint			rayTex;
	float			dayTime;
	float			rayScale;
};

/*
============================

	vsAtmosphere

============================
*/
class vsAtmosphere {
public:
	vsAtmosphere( void );
	~vsAtmosphere( void );

	VSRESULT			BuildGeometry( uint sides, uint slices, const vsVec3f& radius,
									   uint texture, vsShader* pShader );
	void				Render( float dayTime );
	vsVec3f				GetSunPosition( float dayTime );
		
private:
	enum {
		VERTEX_BUFFER = 0,
		INDEX_BUFFER
	};
	uint					vbo[2];
	uint					texture;
	uint					slices;
	uint					slices1;
	uint					sides;
	vsShader*				pShader;
	vsSun					sun;
};

#endif