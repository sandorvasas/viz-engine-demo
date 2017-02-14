#include "Font.h"
#include "RenderSystem.h"
#include "..\NspStruct.h"
#include "..\Geometry\Vertex.h"

vsPrinter2::vsPrinter2( void )
{ 
	zCoord = 0.8f; 
	neverEverRendered = true;
}

vsPrinter2::~vsPrinter2( void )
{
	//for ( uint i = 0; i < fonts.Length(); i++ )
		//glDeleteLists( fonts[ i ]->listBase, 256 );

	glDeleteBuffers( 1, &printBuffer );

	fonts.DeletePointerElements();
	fonts.Free();
}

void vsPrinter2::SetPrintLayer( float zCoord )
{
	this->zCoord = zCoord;
}

int vsPrinter2::NewFont( char* szFontName, uint texture, float fSize )
{
	vsFont2* font = new vsFont2();

	font->charMap = texture;
	if ( ! font->charMap ) {
		delete font;
		return -1;
	}

	font->name = szFontName;
	font->size = fSize;

	fonts.Append( font );	

	return fonts.Length() - 1;
	/*font->listBase = glGenLists( 256 );

	for ( int k=0, float j,i = 0; i < 16.0f; i++ ) {
		for ( j = 0; j < 16.0f; j++ ) {
			glNewList( font->listBase + k++, GL_COMPILE );
				glBegin( GL_QUADS );
					glMultiTexCoord2f( GL_TEXTURE0, i*scale, j*scale );
					glVertex3f
				glEnd();
			glEndList();
		}
	}*/
}

void vsPrinter2::Print( uint id, float x, float y, const vsVec4f& color, char* text, ... )
{
	if ( !fonts.Length() ) 
		return;

	if ( id >= fonts.Length() )
		id = 0;

	if ( neverEverRendered ) {
		glGenBuffers( 1, &printBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, printBuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof( vsSimpleVertex )*512, NULL, GL_DYNAMIC_DRAW );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		neverEverRendered = false;
	}

	va_list list;
	char str[512];

	va_start( list, text );
	vsprintf_s<512>( str, text, list );
	va_end( list );

	// update buffer
	glBindBuffer( GL_ARRAY_BUFFER, printBuffer );
	vsSimpleVertex* vert = ( vsSimpleVertex* )glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

	for ( uint i = 0, k=0; i < strlen( str ); i++ ) 
	{
		vert[ k ].st = vsLib2_fontTextureCoords[ str[ i ] ] + vsVec2f( vsLib2_fontTextureScale*0.2f, vsLib2_fontTextureScale );
		vert[ k++ ].xyz = vsVec3f( x + float( i ) * fonts[ id ]->size,	 y, zCoord );
		
		vert[ k ].st = vsLib2_fontTextureCoords[ str[ i ] ] + vsVec2f( vsLib2_fontTextureScale-vsLib2_fontTextureScale*0.2f, vsLib2_fontTextureScale );
		vert[ k++ ].xyz = vsVec3f( x + float( i+1 ) * fonts[ id ]->size, y, zCoord );
		
		vert[ k ].st = vsLib2_fontTextureCoords[ str[ i ] ] + vsVec2f( vsLib2_fontTextureScale-vsLib2_fontTextureScale*0.2f, 0.0f );
		vert[ k++ ].xyz = vsVec3f( x + float( i+1 ) * fonts[ id ]->size, y + fonts[ id ]->size, zCoord );
	
		vert[ k ].st = vsLib2_fontTextureCoords[ str[ i ] ] + vsVec2f( vsLib2_fontTextureScale*0.2f, 0.0f );
		vert[ k++ ].xyz = vsVec3f( x + float( i ) * fonts[ id ]->size,	 y + fonts[ id ]->size, zCoord );

	}

	glUnmapBuffer( GL_ARRAY_BUFFER );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0.f, 640.f, 0.f, 480.f, -1.0f, 1.0f );
	glPushMatrix();

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, sizeof( vsSimpleVertex ), NULL );

	glColor4fv( color.ToTypePtr() );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, fonts[ id ]->charMap );
	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( vsSimpleVertex ), BUFFER_OFFSET( 12 ) );

	glDrawArrays( GL_QUADS, 0, 4 * strlen( str ) );

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glPopMatrix();
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

int vsPrinter2::GetFontId( char* szName )
{
	REPU ( fonts.Length() )
		if ( !strcmp( fonts[ i ]->name, szName ) )
			return ( int )i;

	return -1;
}