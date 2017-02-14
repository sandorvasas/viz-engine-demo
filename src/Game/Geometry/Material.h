#ifndef __VS_MATERIAL_H__
#define __VS_MATERIAL_H__

#include "..\..\vsLib2\vsLib2.h"

/* 
=====================================

	MD5Material_t

=====================================
*/
struct vsMaterial {
				vsMaterial( void );
				~vsMaterial( void ) {};
				MINIMUM_OPERATORS ( vsMaterial );

	char		name[ 128 ];
	int			id;

	float		shininess;
	float		specular;
	float		ambient;
	uint		diffuseMap;
	uint		normalMap;
	uint		specularMap;
	vsShader*	shader;
};


VS_INLINE vsMaterial::vsMaterial( void ) {
	diffuseMap = normalMap = specularMap = 0;
	shininess = 0;
	ambient = 0;
	specular = 0;
	id = -1;
}

#endif