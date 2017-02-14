#ifndef __VSLIB2_BUFFER_H__
#define __VSLIB2_BUFFER_H__

#include "RenderSystem.h"

typedef struct VBO_s {
	uint	handle;
	uint	count;

	VBO_s( void ) {
	}

	// destructor just to make sure we delete every buffer
	~VBO_s( void )
	{
		glDeleteBuffers( 1, &handle );
	}
} VBO_t;

typedef struct doubleBufferedVBO_s {
	VBO_t		vbos[2];
	unsigned	activeBuffer : 1;

	doubleBufferedVBO_t( void ) {
		activeBuffer = 0;
	}

	void SwapBuffers( void ) {
		activeBuffer = 1 - activeBuffer;
	}

} doubleBufferedVBO_t;

#endif