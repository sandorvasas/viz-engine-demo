#include <windows.h>
#include <stdio.h>
#include "OpenGL/GLexp.h"
#include "Texture.h"


texture_t& texture_t::operator=( const texture_t& b )
{
	name = b.name;
	value = b.value;
	return( *this );
}

bool texture_t::operator==( const texture_t& b )
{
	return( name==b.name && value==b.value );
}

bool texture_t::operator!=( const texture_t& b )
{
	return!( *this==b );
}


TextureData_t* LoadDDSFile( const char* szFilename, uint fromMipmapLevel )
{
	FILE* file;
	DDSURFACEDESC2_copiedfromddraw ddsd;
	char magicNumber[ 4 ];
	TextureData_t * tdata;

	if ( fopen_s( &file, szFilename, "rb" ) ) {
		return NULL;
	}

	fread_s( &magicNumber, sizeof( char )*4, sizeof( char ), 4, file );
	fread_s( &ddsd, sizeof( ddsd ), sizeof( ddsd ), 1, file );

	tdata = new TextureData_t;
	tdata->dwWidth = ddsd.dwWidth;
	tdata->dwHeight = ddsd.dwHeight;
	
	if ( fromMipmapLevel >= ddsd.dwMipMapCount )
		fromMipmapLevel = 0;

	tdata->dwMipmapCount = ddsd.dwMipMapCount - fromMipmapLevel;

	switch( ddsd.ddpfPixelFormat.dwFourCC ) {
		case MAKEFOURCC( 'D','X', 'T', '1' ): tdata->dwFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;

		case MAKEFOURCC( 'D','X', 'T', '3' ): tdata->dwFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;

		case MAKEFOURCC( 'D','X', 'T', '5' ): tdata->dwFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	}


	if ( fromMipmapLevel > 0 ) {
		int blockSize = ( tdata->dwFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )? 8 : 16;

		for ( uint i = 0; i < fromMipmapLevel; i++, tdata->dwWidth /= 2, tdata->dwHeight /= 2 ) 
			fseek( file, ( ( tdata->dwWidth+3 )/4 ) * ( ( tdata->dwHeight+3 )/4 ) * blockSize, SEEK_CUR );
	}



	int currentPos = ftell( file );
	fseek( file, 0, SEEK_END );
	int filesize = ftell( file );
	fseek( file, currentPos, SEEK_SET );

	int remains = filesize - currentPos;

	tdata->pData = new unsigned char[ remains ];

	for ( long i=0; i<remains; ++i ) {
		fread_s( &tdata->pData[i], 1, 1, 1, file );
	}

	fclose( file );

	return tdata;
}

/*
------------------------------------------
	_LoadTexture
		Loads .dds texture starting from mipmap level loadFromMipmapLevel
		Generates 3D texture with depth texture3D_depth if texture3D_depth > 0

	Function is used by vsRenderSystem.
	Call renderSystem->LoadTexture() !
------------------------------------------
*/
unsigned int _LoadTexture(	const char* szFilename, 
							int magfilter, 
							int minfilter, 
							int anisotropy, 
							uint loadFromMipmapLevel, 
							uint texture3D_depth )
{
	TextureData_t * tdata = LoadDDSFile( szFilename, loadFromMipmapLevel );
	int blockSize, offset, mipSize;
	unsigned int id;

	if ( tdata == NULL ) 
		return 0;

	GLenum target = ( texture3D_depth ) ? GL_TEXTURE_3D : GL_TEXTURE_2D;
	glGenTextures( 1, &id );
	glBindTexture( target, id );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, magfilter );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, minfilter );
	glTexParameteri( target, GL_GENERATE_MIPMAP, GL_FALSE );

	int maxAnisotropy;
	glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
	if ( anisotropy > maxAnisotropy )
		anisotropy = maxAnisotropy;
	glTexParameteri( target, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy );

	blockSize = ( tdata->dwFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )? 8 : 16;

	offset = 0;

	if ( !texture3D_depth ) {
		for ( uint i = 0; i < tdata->dwMipmapCount; i++ ) 
		{
			mipSize = ( ( tdata->dwWidth + 3 ) / 4 ) * ( ( tdata->dwHeight + 3 )/4 ) * blockSize;

			glCompressedTexImage2D( GL_TEXTURE_2D, i,
									tdata->dwFormat,
									tdata->dwWidth,
									tdata->dwHeight,
									0, 
									mipSize, 
									tdata->pData + offset );
		

			offset += mipSize;
			tdata->dwWidth /= 2;
			tdata->dwHeight /= 2;
		} 
	} else {
		unsigned len2d = sizeof( tdata->pData ) / sizeof( tdata->pData[ 0 ] );
		unsigned char* data3d = new unsigned char[ len2d * texture3D_depth ];
		unsigned i = 0;
		for ( uint z = 0; z < texture3D_depth; z++ ) {
			for ( uint l = 0; l < len2d; l++ ) {
				data3d[ i++ ] = tdata->pData[ l ];
			}
		}

		for ( uint i = 0; i < tdata->dwMipmapCount && texture3D_depth > 0; i++ ) 
		{
			mipSize = ( ( tdata->dwWidth + 3 ) / 4 ) * ( ( tdata->dwHeight + 3 ) / 4 ) * ( ( texture3D_depth + 3 ) / 4 ) * blockSize;

			glCompressedTexImage3D( target, i,
									tdata->dwFormat,
									tdata->dwWidth,
									tdata->dwHeight,
									texture3D_depth,
									0, 
									mipSize, 
									data3d + offset );
		

			offset += mipSize;
			tdata->dwWidth /= 2;
			tdata->dwHeight /= 2;
			//texture3D_depth /= 2;
		}
		delete [] data3d;
	}

	delete [] tdata->pData;
	delete tdata;

	glBindTexture( target, 0 );

	return id;
}

/*
================================================
	GenerateTestTexture3D
================================================
*/
unsigned GenerateTestTexture3D( void )
{
	unsigned ret_value;

	unsigned char data[ 4 * 4 * 4 * 4 ];

	for ( int i = 0, k = 0; i < 4; i++ )
		for( int j = 0; j < 48; j++ )
			if ( j % 4 == 3 ) 
				data[ k++ ] = 255;
			else
				data[ k++ ] = rand() % 256;

	glGenTextures( 1, &ret_value );
	glBindTexture( GL_TEXTURE_3D, ret_value );

	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, 4, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

	glBindTexture( GL_TEXTURE_3D, 0 );
	return ret_value;
}

TextureData_t* LoadDDSFile2( const char* szFilename, uint fromMipmapLevel )
{
	FILE* file;
	DDSURFACEDESC2_copiedfromddraw ddsd;
	char magicNumber[ 4 ];
	TextureData_t * tdata;

	if ( fopen_s( &file, szFilename, "rb" ) ) {
		return NULL;
	}

	fread_s( &magicNumber, sizeof( char )*4, sizeof( char ), 4, file );
	fread_s( &ddsd, sizeof( ddsd ), sizeof( ddsd ), 1, file );

	tdata = new TextureData_t;
	tdata->dwWidth = ddsd.dwWidth / fromMipmapLevel;
	tdata->dwHeight = ddsd.dwHeight / fromMipmapLevel;

	switch( ddsd.ddpfPixelFormat.dwFourCC ) {
		case MAKEFOURCC( 'D','X', 'T', '1' ): tdata->dwFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;

		case MAKEFOURCC( 'D','X', 'T', '3' ): tdata->dwFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;

		case MAKEFOURCC( 'D','X', 'T', '5' ): tdata->dwFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	}

	int blockSize = ( tdata->dwFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )? 8 : 16;
	int remains = blockSize * tdata->dwWidth * tdata->dwHeight ;

	tdata->pData = new unsigned char[ remains ];

	for ( long i = 0; i < remains; ++i ) {
		fread_s( &tdata->pData[ i ], 1, 1, 1, file );
		fseek( file, fromMipmapLevel-1, SEEK_CUR );
	}

	fclose( file );

	return tdata;
}

unsigned int _LoadTexture2( const char* szFilename, int magfilter, int minfilter, int anisotropy, uint loadFromMipmapLevel )
{
	TextureData_t * tdata;
	unsigned int id;

	glGenTextures( 1, &id );
	glBindTexture( GL_TEXTURE_2D, id );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter );

	int maxAnisotropy;
	glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
	if ( anisotropy > maxAnisotropy )
		anisotropy = maxAnisotropy;
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy );

	int blockSize = ( tdata->dwFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )? 8 : 16;
	int i = 1;
	unsigned int mipSize;
	unsigned int size = 9;

	while ( size > 128 ) {
		tdata = LoadDDSFile2( szFilename, i );

		if ( !tdata ) {
			glDeleteTextures( 1, &id );
			return 0;
		}

		blockSize = ( tdata->dwFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )? 8 : 16;
		size = tdata->dwWidth;
		mipSize = tdata->dwWidth * tdata->dwHeight * blockSize;

		glCompressedTexImage2D    ( GL_TEXTURE_2D, i++ - 1,
									tdata->dwFormat,
									tdata->dwWidth,
									tdata->dwHeight,
									0, mipSize, tdata->pData );
		delete [] tdata->pData;
		delete tdata;
	}

	glBindTexture( GL_TEXTURE_2D, 0 );

	return id;
}