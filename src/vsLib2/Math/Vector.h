// Copyright ( C ) 2006-2010 by Sandor Vasas
//
#ifndef __VS_VECTOR_H__	
#define __VS_VECTOR_H__

#include <stdio.h>
#include <typeinfo>
#include "../System/System.h"

#ifndef __PI__
#define __PI__
 const float PI = 3.1415926535897932384626433f;
#endif

const float EPSILON		= 1.f/32.f;
const float _PIOVER180	= PI/180.0f; 
const float _180OVERPI	= 180.0f/PI;

class vsMat2;
class vsMat3;
class vsMat4;
class vsQuaternion;

template< typename T >
class vsVec2;

template< typename T >
class vsVec3;

template< typename T >
class vsVec4;


//
//	ERRORS:
//		* Unresolved external symbol error when using
//		  non-member operators

/*
=============================================================

   
   vsVec2


=============================================================
*/
template< typename T >
class vsVec2 {
public:
							vsVec2( void );
	explicit				vsVec2( T xy );
	explicit				vsVec2( T x, T y );
	
	T&						operator []( int index );
	vsVec2< T >&			operator =( const vsVec2< T >& v );
	bool					operator ==( const vsVec2< T >& v ) const;
	bool					operator !=( const vsVec2< T >& v ) const;
	vsVec2< T >				operator -( void ) const;
	vsVec2< T >				operator -( const vsVec2< T >& v ) const;
	vsVec2< T >				operator -( const T& s ) const;
	vsVec2< T >&			operator -=( const vsVec2< T >& v );
	vsVec2< T >&			operator -=( const T& s );
	vsVec2< T >				operator *( const vsVec2< T >& v ) const;
	vsVec2< T >				operator *( const T& s ) const;
	vsVec2< T >&			operator *=( const vsVec2< T >& v );
	vsVec2< T >&			operator *=( const T& s );
	vsVec2< T >				operator /( const vsVec2< T >& v ) const;
	vsVec2< T >				operator /( const T& s ) const;
	vsVec2< T >&			operator /=( const vsVec2< T >& v );
	vsVec2< T >&			operator /=( const T& s );
	vsVec2< T >				operator +( const vsVec2< T >& v ) const;
	vsVec2< T >				operator +( const T& s ) const;
	vsVec2< T >&			operator +=	( const vsVec2< T >& v );
	vsVec2< T >&			operator +=( const T& s );
	bool					operator >=( const vsVec2< T >& v ) const;
	bool					operator <=( const vsVec2< T >& v ) const;
	bool					operator >( const vsVec2< T >& v ) const;
	bool					operator <( const vsVec2< T >& v ) const;
	
	template <typename TT>
	friend vsVec2< TT >		operator*( const TT& f, const vsVec2< TT >& v );
	template <typename TT>
	friend vsVec2< TT >		operator/( const TT& f, const vsVec2< TT >& v );
	template <typename TT>
	friend vsVec2< TT >		operator+( const TT& f, const vsVec2< TT >& v );
	template <typename TT>
	friend vsVec2< TT >		operator-( const TT& f, const vsVec2< TT >& v );

	vsVec2< T >				GetOrtho( void ) const;

	T*						ToTypePtr( void );
	const T*				ToTypePtr( void ) const;
	char*					ToCharPtr( void );
	const char*				ToCharPtr( void ) const;
	vsVec3< T >				ToVec3( void ) const;
	vsVec4< T >				ToVec4( void ) const;

	//friend static template<typename S>	ToVec3<S>( void );
	
	vsVec2<float>			ToVec2f( void ) const;
	vsVec2<double>			ToVec2d( void ) const;
	vsVec2<int>				ToVec2i( void ) const;
	vsVec2<unsigned int>	ToVec2ui( void ) const;
	vsVec2<short>			ToVec2s( void ) const;
	vsVec2<unsigned short>	ToVec2us( void ) const;
	vsVec2<unsigned char>	ToVec2b( void ) const;
	vsVec2<char>			ToVec2c( void ) const;
	vsVec3<float>			ToVec3f( void ) const;
	vsVec3<double>			ToVec3d( void ) const;
	vsVec3<int>				ToVec3i( void ) const;
	vsVec3<unsigned int>	ToVec3ui( void ) const;
	vsVec3<short>			ToVec3s( void ) const;
	vsVec3<unsigned short>	ToVec3us( void ) const;
	vsVec3<unsigned char>	ToVec3b( void ) const;
	vsVec3<char>			ToVec3c( void ) const;
	vsVec4<float>			ToVec4f( void ) const;
	vsVec4<double>			ToVec4d( void ) const;
	vsVec4<int>				ToVec4i( void ) const;
	vsVec4<unsigned int>	ToVec4ui( void ) const;
	vsVec4<short>			ToVec4s( void ) const;
	vsVec4<unsigned short>	ToVec4us( void ) const;
	vsVec4<unsigned char>	ToVec4b( void ) const;
	vsVec4<char>			ToVec4c( void ) const;

	T						Dot( const vsVec2< T >& v ) const;
	T						Cross( const vsVec2< T >& v ) const;
	vsVec2< T >				Abs( void ) const;
	T						Length( void ) const;
	T						LengthSqr( void ) const;
	vsVec2< T >&			Normalize( void );
	vsVec2< T >				Normalized( void ) const;
	void					Clamp( const vsVec2< T >& min, const vsVec2< T >& max );
	void					Snap( void );
	vsVec2< T >				Ceil( void );
	vsVec2< T >				Floor( void );
	void					Truncate( T length );
	void					Zero( void );
	vsVec2< T >&			Lerp( const vsVec2< T >& a, const vsVec2< T >& b, T t );

public:
	T						x;
	T						y;
};


template< typename T > VS_INLINE
vsVec2< T >::vsVec2( void ) {
	Zero();
}

template< typename T > VS_INLINE
vsVec2< T >::vsVec2( T xy ) {
	x = y = xy;
}

template< typename T > VS_INLINE
vsVec2< T >::vsVec2( T x, T y ) {
	this->x = x;
	this->y = y;
}

template< typename T > VS_INLINE
T& vsVec2< T >::operator []( int index ) {
	static typename T err = ( T )0.f;
	if ( index<0 || index>1 ) return err;
	return ( &x )[ index ];
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::operator =( const vsVec2< T >& v ) {
	this->x = v.x;
	this->y = v.y;
	return *this;
}

template< typename T > VS_INLINE
bool vsVec2< T >::operator ==( const vsVec2< T >& v ) const {
	return ( this->x == v.x && this->y == v.y );
}

template< typename T > VS_INLINE
bool vsVec2< T >::operator !=( const vsVec2< T >& v ) const {
	return !( *this == v );
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::operator -( void ) const{
	return vsVec2< T >( -x, -y );
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::operator -( const vsVec2< T >& v ) const{
	return vsVec2< T >( x-v.x, y-v.y );
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::operator -( const T& s ) const{
	return vsVec2< T >( x-s, y-s );
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::operator -=( const vsVec2< T >& v ) {
	this->x -= v.x;
	this->y -= v.y;
	return *this;
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::operator -=( const T& s ) {
	this->x -= s;
	this->y -= s;
	return *this;
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::operator *( const vsVec2< T >& v ) const {
	return vsVec2< T >( x*v.x , y*v.y );
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::operator *( const T& s ) const {
	return vsVec2< T >( x*s , y*s );
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::operator *=( const vsVec2< T >& v ) {
	this->x *= v.x;
	this->y *= v.y;
	return *this;
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::operator *=( const T& s ) {
	this->x *= s;
	this->y *= s;
	return *this;
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::operator /( const vsVec2< T >& v ) const {
	return vsVec2< T >( x/v.x , y/v.y );
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::operator /( const T& s ) const {
	typename T s1 = ( T )1.0f/s;
	return vsVec2< T >( x*s1 , y*s1 );
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::operator /=( const vsVec2< T >& v ) {
	this->x /= v.x;
	this->y /= v.y;
	return *this;
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::operator /=( const T& s ) {
	typename T s1 = ( T )1.0f/s;
	this->x *= s1;
	this->y *= s1;
	return *this;
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::operator +( const vsVec2< T >& v ) const {
	return vsVec2< T >( x+v.x , y+v.y );
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::operator +( const T& s ) const {
	return vsVec2< T >( x+s , y+s );
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::operator +=( const vsVec2< T >& v ) {
	this->x += v.x;
	this->y += v.y;
	return *this;
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::operator +=( const T& s ) {
	this->x += s;
	this->y += s;
	return *this;
}

template< typename TT > VS_INLINE
vsVec2< TT > operator*( const TT& f, const vsVec2< TT >& v ) {
	return v*f;
}

template< typename TT > VS_INLINE
vsVec2< TT > operator/( const TT& f, const vsVec2< TT >& v ) {
	return vsVec2< TT >( f/v.x, f/v.y );
}

template< typename TT > VS_INLINE
vsVec2< TT > operator+( const TT& f, const vsVec2< TT >& v ) {
	return v+f;
}

template< typename TT > VS_INLINE
vsVec2< TT > operator-( const TT& f, const vsVec2< TT >& v ) {
	return ( -v )+f;
}

template< typename T > VS_INLINE
bool vsVec2< T >::operator >=( const vsVec2< T >& v ) const {
	return ( x >= v.x && y >= v.y );
}

template< typename T > VS_INLINE
bool vsVec2< T >::operator <=( const vsVec2< T >& v ) const {
	return ( x <= v.x && y <= v.y );
}

template< typename T > VS_INLINE
bool vsVec2< T >::operator >( const vsVec2< T >& v ) const {
	return ( x > v.x && y > v.y );
}

template< typename T > VS_INLINE
bool vsVec2< T >::operator <( const vsVec2< T >& v ) const {
	return ( x < v.x && y < v.y );
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::GetOrtho( void ) const {
	return vsVec2< T >( -y, x );
}

template< typename T > VS_INLINE
T* vsVec2< T >::ToTypePtr( void ) {
	return ( &x );
}

template< typename T > VS_INLINE
const T* vsVec2< T >::ToTypePtr( void ) const {
	return ( &x );
}


template< typename T > VS_INLINE
T vsVec2< T >::Dot( const vsVec2< T >& v ) const {
	return( x*v.x + y*v.y );
}

template< typename T > VS_INLINE
T vsVec2< T >::Cross( const vsVec2< T >& v ) const {
	return( x * v.y - y * v.x );
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::Abs( void ) const {
	return vsVec2< T >( fabs( x ), fabs( y ) );
}

template< typename T > VS_INLINE
T vsVec2< T >::Length( void ) const {
	return sqrtf( x*x + y*y );
}

template< typename T > VS_INLINE
T vsVec2< T >::LengthSqr( void ) const {
	return( x*x + y*y );
}

template< typename T > VS_INLINE
void vsVec2< T >::Snap( void ) {
	x = T( int( x ) );
	y = T( int( y ) );
}
	
template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::Ceil( void )
{
	x = ceil( x );
	y = ceil( y );
	return *this;
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::Floor( void )
{
	x = floor( x );
	y = floor( y );
	return *this;
}

template< typename T > VS_INLINE
void vsVec2< T >::Zero( void ) {
	x = y = ( T )0;
}

template< typename T > VS_INLINE
vsVec2< T >& vsVec2< T >::Normalize( void ) {
	*this /= Length();
	return *this;
}

template< typename T > VS_INLINE
vsVec2< T > vsVec2< T >::Normalized( void ) const {
	float l = Length();
	if ( l == 0.0f ) 
		return vsVec2f( 0.0f );
	return ( *this / l );
}

template< typename T > VS_INLINE	
void vsVec2< T >::Clamp( const vsVec2< T >& min, const vsVec2< T >& max ) {
	if ( x > max.x ) x = max.x;
	if ( x < min.x ) x = min.x;
	if ( y > max.y ) y = max.y;
	if ( y < min.y ) y = min.y;
}

template< typename T > VS_INLINE	
void vsVec2< T >::Truncate( T length ) {
	if ( !length ) {
		Zero();
	}
	else {
		T lr = Length();
		if ( length < lr ) {
			T i = length / lr;
			x = x*i;
			y = y*i;
		}
	}
}

template< typename T > VS_INLINE			
vsVec2< T >& vsVec2< T >::Lerp( const vsVec2< T >& a, const vsVec2< T >& b, T t ) {
	*this = a*( T( 1.0f-t ) ) + b*t;
	return( *this );
}

template< typename T > VS_INLINE			
char* vsVec2< T >::ToCharPtr( void ) {
	char* R = new char[128];
	if ( typeid( T ) == typeid( float ) ||
	   typeid( T ) == typeid( double ) )
			sprintf_s( R, 64, "%5.3f/%5.3f", x, y );
	else
	if ( typeid( T ) == typeid( int ) ||
	   typeid( T ) == typeid( unsigned int ) ||
	   typeid( T ) == typeid( unsigned char ) ||
	   typeid( T ) == typeid( unsigned short ) ||
	   typeid( T ) == typeid( short ) )
			sprintf_s( R, 64, "%d/%d", x, y );
	else
	if ( typeid( T ) == typeid( char ) )
			sprintf_s( R, 64, "%c/%c", x, y );
	else
			strcpy_s( R, 64, "unknown type" );
	return R;
}

template< typename T > VS_INLINE			
const char* vsVec2< T >::ToCharPtr( void ) const {
	char* R = new char[128];
	if ( typeid( T ) == typeid( float ) ||
	   typeid( T ) == typeid( double ) )
			sprintf_s( R, 64, "%5.3f/%5.3f", x, y );
	else
	if ( typeid( T ) == typeid( int ) ||
	   typeid( T ) == typeid( unsigned int ) ||
	   typeid( T ) == typeid( unsigned char ) ||
	   typeid( T ) == typeid( unsigned short ) ||
	   typeid( T ) == typeid( short ) )
			sprintf_s( R, 64, "%d/%d", x, y );
	else
	if ( typeid( T ) == typeid( char ) )
			sprintf_s( R, 64, "%c/%c", x, y );
	else
			strcpy_s( R, 64, "unknown type" );
	return R;
}
/*
template< typename T > VS_INLINE	
template< typename S > vsVec2<T>::ToVec3<S>( void )
{
	return vsVec3<S>( ( S )x, ( S )y, ( S )0 );
}*/


template< typename T > VS_INLINE
vsVec3< T > vsVec2< T >::ToVec3( void ) const {
	return vsVec3< T >( x, y, ( T )0 );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec2< T >::ToVec4( void ) const {
	return vsVec4( x, y, ( T )0, ( T )0 );
}

template< typename T > VS_INLINE
vsVec2<float> vsVec2< T >::ToVec2f( void ) const {
	return vsVec2<float>( ( float )x, ( float )y );
}

template< typename T > VS_INLINE
vsVec2<double> vsVec2< T >::ToVec2d( void ) const {
	return vsVec2<double>( ( double )x, ( double )y );
}

template< typename T > VS_INLINE
vsVec2<int> vsVec2< T >::ToVec2i( void ) const {
	return vsVec2<int>( ( int )x, ( int )y );
}
	
template< typename T > VS_INLINE
vsVec2<unsigned int> vsVec2< T >::ToVec2ui( void ) const {
	return vsVec2<unsigned int>( ( unsigned int )x, ( unsigned int )y );
}
	
template< typename T > VS_INLINE
vsVec2<short> vsVec2< T >::ToVec2s( void ) const {
	return vsVec2<short>( ( short )x, ( short )y );
}
	
template< typename T > VS_INLINE
vsVec2<unsigned short> vsVec2< T >::ToVec2us( void ) const {
	return vsVec2<unsigned short>( ( unsigned short )x, ( unsigned short )y );
}
	
template< typename T > VS_INLINE	
vsVec2<unsigned char> vsVec2< T >::ToVec2b( void ) const {
	return vsVec2<unsigned char>( ( unsigned char )x, ( unsigned char )y );
}
	
template< typename T > VS_INLINE
vsVec2<char> vsVec2< T >::ToVec2c( void ) const {
	return vsVec2<char>( ( char )x, ( char )y );
}
	
template< typename T > VS_INLINE
vsVec3<float> vsVec2< T >::ToVec3f( void ) const {
	return vsVec3<float>( ( float )x, ( float )y, 0.0f );
}
	
template< typename T > VS_INLINE
vsVec3<double> vsVec2< T >::ToVec3d( void ) const {
	return vsVec3<double>( ( double )x, ( double )y, 0.0 );
}
	
template< typename T > VS_INLINE
vsVec3<int> vsVec2< T >::ToVec3i( void ) const {
	return vsVec3<int>( ( int )x, ( int )y, 0 );
}
	
template< typename T > VS_INLINE
vsVec3<unsigned int> vsVec2< T >::ToVec3ui( void ) const {
	return vsVec3<unsigned int>( ( unsigned int )x, ( unsigned int )y, 0 );
}
	
template< typename T > VS_INLINE
vsVec3<short> vsVec2< T >::ToVec3s( void ) const {
	return vsVec3<short>( ( short )x, ( short )y, 0 );
}
	
template< typename T > VS_INLINE
vsVec3<unsigned short> vsVec2< T >::ToVec3us( void ) const {
	return vsVec3<unsigned short>( ( unsigned short )x, ( unsigned short )y, 0 );
}
	
template< typename T > VS_INLINE
vsVec3<unsigned char> vsVec2< T >::ToVec3b( void ) const {
	return vsVec3<unsigned char>( ( unsigned char )x, ( unsigned char )y, 0 );
}
	
template< typename T > VS_INLINE
vsVec3<char> vsVec2< T >::ToVec3c( void ) const {
	return vsVec3<char>( ( char )x, ( char )y, 0 );
}
	
template< typename T > VS_INLINE
vsVec4<float> vsVec2< T >::ToVec4f( void ) const {
	return vsVec4<float>( ( float )x, ( float )y, 0.0f, 0.0f );
}
	
template< typename T > VS_INLINE
vsVec4<double> vsVec2< T >::ToVec4d( void ) const {
	return vsVec4<double>( ( double )x, ( double )y, 0.0, 0.0 );
}
	
template< typename T > VS_INLINE
vsVec4<int> vsVec2< T >::ToVec4i( void ) const {
	return vsVec4<int>( ( int )x, ( int )y, 0, 0 );
}
	
template< typename T > VS_INLINE
vsVec4<unsigned int> vsVec2< T >::ToVec4ui( void ) const {
	return vsVec4<unsigned int>( ( unsigned int )x, ( unsigned int )y, 0, 0 );
}
	
template< typename T > VS_INLINE
vsVec4<short> vsVec2< T >::ToVec4s( void ) const {
	return vsVec4<short>( ( short )x, ( short )y, 0, 0 );
}
	
template< typename T > VS_INLINE
vsVec4<unsigned short> vsVec2< T >::ToVec4us( void ) const {
	return vsVec4<unsigned short>( ( unsigned short )x, ( unsigned short )y, 0, 0 );
}
	
template< typename T > VS_INLINE
vsVec4<unsigned char> vsVec2< T >::ToVec4b( void ) const {
	return vsVec4<unsigned char>( ( unsigned char )x, ( unsigned char )y, 0, 0 );
}


/*
=============================================================

   
   vsVec3


=============================================================
*/
template< typename T >
class vsVec3 {
public:
							vsVec3( void );
	explicit				vsVec3( T xyz );
	explicit				vsVec3( T x, T y, T z );
	explicit				vsVec3( vsVec2< T > v, T z );
	
	T&						operator []( int index );
	vsVec3< T >&			operator =( const vsVec3< T >& v );
	bool					operator !=( const vsVec3< T >& v ) const;
	bool					operator ==( const vsVec3< T >& v ) const;
	vsVec3< T >				operator -( void ) const;
	vsVec3< T >				operator -( const vsVec3< T >& v ) const;
	vsVec3< T >				operator -( const T& s ) const;
	vsVec3< T >&			operator -=( const vsVec3< T >& v );
	vsVec3< T >&			operator -=( const T& s );
	vsVec3< T >				operator *( const vsMat3& m ) const;
	vsVec3< T >				operator *( const vsVec3< T >& v ) const;
	vsVec3< T >				operator *( const T& s ) const;

	vsVec3< T >&			operator *=( const vsMat3& m );
	vsVec3< T >&			operator *=( const vsQuaternion& q );
	vsVec3< T >&			operator *=( const vsVec3< T >& v );
	vsVec3< T >&			operator *=( const T& s );
	vsVec3< T >				operator /( const vsVec3< T >& v ) const;
	vsVec3< T >				operator /( const T& s ) const;
	vsVec3< T >&			operator /=( const vsVec3< T >& v );
	vsVec3< T >&			operator /=( const T& s );
	vsVec3< T >				operator +( const vsVec3< T >& v ) const;
	vsVec3< T >				operator +( const T& s ) const;
	vsVec3< T >&			operator +=( const vsVec3< T >& v );
	vsVec3< T >&			operator +=( const T& s );
	bool					operator >=( const vsVec3< T >& v ) const;
	bool					operator <=( const vsVec3< T >& v ) const;
	bool					operator >( const vsVec3< T >& v ) const;
	bool					operator <( const vsVec3< T >& v ) const;

	template <typename TT>
	friend vsVec3< TT >		operator*( const TT& f, const vsVec3< TT >& v );
	template <typename TT>
	friend vsVec3< TT >		operator/( const TT& f, const vsVec3< TT >& v );
	template <typename TT>
	friend vsVec3< TT >		operator+( const TT& f, const vsVec3< TT >& v );
	template <typename TT>
	friend vsVec3< TT >		operator-( const TT& f, const vsVec3< TT >& v );

	vsVec3< T >				GetOrthoXY( void ) const;
	vsVec3< T >				GetOrthoXZ( void ) const;
	vsVec3< T >				GetOrthoYZ( void ) const;

	T*						ToTypePtr( void );
	const T*				ToTypePtr( void ) const;
	char*					ToCharPtr( void );
	const char*				ToCharPtr( void ) const;
	vsVec2< T >				ToVec2( void ) const;
	vsVec4< T >				ToVec4( void ) const;

	vsVec2<float>			ToVec2f( void ) const;
	vsVec2<double>			ToVec2d( void ) const;
	vsVec2<int>				ToVec2i( void ) const;
	vsVec2<unsigned int>	ToVec2ui( void ) const;
	vsVec2<short>			ToVec2s( void ) const;
	vsVec2<unsigned short>	ToVec2us( void ) const;
	vsVec2<unsigned char>	ToVec2b( void ) const;
	vsVec2<char>			ToVec2c( void ) const;
	vsVec3<float>			ToVec3f( void ) const;
	vsVec3<double>			ToVec3d( void ) const;
	vsVec3<int>				ToVec3i( void ) const;
	vsVec3<unsigned int>	ToVec3ui( void ) const;
	vsVec3<short>			ToVec3s( void ) const;
	vsVec3<unsigned short>	ToVec3us( void ) const;
	vsVec3<unsigned char>	ToVec3b( void ) const;
	vsVec3<char>			ToVec3c( void ) const;
	vsVec4<float>			ToVec4f( void ) const;
	vsVec4<double>			ToVec4d( void ) const;
	vsVec4<int>				ToVec4i( void ) const;
	vsVec4<unsigned int>	ToVec4ui( void ) const;
	vsVec4<short>			ToVec4s( void ) const;
	vsVec4<unsigned short>	ToVec4us( void ) const;
	vsVec4<unsigned char>	ToVec4b( void ) const;
	vsVec4<char>			ToVec4c( void );

	T						Dot( const vsVec3< T >& v ) const;
	vsVec3< T >				Cross( const vsVec3< T >& v ) const;
	vsVec3< T >				Abs( void ) const;
	T						Length( void ) const;
	T						LengthSqr( void ) const;
	vsVec3< T >&			Normalize( void );
	vsVec3< T >				Normalized( void ) const;
	void					Clamp( const vsVec3< T >& min, const vsVec3< T >& max );
	void					Snap( void );
	vsVec3< T >				Ceil( void );
	vsVec3< T >				Floor( void );
	void					Truncate( T length );
	void					Zero( void );
	vsVec3< T >&			Lerp( const vsVec3< T >& a, const vsVec3< T >& b, T t );

public:
	T						x;
	T						y;
	T						z;
};


template< typename T > VS_INLINE
vsVec3< T >::vsVec3( void ) {
	Zero();
}

template< typename T > VS_INLINE
vsVec3< T >::vsVec3( T xyz ) {
	x = y = z = xyz;
}

template< typename T > VS_INLINE
vsVec3< T >::vsVec3( T x, T y, T z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

template< typename T > VS_INLINE
vsVec3< T >::vsVec3( vsVec2< T > v, T z ) {
	this->x = v.x;
	this->y = v.y;
	this->z = z;
}

template< typename T > VS_INLINE
T& vsVec3< T >::operator []( int index ) {
	static T err = ( T )0;
	if ( index<0 || index>2 ) return err;
	return ( &x )[ index ];
}

template< typename T > VS_INLINE
vsVec3< T >& vsVec3< T >::operator =( const vsVec3< T >& v ) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	return *this;
}

template< typename T > VS_INLINE
bool vsVec3< T >::operator ==( const vsVec3< T >& v ) const {
	return ( this->x == v.x && this->y == v.y && this->z == v.z );
}

template< typename T > VS_INLINE
bool vsVec3< T >::operator !=( const vsVec3< T >& v ) const {
	return !( *this == v );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::operator -( void ) const{
	return vsVec3< T >( -x, -y, -z );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::operator -( const vsVec3< T >& v ) const{
	return vsVec3< T >( x-v.x, y-v.y, z-v.z );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::operator -( const T& s ) const{
	return vsVec3< T >( x-s, y-s, z-s );
}

template< typename T > 
VS_INLINE vsVec3< T >& vsVec3< T >::operator -=( const vsVec3< T >& v ) {
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	return *this;
}

template< typename T > 
VS_INLINE vsVec3< T >& vsVec3< T >::operator -=( const T& s ) {
	this->x -= s;
	this->y -= s;
	this->z -= s;
	return *this;
}

/*template< typename T > VS_INLINE vsVec3< T > vsVec3< T >::operator *( const vsQuaternion& q ) const {
	return( q * ( *this ) );
}*/

template< typename T > VS_INLINE 
vsVec3< T > vsVec3< T >::operator *( const vsVec3< T >& v ) const {
	return vsVec3< T >( x*v.x , y*v.y, z*v.z );
}

template< typename T > VS_INLINE 
vsVec3< T > vsVec3< T >::operator *( const T& s ) const {
	return vsVec3< T >( x*s , y*s, z*s );
}

template< typename TT > VS_INLINE
vsVec3< TT > operator*( const TT& f, const vsVec3< TT >& v ) {
	return v*f;
}

template< typename TT > VS_INLINE
vsVec3< TT > operator/( const TT& f, const vsVec3< TT >& v ) {
	return vsVec3< TT >( f/v.x, f/v.y, f/v.z );
}

template< typename TT > VS_INLINE
vsVec3< TT > operator+( const TT& f, const vsVec3< TT >& v ) {
	return v+f;
}

template< typename TT > VS_INLINE
vsVec3< TT > operator-( const TT& f, const vsVec3< TT >& v ) {
	return ( -v )+f;
}

/*vsVec3< T > operator*( const vsMat3& mat, const v3Vec3& vec ) {
	return mat * vec;
}*/

/*template< typename T > VS_INLINE vsVec3< T >& vsVec3< T >::operator *=( const vsQuaternion& q ) {
	*this = *this * q;
	return( *this );
}*/

template< typename T > VS_INLINE 
vsVec3< T >& vsVec3< T >::operator *=( const vsMat3& m ) {
	*this = *this * m;
	return( *this );
}

template< typename T > VS_INLINE 
vsVec3< T >& vsVec3< T >::operator *=( const vsVec3< T >& v ) {
	this->x *= v.x;
	this->y *= v.y;
	this->z *= v.z;
	return *this;
}

template< typename T > VS_INLINE
vsVec3< T >& vsVec3< T >::operator *=( const vsQuaternion& q )
{
	*this = q.ToMat3() * *this;
	return *this;
}

template< typename T > VS_INLINE
vsVec3< T >& vsVec3< T >::operator *=( const T& s ) {
	this->x *= s;
	this->y *= s;
	this->z *= s;
	return *this;
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::operator /( const vsVec3< T >& v ) const {
	return vsVec3( x/v.x , y/v.y, z/v.z );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::operator /( const T& s ) const {
//	T is = 1.0f/s;
	return vsVec3( x/s , y/s, z/s );
}

template< typename T > VS_INLINE
vsVec3< T >& vsVec3< T >::operator /=( const vsVec3< T >& v ) {
	this->x /= v.x;
	this->y /= v.y;
	this->z /= v.z;
	return *this;
}

template< typename T > VS_INLINE
vsVec3< T >& vsVec3< T >::operator /=( const T& s ) {
	//T s1 = ( T )1.0f/s;
	this->x /= s;
	this->y /= s;
	this->z /= s;
	return *this;
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::operator +( const vsVec3< T >& v ) const {
	return vsVec3< T >( x+v.x , y+v.y, z+v.z );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::operator +( const T& s ) const {
	return vsVec3< T >( x+s , y+s, z+s );
}

template< typename T > VS_INLINE
vsVec3< T >& vsVec3< T >::operator +=( const vsVec3< T >& v ) {
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	return *this;
}

template< typename T > VS_INLINE
vsVec3< T >& vsVec3< T >::operator +=( const T& s ) {
	this->x += s;
	this->y += s;
	this->z += s;
	return *this;
}

template< typename T > VS_INLINE
bool vsVec3< T >::operator >=( const vsVec3< T >& v ) const {
	return ( x >= v.x && y >= v.y && z >= v.z );
}

template< typename T > VS_INLINE
bool vsVec3< T >::operator <=( const vsVec3< T >& v ) const {
	return ( x <= v.x && y <= v.y && z <= v.z );
}

template< typename T > VS_INLINE
bool vsVec3< T >::operator >( const vsVec3< T >& v ) const {
	return ( x > v.x && y > v.y && z > v.z  );
}

template< typename T > VS_INLINE
bool vsVec3< T >::operator <( const vsVec3< T >& v ) const {
	return ( x < v.x && y < v.y && z < v.z );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::GetOrthoXY( void ) const {
	return vsVec3< T >( -y, x, z );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::GetOrthoXZ( void ) const {
	return vsVec3< T >( -z, y, x );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::GetOrthoYZ( void ) const {
	return vsVec3< T >( x, -z, y );
}

template< typename T > VS_INLINE
T* vsVec3< T >::ToTypePtr( void ) {
	return ( &x );
}

template< typename T > VS_INLINE
const T* vsVec3< T >::ToTypePtr( void ) const {
	return ( &x );
}

template< typename T > VS_INLINE
T vsVec3< T >::Dot( const vsVec3< T >& v ) const {
	return( x*v.x + y*v.y + z*v.z );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::Cross( const vsVec3< T >& v ) const {
	return vsVec3< T >( 	y * v.z - z * v.y,
					z * v.x - x * v.z,
					x * v.y - y * v.x );
}

template< typename T > VS_INLINE
vsVec3< T >& vsVec3< T >::Normalize( void ) {
	*this /= Length();
	return *this;
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::Normalized( void ) const {
	float l = Length();
	if ( l==0.0f ) 
		return vsVec3f( 0.0f );
	return ( *this / l );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::Abs( void ) const {
	return vsVec3< T >( ( T )fabs( ( float )x ), ( T )fabs( ( float )y ), ( T )fabs( ( float )z ) );
}

template< typename T > VS_INLINE
T vsVec3< T >::Length( void ) const {
	return ( T )sqrtf( x*x + y*y + z*z );
}

template< typename T > VS_INLINE
T vsVec3< T >::LengthSqr( void ) const {
	return x*x + y*y + z*z;
}

template< typename T > VS_INLINE
void vsVec3< T >::Zero( void ) {
	x = y = z = ( T )0;
}

template< typename T > VS_INLINE
void vsVec3< T >::Snap( void ) {
	x = T( int( x ) );
	y = T( int( y ) );
	z = T( int( z ) );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::Ceil( void )
{
	x = ceil( x );
	y = ceil( y );
	z = ceil( z );
	return *this;
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::Floor( void )
{
	x = floor( x );
	y = floor( y );
	z = floor( z );
	return *this;
}

template< typename T > VS_INLINE
vsVec3< T > vsVec3< T >::operator *( const vsMat3& m ) const {
	return m * ( *this );
}

template< typename T > VS_INLINE
void vsVec3< T >::Clamp( const vsVec3< T >& min, const vsVec3< T >& max ) {
	if ( x > max.x ) x = max.x;
	if ( x < min.x ) x = min.x;
	if ( y > max.y ) y = max.y;
	if ( y < min.y ) y = min.y;
	if ( z > max.z ) z = max.z;
	if ( z < min.z ) z = min.z;
}

template< typename T > VS_INLINE
void vsVec3< T >::Truncate( T length ) {
	if ( !length ) {
		Zero();
	} else {
		T lr = Length();
		if ( length < lr ) {
			T i = length / lr;
			x *= i;
			y *= i;
			z *= i;
		}
	}
}

template< typename T > VS_INLINE
vsVec3< T >& vsVec3< T >::Lerp( const vsVec3< T >& a, const vsVec3< T >& b, T t ) {
	if ( t>=( T )1 ) {
		*this = a;
	} else if ( t<=( T )0 ) {
		*this = b;
	} else 
		*this = a*( T( 1.0f-t ) ) + b*t;

	return *this;
}

template< typename T > VS_INLINE
char* vsVec3< T >::ToCharPtr( void ) {
	char* R = new char[128];
	if ( typeid( T ) == typeid( float ) ||
	   typeid( T ) == typeid( double ) )
			sprintf_s( R, 64, "%5.3f/%5.3f/%5.3f", x, y, z );
	else
	if ( typeid( T ) == typeid( int& ) ||
	   typeid( T ) == typeid( unsigned int ) ||
	   typeid( T ) == typeid( unsigned char ) ||
	   typeid( T ) == typeid( unsigned short ) ||
	   typeid( T ) == typeid( short ) )
			sprintf_s( R, 64, "%d/%d/%d", x, y, z );
	else
	if ( typeid( T ) == typeid( char ) )
			sprintf_s( R, 64, "%c/%c/%c", x, y, z );
	else
			strcpy_s( R, 64, "unknown type" );
	return R;
}

template< typename T > VS_INLINE
const char* vsVec3< T >::ToCharPtr( void ) const {
	char* R = new char[128];
	if ( typeid( T ) == typeid( float ) ||
	   typeid( T ) == typeid( double ) )
			sprintf_s( R, 64, "%5.3f/%5.3f/%5.3f", x, y, z );
	else
	if ( typeid( T ) == typeid( int ) ||
	   typeid( T ) == typeid( unsigned int ) ||
	   typeid( T ) == typeid( unsigned char ) ||
	   typeid( T ) == typeid( unsigned short ) ||
	   typeid( T ) == typeid( short ) )
			sprintf_s( R, 64, "%d/%d/%d", x, y, z );
	else
	if ( typeid( T ) == typeid( char ) )
			sprintf_s( R, 64, "%c/%c/%c", x, y, z );
	else
			strcpy_s( R, 64, "unknown type" );
	return R;
}

template< typename T > VS_INLINE
vsVec2< T > vsVec3< T >::ToVec2( void ) const {
	return vsVec2< T >( x, y );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec3< T >::ToVec4( void ) const {
	return vsVec4< T >( x, y, z, ( T )0 );
}

/*
template< typename T > VS_INLINE	
template< typename S > vsVec2<T>::ToVec3<S>( void )
{
	return vsVec3<S>( ( S )x, ( S )y, ( S )0 );
}
*/

template< typename T > VS_INLINE
vsVec2<float> vsVec3< T >::ToVec2f( void )const {
	return vsVec2<float>( ( float )x, ( float )y );
}

template< typename T > VS_INLINE
vsVec2<double> vsVec3< T >::ToVec2d( void )const {
	return vsVec2<double>( ( double )x, ( double )y );
}

template< typename T > VS_INLINE
vsVec2<int> vsVec3< T >::ToVec2i( void )const {
	return vsVec2<int>( ( int )x, ( int )y );
}
	
template< typename T > VS_INLINE
vsVec2<unsigned int> vsVec3< T >::ToVec2ui( void )const {
	return vsVec2<unsigned int>( ( unsigned int )x, ( unsigned int )y );
}
	
template< typename T > VS_INLINE
vsVec2<short> vsVec3< T >::ToVec2s( void )const {
	return vsVec2<short>( ( short )x, ( short )y );
}
	
template< typename T > VS_INLINE
vsVec2<unsigned short> vsVec3< T >::ToVec2us( void )const {
	return vsVec2<unsigned short>( ( unsigned short )x, ( unsigned short )y );
}
	
template< typename T > VS_INLINE	
vsVec2<unsigned char> vsVec3< T >::ToVec2b( void )const {
	return vsVec2<unsigned char>( ( unsigned char )x, ( unsigned char )y );
}
	
template< typename T > VS_INLINE
vsVec2<char> vsVec3< T >::ToVec2c( void )const {
	return vsVec2<char>( ( char )x, ( char )y );
}
	
template< typename T > VS_INLINE
vsVec3<float> vsVec3< T >::ToVec3f( void )const {
	return vsVec3<float>( ( float )x, ( float )y, ( float )z );
}
	
template< typename T > VS_INLINE
vsVec3<double> vsVec3< T >::ToVec3d( void )const {
	return vsVec3<double>( ( double )x, ( double )y, ( double )z );
}
	
template< typename T > VS_INLINE
vsVec3<int> vsVec3< T >::ToVec3i( void )const {
	return vsVec3<int>( ( int )x, ( int )y, ( int )z );
}
	
template< typename T > VS_INLINE
vsVec3<unsigned int> vsVec3< T >::ToVec3ui( void )const {
	return vsVec3<unsigned int>( ( unsigned int )x, ( unsigned int )y, ( unsigned int )z );
}
	
template< typename T > VS_INLINE
vsVec3<short> vsVec3< T >::ToVec3s( void )const {
	return vsVec3<short>( ( short )x, ( short )y, ( short )z );
}
	
template< typename T > VS_INLINE
vsVec3<unsigned short> vsVec3< T >::ToVec3us( void )const {
	return vsVec3<unsigned short>( ( unsigned short )x, ( unsigned short )y, ( unsigned short )z );
}
	
template< typename T > VS_INLINE
vsVec3<unsigned char> vsVec3< T >::ToVec3b( void )const {
	return vsVec3<unsigned char>( ( unsigned char )x, ( unsigned char )y, ( unsigned char )z );
}
	
template< typename T > VS_INLINE
vsVec3<char> vsVec3< T >::ToVec3c( void )const {
	return vsVec3<char>( ( char )x, ( char )y, ( char )z );
}
	
template< typename T > VS_INLINE
vsVec4<float> vsVec3< T >::ToVec4f( void )const {
	return vsVec4<float>( ( float )x, ( float )y, ( float )z, 0.0f );
}
	
template< typename T > VS_INLINE
vsVec4<double> vsVec3< T >::ToVec4d( void )const {
	return vsVec4<double>( ( double )x, ( double )y, ( double )z, 0.0 );
}
	
template< typename T > VS_INLINE
vsVec4<int> vsVec3< T >::ToVec4i( void )const {
	return vsVec4<int>( ( int )x, ( int )y, ( int )z, 0 );
}
	
template< typename T > VS_INLINE
vsVec4<unsigned int> vsVec3< T >::ToVec4ui( void )const {
	return vsVec4<unsigned int>( ( unsigned int )x, ( unsigned int )y, ( unsigned int )z, 0 );
}
	
template< typename T > VS_INLINE
vsVec4<short> vsVec3< T >::ToVec4s( void )const {
	return vsVec4<short>( ( short )x, ( short )y, ( short )z, 0 );
}
	
template< typename T > VS_INLINE
vsVec4<unsigned short> vsVec3< T >::ToVec4us( void )const {
	return vsVec4<unsigned short>( ( unsigned short )x, ( unsigned short )y, ( unsigned short )z, 0 );
}
	
template< typename T > VS_INLINE
vsVec4<unsigned char> vsVec3< T >::ToVec4b( void )const {
	return vsVec4<unsigned char>( ( unsigned char )x, ( unsigned char )y, ( unsigned char )z, 0 );
}

/*
=============================================================

   
   vsVec4


=============================================================
*/
template< typename T >
class vsVec4 {
public:
							vsVec4( void );
	explicit				vsVec4( T xyzw );
	explicit				vsVec4( T x, T y, T z, T w );
	explicit				vsVec4( vsVec2< T > v, T z, T w );
	explicit				vsVec4( vsVec3< T > v, T w );
	
	T&						operator []( int index );
	vsVec4< T >&			operator =( const vsVec4< T >& v );
	bool					operator ==( const vsVec4< T >& v ) const;
	bool					operator !=( const vsVec4< T >& v ) const;
	vsVec4< T >				operator -( void ) const;
	vsVec4< T >				operator -( const vsVec4< T >& v ) const;
	vsVec4< T >				operator -( const T& s ) const;
	vsVec4< T >&			operator -=( const vsVec4< T >& v );
	vsVec4< T >&			operator -=( const T& s );
	vsVec4< T >				operator *( const vsVec4< T >& v ) const;
	vsVec4< T >				operator *( const T& s ) const;
	vsVec4< T >&			operator *=( const vsVec4< T >& v );
	vsVec4< T >&			operator *=( const T& s );
	vsVec4< T >				operator /( const vsVec4< T >& v ) const;
	vsVec4< T >				operator /( const T& s ) const;
	vsVec4< T >&			operator /=( const vsVec4< T >& v );
	vsVec4< T >&			operator /=( const T& s );
	vsVec4< T >				operator +( const vsVec4< T >& v ) const;
	vsVec4< T >				operator +( const T& s ) const;
	vsVec4< T >&			operator +=( const vsVec4< T >& v );
	vsVec4< T >&			operator +=( const T& s );
	bool					operator >=( const vsVec4< T >& v ) const;
	bool					operator <=( const vsVec4< T >& v ) const;
	bool					operator >( const vsVec4< T >& v ) const;
	bool					operator <( const vsVec4< T >& v ) const;

	template <typename TT>
	friend vsVec4< TT >		operator*( const TT& f, const vsVec4< TT >& v );
	template <typename TT>
	friend vsVec4< TT >		operator/( const TT& f, const vsVec4< TT >& v );
	template <typename TT>
	friend vsVec4< TT >		operator+( const TT& f, const vsVec4< TT >& v );
	template <typename TT>
	friend vsVec4< TT >		operator-( const TT& f, const vsVec4< TT >& v );

	const T*				ToTypePtr( void ) const;
	T*						ToTypePtr( void );
	const char*				ToCharPtr( void ) const;
	char*					ToCharPtr( void );
	vsVec2< T >				ToVec2( void ) const;
	vsVec3< T >				ToVec3( void ) const;

	vsVec2<float>			ToVec2f( void ) const;
	vsVec2<double>			ToVec2d( void ) const;
	vsVec2<int>				ToVec2i( void ) const;
	vsVec2<unsigned int>	ToVec2ui( void ) const;
	vsVec2<short>			ToVec2s( void ) const;
	vsVec2<unsigned short>	ToVec2us( void ) const;
	vsVec2<unsigned char>	ToVec2b( void ) const;
	vsVec2<char>			ToVec2c( void ) const;
	vsVec3<float>			ToVec3f( void ) const;
	vsVec3<double>			ToVec3d( void ) const;
	vsVec3<int>				ToVec3i( void ) const;
	vsVec3<unsigned int>	ToVec3ui( void ) const;
	vsVec3<short>			ToVec3s( void ) const;
	vsVec3<unsigned short>	ToVec3us( void ) const;
	vsVec3<unsigned char>	ToVec3b( void ) const;
	vsVec3<char>			ToVec3c( void ) const;
	vsVec4<float>			ToVec4f( void ) const;
	vsVec4<double>			ToVec4d( void ) const;
	vsVec4<int>				ToVec4i( void ) const;
	vsVec4<unsigned int>	ToVec4ui( void ) const;
	vsVec4<short>			ToVec4s( void ) const;
	vsVec4<unsigned short>	ToVec4us( void ) const;
	vsVec4<unsigned char>	ToVec4b( void ) const;
	vsVec4<char>			ToVec4c( void ) const;

	T						Dot( const vsVec4< T >& v ) const;
	vsVec4< T >				Cross( const vsVec4< T >& v ) const;
	vsVec4< T >				Abs( void ) const;
	T						Length( void ) const;
	T						LengthSqr( void ) const;
	vsVec4< T >&			Normalize( void );
	vsVec4< T >				Normalized( void ) const;
	void					Clamp( const vsVec4< T >& min, const vsVec4< T >& max );
	void					Snap( void );
	vsVec4< T >				Ceil( void );
	vsVec4< T >				Floor( void );
	void					Truncate( T length );
	void					Zero( void );
	vsVec4< T >&			Lerp( const vsVec4< T >& a, const vsVec4< T >& b, T t );

public:
	T						x;
	T						y;
	T						z;
	T						w;
};

template< typename T > VS_INLINE
vsVec4< T >::vsVec4( void ) {
	Zero();
}

template< typename T > VS_INLINE
vsVec4< T >::vsVec4( T xyzw ) {
	x = y = z = w = xyzw;
}

template< typename T > VS_INLINE
vsVec4< T >::vsVec4( T x, T y, T z, T w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

template< typename T > VS_INLINE
vsVec4< T >::vsVec4( vsVec2< T > v, T z, T w ) {
	this->x = v.x;
	this->y = v.y;
	this->z = z;
	this->w = w;
}

template< typename T > VS_INLINE
vsVec4< T >::vsVec4( vsVec3< T > v, T w ) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = w;
}
	
template< typename T > VS_INLINE
T& vsVec4< T >::operator []( int index ) {
	static T err = ( T )0;
	if ( index<0 || index>3 ) return err;
	return ( &x )[ index ];
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::operator =( const vsVec4< T >& v ) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = v.w;
	return *this;
}

template< typename T > VS_INLINE
bool vsVec4< T >::operator ==( const vsVec4< T >& v ) const {
	return ( this->x == v.x 
		&&  this->y == v.y 
		&&  this->z == v.z 
		&&  this->w == v.w );
}

template< typename T > VS_INLINE
bool vsVec4< T >::operator !=( const vsVec4< T >& v ) const {
	return !( *this == v );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::operator -( void ) const{
	return vsVec4< T >( -x, -y, -z, -w );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::operator -( const vsVec4< T >& v ) const{
	return vsVec4< T >( x-v.x, y-v.y, z-v.z, w-v.w );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::operator -( const T& s ) const{
	return vsVec4< T >( x-s, y-s, z-s, w-s );
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::operator -=( const vsVec4< T >& v ) {
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	this->w -= v.w;
	return *this;
}


template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::operator -=( const T& s ) {
	this->x -= s;
	this->y -= s;
	this->z -= s;
	this->w -= s;
	return *this;
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::operator *( const vsVec4< T >& v ) const {
	return vsVec4< T >( x*v.x , y*v.y, z*v.z, w*v.w );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::operator *( const T& s ) const {
	return vsVec4< T >( x*s , y*s, z*s,w*s );
}

template< typename TT > VS_INLINE
vsVec4< TT > operator*( const TT& f, const vsVec4< TT >& v ) {
	return v*f;
}

template< typename TT > VS_INLINE
vsVec4< TT > operator/( const TT& f, const vsVec4< TT >& v ) {
	return vsVec4< TT >( f/v.x, f/v.y, f/v.z, f/v.w );
}

template< typename TT > VS_INLINE
vsVec4< TT > operator+( const TT& f, const vsVec4< TT >& v ) {
	return v+f;
}

template< typename TT > VS_INLINE
vsVec4< TT > operator-( const TT& f, const vsVec4< TT >& v ) {
	return ( -v )+f;
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::operator *=( const vsVec4< T >& v ) {
	this->x *= v.x;
	this->y *= v.y;
	this->z *= v.z;
	this->w *= v.w;
	return *this;
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::operator *=( const T& s ) {
	this->x *= s;
	this->y *= s;
	this->z *= s;
	this->w *= w;
	return *this;
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::operator /( const vsVec4< T >& v ) const {
	return vsVec4< T >( x/v.x , y/v.y, z/v.z, w/v.w );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::operator /( const T& s ) const {
	T is = ( T )1.0f / s;
	return vsVec4< T >( x*is , y*is, z*is, w*is );
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::operator /=( const vsVec4< T >& v ) {
	this->x /= v.x;
	this->y /= v.y;
	this->z /= v.z;
	this->w /= v.w;
	return *this;
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::operator /=( const T& s ) {
	T s1 = ( T )1.0f/s;
	this->x *= s1;
	this->y *= s1;
	this->z *= s1;
	this->w *= s1;
	return *this;
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::operator +( const vsVec4< T >& v ) const {
	return vsVec4< T >( x+v.x , y+v.y, z+v.z, w+v.w );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::operator +( const T& s ) const {
	return vsVec4< T >( x+s , y+s, z+s, w+s );
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::operator +=( const vsVec4< T >& v ) {
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	this->w += v.w;
	return *this;
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::operator +=( const T& s ) {
	this->x += s;
	this->y += s;
	this->z += s;
	this->w += s;
	return *this;
}

template< typename T > VS_INLINE
bool vsVec4< T >::operator >=( const vsVec4< T >& v ) const {
	return ( x >= v.x && y >= v.y && z >= v.z && w >= v.w );
}

template< typename T > VS_INLINE
bool vsVec4< T >::operator <=( const vsVec4< T >& v ) const {
	return ( x <= v.x && y <= v.y && z <= v.z && w <= v.w );
}

template< typename T > VS_INLINE
bool vsVec4< T >::operator >( const vsVec4< T >& v ) const {
	return ( x > v.x && y > v.y && z > v.z && w > v.w );
}

template< typename T > VS_INLINE
bool vsVec4< T >::operator <( const vsVec4< T >& v ) const {
	return ( x < v.x && y < v.y && z < v.z && w < v.w );
}

template< typename T > VS_INLINE
T* vsVec4< T >::ToTypePtr( void ) {
	return ( &x );
}

template< typename T > VS_INLINE
const T* vsVec4< T >::ToTypePtr( void ) const {
	return ( &x );
}

template< typename T > VS_INLINE
T vsVec4< T >::Dot( const vsVec4< T >& v ) const {
	return( x*v.x + y*v.y + z*v.z + w*v.w );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::Cross( const vsVec4< T >& v ) const {
	return vsVec4< T >( 	y * v.w - y * v.w,
					z * v.x - z * v.x,
					w * v.y - w * v.y,
					x * v.z - x * v.z );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::Abs( void ) const {
	return vsVec4< T >( ( T )fabs( x ), ( T )fabs( y ), ( T )fabs( z ), ( T )fabs( w ) );
}

template< typename T > VS_INLINE
T vsVec4< T >::Length( void ) const {
	return ( T )sqrtf( ( T )( x*x + y*y + z*z + w*w ) );
}

template< typename T > VS_INLINE
T vsVec4< T >::LengthSqr( void ) const {
	return x*x + y*y + z*z + w*w;
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::Normalize( void ) {
	*this /= Length();
	return *this;
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::Normalized( void ) const {
	float l = Length();
	if ( l==0.0f ) 
		return vsVec4f( 0.0f );
	return ( *this / l );
}

template< typename T > VS_INLINE
void vsVec4< T >::Snap( void ) {
	x = T( int( x ) );
	y = T( int( y ) );
	z = T( int( z ) );
	w = T( int( w ) );
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::Ceil( void )
{
	x = ceil( x );
	y = ceil( y );
	z = ceil( z );
	w = ceil( w );
	return *this;
}

template< typename T > VS_INLINE
vsVec4< T > vsVec4< T >::Floor( void )
{
	x = floor( x );
	y = floor( y );
	z = floor( z );
	w = floor( w );
	return *this;
}

template< typename T > VS_INLINE
void vsVec4< T >::Zero( void ) {
	x = y = z = w = ( T )0;
}

template< typename T > VS_INLINE
vsVec4< T >& vsVec4< T >::Lerp( const vsVec4< T >& a, const vsVec4< T >& b, T t ) {
	*this = a * ( T( 1.0f-t ) ) + b * t;

	return( *this );
}

template< typename T > VS_INLINE
vsVec2< T > vsVec4< T >::ToVec2( void ) const {
	return vsVec2< T >( x,y );
}

template< typename T > VS_INLINE
vsVec3< T > vsVec4< T >::ToVec3( void ) const {
	return vsVec3< T >( x,y,z );
}

template< typename T > VS_INLINE
char* vsVec4< T >::ToCharPtr( void ) {
	char* R = new char[128];
	if ( typeid( T ) == typeid( float ) ||
	   typeid( T ) == typeid( double ) )
			sprintf_s( R, 64, "%5.3f/%5.3f/%5.3f/%5.3f", x, y, z, w );
	else
	if ( typeid( T ) == typeid( int ) ||
	   typeid( T ) == typeid( unsigned int ) ||
	   typeid( T ) == typeid( unsigned char ) ||
	   typeid( T ) == typeid( unsigned short ) ||
	   typeid( T ) == typeid( short ) )
			sprintf_s( R, 64, "%d/%d/%d/%d", x, y, z, w );
	else
	if ( typeid( T ) == typeid( char ) )
			sprintf_s( R, 64, "%c/%c/%c/%c", x, y, z, w );
	else
			strcpy_s( R, 64, "unknown type" );
	return R;
}

template< typename T > VS_INLINE
const char* vsVec4< T >::ToCharPtr( void ) const{
	char* R = new char[128];
	if ( typeid( T ) == typeid( float ) ||
	   typeid( T ) == typeid( double ) )
			sprintf_s( R, 64, "%5.3f/%5.3f/%5.3f/%5.3f", x, y, z, w );
	else
	if ( typeid( T ) == typeid( int ) ||
	   typeid( T ) == typeid( unsigned int ) ||
	   typeid( T ) == typeid( unsigned char ) ||
	   typeid( T ) == typeid( unsigned short ) ||
	   typeid( T ) == typeid( short ) )
			sprintf_s( R, 64, "%d/%d/%d/%d", x, y, z, w );
	else
	if ( typeid( T ) == typeid( char ) )
			sprintf_s( R, 64, "%c/%c/%c/%c", x, y, z, w );
	else
			strcpy_s( R, 64, "unknown type" );
	return R;
}

template< typename T > VS_INLINE
void vsVec4< T >::Clamp( const vsVec4< T >& min, const vsVec4< T >& max ) {
	if ( x > max.x ) x = max.x;
	if ( x < min.x ) x = min.x;
	if ( y > max.y ) y = max.y;
	if ( y < min.y ) y = min.y;
	if ( z > max.z ) z = max.z;
	if ( z < min.z ) z = min.z;
	if ( w > max.w ) w = max.w;
	if ( w < min.w ) w = min.w;
}

template< typename T > VS_INLINE
void vsVec4< T >::Truncate( T length ) {
	if ( !length ) {
		Zero();
	}
	else {
		T lr = Length();
		if ( length < lr ) {
			T i = length / lr;
			x *= i;
			y *= i;
			z *= i;
			w *= i;
		}
	}
}

template< typename T > VS_INLINE
vsVec2<float> vsVec4< T >::ToVec2f( void ) const {
	return vsVec2<float>( ( float )x, ( float )y );
}

template< typename T > VS_INLINE
vsVec2<double> vsVec4< T >::ToVec2d( void ) const {
	return vsVec2<double>( ( double )x, ( double )y );
}

template< typename T > VS_INLINE
vsVec2<int> vsVec4< T >::ToVec2i( void ) const {
	return vsVec2<int>( ( int )x, ( int )y );
}
	
template< typename T > VS_INLINE
vsVec2<unsigned int> vsVec4< T >::ToVec2ui( void ) const {
	return vsVec2<unsigned int>( ( unsigned int )x, ( unsigned int )y );
}
	
template< typename T > VS_INLINE
vsVec2<short> vsVec4< T >::ToVec2s( void ) const {
	return vsVec2<short>( ( short )x, ( short )y );
}
	
template< typename T > VS_INLINE
vsVec2<unsigned short> vsVec4< T >::ToVec2us( void ) const {
	return vsVec2<unsigned short>( ( unsigned short )x, ( unsigned short )y );
}
	
template< typename T > VS_INLINE	
vsVec2<unsigned char> vsVec4< T >::ToVec2b( void ) const {
	return vsVec2<unsigned char>( ( unsigned char )x, ( unsigned char )y );
}
	
template< typename T > VS_INLINE
vsVec2<char> vsVec4< T >::ToVec2c( void ) const {
	return vsVec2<char>( ( char )x, ( char )y );
}
	
template< typename T > VS_INLINE
vsVec3<float> vsVec4< T >::ToVec3f( void ) const {
	return vsVec3<float>( ( float )x, ( float )y, ( float )z );
}
	
template< typename T > VS_INLINE
vsVec3<double> vsVec4< T >::ToVec3d( void ) const {
	return vsVec3<double>( ( double )x, ( double )y, ( double )z );
}
	
template< typename T > VS_INLINE
vsVec3<int> vsVec4< T >::ToVec3i( void ) const {
	return vsVec3<int>( ( int )x, ( int )y, ( int )z );
}
	
template< typename T > VS_INLINE
vsVec3<unsigned int> vsVec4< T >::ToVec3ui( void ) const {
	return vsVec3<unsigned int>( ( unsigned int )x, ( unsigned int )y, ( unsigned int )z );
}
	
template< typename T > VS_INLINE
vsVec3<short> vsVec4< T >::ToVec3s( void ) const {
	return vsVec3<short>( ( short )x, ( short )y, ( short )z );
}
	
template< typename T > VS_INLINE
vsVec3<unsigned short> vsVec4< T >::ToVec3us( void ) const {
	return vsVec3<unsigned short>( ( unsigned short )x, ( unsigned short )y, ( unsigned short )z );
}
	
template< typename T > VS_INLINE
vsVec3<unsigned char> vsVec4< T >::ToVec3b( void ) const {
	return vsVec3<unsigned char>( ( unsigned char )x, ( unsigned char )y, ( unsigned char )z );
}
	
template< typename T > VS_INLINE
vsVec3<char> vsVec4< T >::ToVec3c( void ) const {
	return vsVec3<char>( ( char )x, ( char )y, ( char )z );
}
	
template< typename T > VS_INLINE
vsVec4<float> vsVec4< T >::ToVec4f( void ) const {
	return vsVec4<float>( ( float )x, ( float )y, ( float )z, ( float )w );
}
	
template< typename T > VS_INLINE
vsVec4<double> vsVec4< T >::ToVec4d( void ) const {
	return vsVec4<double>( ( double )x, ( double )y, ( double )z, ( double )w );
}
	
template< typename T > VS_INLINE
vsVec4<int> vsVec4< T >::ToVec4i( void ) const {
	return vsVec4<int>( ( int )x, ( int )y, ( int )z, ( int )w );
}
	
template< typename T > VS_INLINE
vsVec4<unsigned int> vsVec4< T >::ToVec4ui( void ) const {
	return vsVec4<unsigned int>( ( unsigned int )x, ( unsigned int )y, ( unsigned int )z, ( unsigned int )w );
}
	
template< typename T > VS_INLINE
vsVec4<short> vsVec4< T >::ToVec4s( void ) const {
	return vsVec4<short>( ( short )x, ( short )y, ( short )z, ( short )w );
}
	
template< typename T > VS_INLINE
vsVec4<unsigned short> vsVec4< T >::ToVec4us( void ) const {
	return vsVec4<unsigned short>( ( unsigned short )x, ( unsigned short )y, ( unsigned short )z, ( unsigned short )w );
}
	
template< typename T > VS_INLINE
vsVec4<unsigned char> vsVec4< T >::ToVec4b( void ) const {
	return vsVec4<unsigned char>( ( unsigned char )x, ( unsigned char )y, ( unsigned char )z, ( unsigned char )w );
}




typedef vsVec2<float>			vsVec2f;
typedef vsVec3<float>			vsVec3f;
typedef vsVec4<float>			vsVec4f;
typedef vsVec2<double>			vsVec2d;
typedef vsVec3<double>			vsVec3d;
typedef vsVec4<double>			vsVec4d;
typedef vsVec2<int>				vsVec2i;
typedef vsVec3<int>				vsVec3i;
typedef vsVec4<int>				vsVec4i;
typedef vsVec2<unsigned int>	vsVec2ui;
typedef vsVec3<unsigned int>	vsVec3ui;
typedef vsVec4<unsigned int>	vsVec4ui;
typedef vsVec2<unsigned char>	vsVec2b;
typedef vsVec3<unsigned char>	vsVec3b;
typedef vsVec4<unsigned char>	vsVec4b;
typedef vsVec2<char>			vsVec2c;
typedef vsVec3<char>			vsVec3c;
typedef vsVec4<char>			vsVec4c;
typedef vsVec2<short>			vsVec2s;
typedef vsVec3<short>			vsVec3s;
typedef vsVec4<short>			vsVec4s;
typedef vsVec2<unsigned short>	vsVec2us;
typedef vsVec3<unsigned short>	vsVec3us;
typedef vsVec4<unsigned short>	vsVec4us;

const vsVec2f NULLVEC2F = vsVec2f( 0.f );
const vsVec3f NULLVEC3F = vsVec3f( 0.f );
const vsVec4f NULLVEC4F = vsVec4f( 0.f );
const vsVec2i NULLVEC2I = vsVec2i( 0 );
const vsVec3i NULLVEC3I = vsVec3i( 0 );
const vsVec4i NULLVEC4I = vsVec4i( 0 );
const vsVec2ui NULLVEC2UI = vsVec2ui( 0 );
const vsVec3ui NULLVEC3UI = vsVec3ui( 0 );
const vsVec4ui NULLVEC4UI = vsVec4ui( 0 );
const vsVec2s NULLVEC2S = vsVec2s( 0 );
const vsVec3s NULLVEC3S = vsVec3s( 0 );
const vsVec4s NULLVEC4S = vsVec4s( 0 );
const vsVec2us NULLVEC2US = vsVec2us( 0 );
const vsVec3us NULLVEC3US = vsVec3us( 0 );
const vsVec4us NULLVEC4US = vsVec4us( 0 );
const vsVec2d NULLVEC2D = vsVec2d( 0 );
const vsVec3d NULLVEC3D = vsVec3d( 0 );
const vsVec4d NULLVEC4D = vsVec4d( 0 );
const vsVec2b NULLVEC2B = vsVec2b( 0 );
const vsVec3b NULLVEC3B = vsVec3b( 0 );
const vsVec4b NULLVEC4B = vsVec4b( 0 );
const vsVec2c NULLVEC2C = vsVec2c( 0 );
const vsVec3c NULLVEC3C = vsVec3c( 0 );
const vsVec4c NULLVEC4C = vsVec4c( 0 );

#endif