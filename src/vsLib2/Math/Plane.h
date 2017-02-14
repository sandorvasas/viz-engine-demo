

#ifndef __CR2_PLANE_H__
#define __CR2_PLANE_H__

#include "Vector.h"
#include "Matrix.h"

#define PLANESIDE_FRONT 1
#define PLANESIDE_BACK -1
#define PLANESIDE_ON 0


class vsPlane {
public:
							vsPlane( void );
	explicit				vsPlane( const vsVec3f& Normal, const vsVec3f& pointOnPlane );
	explicit				vsPlane( float a, float b, float c, float d );
							~vsPlane( void );

	vsPlane&				operator=( const vsPlane& p );
	bool					operator==( const vsPlane& p ) const;
	bool					operator!=( const vsPlane& p ) const;
	vsPlane					operator-( void ) const;
	vsPlane					operator-( const vsPlane& p ) const;
	vsPlane					operator+( const vsPlane& p ) const;
	vsPlane					operator*( const vsPlane& p ) const;
	vsPlane&				operator+=( const vsPlane& p );
	vsPlane&				operator*=( const vsPlane& p );

	void					Zero( void );
	vsVec3f					Normal( void ) const;
	void					Normalize( void );
	void					Set( float a, float b, float c, float d );
	void					Set( const vsVec3f& Normal, const vsVec3f& pointOnPlane );
	void					FromTriangle( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& p3 );
	void					FromVectors( const vsVec3f& v1, const vsVec3f& v2, const vsVec3f& p );

	float					Distance( const vsVec3f& v ) const;
	int						ClassifyPoint( const vsVec3f& v, float epsilon ) const;
	vsVec3f					ProjectOrthogonal( const vsVec3f& p ) const;
	float					ProjectOblique( const vsVec3f& p, const vsVec3f& ray, vsVec3f& result ) const;

public:

	float					a, b, c, d;

};


#endif