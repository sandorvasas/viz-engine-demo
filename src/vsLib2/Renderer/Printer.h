#ifndef __VSPRINTER_H__
#define __VSPRINTER_H__


#include "OpenGL/GLexp.h"
#include "../Math/Math.h"

#define MAX_FONTS	64
#define MAX_PRINTINFO 100

/*
==============================
     vsFont
==============================
*/
struct vsFont {
	unsigned int	id;
	char			szName[512];
	int				iSize;
	bool			fromFile;
};

struct printInfo_t {
	unsigned int	id;
	vsVec3f			pos;
	vsVec4f			color;
	char			text[ 512 ];
};

/*
==============================
     vsPrinter
==============================
*/
class vsPrinter {
public:
						vsPrinter( void );
						~vsPrinter( void );

	int					NewFont( const char* szFontname, unsigned int size, bool bBold, bool bItalic, bool bUnderline, bool bStrikeOut );	// Loads a font from a file; returns -1 if fail; else returns id of font
	int					NewFontFromFile( const char* szFontFileName, char* szFontname, unsigned int size, bool bBold, bool bItalic, bool bUnderline, bool bStrikeOut );	// Loads a font from a file; returns -1 if fail; else returns id of font
	void				RemoveFont( unsigned int fontId );
	void				Print( unsigned int id, float x, float y, float z, const vsVec4f& color, bool ortho, char* text, ... );

	void				PutInQueue( unsigned int id, float x, float y, float z, const vsVec4f& color, char* text, ... );
	void				PrintQueue( bool ortho );


private:
	vsFont				Fonts[ MAX_FONTS ];
	unsigned int		numFonts;
	printInfo_t			printInfos[ MAX_PRINTINFO ];
	int					printInfoCount;
};



VS_INLINE
vsPrinter::vsPrinter( void )
{
	ZeroMemory( Fonts, sizeof( vsFont ) * MAX_FONTS );
	numFonts = printInfoCount = 0;
}

VS_INLINE
vsPrinter::~vsPrinter( void )
{
	/*for ( unsigned int i=0; i<numFonts; ++i )
		if ( Fonts[ i ].fromFile )
			RemoveFont( i );*/
}


#endif