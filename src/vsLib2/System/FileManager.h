#ifndef __VSFILEMANAGER_H__
#define __VSFILEMANAGER_H__

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

#include "String.h"
#include "..\Math\Math.h"

class vsInputFile {
public:
					vsInputFile( void );
					~vsInputFile( void );

	void			AssignOpenedFileDesc( FILE* f );
	int				Open( char* szFileName, const char* openmode  );
	int				Open( const vsStr& szFileName, const char* openmode );
	void			Close( void );
	void			Seek( long offset, int origin );
	bool			Eof( void );
	long			Size( void );
	long			Tell( void );
	long			Remaining( void );

	void			ReadChar( char* i );
	void			ReadByte( unsigned char* i );
	void			ReadInt( int * i );
	void			ReadUnsignedInt( unsigned int * i );
	void			ReadShort( short * i );
	void			ReadUnsignedShort( unsigned short * i );
	void			ReadLong( long * i );
	void			ReadUnsignedLong( unsigned long * i );
	void			ReadFloat( float* f );
	void			ReadVec2( vsVec2f* v );
	void			ReadVec3( vsVec3f* v );
	void			ReadVec4( vsVec4f* v );
	void			ReadMat2( vsMat2* m );
	void			ReadMat3( vsMat3* m );
	void			ReadMat4( vsMat4* m );
	void			ReadDouble( double * d );
	void			ReadBool( bool * b );
	void			Read( void* i, int size );
	void			ReadLine( char* buf, int size );
	void			ReadLine( vsStr* str );
	//void			Scanf( const char* format, ... );

public:
	FILE*			Handle;
	bool			open;
	char*			fileName;
};

class vsOutputFile {
public:
					vsOutputFile( void );
					~vsOutputFile( void );

	void			AssignOpenedFileDesc( FILE* f );
	int				Open( char* szFileName, const char* openmode );
	void			Close( void );
	long			Tell( void );
	void			Seek( long offset, int origin );
	long			Size( void );

	void			WriteChar( char* i );
	void			WriteByte( unsigned char* i );
	void			WriteInt( int * i );
	void			WriteUnsignedInt( unsigned int * i );
	void			WriteShort( short * i );
	void			WriteUnsignedShort( unsigned short * i );
	void			WriteLong( long * i );
	void			WriteUnsignedLong( unsigned long * i );
	void			WriteFloat( float* f );
	void			WriteVec2( vsVec2f* v );
	void			WriteVec3( vsVec3f* v );
	void			WriteVec4( vsVec4f* v );
	void			WriteMat2( vsMat2* m );
	void			WriteMat3( vsMat3* m );
	void			WriteMat4( vsMat4* m );
	void			WriteBool( bool* b );
	void			WriteDouble( double * d );
	void			Write( void* i, int size );
	void			WriteText( char* format, ... );

public:
	FILE*			Handle;
	bool			open;
	char*			filename;
};

#endif