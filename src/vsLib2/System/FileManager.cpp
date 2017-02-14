#include "Filemanager.h"
#include "System.h"

/*
=============================================
		vsInputFile
=============================================
*/
vsInputFile::vsInputFile( void ) {
	Handle = NULL;
	open = false;
}

vsInputFile::~vsInputFile( void ) {
	Close();
	//if ( Handle ) delete Handle;
}

void vsInputFile::AssignOpenedFileDesc( FILE* f )
{
	open = true;
	Handle = f;
}

int vsInputFile::Open( char* szFileName, const char* openmode ) {
	int f = fopen_s( &Handle, ( const char* )szFileName,  openmode );
	if ( !f ) {
		open=true;
		return VS_OK;
	} else {
		open=false;
		return VS_ERROR;
	}
}

int vsInputFile::Open( const vsStr& szFileName, const char* openmode ) {
	return Open( ( char* )szFileName(), openmode );
}

void vsInputFile::Close( void ) {
	if ( open )
		fclose( Handle );
}

long vsInputFile::Tell( void ) {
	if ( open )
	return ftell( Handle );
	else return NULL;
}

long vsInputFile::Remaining( void ) {
	return Size() - Tell();
}

void vsInputFile::Seek( long offset, int origin ) {
	if ( !open ) return;
	fseek( Handle, offset, origin );
}

long vsInputFile::Size( void ) {
	if ( !open ) return 0;
	long p = Tell();
	Seek( 0, SEEK_END );
	long a = Tell();
	Seek( p, SEEK_SET );
	return a;
}

bool vsInputFile::Eof( void ) {
	return !( Remaining() > 0 );
}

void vsInputFile::ReadChar( char* i ) {
	if ( !open )return;
	fread( i, sizeof( char ), 1, Handle );
}

void vsInputFile::ReadByte( unsigned char* i ) {
	if ( !open )return;
	fread( i, sizeof( unsigned char ), 1, Handle );
}

void vsInputFile::ReadInt( int* i ) {
	if ( !open ) return;
	fread( i, sizeof( int ), 1, Handle );
}

void vsInputFile::ReadUnsignedInt( unsigned int* i ) {
	if ( !open ) return;
	fread( i, 4, 1, Handle );
}

void vsInputFile::ReadShort( short* i ) {
	if ( !open ) return;
	fread( i, sizeof( short ), 1, Handle );
}

void vsInputFile::ReadUnsignedShort( unsigned short* i ) {
	if ( !open ) return;
	fread( i, sizeof( unsigned short ), 1, Handle );
}

void vsInputFile::ReadLong( long* i ) {
	if ( !open ) return;
	fread( i, sizeof( long ), 1, Handle );
}

void vsInputFile::ReadUnsignedLong( unsigned long* i ) {
	if ( !open ) return;
	fread( i, sizeof( unsigned long ), 1, Handle );
}

void vsInputFile::ReadFloat( float* f ) {
	if ( !open ) return;
	fread_s( f, sizeof_float, sizeof_float, 1, Handle );
}

void vsInputFile::ReadVec2( vsVec2f* v ) {
	if ( !open ) return;
	fread( v, 8, 1, Handle );
}

void vsInputFile::ReadVec3( vsVec3f* v ) {
	if ( !open ) return;
	fread( v, 12, 1, Handle );
}

void vsInputFile::ReadVec4( vsVec4f* v ) {
	if ( !open ) return;
	fread( v, 16, 1, Handle );
}

void vsInputFile::ReadMat2( vsMat2* m )
{
	if ( !open ) return;
	fread( m, 16, 1, Handle );
}

void vsInputFile::ReadMat3( vsMat3* m )
{
	if ( !open ) return;
	fread( m, 36, 1, Handle );
}

void vsInputFile::ReadMat4( vsMat4* m )
{
	if ( !open ) return;
	fread( m, 64, 1, Handle );
}

void vsInputFile::ReadDouble( double * d ) {
	if ( !open ) return;
	fread( d, sizeof( double ), 1, Handle );
}

void vsInputFile::ReadBool( bool * b ) {
	if ( !open ) return;
	fread( b, sizeof( bool ), 1, Handle );
}

void vsInputFile::Read( void* i, int size ) {
	if ( !open ) return;
	fread( i, size, 1, Handle );
}

void vsInputFile::ReadLine( char* buf, int size ) 
{
	ZeroMemory( buf, size );
	fgets( buf, size, Handle );
}

void vsInputFile::ReadLine( vsStr * str ) 
{
	fgets( ( *str )(), str->Length(), Handle );
}

/*
void vsInputFile::Scanf( const char* format, ... ) {
	fscanf_s( format, ... );
}*/



/*
=============================================


		vsOutputFile


=============================================
*/
vsOutputFile::vsOutputFile( void ) {
	Handle = NULL;
	open = false;
}

vsOutputFile::~vsOutputFile( void ) {
	Close();
//	if ( Handle ) delete Handle;
}

void vsOutputFile::AssignOpenedFileDesc( FILE* f )
{
	open = true;
	Handle = f;
}

int vsOutputFile::Open( char* szFileName, const char* openmode  ) {
	int f = fopen_s( &Handle, ( const char* )szFileName, openmode );
	if ( !f ) {
		open = true;
		filename = szFileName;
		return VS_OK;
	} else {
		open=false;
		return VS_ERROR;
	}
}

void vsOutputFile::Close( void ) {
	if ( open )
		fclose( Handle );
}

long vsOutputFile::Tell( void ) {
	if ( open )
	return ftell( Handle );
	else return NULL;
}

void vsOutputFile::Seek( long offset, int origin ) {
	if ( !open ) return;
	fseek( Handle, offset, origin );
}

long vsOutputFile::Size( void ) {
	if ( !open ) return 0;
	long p = Tell();
	Seek( 0, SEEK_END );
	long a = Tell();
	Seek( p, SEEK_SET );
	return a;
}

void vsOutputFile::WriteChar( char* i ) {
	if ( !open )return;
	fwrite( i, sizeof( char ), 1, Handle );
}

void vsOutputFile::WriteByte( unsigned char* i ) {
	if ( !open )return;
	fwrite( i, sizeof( unsigned char ), 1, Handle );
}

void vsOutputFile::WriteInt( int* i ) {
	if ( !open ) return;
	fwrite( i, sizeof( int ), 1, Handle );
}

void vsOutputFile::WriteUnsignedInt( unsigned int* i ) {
	if ( !open ) return;
	fwrite( i, sizeof( unsigned int ), 1, Handle );
}

void vsOutputFile::WriteShort( short* i ) {
	if ( !open ) return;
	fwrite( i, sizeof( short ), 1, Handle );
}

void vsOutputFile::WriteUnsignedShort( unsigned short* i ) {
	if ( !open ) return;
	fwrite( i, sizeof( unsigned short ), 1, Handle );
}

void vsOutputFile::WriteLong( long* i ) {
	if ( !open ) return;
	fwrite( i, sizeof( long ), 1, Handle );
}

void vsOutputFile::WriteUnsignedLong( unsigned long* i ) {
	if ( !open ) return;
	fwrite( i, sizeof( unsigned long ), 1, Handle );
}

void vsOutputFile::WriteFloat( float* f ) {
	if ( !open ) return;
	fwrite( f, sizeof( float ), 1, Handle );
}

void vsOutputFile::WriteVec2( vsVec2f *v ) {
	if ( !open ) return;
	fwrite( v, sizeof( vsVec2f ), 1, Handle );
}

void vsOutputFile::WriteVec3( vsVec3f* v ) {
	if ( !open ) return;
	fwrite( v, sizeof( vsVec3f ), 1, Handle );
}

void vsOutputFile::WriteVec4( vsVec4f* v ) {
	if ( !open ) return;
	fwrite( v, sizeof( vsVec4f ), 1, Handle );
}

void vsOutputFile::WriteMat2( vsMat2* m ) {
	if ( !open ) return;
	fwrite( m, 16, 1, Handle );
}

void vsOutputFile::WriteMat3( vsMat3* m ) {
	if ( !open ) return;
	fwrite( m, 36, 1, Handle );
}

void vsOutputFile::WriteMat4( vsMat4* m ) {
	if ( !open ) return;
	fwrite( m, 64, 1, Handle );
}

void vsOutputFile::WriteBool( bool* b ) {
	if ( !open ) return;
	fwrite( b, sizeof( bool ), 1, Handle );
}

void vsOutputFile::WriteDouble( double * d ) {
	if ( !open ) return;
	fwrite( d, sizeof( double ), 1, Handle );
}

void vsOutputFile::WriteText( char* format, ... ) {
	if ( !open )return;
	char* text;
	size_t l = strlen( format )+4096;
	text = new char[l];

	va_list lista;
	va_start( lista, format );
	vsprintf_s( text, l, format, lista );
	va_end( lista );
	fwrite( text, 1, strlen( text ), Handle );

	delete [] text;
}

void vsOutputFile::Write( void* i, int size ) {
	if ( !open ) return;
	fwrite( i, size, 1, Handle );
}