#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdio.h>
#include <memory.h>
#include <string.h>

#if defined( WIN32 ) || defined( _WIN32 )
#include <windows.h>
#endif


#define VS_INLINE inline
#define ON true
#define OFF false

#ifndef UINT
#define UINT unsigned int
#endif

#ifndef ULONG
#define ULONG unsigned long
#endif

#ifndef CHEWBACCA
#define CHEWBACCA tall_furry_guy_from_kashyyk
#endif

#ifndef JAWA
#define JAWA little_robot_sellers_from_tatooine
#endif

#ifndef DARTHVADER
#define DARTHVADER phooooo_phaaaa_phoooo_phaaaa
#endif


typedef unsigned char	byte;
typedef unsigned int	uint;
typedef unsigned long	ulong;
typedef unsigned short	ushort;
typedef unsigned int	Enum;
typedef unsigned int	Texture;

const size_t sizeof_float	= sizeof( float );
const size_t sizeof_int		= sizeof( int );
const size_t sizeof_uint	= sizeof( unsigned int );
const size_t sizeof_short	= sizeof( short );
const size_t sizeof_ulong	= sizeof( unsigned long );

#define VS_INLINE inline
#define BUFFER_OFFSET( i ) ( ( ( char* )NULL ) + ( i ) )
#define BETWEEN( var, lo, hi ) ( lo<var && var<hi )
#define REPEAT( times ) for ( int i=0;i<times; ++i )
#define REPEAT2( var_name, times ) \
			for ( int var_name=0; var_name<times; ++var_name )
#define REPU( times ) for ( uint i=0; i<times; ++i )
#define REPU2( var_name, times ) \
			for ( uint var_name=0; var_name<times; ++var_name )
#define REP( type, times ) for ( type i=0;i<times; ++i )

#define EMPTY_TRUCTORS( type )	\
			type( void ) {};	\
			~type( void ) {};	

#define MINIMUM_OPERATORS( type )								\
		type& operator=( const type& r ) {					\
			memcpy_s( this, sizeof( *this ), &r, sizeof( r ) );	\
			return *this;									\
		};													\
		bool operator==( const type& r ) {					\
			return memcmp( this, &r, sizeof( *this ) )==0;		\
		};													\
		bool operator!=( const type& r ) {					\
			return memcmp( this, &r, sizeof( *this ) )!=0;		\
		};

/*
==============================
   return types
==============================
*/
enum VSRESULT {
	VS_OK = 0,				// everything went fine
	VS_ERROR,				// defines an unknown error
	VS_FILE_NOT_FOUND,		// for file loading
	VS_INVALID_FILE			// invalid file 
};



/*
==============================================
   
	vsSystem

==============================================
*/
namespace vsSystem {
	void						msg32( char* text, ... );
	template< class T >void		Swap( T& a, T& b );
	void						CompressByteArray( byte* byteArray, uint len, int compressionSign, byte* outArray, uint& outLen );
	void						DecompressByteArray( byte* byteArray, uint len, int compressionSign, byte* outArray, uint& outLen );
};


/*
==============================
   msg32 - Win32 message
==============================
*/
VS_INLINE void vsSystem::msg32( char* text, ... ) {
	va_list list;
	char str[ 1024 ];

	va_start( list, text );
	vsprintf_s( str, 1024, text, list );
	va_end( list );

	MessageBox( NULL, str, "message", MB_OK );
}

/*
==============================
	Swap<T>
==============================
*/
template< class T >
VS_INLINE void vsSystem::Swap( T& a, T& b ) {
	T temp = a;
	a = b;
	b = temp;
}


#endif
