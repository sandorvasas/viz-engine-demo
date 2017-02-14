#include "FrameCounter.h"
#include <windows.h>


vsFrameCounter::vsFrameCounter( void )
{
	current = 80;
	inverseCurrent = 1.0f/80.0f;
	max = 0;
	min = 9999;
	avarage = 0;
}

vsFrameCounter::~vsFrameCounter( void ) {}

void vsFrameCounter::Update( void )
{
	static int lastTime=GetTickCount();
	static int Time, cfps=0;

	Time=GetTickCount();
	if ( Time-lastTime >= 1000 ) {
		current=cfps;
		cfps=0;
		lastTime=Time;

		if ( current>max ) max=current;
		if ( current<min ) min=current;
		inverseCurrent = 1.0f / ( float )current;
		avarage = ( max+min ) / 2;
	} else ++cfps;
}

int vsFrameCounter::GetCurrent( void ) const
{
	return current;
}

int vsFrameCounter::GetMin( void ) const
{
	return min;
}

int vsFrameCounter::GetMax( void ) const
{
	return max;
}

int vsFrameCounter::GetAvarage( void ) const
{
	return avarage;
}

float vsFrameCounter::GetInverseCurrent( void ) const
{
	return ( 1.0f/80.0f );//inverseCurrent;
}
