#ifndef __VSVIDEOMODE_H__
#define __VSVIDEOMODE_H__

#include <windows.h>
#include "../Math/Math.h"

/*
=====================================================

	vsVideoMode

=====================================================
*/
class vsVideoMode {
public:
	ushort		iWidth;
	ushort		iHeight;
	byte		iBpp;
	ushort		iScreenCenterX;
	ushort		iScreenCenterY;
	bool		bFullscreen;

public:
	void		Change( ushort w, ushort h, byte bpp, ushort freq, bool bFullscreen );
	void		RestoreOriginal( void );
	vsVec2f		GetScreenCenter( void ) const;
};

// **** declared in RenderSystem.cpp ****



/*
============================
	Change()
============================
*/
VS_INLINE
void vsVideoMode::Change( ushort w, ushort h, byte bpp, ushort freq, bool bFullscreen )
{
	if ( bFullscreen ) 
	{
		DEVMODE dm;

		EnumDisplaySettings( NULL, 0, &dm );
		dm.dmSize = sizeof( DEVMODE );
		dm.dmPelsWidth = w;
		dm.dmPelsHeight = h;
		dm.dmBitsPerPel = bpp;
		dm.dmDisplayFrequency = freq;
		dm.dmFields = DM_BITSPERPEL | DM_PELSHEIGHT | DM_PELSWIDTH;
		if ( freq ) dm.dmFields |= DM_DISPLAYFREQUENCY;
		ChangeDisplaySettings( &dm, CDS_FULLSCREEN );
	}

	this->iWidth = w;
	this->iHeight = h;
	this->iBpp = bpp;
	this->iScreenCenterX = w/2;
	this->iScreenCenterY = h/2;
	this->bFullscreen = bFullscreen;
}


/*
============================
	RestoreOriginal()
============================
*/
VS_INLINE
void vsVideoMode::RestoreOriginal( void ) 
{
	ChangeDisplaySettings( NULL, CDS_FULLSCREEN );
}

VS_INLINE
vsVec2f vsVideoMode::GetScreenCenter( void ) const
{
	return vsVec2f( float( iScreenCenterX ), float( iScreenCenterY ) );
}


#endif