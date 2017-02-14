#include "Plane.h"


vsPlane::vsPlane( void ) {
	Zero();
}

vsPlane::vsPlane( const vsVec3f& Normal, const vsVec3f& pointOnPlane ) {
	Set( Normal, pointOnPlane );
}

vsPlane::vsPlane( float a, float b, float c, float d ) {
	Set( a, b, c, d );
}

vsPlane::~vsPlane( void ) {
}

vsPlane& vsPlane::operator=( const vsPlane& p ) {
	a = p.a;
	b = p.b;
	c = p.c;
	d = p.d;
	return *this;
}

bool vsPlane::operator==( const vsPlane& p ) const
{
	return( p.a == a && p.b == b && p.c == c && p.d == d );
}

bool vsPlane::operator!=( const vsPlane& p ) const
{
	return!( *this == p );
}

vsPlane vsPlane::operator-( void ) const {
	return vsPlane( -a, -b, -c, -d );
}

vsPlane vsPlane::operator-( const vsPlane& p ) const {
	return vsPlane( a-p.a, b-p.b, c-p.c, d-p.d );
}

vsPlane vsPlane::operator+( const vsPlane& p ) const {
	return vsPlane( a+p.a, b+p.b, c+p.c, d+p.d );
}

vsPlane vsPlane::operator*( const vsPlane& p ) const {
	return vsPlane( a*p.a, b*p.b, c*p.c, d*p.d );
}

vsPlane& vsPlane::operator+=( const vsPlane& p ) {
	a += p.a;
	b += p.b;
	c += p.c;
	d += p.d;
	return *this;
}

vsPlane& vsPlane::operator*=( const vsPlane& p ) {
	a *= p.a;
	b *= p.b;
	c *= p.c;
	d *= p.d;
	return *this;
}

void vsPlane::Zero( void ) {
	a = b = c = d = 0.0f;
}

vsVec3f vsPlane::Normal( void ) const {
	return vsVec3f( a, b, c );
}

void vsPlane::Normalize( void ) {
	float len = 1.0f/sqrtf( a*a + b*b + c*c );
	a *= len; 
	b *= len;
	c *= len;
	d *= len;
}

void vsPlane::Set( float a, float b, float c, float d ) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

void vsPlane::Set( const vsVec3f& Normal, const vsVec3f& pointOnPlane ) {
	a = Normal.x;
	b = Normal.y;
	c = Normal.z;
	d = -( a * pointOnPlane.x + b * pointOnPlane.y + c * pointOnPlane.z );
}

void vsPlane::FromTriangle( const vsVec3f& p1, const vsVec3f& p2, const vsVec3f& p3 ) {
	vsVec3f v1, v2;
	v1 = p2 - p1;
	v2 = p3 - p1;
	FromVectors( v1, v2, p1 );
}

void vsPlane::FromVectors( const vsVec3f& v1, const vsVec3f& v2, const vsVec3f& p ) {
	vsVec3f n;
	n = ( ( vsVec3f )v1 ).Cross( v2 );
	n.Normalize();
	d = -n.Dot( p );
	a = n.x;
	b = n.y;
	c = n.z;
}

float vsPlane::Distance( const vsVec3f& v ) const {
	return ( a*v.x + b*v.y + c*v.z + d );
}

int vsPlane::ClassifyPoint( const vsVec3f& v, float epsilon ) const {
	float d = Distance( v );
	if ( d > epsilon ) {
		return PLANESIDE_FRONT;
	}

	if ( d < -epsilon ) {
		return PLANESIDE_BACK;
	}

	return PLANESIDE_ON;
}

vsVec3f vsPlane::ProjectOrthogonal( const vsVec3f& p ) const {
	vsVec3f P = p;
	vsVec3f N = Normal();
	return ( P - N*( P.Dot( N ) + d ) );
}

/*
===========================
	ProjectOblique
		Projects point p onto plane along UNIT vector ray
===========================
*/
float vsPlane::ProjectOblique( const vsVec3f& p, const vsVec3f& ray, vsVec3f& result ) const
{
	vsVec3f N = Normal();
	float denom = N.Dot( ray );

	if ( denom == 0.0f )
		return -1.0f;

	float numer = N.Dot( p ) + d;
	
	numer /= -denom;
	result = p + numer * ray;

	return numer;
}
