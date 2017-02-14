#ifndef __VS_PLAYERVIEW_H__
#define __VS_PLAYERVIEW_H__


#include "..\vsLib2\vsLib2.h"

class vsPlayer;




/*
===============================================

	vsPlayer

===============================================
*/
class vsPlayerView {

	friend class vsPlayer;

public:
	vsPlayerView( void );
	~vsPlayerView( void );


	void			Render( void );

private:
	vsShader*		shader;
	uint			buffers[3];
	uint			megaTexQ1;
	uint			fontId;
};

#endif