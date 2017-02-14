#include "Math.h"


float vsMath::Clamp( float& s, float min, float max ) {
	if ( s<min ) s = min;
	if ( s>max ) s = max;
	return s;
}

float vsMath::Sign( float s ) {
	if ( s == 0.0f ) return 0.0f;
	if ( s > 0.0f ) return 1.0f;
	return -1.0f;
}

float vsMath::Sin( float x ) {
	return sinf( x );
}

float vsMath::Cos( float x ) {
	return cosf( x );
}

float vsMath::Tan( float x ) {
	return tanf( x );
}

void vsMath::SinCos( float a, float& s, float& c ) {
	s = sinf( a );
	c = cosf( a );
}

float vsMath::ASin( float x ) {
	return asinf( x );
}

float vsMath::ACos( float x ) {
	return acosf( x );
}

float vsMath::ATan( float x ) {
	return atanf( x );
}

float vsMath::Ctg( float x ) {
	return ( 1.0f/tanf( x ) );
}


float vsMath::Sqrt( float x ) {
	return sqrtf( x );
}

float vsMath::InvSqrt( float x ) {
	return ( 1.0f/sqrtf( x ) );
}

float vsMath::Pow( float x, float y ) {
	return powf( x, y );
}

float vsMath::Exp( float x ) {
	return expf( x );
}

float vsMath::Log( float x ) {
	return logf( x );
}

float vsMath::Floor( float x ) {
	return floorf( x );
}

float vsMath::Ceil( float x ) {
	return ceilf( x );
}

float vsMath::Abs( float x ) {
	return fabs( x );
}

/*
=============================================
	PointInTriangle ( slower approach )
=============================================
*/
int vsMath::PointInTriangle_Traditional( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& p3, const vsVec3f& point, const float& epsilon ) 
{
	vsVec3f v1, v2, v3;

	v1 = point-p1;
	v2 = point-p2;
	v3 = point-p3;
	v1.Normalize();
	v2.Normalize();
	v3.Normalize();

	float a, b, c;
	a = ACos( v1.Dot( v2 ) );
	b = ACos( v2.Dot( v3 ) );
	c = ACos( v3.Dot( v1 ) );

	float cosEq = Cos( a+b+c - 2.f*PI );

	if ( Abs( a+b+c - 2*PI ) <= epsilon ) return INSIDE;
	else
		if ( Abs( a+b+c - PI ) <= epsilon ) return ON_EDGE;
		else
			return OUTSIDE;
}

/*
=============================================
	PointInTriangle ( fast approach )
=============================================
*/
int vsMath::PointInsideTriangle( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& p3, const vsVec3f& point )
{
	if ( !Line_SameSide( point, p3, p1, p2 ) )
		return false;
	if ( !Line_SameSide( point, p1, p2, p3 ) )
		return false;
	if ( !Line_SameSide( point, p2, p3, p1 ) )
		return false;

	return true;
}

/*
=============================================
	Line_SameSide
		Returns true if point1 & point2 are on the same side of 
		the line defined by points lineA & lineB.

		Note: should have some epsilon parameter
=============================================
*/
bool vsMath::Line_SameSide( const vsVec3f& point1, const vsVec3f& point2, const vsVec3f& lineA, const vsVec3f& lineB )
{
	vsVec3f line = lineB-lineA;
	vsVec3f cp1 = line.Cross( point1-lineA );
	vsVec3f cp2 = line.Cross( point2-lineA );

	return( cp1.Dot( cp2 ) >= 0.0f );
}

/*
=============================================
	IsPointInSphere
=============================================
*/
bool vsMath::IsPointInSphere( const vsVec3f& sphereOrigin, float radius, const vsVec3f& point ) 
{
	return ( ( point-sphereOrigin ).Length() <= radius ) ;
}

/*
=============================================
	IntersectRaySphere
		Assumes rayDir is unit vector.
		Returns t of sphereOrig + t*rayDir = intersectionPoint
=============================================
*/
float vsMath::IntersectRaySphere( const vsVec3f& sphereOrig, float radius, const vsVec3f& rayOrig, const vsVec3f& rayDir, vsVec3f& result )
{
	vsVec3f v = sphereOrig - rayOrig;
	float len2 = v.LengthSqr();
	float a = v.Dot( rayDir );
	float d = radius*radius - ( len2 - a*a );

	if ( d<0.0f ) 
		return -1.0f;

	a -=  sqrt( d );
	
	result = rayOrig + a * rayDir;
	return a;
}

/*
=============================================
	ClosestPointOnTriangle
=============================================
*/
vsVec3f vsMath::ClosestPointOnTriangle( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& p3, const vsVec3f& point )
{
	vsVec3f c1 = ClosestPointOnLine( p1, p2, point );
	vsVec3f c2 = ClosestPointOnLine( p2, p3, point );
	vsVec3f c3 = ClosestPointOnLine( p3, p1, point );

	float dc[3];
	dc[0] = ( point-c1 ).Length();
	dc[1] = ( point-c2 ).Length();
	dc[2] = ( point-c3 ).Length();

	float min = dc[0];
	vsVec3f v3min = c1;
	if ( dc[1]<min ) { min=dc[1]; v3min=c2; }
	if ( dc[2]<min ) { min=dc[2]; v3min=c3; }

	return( v3min );
}

/*
=============================================
	ClosestPointOnLine
=============================================
*/
vsVec3f vsMath::ClosestPointOnLine( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& point )
{
	vsVec3f v1 = p2-p1;
	vsVec3f v2 = point-p1;

	float len_v1 = v1.Length();
	v1.Normalize();
	float t = v1.Dot( v2 );

	// Clamp t [0 len_v1 ]
	if ( t<=0.0f  ) return p1;
	if ( t>=len_v1 ) return p2;

	return( p1+v1*t );
}