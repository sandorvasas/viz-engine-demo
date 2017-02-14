#ifndef __VSTEXTURE_H__
#define __VSTEXTURE_H__

#include "..\System\String.h"
#include "..\System\System.h"


class texture_t {
public:
	texture_t( void ) {};
	~texture_t( void ) {};
	texture_t&			operator=( const texture_t& b );
	bool				operator==( const texture_t& b );
	bool				operator!=( const texture_t& b );

public:
	vsStr		name;
	uint		value;
	int			mipLevel;
};


typedef struct _tagTextureData_t {
	uint	dwMipmapCount;
	uint	dwWidth;
	uint	dwHeight;
	uint	dwFormat;
	byte*	pData;
} TextureData_t, *PTextureData_t;


TextureData_t*	LoadDDSFile( const char* szFilename, uint fromMipmapLevel=0 );
unsigned int	_LoadTexture( const char* szFilename, int magfilter, int minfilter, 
							  int anisotropy, uint loadFromMipmapLevel = 0, uint texture3D_depth = 0 );
unsigned		GenerateTestTexture3D( void );

TextureData_t*	LoadDDSFile2( const char* szFilename, uint fromMipmapLevel=1 );
unsigned int	_LoadTexture2( const char* szFilename, int magfilter, int minfilter, 
							  int anisotropy, uint loadFromMipmapLevel = 1 );

/*
================================================

	DDSURFACEDESC2 struct from MSDN

	Contains information about 
	.DDS texture files.

================================================
*/
struct DDSURFACEDESC2_copiedfromddraw {
		DWORD			dwSize;
		DWORD			dwFlags;
		DWORD			dwHeight;
		DWORD			dwWidth;
		DWORD			dwPitchOrLinearSize;
		DWORD			dwDepth;
		DWORD			dwMipMapCount;
		DWORD			dwReserved[11];
	
		struct {
			DWORD	dwSize;
			DWORD	dwFlags;
			DWORD	dwFourCC;
			DWORD	dwRGBBitCount;
			DWORD	dwRBitMask;
			DWORD	dwGBitMask;
			DWORD	dwBBitMask;
			DWORD	dwRGBAlphaBitMask;	
		} ddpfPixelFormat;

		struct  {
			DWORD	dwCaps1;
			DWORD	dwCaps2;
			DWORD	dwReserved[2];
		} ddsCaps;

		DWORD			dwReserved2;
};


#endif