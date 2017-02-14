#include "Console.h"
#include <stdio.h>
#include "FileManager.h"
#include "..\NspStruct.h"
#include "..\Renderer\Colors.h"

/*
====================================
	consoleMsg_t::=
====================================
*/
consoleMsg_t& consoleMsg_t::operator=( const consoleMsg_t& b )
{
	text = b.text;
	this->type = b.type;
	return( *this );
}

/*
====================================
	==
====================================
*/
bool consoleMsg_t::operator==( const consoleMsg_t& b )
{
	return ( text == b.text && type==b.type );
}

/*
====================================
	!=
====================================
*/
bool consoleMsg_t::operator!=( const consoleMsg_t& b )
{
	return!( *this==b );
}

/*
====================================
	vsCmd_t
====================================
*/
vsCmd_t::vsCmd_t( void ) {
	ZeroMemory( name, 32 );
}

/*
====================================
	vsCmd_t:: =
====================================
*/
vsCmd_t& vsCmd_t::operator=( const vsCmd_t& b ) {
	this->type = b.type;
	strcpy_s( this->name, 32, b.name );
	intFunc = b.intFunc;
	boolFunc = b.boolFunc;
	floatFunc = b.floatFunc;
	vec2Func = b.vec2Func;
	vec3Func = b.vec3Func;
	vec4Func = b.vec4Func;
	return( *this );
}

/*
====================================
	vsCmd_t:: ==
====================================
*/
bool vsCmd_t::operator==( const vsCmd_t& b ) {
	return( 	this->type	 == b.type		&&
			strncmp( name, b.name, 32 )	&&
			intFunc		== b.intFunc	&&
			boolFunc	== b.boolFunc	&&
			floatFunc	== b.floatFunc	&&
			vec2Func	== b.vec2Func	&&
			vec3Func	 == b.vec3Func	&&
			vec4Func	== b.vec4Func		 );
}

/*
====================================
	vsCmd_t:: !=
====================================
*/
bool vsCmd_t::operator!=( const vsCmd_t& b ) {
	return!( *this==b );
}

/*
====================================
	vsCmd_t::CreateVoid
====================================

void vsCmd_t::CreateVoid( char* name, void ( *func )( void ) ) {
	voidFunc = func;
	strcpy_s( this->name, 32, name ); type = CMD_TYPE_VOID;
}*/

/*
====================================
	vsCmd_t::CreateInt
====================================
*/
void vsCmd_t::CreateInt( char* name, void ( *func )( const int& ) ) {
	intFunc.Set(  func );
	strcpy_s( this->name, 32, name ); type = CMD_TYPE_INT;
}

/*
====================================
	vsCmd_t::CreateBool
====================================
*/
void vsCmd_t::CreateBool( char* name, void ( *func )( const bool& ) ) {
	boolFunc.Set(  func );
	strcpy_s( this->name, 32, name ); type = CMD_TYPE_BOOL;
}

/*
====================================
	vsCmd_t::CreateFloat
====================================
*/
void vsCmd_t::CreateFloat( char* name, void ( *func )( const float& ) ) {
	floatFunc.Set(  func );
	strcpy_s( this->name, 32, name ); type = CMD_TYPE_FLOAT;
}

/*
====================================
	vsCmd_t::CreateVec2
====================================
*/
void vsCmd_t::CreateVec2( char* name, void ( *func )( const vsVec2f& ) ) {
	vec2Func.Set(  func );
	strcpy_s( this->name, 32, name ); type = CMD_TYPE_VEC2;
}

/*
====================================
	vsCmd_t::CreateVec3
====================================
*/
void vsCmd_t::CreateVec3( char* name, void ( *func )( const vsVec3f& ) ) {
	vec3Func.Set(  func );
	strcpy_s( this->name, 32, name ); type = CMD_TYPE_VEC3;
}

/*
====================================
	vsCmd_t::CreateVec4
====================================
*/
void vsCmd_t::CreateVec4( char* name, void ( *func )( const vsVec4f& ) ) {
	vec4Func.Set(  func );
	strcpy_s( this->name, 32, name ); type = CMD_TYPE_VEC4;
}

/*
====================================
	vsCmd_t::Do()
====================================
*/
void vsCmd_t::Do( char* format ) {
	int i;
	bool b;
	float f;
	vsVec2f v2;
	vsVec3f v3;
	vsVec4f v4;
	char	t[ 128 ];
	char c = ( char )37;

	ZeroMemory( t, 128 );

	if ( strstr( format, ( const char* )name ) ) 
	{
		switch( type ) {
			/*case CMD_TYPE_VOID: 
					voidFunc.Call();
			break;*/

			case CMD_TYPE_BOOL:
				sprintf_s( t, 128, "%s %cd", name, c );
				sscanf_s( ( const char* )format, t, &i );
				b = ( i>0 );
				boolFunc.Call( b );
			break;
			case CMD_TYPE_INT:
				sprintf_s( t, 128, "%s %cd", name, c );
				sscanf_s( ( const char* )format, t, &i );
				intFunc.Call( i );
			break;
			case CMD_TYPE_FLOAT: 
				sprintf_s( t, 128, "%s %cf", name,c );
				sscanf_s( ( const char* )format, t, &f );
				floatFunc.Call( f );
			break;
			case CMD_TYPE_VEC2: 
				sprintf_s( t, 128, "%s %cf %cf", name,c ,c );
				sscanf_s( ( const char* )format, t, &v2.x, &v2.y );
				vec2Func.Call( v2 );
			break;
			case CMD_TYPE_VEC3: 
				sprintf_s( t, 128, "%s %cf %cf %cf", name,c,c,c );
				sscanf_s( ( const char* )format, t,  &v3.x, &v3.y, &v3.z );
				vec3Func.Call( v3 );
			break;
			case CMD_TYPE_VEC4: 
				sprintf_s( t, 128, "%s %cf %cf %cf %fc", name,c,c,c,c );
				sscanf_s( ( const char* )format, t,  &v4.x, &v4.y, &v4.z, &v4.w );
				vec4Func.Call( v4 );
			break;
		}
	}
}


/*
===================================
		vsConsole
===================================
*/
vsConsole::vsConsole( void ) {
	Clear();
	visible = false;
	texture = false;
	scroll = 0;
	printingEnabled = true;
	logToFile = false;
	bfont = false;
}

/*
===================================
		~vsConsole
===================================
*/
vsConsole::~vsConsole( void ) {
	Clear();
	visible = false;
}

/*
===================================
		Initialize
===================================
*/
void vsConsole::Initialize( bool logToFile )
{
	//Clear();
	visible = false;
	texture = false;
	scroll = scrollTop = numLines = 0;
	this->logToFile = logToFile;
	fontId = vsLib2::printer->NewFont( "Arial", 16, true, false, false, false );
	if ( logToFile )
	{
		FILE* f;
		fopen_s( &f, "log.txt", "w+" );
		fclose( f );
	}
}

/*
===================================
		Clear
===================================
*/
VS_INLINE void vsConsole::Clear( void ) {
	numLines = 0;
	scroll = 0;
	scrollTop = 0;
	ZeroMemory( lines, sizeof( consoleMsg_t )*MAX_CONSOLE_LINES );
}

/*
====================================
	vsConsole::Print
====================================
*/
void vsConsole::Print( const vsVec4f& color, char* msg, ... ) 
{
	if ( !printingEnabled ) return;
		
	char text[256];

	va_list	list;
	va_start( list, msg );
	 vsprintf_s( text, 256, msg, list );
	va_end( list );

	LogToFile( text );
	NewLine( vsStr( text ), color );
}

/*
====================================
	Print
====================================
*/
void vsConsole::Print( char* msg, ... )
{
	if ( !printingEnabled ) return;
		
	char text[256];

	va_list	list;
	va_start( list, msg );
	 vsprintf_s( text, 256, msg, list );
	va_end( list );

	LogToFile( text );
	NewLine( vsStr( text ), CMT_LOG );
}

/*
====================================
	NewLine
====================================
*/
void vsConsole::NewLine( const vsStr& text, const vsVec4f& cmtype )
{
	if ( numLines<MAX_CONSOLE_LINES ) {
		lines[numLines].type = cmtype;
		lines[numLines++].text = text;

		if ( scrollTop < MAX_VISIBLE_CONSOLE_LINES ) scrollTop++;
		if ( scrollTop > numLines )
			scrollTop = numLines;
	} else {
		for ( int i=0; i<MAX_CONSOLE_LINES-1; ++i )
		{
			lines[i] = lines[i+1];
		}
		lines[MAX_CONSOLE_LINES-1].type = cmtype;
		lines[MAX_CONSOLE_LINES-1].text = text;
	}
}

/*
====================================
	Render
====================================
*/
void vsConsole::Render( void ) 
{
	if ( !visible ) return;

		glMatrixMode( GL_PROJECTION );
	glPushMatrix();
		glOrtho( -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f );
		glLoadIdentity();
		glPushMatrix();
			if ( texture ) {
				glColor3f( 1.0f, 1.0f, 1.0f );
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, texture );
			} else
				glColor3f( 0.f, 0.f, 0.f );
			glBegin( GL_QUADS );
			glMultiTexCoord2f( GL_TEXTURE0, 0.0f, 0.0f ); glVertex3f( -1.0f, 0.25f, -0.9f );
			glMultiTexCoord2f( GL_TEXTURE0, 1.0f, 0.0f ); glVertex3f( 1.0f, 0.25f, -0.9f );
			glMultiTexCoord2f( GL_TEXTURE0, 1.0f, 1.0f ); glVertex3f( 1.0f, 1.00f, -0.9f );
			glMultiTexCoord2f( GL_TEXTURE0, 0.0f, 1.0f ); glVertex3f( -1.0f, 1.00f, -0.9f );
			glEnd();
			if ( texture ) {
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, 0 );
			} else
				glColor3f( 1.f, 1.f, 1.f );

			RenderTexts( -0.98f, 0.3f );
			
			glBegin( GL_QUADS );
				glVertex3f( -1.0f, 0.25f, -0.94f );
				glVertex3f( 1.0f, 0.25f, -0.94f );
				glVertex3f( 1.0f, 0.20f, -0.94f );
				glVertex3f( -1.0f, 0.20f, -0.94f );
			glEnd();

			vsLib2::printer->Print( fontId, -0.98f, 0.21f, -0.95f, vsColors::Black, false, ">%s_", userInput() );
		glPopMatrix();
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glColor3f( 1.0f, 1.0f, 1.0f );
}

/*
====================================
	ApplyTexture
====================================
*/
void vsConsole::ApplyTexture( unsigned int texture )
{
	this->texture = texture;
}

/*
====================================
	SetFont
====================================
*/
void vsConsole::SetFont( const char* fontname, int size, bool bold, bool italic ) 
{
	vsLib2::printer->RemoveFont( 0 );
	vsLib2::printer->NewFont( fontname, size, bold, italic, false, false );
}

/*
====================================
	ProcessUserInput
====================================
*/
void vsConsole::ProcessUserInput( void )
{
	if ( userInput == vsStr( "clear" ) ) {
		Clear();
	} else {
		getline = userInput;
	}
	/*
	for ( unsigned int i=0; i<cmds.Length(); ++i ) {
		if ( strstr( userInput(), ( const char* )cmds.list[i].name ) ) {
			cmds.list[i].Do( userInput() );
			userInput.Zero();
			return;
		}
	}
	*/
//	Print( CMT_ERROR, "Invalid command: %s", userInput() );
	userInput.Zero();
}

/*
====================================
	GetLine
		Returns true if user p
====================================
*/
bool vsConsole::GetLine( vsStr& cmd ) {
	if ( getline.Length() > 0 ) {
		cmd = getline;
		getline = vsStr( "" );
		return true;
	} else
		return false;
}

/*
====================================
	CheckInput
====================================
*/
void vsConsole::CheckInput( bool* keys ) {
	if ( visible ) 
	{
			if ( keys[VK_RETURN] ) {
				ProcessUserInput();
				keys[VK_RETURN] = false;
			} else 
			if ( keys[VK_BACK] ) {
				userInput.Narrow();	
				keys[VK_BACK] = false;
			} else 
			if ( keys[VK_TAB] ) {
				Toggle();
				keys[VK_TAB] = false;
			} else 
			if ( keys[VK_UP] ) {
				ScrollUp();
				keys[VK_UP] = false;
			} else
			if ( keys[VK_DOWN] ) {
				ScrollDown();
				keys[VK_DOWN] = false;
			} 
			//OTHER
			else {
					for ( int i=65; i<97; ++i ) 
					{
						if ( keys[ i ] ) {
							char ch = i+32;
							userInput.Append( ch );
							keys[i]=false;
						}
					}
					for ( int i=48; i<58; ++i ) 
					{
						if ( keys[ i ] ) {
							userInput.Append( char( i ) );
							keys[i]=false;
						}
					}
					if ( keys[ VK_SPACE ] ) {
						userInput.Append( ' ' );
						keys[VK_SPACE]=false;
					}
					if ( keys[VK_DECIMAL] ) {
						userInput.Append( '.' );
						keys[VK_DECIMAL]=false;
					}
					/*if ( keys[VK_COMMA ] ) {
						userInput.Append( ',' );
						keys[VK_COMMA ]=false;
					}*/
							
			}
	} else 
	{
		if ( keys[VK_TAB] ) 
		{
			keys[VK_TAB] = false;
			Toggle();
		}
	}

	
}

/*
====================================
	ScrollUp
====================================
*/
void vsConsole::ScrollUp( void ) 
{
	if ( scrollTop<( int )numLines-1 ) {
		++scroll;
		++scrollTop;
	}
}

/*
====================================
	ScrollDown
====================================
*/
void vsConsole::ScrollDown( void )
{
	if ( scroll>0 ) {
		--scroll;
		--scrollTop;
	}
}

/*
====================================
	AddCommand
====================================
*/
void vsConsole::AddCommand( const vsCmd_t& cmd )
{
	cmds.Append( cmd );
}

/*
====================================
	AddCommand
====================================
*/
void vsConsole::DisablePrinting( void )
{
	printingEnabled = false;
}

/*
====================================
	AddCommand
====================================
*/
void vsConsole::EnablePrinting( void )
{
	printingEnabled = true;
}

/*
====================================
	LogToFile
====================================
*/
void vsConsole::LogToFile( char* text )
{
	if ( logToFile )
	{
		vsOutputFile f;
		f.Open( "log.txt", "a+" );
		f.WriteText( "%s\n", text );
		f.Close();
	}
}

void vsConsole::RenderTexts( float x, float y )
{
	vsVec4f Color;

	float f = 0.0;
	float sinF;
	for ( int i=scroll; i<scrollTop; ++i ) 
	{
		Color = lines[numLines-1-i].type;


		sinF = sinf( f*3.0f ) * 0.05f + 0.05f;
		Color -= vsVec4f( sinF, sinF, sinF, 0.0f );

		vsLib2::printer->Print( fontId, x, f+y, -0.95f, Color, false, lines[numLines-1-i].text() );

		f+=0.04f;
	}


}