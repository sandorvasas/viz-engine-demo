#ifndef __RENDERSYSTEM_H__
#define __RENDERSYSTEM_H__

#pragma comment( lib, "opengl32.lib" )
#include "..\Math\Math.h"
#include "..\System\Console.h"
#include "..\System\List.h"
#include "FrameCounter.h"
#include "Texture.h"
#include "Shader.h"
#include "OpenGL\GLexp.h"

/*
=====================================================
?													?	
?													?
?					RenderSystem					?
?													?
?													?
=====================================================
*/

/*
===========================
	OPENGL VERSION ENUM
===========================
*/
typedef enum {
	OPENGL_VERSION_2_0 = 0,
	OPENGL_VERSION_3_2,	// These version of OpenGL
	OPENGL_VERSION_4_0,	// are not supported due to
	OPENGL_VERSION_4_2	// the usage of SDL Version 1.2
} oglVersion_t;

/*
===========================
	renderSettings_t
===========================
*/
typedef struct _renderSettings_t {
	PIXELFORMATDESCRIPTOR pfd;

	struct {
		int			magFilter;
		int			minFilter;
		int			anisotropyLevel;
	} textureSettings;

	oglVersion_t	OpenGLVersion;
} renderSettings_t;

/*
===========================
	VertexBufferObject
===========================
*/
typedef struct rsvbo_s {
	 uint id;
	 uint size;
} rsvbo_t;



/*
==================================================

	vsRenderSystem

==================================================
*/
class vsRenderSystem {
	friend class vsFramebuffer;
public:
							vsRenderSystem( void );
							~vsRenderSystem( void );

	void					Initialize( HWND* window, const renderSettings_t& renderSettings );
	void					Shutdown( void );
	unsigned int			LoadTexture( const char* filename, int loadFromMipmapLevel=0 );										//returns pointer to texture in array
	vsShader*				LoadShaderFromFile( const char* name, char* filename, int geomInputType, int geomOutputType ); // //returns pointer to shader in array
	vsShader*				LoadShaderFromText( const char* name, char** text, int numLines, int geomInputType, int geomOutputType ); // //returns pointer to shader in array
	vsShader*				GetShaderByName( const vsStr& name ) const;
	vsShader*				GetShaderById( uint id );
	unsigned int			GetTextureById( unsigned int num );
	void					SwapBuffers( void );
	void					MultMatrix( float* matrix );
	void					MultMatrix( vsMat4* matrix );
	void					LoadMatrix( float* matrix );
	void					LoadMatrix( vsMat4* matrix );
	void					GlobalEnable( int what );
	void					GlobalDisable( int what );
	void					Enable( int what );
	void					Disable( int what );
	vsMat4					GetModelViewMatrix( void ) const;
	vsMat4					GetProjectionMatrix( void ) const;
	void					OrthoMode( bool onOff );
	void					GetGLVersion( unsigned char& major, unsigned char& minor ) const;
	void					GenVertexBuffer( uint* buffer, GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage );
	void					DeleteVertexBuffer( uint buffer );
	ulong					GetUsedVRAM( void );

private:
	HWND*					pWindow;
	HGLRC					renderContext;
	HDC						deviceContext;
	renderSettings_t		settings;
	vsList<texture_t>		textures;
	vsList<vsShader*>		shaders;
	vsList<int>				disabledTokens;
	vsList<rsvbo_t>			vbos;

public:
	vsFrameCounter			FPSCounter;
};


//extern vsRenderSystem* renderSystem;

#endif