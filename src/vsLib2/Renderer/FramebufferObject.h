#ifndef __VS_FRAMEBUFFER_OBJECT_H__
#define __VS_FRAMEBUFFER_OBJECT_H__


#include "OpenGL/GLexp.h"

/*
====================================================================

	vsFramebuffer

====================================================================
*/
class vsFramebuffer {
public:
	enum {
		MSTYPE_NONE =0,
		MSTYPE_MULTISAMPLE,
		MSTYPE_MULTISAMPLE_COVERAGE
	};

	vsFramebuffer( void );
	~vsFramebuffer( void );

	void						Create( int width, int height );
	int							AttachTexture2D(  unsigned int attachmentType, unsigned int internalFormat, unsigned int format );
	int							AttachRenderbuffer( unsigned int attachmentType, unsigned int format );
	int							AttachRenderbufferMultisample( unsigned int attachmentType, unsigned int format, int samples );
	int							AttachRenderbufferMultisampleCoverage( unsigned int attachmentType, unsigned int format, int coverageSamples, int colorSamples );
	void						BlitTo( vsFramebuffer* fb,  int attachment, int x0, int y0, int w0, int h0, int x1, int y1, int w1, int h1, int mask );
	void						BlitWholeTo( const vsFramebuffer& fb, int attachment );
	const int&					GetWidth( void ) const;
	const int&					GetHeight( void ) const;
	const unsigned int&			GetTexture( int index ) const;
	const unsigned int&			GetHandle( void ) const;
	void						BeginRender( void );
	void						EndRender( void );

protected:
	int							width;
	int							height;
	int							mstype;
	unsigned int				Handle;
	unsigned int				renderbuffers[18];
	unsigned int				textures[18];
	int							depthTextureIndex;
	int							stencilTextureIndex;
	int							texCount;
	int							renderbufferCount;
};


#endif