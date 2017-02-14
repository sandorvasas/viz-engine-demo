#ifndef __VS_FX_H__
#define __VS_FX_H__


#include "..\..\vsLib2\vsLib2.h"

/* 
=====================================

	vsLensFlare

=====================================
*/
class vsLensFlare {
public:
							vsLensFlare( void );
							~vsLensFlare( void );

	void					Initialize( uint textures[ 4 ], vsShader* shader );
	void					Render( const vsVec3f& lightSource );

private:
	void					Calculate( const vsVec3f& lightSource );

private:
	uint					textures[ 4 ];
	uint					buffer;
	vsShader*				shader;
};


#endif