#include "RenderSystem.h"
#include "Texture.h"
#include "VideoMode.h"
#include <stdio.h>
#include "..\NspStruct.h"
using namespace vsLib2;

/*
==================================
	Constructor
==================================
*/
vsRenderSystem::vsRenderSystem( void )
{
	pWindow = NULL;
}

/*
==================================
	Destructor
==================================
*/
vsRenderSystem::~vsRenderSystem( void )
{
	Shutdown();
}

/*
==================================
	Initialize
==================================
*/
void vsRenderSystem::Initialize( HWND* window, const renderSettings_t& renderSettings )
{
	pWindow = window;
	memcpy_s( 	&settings, sizeof( renderSettings_t ), 
				&renderSettings, sizeof( renderSettings_t ) );

	if ( pWindow!=NULL ) 
	{
		deviceContext = GetDC( *pWindow );
		int pf = ChoosePixelFormat( deviceContext, &settings.pfd );
		SetPixelFormat( deviceContext, pf, &settings.pfd );

		renderContext = wglCreateContext( deviceContext );
		wglMakeCurrent( deviceContext, renderContext );

		unsigned char verMinor, verMajor;

		GetGLVersion( verMajor, verMinor );

		if ( settings.OpenGLVersion == OPENGL_VERSION_4_2 &&
			( ( verMajor < 4 ) || !vsGL::LoadOpenGLCore_4_2() ) )
		{
		//	console->Print( CMT_ERROR, "Failed to load OpenGL 4.2 core" );
		} else
		if ( settings.OpenGLVersion == OPENGL_VERSION_4_0 &&
			( ( verMajor < 4 ) || !vsGL::LoadOpenGLCore_4_0() ) )
		{
		//	console->Print( CMT_ERROR, "Failed to load OpenGL 4.0 core" );
		} else
		if ( settings.OpenGLVersion == OPENGL_VERSION_3_2 &&
			( ( verMajor < 3 && verMinor < 2 ) || !vsGL::LoadOpenGLCore_3_2() ) )
		{
		//	console->Print( CMT_ERROR, "Failed to load OpenGL 3.2 core" );
		} else
		if ( settings.OpenGLVersion == OPENGL_VERSION_2_0 //&&
			//( verMajor < 2 || !vsGL::LoadOpenGLCore_2_0() ) 
			 )
		{	vsGL::LoadOpenGLCore_2_0();
		//	console->Print( CMT_ERROR, "OpenGL 2.0 core" );
		}

		if ( settings.OpenGLVersion >= OPENGL_VERSION_3_2 &&
			verMajor >= 3 )
		{
			const int attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, verMajor,
				WGL_CONTEXT_MINOR_VERSION_ARB, verMinor,
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
				0
			};

			HGLRC rc30 = wglCreateContextAttribsARB( this->deviceContext, 
													0,
													attribs );
			wglMakeCurrent( NULL, NULL );
			wglDeleteContext( this->renderContext );
			this->renderContext = rc30;
			wglMakeCurrent( this->deviceContext, this->renderContext );
		}
	}

	//	Create default font
	printer->NewFont( "Arial", 14, false, false, false, false );
}

/*
==================================
	Shutdown
==================================
*/
void vsRenderSystem::Shutdown( void )
{
	for ( uint i=0; i<this->vbos.Length(); ++i )
		if ( glIsBuffer( vbos[i].id ) )
			glDeleteBuffers( 1, &vbos[i].id );

	wglMakeCurrent( deviceContext, NULL );
	wglDeleteContext( renderContext );
	ReleaseDC( *pWindow, deviceContext );

	for ( unsigned int i=0; i<shaders.Length(); ++i ) {
		shaders.list[i]->Delete();
		delete shaders.list[i];
	}
	shaders.Free();
}

/*
==================================
	LoadTexture
==================================
*/
unsigned int vsRenderSystem::LoadTexture( const char* filename, int loadFromMipmapLevel )
{	
	uint cicc = -1;
	for ( uint i = 0; i < textures.Length(); ++i )
		if ( textures.list[i].name == vsStr( filename )
			&& textures.list[i].mipLevel == loadFromMipmapLevel ) 
		{
			cicc = i;
			break;
		}
	
	if ( cicc != -1 && cicc < textures.Length() ) 
		return( ( textures[ cicc ].value ) );
	
	unsigned int tex =  _LoadTexture( filename, 
									  settings.textureSettings.magFilter,
									  settings.textureSettings.minFilter,
									  settings.textureSettings.anisotropyLevel,
									  loadFromMipmapLevel );
	if ( !tex ) {
		console->Print( CMT_ERROR, "Texture not found: %s", filename );
		return NULL;
	}
	else {
		texture_t t;
		t.name = vsStr( filename );
		t.value = tex;
		t.mipLevel = loadFromMipmapLevel;
		textures.Append( t );
		console->Print( CMT_LOG, "Texture%d loaded: %s", textures.Length(), filename );
	}

	return( ( textures[textures.Length()-1].value ) );
}

/*
==================================
	LoadShaderFromFile
==================================
*/
vsShader* vsRenderSystem::LoadShaderFromFile( const char* name, char* filename, int geomInputType, int geomOutputType )
{
	//if ( !GetShader( vsStr( name ) ) )
	{

		vsShader* shader = new vsShader();

		int result = shader->LoadFromFile( name, filename, geomInputType, geomOutputType );

		if ( result!=VS_OK )
		{
			console->Print( CMT_ERROR, "Failed to load shader( name: %s ) from file: %s", name, filename );
			return NULL;
		}
		else {
			shaders.Append( shader );
			console->Print( CMT_LOG, "Shader%d loaded: %s", shaders.Length(), name );
		}
		return( shaders[shaders.Length()-1] );
	}
}

/*
==================================
	LoadShaderFromText
==================================
*/
vsShader* vsRenderSystem::LoadShaderFromText( const char* name, char** text, int numLines, int geomInputType, int geomOutputType )
{
	//if ( !GetShader( vsStr( name ) ) )
	{
		vsShader* shader = new vsShader();

		int result = shader->LoadFromText( name, text, numLines, geomInputType, geomOutputType );

		if ( result!=VS_OK )
		{
			console->Print( CMT_ERROR, "Failed to load shader from text: %s", name );
			return NULL;
		}
		else {
			shaders.Append( shader );
			console->Print( CMT_LOG, "Shader%d loaded: %s", shaders.Length(), name );
		}
		return( shaders[shaders.Length()-1] );
	}
}

/*
==================================
	GetShaderByName
==================================
*/
vsShader* vsRenderSystem::GetShaderByName( const vsStr& name ) const
{
	for ( unsigned int i=0; i<shaders.Length(); ++i )
	{
		if ( shaders.list[i]->name == name )
		{
			return shaders.list[i];
		}
	}
	return NULL;
}

/*
==================================
	GetShaderById
==================================
*/
vsShader* vsRenderSystem::GetShaderById( uint id )
{
	if ( id<shaders.Length() )
		return ( shaders.list[id] );
	else
		return NULL;
}

/*
==================================
	Swapbuffers
==================================
*/
void vsRenderSystem::SwapBuffers( void )
{
	::SwapBuffers( deviceContext );
	FPSCounter.Update();
}

/*
==================================
	MultMatrix
==================================
*/
void vsRenderSystem::MultMatrix( float* matrix )
{
	glMultMatrixf( matrix );
}

/*
==================================
	MultMatrix
==================================
*/
void vsRenderSystem::MultMatrix( vsMat4* matrix )
{
	glMultMatrixf( matrix->ToTypePtr() );
}

/*
==================================
	LoadMatrix
==================================
*/
void vsRenderSystem::LoadMatrix( float* matrix )
{
	glLoadMatrixf( matrix );
}

/*
==================================
	LoadMatrix
==================================
*/
void vsRenderSystem::LoadMatrix( vsMat4 * matrix )
{
	glLoadMatrixf( matrix->ToTypePtr() );
}

/*
==================================
	GlobalEnable
==================================
*/
void vsRenderSystem::GlobalEnable( int what ) 
{
	int i = disabledTokens.SimpleSearch( what );
	if ( i != -1 )
	{
		disabledTokens.RemoveElementAt( i ); 
	}
}

/*
==================================
	GlobalDisable
==================================
*/
void vsRenderSystem::GlobalDisable( int what )
{
	int i = disabledTokens.SimpleSearch( what );
	if ( i == -1 )
	{
		disabledTokens.Append( what );
	}
}

/*
==================================
	Enable
==================================
*/
void vsRenderSystem::Enable( int what )
{
	int i = disabledTokens.SimpleSearch( what );
	if ( i == -1 )
	{
		glEnable( what );
	}
}

/*
==================================
	Disable
==================================
*/
void vsRenderSystem::Disable( int what )
{
	glDisable( what );
}

/*
==================================
	GetModelviewMatrix
==================================
*/
vsMat4 vsRenderSystem::GetModelViewMatrix( void ) const {
	vsMat4 m;
	glGetFloatv( GL_MODELVIEW_MATRIX, m.ToTypePtr() );
	return m;
}

/*
==================================
	GetProjectionMatrix
==================================
*/
vsMat4 vsRenderSystem::GetProjectionMatrix( void ) const {
	vsMat4 m;
	glGetFloatv( GL_PROJECTION_MATRIX, m.ToTypePtr() );
	return m;
}

/*
==================================
	OrthoMode
==================================
*/
void vsRenderSystem::OrthoMode( bool onOff )  {
	if ( onOff ) {
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
			glOrtho( -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f );
			glLoadIdentity();
			glPushMatrix();
	}
	else 
	{
			glPopMatrix();
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
	}
}

/*
==================================
	GetTexture
==================================
*/
unsigned int vsRenderSystem::GetTextureById( unsigned int num ) {
	if ( num<textures.Length() )
		return textures.list[ num ].value;
	else
		return 0;
}

/*
==================================
	GetTexture
==================================
*/
void vsRenderSystem::GetGLVersion( unsigned char& major, unsigned char& minor ) const {
	glGetIntegerv( GL_MAJOR_VERSION, ( int* )&major );
	glGetIntegerv( GL_MINOR_VERSION, ( int* )&minor );
}

/*
==================================
	GenVertexBuffer
==================================
*/
void vsRenderSystem::GenVertexBuffer( uint* buffer, GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage )
{
	glGenBuffers( 1, buffer );
	glBindBuffer( target, *buffer );
	glBufferData( target, size, data, usage );
	glBindBuffer( target, 0 );
	rsvbo_t v;
	v.id = *buffer;
	v.size = size;
	vbos.Append( v );
}

/*
==================================
	DeleteVertexBuffer
==================================
*/
void vsRenderSystem::DeleteVertexBuffer( uint buffer )
{
	uint i=0;
	while( vbos[i++].id != buffer );
	if ( i<vbos.Length() ) vbos.RemoveElementAt( i );
}

/*
==================================
	GetUsedVRAM
==================================
*/
ulong vsRenderSystem::GetUsedVRAM( void )
{
	ulong total=0;
	for ( uint i=0; i<vbos.Length(); ++i )
		total += vbos[i].size;

	return total;
}