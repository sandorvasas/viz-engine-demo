#include "Shader.h"	
#include <stdio.h>

/*
================================
		=
================================
*/
vsShader& vsShader::operator=( const vsShader& b )
{
	this->name		= b.name;
	this->program	= b.program;
	this->uniformCaching = b.uniformCaching;
	return ( *this );
}

/*
================================
		==
================================
*/
bool vsShader::operator==( const vsShader& b )
{
	return( 	this->program	== b.program && 
			name			== b.name	 &&
			uniformCaching  == b.uniformCaching );
}

/*
================================
		!=
================================
*/
bool vsShader::operator!=( const vsShader& b )
{
	return!( *this==b );
}

/*
================================
		Delete
================================
*/
void vsShader::Delete( void ) 
{
	if ( glIsProgram( program ) )
		glDeleteShader( program );
}

/*
================================
		LoadFromText
================================
*/
int vsShader::LoadFromText( const char* name, char** text, int numLines, int geomInputType, int geomOutputType ) {

	int lineIndex = 0, fl=0, gl=0, vl=0;
	char** fragShaderCode;
	char** geomShaderCode;
	char** vertShaderCode;
	unsigned int vertShader, fragShader, geomShader;
	bool vertShaderCompiled = false;
	bool fragShaderCompiled = false;
	bool geomShaderCompiled = false;

	while( lineIndex < numLines ) {

		if ( strstr( text[lineIndex], "@FRAGMENT_SHADER" ) ) {
			fragShaderCode = &text[++lineIndex];

			while( ( lineIndex+fl < numLines ) && ( text[lineIndex+fl][0] != '@' ) ) ++fl;

			lineIndex += ( --fl );

			fragShader = glCreateShader( GL_FRAGMENT_SHADER );
			glShaderSource( fragShader, fl, ( const char** )fragShaderCode, NULL );
			glCompileShader( fragShader );

			fragShaderCompiled = true;
		}

		if ( strstr( text[lineIndex], "@VERTEX_SHADER" ) ) {
			vertShaderCode = &text[++lineIndex];

			while( ( lineIndex+vl < numLines ) && ( text[lineIndex+vl][0] != '@' ) ) ++vl;

			lineIndex += ( --vl );

			vertShader = glCreateShader( GL_VERTEX_SHADER );
			glShaderSource( vertShader, vl, ( const char** )vertShaderCode, NULL );
			glCompileShader( vertShader );
			vertShaderCompiled = true;
		}

		if ( ( strstr( text[lineIndex], "@GEOMETRY_SHADER" ) ) && ( vsGL::IsGLExtensionSupported( "GL_EXT_geometry_shader4" ) ) ) {
			geomShaderCode = &text[++lineIndex];

			while( ( lineIndex+gl < numLines ) && ( text[lineIndex+gl][0] != '@' ) ) ++gl;

			lineIndex += ( --gl );
			
			geomShader = glCreateShader( GL_GEOMETRY_SHADER_EXT );
			glShaderSource( geomShader, gl, ( const char** )geomShaderCode, NULL );
			glCompileShader( geomShader );
			geomShaderCompiled = true;
		}
		++lineIndex;
	} 

	program = glCreateProgram();
	if ( fragShaderCompiled ) glAttachShader( program, fragShader );
	if ( vertShaderCompiled ) glAttachShader( program, vertShader );
	if ( geomShaderCompiled ) {
		glAttachShader( program, geomShader );
		glProgramParameteriEXT( program, GL_GEOMETRY_INPUT_TYPE_EXT, geomInputType );
		glProgramParameteriEXT( program, GL_GEOMETRY_OUTPUT_TYPE_EXT, geomOutputType );
		int maxVertices;
		glGetIntegerv( GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &maxVertices );
		glProgramParameteriEXT( program, GL_GEOMETRY_VERTICES_OUT_EXT, maxVertices );
	}
	glLinkProgram( program );
	
	this->name = vsStr( name );
	return VS_OK;
}

/*
================================
		LoadFromFile
================================
*/
int vsShader::LoadFromFile( const char* name, char* shaderFile, int geomInputType, int geomOutputType ) {
	FILE* file;

	if ( fopen_s( &file, shaderFile, "r" ) )
		return VS_FILE_NOT_FOUND;

	//get lines
	int numLines = 1;
	char temp[512];
	while( !feof( file ) ) {
		fgets( temp, 512, file );
		++numLines;
	}

	fseek( file, 0, SEEK_SET );

	// alloc memory
	char** text = new char*[numLines];
	for ( int i=0; i<numLines; ++i ) {
		text[i] = new char[512];
		//ZeroMemory( text[i], 512 );
	}

	// read shader
	int i=0;
	while( !feof( file ) ) {
		fgets( text[i++], 512, file );
	}

	fclose( file );

	return LoadFromText( name, text, numLines, geomInputType, geomOutputType );
}