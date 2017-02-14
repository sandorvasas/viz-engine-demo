#include "Printer.h"


/*
==============================
		NewFont
==============================
*/
int vsPrinter::NewFont( const char* szFontname, 
						unsigned int size, 
						bool bBold, 
						bool bItalic, 
						bool bUnderline, 
						bool bStrikeOut )
{
	HFONT font = CreateFont( size, 0, 0, 0, 
							 ( bBold )? FW_BOLD : FW_NORMAL, bItalic, bUnderline, bStrikeOut,
							 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
							 DEFAULT_PITCH | FF_DONTCARE, szFontname );

	
	Fonts[numFonts].id = glGenLists( 256 );

	HDC dc = wglGetCurrentDC();
	SelectObject( dc, font );
	wglUseFontBitmaps( dc, 0, 255, Fonts[numFonts].id );
	SelectObject( dc, NULL );

	Fonts[ numFonts ].iSize = size;
	strcpy_s( Fonts[ numFonts ].szName, 512, szFontname );
	Fonts[ numFonts ].fromFile = false;

	numFonts++;
	return numFonts-1;
}

/*
==============================
		NewFontFromFile
==============================
*/
int vsPrinter::NewFontFromFile( const char* szFontFileName, char* szFontname, unsigned int size, 
								bool bBold, bool bItalic, bool bUnderline, bool bStrikeOut )
{
	AddFontResource( szFontFileName );

	HFONT font = CreateFont( size, 0, 0, 0, 
							 ( bBold )? FW_BOLD : FW_NORMAL, bItalic, bUnderline, bStrikeOut,
							 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
							 DEFAULT_PITCH | FF_DONTCARE, szFontname );

	Fonts[numFonts].id = glGenLists( 256 );

	HDC dc = wglGetCurrentDC();
	SelectObject( dc, font );
	wglUseFontBitmaps( dc, 0, 255, Fonts[numFonts].id );

	Fonts[ numFonts ].iSize = size;
	strcpy_s( Fonts[ numFonts ].szName, 512, szFontname );
	Fonts[ numFonts ].fromFile = true;

	return numFonts++;
}


/*
==============================
		RemoveFont
==============================
*/
void vsPrinter::RemoveFont( unsigned int fontId )
{
	//RemoveFontResource( Fonts[ fontId ].szName ); fixme
	numFonts--;
}

/*
==============================
		Print
==============================
*/
void vsPrinter::Print( unsigned int id, float x, float y, float z, const vsVec4f& color, bool ortho, char* text, ... )
{
	char str[1000];
	va_list list;

	ZeroMemory( str, 1000 );

	va_start( list, text );
	vsprintf_s( str, 1000, text, list );
	va_end( list );

	if ( ortho ) {
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
			glOrtho( -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f );
			glLoadIdentity();
			glPushMatrix();
	}

	glPushAttrib( GL_LIGHTING_BIT );
	glDisable( GL_LIGHTING );

	glBindTexture( GL_TEXTURE_2D, 0 );
	glColor4fv( color.ToTypePtr() );

	glRasterPos3f( x, y, z );
	glListBase( Fonts[ id ].id );
	glCallLists( ( int )strlen( str ), GL_UNSIGNED_BYTE, str );

	glColor3f( 1.0f, 1.0f, 1.0f );

	glPopAttrib();

	if ( ortho ) {
			glPopMatrix();
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
	}
}

/*
==============================
		PrintQueue
==============================
*/
void vsPrinter::PutInQueue( unsigned int id, float x, float y, float z, const vsVec4f& color, char* text, ... ) {
	if ( printInfoCount >=  MAX_PRINTINFO ) return;

	char str[1000];
	va_list list;

	ZeroMemory( str, 1000 );

	va_start( list, text );
	vsprintf_s( str, 1000, text, list );
	va_end( list );

	printInfos[ printInfoCount ].id = id;
	printInfos[ printInfoCount ].pos = vsVec3f( x, y, z );
	printInfos[ printInfoCount ].color = color;
	strcpy_s( printInfos[ printInfoCount++ ].text, 512, str );
}

/*
==============================
		PrintQueue
==============================
*/
void vsPrinter::PrintQueue( bool ortho ) {

	if ( ortho ) {
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
			glOrtho( -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f );
			glLoadIdentity();
			glPushMatrix();
	}

	glDepthFunc( GL_ALWAYS );

	for ( int i=0; i<printInfoCount; ++i ) {
			glColor4fv( printInfos[ i ].color.ToTypePtr() );
			glRasterPos3fv( printInfos[ i ].pos.ToTypePtr() );
			glListBase( Fonts[ printInfos[ i ].id ].id );
			glCallLists( ( int )strlen( printInfos[ i ].text ), GL_UNSIGNED_BYTE, printInfos[ i ].text );
	}

	glDepthFunc( GL_LESS );

	glColor3f( 1.0f, 1.0f, 1.0f );

	if ( ortho ) {
			glPopMatrix();
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
	}

	printInfoCount = 0;
}