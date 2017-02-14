
#ifndef __CR2_QUATERNION_H__
#define __CR2_QUATERNION_H__

#include "Vector.h"
#include "Matrix.h"

class vsQuaternion {
public:
							vsQuaternion( void );
							vsQuaternion( const vsQuaternion& q );
		explicit			vsQuaternion( float x, float y, float z, float w );
		explicit			vsQuaternion( const vsVec3f& v, float w );
		explicit			vsQuaternion( const vsVec3f& v );
							~vsQuaternion( void );

	vsQuaternion			operator*( const vsQuaternion& q );
	vsVec3f					operator*( const vsVec3f& v );
	vsQuaternion			operator*( const float& v );
	vsQuaternion			operator/( const float& f );
	vsQuaternion			operator+( const vsQuaternion& q );
	vsQuaternion			operator+( const vsVec3f& q );
	vsQuaternion			operator+( const float& q );
	vsQuaternion			operator-( const vsQuaternion& q );
	vsQuaternion			operator-( const vsVec3f& q );
	vsQuaternion			operator-( const float& q );
	vsQuaternion			operator-( void ) const;
	vsQuaternion&			operator*=( const vsQuaternion& q );
	vsQuaternion&			operator*=( const float& v );
	vsQuaternion&			operator+=( const vsQuaternion& q );
	vsQuaternion&			operator+=( const vsVec3f& q );
	vsQuaternion&			operator+=( const float& q );
	vsQuaternion&			operator-=( const vsQuaternion& q );
	vsQuaternion&			operator-=( const vsVec3f& q );
	vsQuaternion&			operator-=( const float& q );
	vsQuaternion&			operator=( const vsQuaternion& q );
	bool					operator==( const vsQuaternion& q ) const;
	bool					operator!=( const vsQuaternion& q ) const;
	bool					operator>( const vsQuaternion& q ) const;
	bool					operator<( const vsQuaternion& q ) const;

	friend vsQuaternion		operator*( const float& f, const vsQuaternion& q );
	friend vsVec3f			operator*( const vsVec3f& v, const vsQuaternion& q );
	friend vsVec3f			operator*( const vsVec3f& v, const vsQuaternion& q );
	friend vsVec3f			operator*=( const vsVec3f& v, const vsQuaternion& q );
	friend vsQuaternion		operator/( const float& f, const vsQuaternion& q );


	void					CreateFromEulerAnglesDEG( float x, float y, float z );
	void					CreateFromEulerAnglesDEG( const vsVec3f& v );
	void					CreateFromEulerAnglesRAD( float x, float y, float z );
	void					CreateFromEulerAnglesRAD( const vsVec3f& v );
	void					CalcW( void );
	vsQuaternion			Inverse( void ) const;
	vsQuaternion			Conjugate( void ) const;
	float					Length( void ) const;
	float					LengthSqr( void ) const;
	void					Normalize( void );
	float					Dot( const vsQuaternion& q ) const;
	vsQuaternion&			Lerp( const vsQuaternion& qa, const vsQuaternion& qb, float t );
	vsQuaternion&			Slerp( const vsQuaternion& qa, const vsQuaternion& qb, float t );
	
	float*					ToTypePtr( void );
	char*					ToCharPtr( void );
	vsVec3f					ToVec3( void ) const;
	vsMat3					ToMat3( void ) const;
	vsMat4					ToMat4( void ) const;

public:
	float	x, y, z, w;
};




VS_INLINE vsQuaternion::vsQuaternion( void ) {
	x = y= z= 0.0f;
	w=1.0f;
}

VS_INLINE vsQuaternion::vsQuaternion( const vsQuaternion& q ) {
	this->x = q.x;
	this->y = q.y;
	this->z = q.z;
	this->w = q.w;
}

VS_INLINE vsQuaternion::vsQuaternion( float x, float y, float z, float w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

VS_INLINE vsQuaternion::vsQuaternion( const vsVec3f& v, float w ) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = w;
}

VS_INLINE vsQuaternion::vsQuaternion( const vsVec3f& v ) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	CalcW();
}

VS_INLINE vsQuaternion::~vsQuaternion( void ) {
}


VS_INLINE vsQuaternion vsQuaternion::operator*( const float& v ) {
	return vsQuaternion( x*v,y*v, z*v, w*v );
}

VS_INLINE vsQuaternion operator*( const float& f, const vsQuaternion& q ) {
	return vsQuaternion( f*q.x, f*q.y, f*q.z, f*q.w );
}

VS_INLINE vsVec3f operator*( const vsVec3f& v, const vsQuaternion& q ) {
	return vsQuaternion( q )*v;
}

VS_INLINE vsVec3f operator*=( const vsVec3f& v, const vsQuaternion& q ) {
	return vsQuaternion( q )*v;
}

VS_INLINE vsQuaternion vsQuaternion::operator/( const float& f ) {
	float v = 1.0f/f;
	return vsQuaternion( x*v,y*v, z*v, w*v );
}

VS_INLINE vsQuaternion operator/( const float& f, const vsQuaternion& q ) {
	return vsQuaternion( f/q.x, f/q.y, f/q.z, q.w );
}

VS_INLINE vsQuaternion vsQuaternion::operator+( const vsQuaternion& q ) {
	return vsQuaternion( x + q.x, y + q.y, z + q.z, w + q.w );
}

VS_INLINE vsQuaternion vsQuaternion::operator+( const vsVec3f& v ) {
	return *this+vsQuaternion( v );
}

VS_INLINE vsQuaternion vsQuaternion::operator+( const float& v ) {
	return vsQuaternion( x+v,y+v, z+v, w+v );
}

VS_INLINE vsQuaternion vsQuaternion::operator-( const vsQuaternion& q ) {
	return vsQuaternion( x - q.x, y - q.y, z - q.z, w - q.w );
}

VS_INLINE vsQuaternion vsQuaternion::operator-( const vsVec3f& v ) {
	return *this-vsQuaternion( v );
}

VS_INLINE vsQuaternion vsQuaternion::operator-( const float& v ) {
	return vsQuaternion( x-v,y-v, z-v, w-v );
}

VS_INLINE vsQuaternion vsQuaternion::operator-( void ) const {
	return vsQuaternion( -x, -y, -z, -w );
}

VS_INLINE vsQuaternion& vsQuaternion::operator*=( const vsQuaternion& q ) {
	*this = *this * q;
	return *this;
}

VS_INLINE vsQuaternion& vsQuaternion::operator*=( const float& v ) {
	*this = *this * v;
	return *this;
}

VS_INLINE vsQuaternion& vsQuaternion::operator+=( const vsQuaternion& q ) {
	*this = *this + q;
	return *this;
}

VS_INLINE vsQuaternion& vsQuaternion::operator+=( const vsVec3f& v ) {
	*this = *this + v;
	return *this;
}

VS_INLINE vsQuaternion& vsQuaternion::operator+=( const float& v ) {
	*this = *this + v;
	return *this;
}

VS_INLINE vsQuaternion& vsQuaternion::operator-=( const vsQuaternion& q ) {
	*this = *this - q;
	return *this;
}

VS_INLINE vsQuaternion& vsQuaternion::operator-=( const vsVec3f& v ) {
	*this = *this - v;
	return *this;
}

VS_INLINE vsQuaternion& vsQuaternion::operator-=( const float& v ) {
	*this = *this - v;
	return *this;
}

VS_INLINE vsQuaternion& vsQuaternion::operator=( const vsQuaternion& q ) {
	this-> x = q.x;
	this-> y = q.y;
	this-> z = q.z;
	this-> w = q.w;
	return *this;
}

VS_INLINE bool vsQuaternion::operator==( const vsQuaternion& q ) const {
	return ( this->x == q.x && this->y == q.y && this->z == q.z && this->w == q.w );
}

VS_INLINE bool vsQuaternion::operator!=( const vsQuaternion& q ) const {
	return !( q == *this );
}

VS_INLINE bool vsQuaternion::operator<( const vsQuaternion& q ) const {
	return ( q.x>x && q.y>y && q.z>z && q.w>w );
}

VS_INLINE bool vsQuaternion::operator>( const vsQuaternion& q ) const {
	return ( q.x<x && q.y<y && q.z<z && q.w<w );
}


VS_INLINE float vsQuaternion::Dot( const vsQuaternion& q ) const {
	return x*q.x + y*q.y + z*q.z + w*q.w;
}


VS_INLINE float vsQuaternion::Length( void ) const {
	return sqrtf( x*x + y*y + z*z + w*w );
}


VS_INLINE float vsQuaternion::LengthSqr( void ) const {
	return x*x + y*y + z*z + w*w;
}


VS_INLINE vsQuaternion vsQuaternion::Conjugate( void ) const {
	return vsQuaternion( -x, -y, -z, w );
}

VS_INLINE float* vsQuaternion::ToTypePtr( void ) {
	return( &x );
}

VS_INLINE char* vsQuaternion::ToCharPtr( void ) {
	char* R = new char[128];
	sprintf_s( R, 64, "%5.3f/%5.3f/%5.3f/%5.3f", x, y, z, w );
	return( R );
}

VS_INLINE vsVec3f vsQuaternion::ToVec3( void ) const {
	return vsVec3f( x, y, z );
}

VS_INLINE void vsQuaternion::CalcW( void ) {
	float s = 1.0f - x*x - y*y - z*z;

	if ( s<0.0f ) w = 0.0f;
	else w = -sqrtf( s );
}


#endif