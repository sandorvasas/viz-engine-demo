#ifndef __VS_PRIMITIVES_H__
#define __VS_PRIMITIVES_H__

#include "..\..\vsLib2\vsLib2.h"


class vsEllipsoid {
public:
	vsEllipsoid( void ) {};
	~vsEllipsoid( void ) {};



public:
	vsVec3f		radius;
	vsVec3f		origin;
};

#endif