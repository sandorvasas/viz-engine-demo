#include "String.h"
#include <stdio.h>
#include <string.h>
#include "system.h"
/*
==================================
	Constructor1()
==================================
*/
vsStr::vsStr( void ) {
	len = 0;
	string = "\0";
}

/*
==================================
	Constructor2()
==================================
*/
vsStr::vsStr( const char* str ) {
	len = ( int )strlen( str );
	string = new char[len+1];
	for ( int i=0; i<len; ++i )
		string[i] = str[i];
	string[len] = '\0';
}

/*
==================================
	Constructor3()
==================================
*/
vsStr::vsStr( char* format, ... ) {
	int length = ( int )strlen( format )+256;
	va_list list;
	char* text = new char[length];
	va_start( list, format );
		vsprintf_s( text, length, format, list );
	va_end( list );

	len = ( int )strlen( text );
	string = new char[len+1];
	for ( int i=0; i<len; ++i )
		string[i] = text[i];
	string[len] = '\0';

	delete [] text;
}

/*
==================================
	Constructor4()
==================================

vsStr::vsStr( char* str ) {
	len = ( int )strlen( str );
	string = new char[len+1];
	for ( int i=0; i<len; ++i )
		string[i] = str[i];
	string[len] = '\0';
}*/

/*
==================================
	Constructor4()
==================================

vsStr::vsStr( const vsStr& str ) {
	len = str.Length();
	string = new char[len];
	strcpy_s( string, len, str.string );
}
*/
/*
==================================
	Destructor()
==================================
*/
vsStr::~vsStr( void ) {
	if ( len>0 ) {
		delete [] string;
		len = 0;
		string = "\0";
	}
}

/*
==================================
	operator =
==================================
*/
vsStr& vsStr::operator=( const vsStr& b ) {
	Zero();
	this->len = b.len;
	string = new char[len+1];
	for ( int i=0; i<len; ++i )
		string[i] = b.string[i];
	string[len] = '\0';

	return( *this );
}

/*
==================================
	operator ==
==================================
*/
bool vsStr::operator==( const vsStr& b ) {
	if ( b.len == 0 || len == 0 ) 
		return false;
	return( !strcmp( string, b.string ) );
}

/*
==================================
	operator !=
==================================
*/
bool vsStr::operator!=( const vsStr& b ) {
	return!( *this == b );
}

/*
==================================
	operator =
==================================
*/
vsStr& vsStr::operator=( const char* b ) {
	Zero();
	delete [] string;
	this->len = int( strlen( b ) );
	string = new char[len+1];
	for ( int i=0; i<len; ++i )
		string[i] = b[i];
	string[len] = '\0';

	return( *this );
}

/*
==================================
	operator ==
==================================
*/
bool vsStr::operator==( const char* b ) {
	return( len == int( strlen( b ) ) && !strcmp( string, b ) );
}

/*
==================================
	operator !=
==================================
*/
bool vsStr::operator!=( const char* b ) {
	return!( *this == b );
}

/*
==================================
	operator =
==================================
*/
vsStr& vsStr::operator=( char* b ) {
	return ( this->operator=( ( const char* )b ) );
}

/*
==================================
	operator ==
==================================
*/
bool vsStr::operator==( char* b ) {
	return ( this->operator==( ( const char* )b ) );
}

/*
==================================
	operator !=
==================================
*/
bool vsStr::operator!=( char* b ) {
	return ( this->operator!=( ( const char* )b ) );
}

/*
==================================
	operator >
==================================
*/
bool vsStr::operator>( const vsStr& b ) {

	int l = ( len>b.len )? b.len : len;
	for ( int i=0; i<l; ++i ) 
	{
		if ( string[i] > b.string[i] )
			return true;
	}

	return( len > b.len );
}

/*
==================================
	operator <
==================================
*/
bool vsStr::operator<( const vsStr& b ) {

	int l = ( len>b.len )? b.len : len;
	for ( int i=0; i<l; ++i ) 
	{
		if ( string[i] < b.string[i] )
			return true;
	}

	return( len < b.len );
}

/*
==================================
	operator >=
==================================
*/
bool vsStr::operator>=( const vsStr& b ) {

	int l = ( len>b.len )? b.len : len;
	for ( int i=0; i<l; ++i ) 
	{
		if ( string[i] > b.string[i] )
			return true;
	}

	return( len >= b.len );
}

/*
==================================
	operator <=
==================================
*/
bool vsStr::operator<=( const vsStr& b ) {
	int l = ( len>b.len )? b.len : len;
	for ( int i=0; i<l; ++i ) 
	{
		if ( string[i] < b.string[i] )
			return true;
	}

	return( len <= b.len );
}

/*
==================================
	operator []
==================================
*/
char& vsStr::operator[]( int num ) {
	static char f = '\0';
	if ( num>-1 && num < len )
		return string[num];
	else
		return f;
}

/*
==================================
	operator ()
==================================
*/
char* vsStr::operator()( void ) {
	return( string );
}

/*
==================================
	operator ()
==================================
*/
const char* vsStr::operator()( void ) const {
	return( string );
}

/*
==================================
	operator +
==================================
*/
vsStr vsStr::operator+( const vsStr& b ) const {
	vsStr result = *this;
	result.Append( b );
	return( result );
}

/*
==================================
	operator +=
==================================
*/
const vsStr& vsStr::operator+=( const vsStr& b ) {
	*this = *this + b;
	return( *this );
}

/*
==================================
	Zero()
==================================
*/
void vsStr::Zero( void ) {
	if ( len>0 ) {
		len = 0;
		delete [] string;
		string = "\0";
	}
}

/*
==================================
	CorrectLength()
==================================
*/
void vsStr::CorrectLength( void ) {
	if ( len>0 ) {
		int i=0;
		while( string[i] != '\0' && i <= len ) i++;
		if ( i<len ) len = i-1;
	}
}

/*
==================================
	Reserve()
==================================
*/
void vsStr::Reserve( int length ) {
	if ( length > 0 ) {
		Zero();
		string = new char[ length ];
		len = length;
		ZeroMemory( string, length );
	}
}

/*
==================================
	Length()
==================================
*/
int vsStr::Length( void ) const {
	return len;
}

/*
==================================
	TruncateTo()
==================================
*/
void vsStr::TruncateTo( int maxLen ) {
	if ( len>maxLen ) {
		if ( maxLen>0 ) {
			len = maxLen;
			char* tmp = new char[maxLen+1];
			for ( int i=0; i<maxLen; ++i )
				tmp[i] = string[i];
			if ( len ) delete [] string;
			string = tmp;
			string[ len ] = '\0';
		} else
		if ( maxLen==0 ) {
			Zero();
		}
	}
}

/*
==================================
	Find()
==================================
*/
int vsStr::Find( char ch ) const {
	for ( int i=0; i<len; ++i ) 
		if ( ch==string[i] )
			return i;
	return -1;
}

/*
==================================
	Find()
==================================
*/
int vsStr::Find( char* str ) const {
	int p = ( int )strstr( string, str );

	if ( p )
		return p - ( int )( string );

	return( -1 );
}

/*
==================================
	Find()
==================================
*/
int vsStr::Find( const vsStr& str ) const {
	int l = str.Length();
	bool found;

	for ( int i=0; i<len; ++i ) {
		found=true;
		for ( int j=0; j<l; ++j ) {
			if ( string[i] != str.string[j] )
				found = false;
		}
		if ( found )
			return i;
	}

	return( -1 );
}

/*
==================================
	FindFromBack()
==================================
*/
int vsStr::FindFromBack( char ch ) const {
	for ( int i=len-1; i>0; --i ) 
		if ( ch==string[i] )
			return i;
	return -1;
}

/*
==================================
	FindFromBack()
==================================
*/
int vsStr::FindFromBack( char* str ) const {
	int l = ( int )strlen( str );
	bool found;

	for ( int i=0; i<len; ++i ) {
		found=true;
		for ( int j=0; j<l; ++j ) {
			if ( string[i] != str[j] )
				found = false;
		}
		if ( found )
			return i;
	}

	return( -1 );
}

/*
==================================
	FindFromBack()
==================================
*/
int vsStr::FindFromBack( const vsStr& str ) const {
	return 0;
}

/*
==================================
	CharAt()
==================================
*/
char vsStr::CharAt( int index ) const {
	if ( index>-1 && index<len )
		return string[index];
	else
		return '\0';
}

/*
==================================
	ReplaceFirst()
==================================
*/
void vsStr::ReplaceFirst( char ch, char with ) {
	int i = Find( ch );
	if ( i!=-1 )
		string[i] = with;
}

/*
==================================
	ReplaceAll()
==================================
*/
void vsStr::ReplaceAll( char ch, char with ) {
	int i ;
	while( ( i=Find( ch ) ) != -1 ) {
		string[i] = with;
	}
}

/*
==================================
	ReplaceFirst()
==================================
*/
void vsStr::ReplaceFirst( const vsStr& str, const vsStr& with ) {
}

/*
==================================
	ReplaceAll()
==================================
*/
void vsStr::ReplaceAll( const vsStr& str, const vsStr& with ) {
}

/*
==================================
	DeleteAll()
==================================
*/
void vsStr::DeleteAll( char ch ) {
	for ( int i=0; i<len && len>1; ++i )
	{
		if ( string[i] == ch )
		{
			if ( i<len )
				for ( int j=i; j<len-1; ++j )
				{
					string[j] = string[j+1];
				}
			//else
			//	string[i] = '\0';
			--len;
		}
	}

}

/*
==================================
	SubStr()
==================================
*/
vsStr vsStr::SubStr( int start, int length ) 
{
	if ( start >= len || length == 0 ) return vsStr();
	if ( start < 0 )					start = 0;
	if ( start + length > len )		length = len - start;

	vsStr res;
	res.Reserve( length+1 );
//	for ( int i = start, j=0; j<length; ++i, ++j )
//		res[j] = string[i];
//	res[ length ] = '\0';
	for ( int i=0; i<length; i++ )
		res.string[i] = 'D';
	return( res );
}

/*
==================================
	Insert()
==================================
*/
void vsStr::Insert( int to, char ch ) {
	if ( to>-1 && to<len ) {
		*this = SubStr( 0, to ) + vsStr( &ch ) + SubStr( to+1, len-to-1 );
	}
}

/*
==================================
	Insert()
==================================
*/
void vsStr::Insert( int to, char* str ) {
	if ( to>-1 && to<len ) {
		vsStr s( str );
		*this = SubStr( 0, to ) + s + SubStr( to+s.Length(), len-to-s.Length() );
	}
}

/*
==================================
	Insert()
==================================
*/
void vsStr::Insert( int to, const vsStr& str ) {
	if ( to>-1 && to<len ) {
		*this = SubStr( 0, to ) + str + SubStr( to+str.Length(), len-to-str.Length() );
	}
}

/*
==================================
	Reverse()
==================================
*/
void vsStr::Reverse( void ) {
	for ( int i=0; i<len/2; ++i )
		string[i] = string[len-1-i];
}

/*
==================================
	ToLowerCase()
==================================
*/
void vsStr::ToLowerCase( void ) {
	for ( int i=0; i<len; ++i )
		if ( string[i]>64 && string[i] < 91 ) 
			string[i] += 32;
}

/*
==================================
	ToUpperCase()
==================================
*/
void vsStr::ToUpperCase( void ) {
	for ( int i=0; i<len; ++i )
		if ( string[i]>96 && string[i] < 123 ) 
			string[i] -= 32;
}

/*
==================================
	Append()
==================================
*/
void vsStr::Append( char ch ) {
	int newLen = len + 1;
	char *newBuf = new char[newLen+1];

	for ( int i=0; i<len; ++i ) 
		newBuf[ i ] = string[ i ];
		
	newBuf[ len ] = ch;

	if ( len )delete [] string;
	string = newBuf;
	len = newLen;
	string[len] = '\0';
}

/*
==================================
	Append()
==================================
*/
void vsStr::Append( char* str ) {
	Append( vsStr( str ) );
}

/*
==================================
	Append()
==================================
*/
void vsStr::Append( const vsStr& str ) {
	if ( str.len>0 ) {
		int newLen = len + str.len;
		char *newBuf = new char[newLen+1];

		for ( int i=0; i<newLen; ++i ) 
			if ( i<len )
				newBuf[ i ] = string[ i ];
			else
				newBuf[ i ] = str.string[ i - len ];

		if ( len )delete [] string;
		string = newBuf;
		len = newLen;
		string[len] = '\0';
	}
}

/*
==================================
	Expand()
==================================
*/
void vsStr::Expand( void ) {
	Append( '\0' );
}

/*
==================================
	Narrow()
==================================
*/
void vsStr::Narrow( void )  {
	TruncateTo( len - 1 );
}


/*
==================================
	ToCharPtr()
==================================
*/
char* vsStr::ToCharPtr( void ) {
	return( string );
}

/*
==================================
	ToCharPtr()
==================================
*/
const char* vsStr::ToCharPtr( void ) const {
	return( string );
}
