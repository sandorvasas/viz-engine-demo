#ifndef __VS_INPUT_H__
#define __VS_INPUT_H__

#include "../Math/Math.h"


/*
===============================================================

		Game Input

===============================================================
*/

enum mouseButton_t {
	MOUSE_LEFT = 0,
	MOUSE_MIDDLE, 
	MOUSE_RIGHT
};

typedef struct _mouseInput_t {
	bool		leftDown;
	bool		middleDown;
	bool		rightDown;
	int			wheelDelta;
} mouseInput_t;


class vsInput {
public:
					vsInput( void );
					~vsInput( void ) {};

	void			Win32Input( unsigned int uiMsg, WPARAM wParam, LPARAM lParam );
	vsVec2f			GetMousePos( void );
	void			SetMousePos( const vsVec2f& pos );
	void			SetMousePos( int x, int y );
	vsVec2f			GetMouseDeltaSinceLast( void );
	void			Defaults( void );
	void			KeyDown( int key );
	void			KeyUp( int key );
	void			MouseButtonDown( mouseButton_t button );
	void			MouseButtonUp( mouseButton_t button );
	void			MouseWheelMove( int delta );
	bool			IsKeyDown( int key ) const;
	bool			IsMouseButtonDown( mouseButton_t button );
	//get mousewheeldelta?
public:
	bool			keys[ 256 ];
private:
	vsVec2f			lastKnownMousePos;
	mouseInput_t	mouse;
};


#endif