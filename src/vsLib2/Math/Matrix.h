// Original version by sonka 2006-2007
//	Updated in 2007

#ifndef __VS_MATRIX_H__
#define __VS_MATRIX_H__

#include <math.h>
#include "Vector.h"

class vsMat2;
class vsMat3;
class vsMat4;
template< typename T > class vsVec2;
template< typename T > class vsVec3;
template< typename T > class vsVec4;

typedef vsVec2<float> vsVec2f;
typedef vsVec3<float> vsVec3f;
typedef vsVec4<float> vsVec4f;

//=============================
//	vsMat2 class
//=============================
class vsMat2 {
public:
					vsMat2( void );
	explicit		vsMat2( float m11, float m12,
						    float m21, float m22 );
	explicit		vsMat2( float mat[4] );

	float&			operator[]( int i );
	vsMat2			operator+( const vsMat2 &mat ) const;
	vsMat2			operator+( float m ) const;
	vsMat2			operator-( const vsMat2 &mat ) const;
	vsMat2			operator-( float s ) const;
	vsMat2			operator-( void ) const;
	vsMat2			operator*( const vsMat2 &mat ) const;
	vsMat2			operator*( float scalar ) const;
	vsVec2f			operator*( const vsVec2f& v ) const;

	vsMat2			operator/( float s ) const;
	vsMat2			operator/( const vsMat2& m ) const;
	vsVec2f			operator/( const vsVec2f& v ) const;
	vsMat2&			operator/=( const float& s );
	vsMat2&			operator/=( const vsMat2& m );

	vsMat2&			operator=( const vsMat2& m );
	vsMat2&			operator*=( const vsMat2& m );
	vsMat2&			operator*=( const float& s );
	vsMat2&			operator+=( const float& s );
	vsMat2&			operator+=( const vsMat2& m );
	vsMat2&			operator-=( const float& s );
	vsMat2&			operator-=( const vsMat2& m );
	bool			operator==( const vsMat2& m ) const;
	bool			operator!=( const vsMat2& m ) const;

	void			LoadIdentity( void );
	vsMat2			Transpose( void ) const;
	vsMat2			Inverse( void ) const;
	vsMat2			Adjung( void ) const;
	float			Determinant( void ) const;

	vsMat3			ToMat3( void );
	vsMat4			ToMat4( void );
	float*			ToTypePtr( void );

	bool			IsIdentity( void );
	bool			IsProjective( void );

	vsMat2&			Rotate( float angle );

public:
	float			_mat[4];
};


//========================
// vsMat3 class 
//========================
class vsMat3 {
public:
					vsMat3( void );
	explicit		vsMat3( float m11, float m12, float m13, 
						    float m21, float m22, float m23, 
							float m31, float m32, float m33 );
	explicit		vsMat3( float degX, float degY, float degZ );
	explicit		vsMat3( float mat[9] );

	float&			operator[]( int i );
	vsMat3			operator+( const vsMat3 &m ) const;
	vsMat3			operator+( float m ) const;
	vsMat3			operator-( const vsMat3 &m ) const;
	vsMat3			operator-( float s ) const;
	vsMat3			operator-( void ) const;
	vsMat3			operator*( const vsMat3 &m ) const;
	vsMat3			operator*( float s ) const;
	vsVec3f			operator*( const vsVec3f& v ) const;

	vsMat3			operator/( float s ) const;
	vsMat3			operator/( const vsMat3& m ) const;
	vsVec3f			operator/( const vsVec3f& v ) const;
	vsMat3&			operator/=( const float& s );
	vsMat3&			operator/=( const vsMat3& m );

	vsMat3&			operator=( const vsMat3& m );
	vsMat3&			operator*=( const vsMat3& m );
	vsMat3&			operator*=( const float& s );
	vsMat3&			operator+=( const float& s );
	vsMat3&			operator+=( const vsMat3& m );
	vsMat3&			operator-=( const float& s );
	vsMat3&			operator-=( const vsMat3& m );
	bool			operator==( const vsMat3& m ) const;
	bool			operator!=( const vsMat3& m ) const;

	void			LoadIdentity( void );
	vsMat3			Transpose( void ) const;
	vsMat3			Inverse( void ) const;
	vsMat3			Adjung( void ) const;
	float           Determinant( void ) const;

	vsMat4			ToMat4( void );
	float*			ToTypePtr( void );

	bool			IsIdentity( void );
	bool			IsProjective( void );

	vsMat3&			Rotate( float x, float y, float z );
	vsMat3&			Rotate( const vsVec3f& r );

public:
	float			_mat[9];
};

//========================
// vsMat4 class 
//========================
class vsMat4 {
public:
					vsMat4( void );
	explicit		vsMat4( float m11,	float m12,  float m13,	float m14,
							float m21,	float m22,  float m23,	float m24,
							float m31,	float m32,  float m33,	float m34,
							float m41,	float m42,  float m43,	float m44 );
	explicit		vsMat4( float mat[15] );
	explicit		vsMat4( const vsMat3& mat );

	float&			operator[]( int i ); 
	vsMat4			operator+( const vsMat4& m ) const;
	vsMat4			operator+( float s ) const;
	vsMat4			operator-( const vsMat4& m ) const;
	vsMat4			operator-( float s ) const;
	vsMat4			operator-( void ) const;
	vsMat4			operator*( const vsMat4& m ) const;
	vsMat4			operator*( float s ) const;
	vsVec4f			operator*( const vsVec4f& v ) const;

	vsMat4			operator/( float s ) const;
	vsMat4			operator/( const vsMat4& m ) const;
	vsVec4f			operator/( const vsVec4f& v ) const;
	vsMat4&			operator/=( const float& s );
	vsMat4&			operator/=( const vsMat4& m );

	vsMat4&			operator=( const vsMat4& m );
	vsMat4&			operator*=( const vsMat4& m );
	vsMat4&			operator*=( const float& s );
	vsMat4&			operator+=( const float& s );
	vsMat4&			operator+=( const vsMat4& m );
	vsMat4&			operator-=( const float& s );
	vsMat4&			operator-=( const vsMat4& m );
	bool			operator==( const vsMat4& m ) const;
	bool			operator!=( const vsMat4& m ) const;

	void			LoadIdentity( void );
	vsMat4			Transpose( void ) const;
	vsMat4			Inverse( void ) const;
	vsMat4			Adjung( void ) const;
	float			Determinant( void ) const;

	bool			IsIdentity( void );
	bool			IsProjective( void );


	void			Scale( float x, float y, float z );
	void			Scale( const vsVec3f& factor );
	void			Translate( float x, float y, float z );
	void			Translate( const vsVec3f& pos );
	vsMat4&			Rotate( float x, float y, float z );
	vsMat4&			Rotate( const vsVec3f& r );

	float*			ToTypePtr( void );
	vsMat3			ToMat3( void );


	void			SetupInfProjMatrix( float fieldOfView, float aspect, float nearPlane );

public:
	float			_mat[16];
};



const vsMat2 NULLMAT2 = vsMat2();		//  identity matrix
const vsMat3 NULLMAT3 = vsMat3();		//  identity matrix
const vsMat4 NULLMAT4 = vsMat4();		//  identity matrix

#endif