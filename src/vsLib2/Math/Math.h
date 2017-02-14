#ifndef __VS_MATH_H__
#define __VS_MATH_H__

#include <math.h>

#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "Plane.h"

//#include "..\NspStruct.h"

namespace vsMath 
{

	enum {
		INSIDE = 0,
		ON_EDGE,
		OUTSIDE
	};

	enum lineRetVal_t {
		LINE_SIDE_ON = 0,
		LINE_SIDE_SAME,
		LINE_SIDE_DIFFERENT
	};

	const float			_cos60 = 0.50f;
	const float			_cos30 = 0.86603f;
	const float			_cos45 = 0.70710f;
	const float			_sin45 = _cos45;
	const float			_sin30 = _cos60;
	const float			_sin60 = _cos30;
	const float			_tg45  = 1.0f;

	float						Clamp( float& s, float min, float max );
	template<typename T> T		Clamp( T& s, T min, T max );
	float						Sign( float s );
	template<typename T> T		Sign( T s );


	float						Sin( float x );
	float						Cos( float x );
	float						Tan( float x );
	void						SinCos( float a, float& s, float& c );
	float						ASin( float x );
	float						ACos( float x );
	float						ATan( float x );
	float						Ctg( float x );
	float						Sqrt( float x );
	float						InvSqrt( float x );
	float						Pow( float x, float y );
	float						Exp( float x );
	float						Log( float x );
	float						Floor( float x );
	float						Ceil( float x );
	float						Abs( float x );


	int							PointInTriangle_Traditional( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& p3, const vsVec3f& point, const float& epsilon );
	int							PointInsideTriangle( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& p3, const vsVec3f& point );
	bool						Line_SameSide( const vsVec3f& point1, const vsVec3f& point2, const vsVec3f& lineA, const vsVec3f& lineB );
	bool						IsPointInSphere( const vsVec3f& sphereOrigin, float radius, const vsVec3f& point );
	
	float						IntersectRaySphere( const vsVec3f& sphereOrig, float radius, const vsVec3f& rayOrig, const vsVec3f& rayDir, vsVec3f& result );

	vsVec3f						ClosestPointOnTriangle( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& p3, const vsVec3f& point );
	vsVec3f						ClosestPointOnLine( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& point );
};



template<typename T>
VS_INLINE 
T vsMath::Clamp( T& s, T min, T max ) {
	if ( s<min ) s = min;
	if ( s>max ) s = max;
	return s;
}

template<typename T> 
VS_INLINE
T vsMath::Sign( T s ) {
	if ( s>T( 0 ) ) return T( 1 );
	if ( s<T( 0 ) ) return T( -1 );
	
	return T( 0 );
}

#endif