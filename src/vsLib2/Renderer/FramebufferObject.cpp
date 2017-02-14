#include "FramebufferObject.h"
#include "../System/Console.h"
#include "RenderSystem.h"


/*
====================================
	Constructor
====================================
*/
vsFramebuffer::vsFramebuffer( void )
{
	Handle				= NULL;
	height				= -1;
	width				= -1;
	mstype				= -1;
	depthTextureIndex	= -1;
	stencilTextureIndex	= -1;
	texCount			= 0;
	renderbufferCount	= 0;
	ZeroMemory( renderbuffers, 18 );
	ZeroMemory( textures, 18 );
}

/*
====================================
	Destructor
====================================
*/
vsFramebuffer::~vsFramebuffer( void )
{
	glDeleteFramebuffers( 1, &Handle );
	glDeleteRenderbuffers( 18, renderbuffers );
	glDeleteTextures( 18, textures );
}

/*
====================================
	Create()
====================================
*/
void vsFramebuffer::Create( int width, int height )
{
	this->width		= width;
	this->height	= height;
	glGenFramebuffers( 1, &Handle );
}

/*
====================================
	AttachTexture2D()
====================================
*/
int vsFramebuffer::AttachTexture2D(  unsigned int attachmentType, unsigned int internalFormat, unsigned int format )
{
	if ( texCount > 16 ) 
		return -1;

	glGenTextures( 1, &textures[texCount] );
	glBindTexture( GL_TEXTURE_2D, textures[texCount] );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, renderSystem->settings.textureSettings.magFilter );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, renderSystem->settings.textureSettings.minFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, NULL );

	glBindFramebuffer( GL_FRAMEBUFFER, Handle );
	glFramebufferTexture2D( GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, textures[texCount], 0 ); 
	glBindFramebuffer( GL_FRAMEBUFFER, NULL );

	return( texCount++ );
}

/*
====================================
	AttachRenderbuffer()
====================================
*/
int vsFramebuffer::AttachRenderbuffer( unsigned int attachmentType, unsigned int format )
{
	if ( renderbufferCount >= 17 )
		return -1;

	glGenRenderbuffers( 1, &renderbuffers[renderbufferCount] );
	glBindRenderbuffer( GL_RENDERBUFFER, renderbuffers[renderbufferCount] );
	glRenderbufferStorage( GL_RENDERBUFFER, format, width, height );
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	glBindFramebuffer( GL_FRAMEBUFFER, Handle );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, renderbuffers[renderbufferCount] );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return( renderbufferCount++ );
}

/*
====================================
	AttachRenderbufferMultisample
====================================
*/
int vsFramebuffer::AttachRenderbufferMultisample( unsigned int attachmentType, unsigned int format, int samples )
{
	if ( renderbufferCount > 16 || !vsGL::IsGLExtensionSupported( "GL_EXT_framebuffer_multisample" ) )
		return -1;

	glGenRenderbuffers( 1, &renderbuffers[renderbufferCount] );
	glBindRenderbuffer( GL_RENDERBUFFER, renderbuffers[renderbufferCount] );
	glRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER, samples, format, width, height );
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	glBindFramebuffer( GL_FRAMEBUFFER, Handle );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, renderbuffers[renderbufferCount] );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return( renderbufferCount++ );
}

/*
====================================
	AttachRenderbufferMultisampleCoverage
====================================
*/
int vsFramebuffer::AttachRenderbufferMultisampleCoverage( unsigned int attachmentType, unsigned int format, int coverageSamples, int colorSamples )
{
	if ( renderbufferCount >= 17 || !vsGL::IsGLExtensionSupported( "GL_NV_framebuffer_multisample_coverage" ) )
		return -1;

	glGenRenderbuffers( 1, &renderbuffers[renderbufferCount] );
	glBindRenderbuffer( GL_RENDERBUFFER, renderbuffers[renderbufferCount] );
	glRenderbufferStorageMultisampleCoverageNV( GL_RENDERBUFFER, coverageSamples, colorSamples, format, width, height );
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	glBindFramebuffer( GL_FRAMEBUFFER, Handle );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, renderbuffers[renderbufferCount] );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return( renderbufferCount++ );
}

/*
====================================
	BlitTo()
====================================
*/
void vsFramebuffer::BlitTo( vsFramebuffer* fb, int attachment, int x0, int y0, int w0, int h0, int x1, int y1, int w1, int h1, int mask )
{
	glBindFramebuffer( GL_READ_FRAMEBUFFER, Handle );
	glReadBuffer( attachment );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fb->Handle );
	glDrawBuffer( attachment );
	glBlitFramebufferEXT( x0, y0, w0, h0, 
						 x1, y1, w1, h1,
						 mask, GL_LINEAR );
	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
}

/*
====================================
	BlitWholeTo()
====================================
*/
void vsFramebuffer::BlitWholeTo( const vsFramebuffer& fb, int attachment ) 
{
	if ( this->width == fb.width && this->height == fb.height ) {
		glBindFramebuffer( GL_READ_FRAMEBUFFER, Handle );
		glReadBuffer( attachment );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fb.Handle );
		glDrawBuffer( attachment );
		glBlitFramebufferEXT( 	0, 0, width, height, 0, 0, width, height,
								GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
								GL_LINEAR );
		glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}

/*
====================================
	GetWidth()
====================================
*/
const int& vsFramebuffer::GetWidth( void ) const
{
	return width;
}

/*
====================================
	GetHeight()
====================================
*/
const int& vsFramebuffer::GetHeight( void ) const
{
	return height;
}

/*
====================================
	GetTexture()
====================================
*/
const unsigned int& vsFramebuffer::GetTexture( int index ) const 
{
	if ( index<18 && index>-1 )
		return textures[ index ];
	else 
		return textures[0];
}

/*
====================================
	GetHandle()
====================================
*/
const unsigned int& vsFramebuffer::GetHandle( void ) const
{
	return Handle;
}

/*
====================================
	BeginRender()
====================================
*/
void vsFramebuffer::BeginRender( void )
{
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, Handle );
}

/*
====================================
	EndRender()
====================================
*/
void vsFramebuffer::EndRender( void )
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}