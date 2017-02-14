#include "Input.h"

vsInput Input;

vsInput::vsInput( void )
{
	Defaults();
}

void vsInput::Win32Input( unsigned int uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg ) {
		case WM_KEYDOWN:	KeyDown( wParam );				break;
		case WM_KEYUP:		KeyUp( wParam );				break;
		case WM_LBUTTONDOWN:MouseButtonDown( MOUSE_LEFT );		break;
		case WM_LBUTTONUP:	MouseButtonUp( MOUSE_LEFT );			break;
		case WM_RBUTTONDOWN:MouseButtonDown( MOUSE_RIGHT );		break;
		case WM_RBUTTONUP:	MouseButtonUp( MOUSE_RIGHT );			break;
		case WM_MBUTTONDOWN:MouseButtonDown( MOUSE_MIDDLE );		break;
		case WM_MBUTTONUP:	MouseButtonUp( MOUSE_MIDDLE );		break;
	}
}

vsVec2f vsInput::GetMousePos( void )
{
	POINT p;
	GetCursorPos( &p );

	return vsVec2f( float( p.x ), float( p.y ) );
}

void vsInput::SetMousePos( const vsVec2f& pos ) 
{
	SetMousePos( int( pos.x ), int( pos.y ) );
}

void vsInput::SetMousePos( int x, int y ) 
{
	SetCursorPos( x, y );
	lastKnownMousePos = vsVec2f( float( x ),float( y ) );
}

vsVec2f vsInput::GetMouseDeltaSinceLast( void )
{
	return( GetMousePos() - lastKnownMousePos );
}

void vsInput::Defaults( void )
{
	ZeroMemory( keys, 256 * sizeof( bool ) );
	mouse.leftDown = false;
	mouse.rightDown = false;
	mouse.middleDown = false;
	mouse.wheelDelta = 0;
	lastKnownMousePos = vsVec2f( 0.0f );
}

void vsInput::KeyDown( int key )
{
	keys[key] = true;
}

void vsInput::KeyUp( int key )
{
	keys[key] = false;
}

void vsInput::MouseButtonDown( mouseButton_t button )
{
	switch( button ) {
		case MOUSE_LEFT:	mouse.leftDown = true; break;
		case MOUSE_MIDDLE:	mouse.middleDown = true; break;
		case MOUSE_RIGHT:	mouse.rightDown = true; break;
	}
}

void vsInput::MouseButtonUp( mouseButton_t button )
{
	switch( button ) {
		case MOUSE_LEFT:	mouse.leftDown = false; break;
		case MOUSE_MIDDLE:	mouse.middleDown = false; break;
		case MOUSE_RIGHT:	mouse.rightDown = false; break;
	}
}

void vsInput::MouseWheelMove( int delta )
{
	mouse.wheelDelta = delta;
}

bool vsInput::IsKeyDown( int key ) const
{
	return( keys[ key ] );
}

bool vsInput::IsMouseButtonDown( mouseButton_t button )
{
	switch( button ) {
		case MOUSE_LEFT:	return( mouse.leftDown );
		case MOUSE_MIDDLE:	return( mouse.middleDown );
		case MOUSE_RIGHT:	return( mouse.rightDown );
		default:			return( false );
	}
}
