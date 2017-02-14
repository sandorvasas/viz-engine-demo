#ifndef __VS_TIMER_H__
#define __VS_TIMER_H__

#include <Windows.h>
#include "..\vsLib2\vsLib2.h"

#ifndef VS_INLINE
#define VS_INLINE inline
#endif

/*
==================================

	vsTimer

==================================
*/
class vsTimer {
public:
						vsTimer( void ) {};
						~vsTimer( void ) {};

	void				Init( void );
	void				Update( void );
	const float&		GetTime( void ) const;
	const float			GetDeltaTime( void ) const;

private:
	unsigned long		startTime;
	unsigned long		currentTime;
	unsigned long		lastTime;
	unsigned long		deltaTime;
	float				fCurrentTime;
	float				fDeltaTime;
};

extern vsTimer timer;



VS_INLINE void vsTimer::Init( void ) {
	startTime = GetTickCount();
	currentTime = 0;
	lastTime = 0;
}

VS_INLINE void vsTimer::Update( void ) {
	lastTime = currentTime;
	currentTime = GetTickCount() - startTime;
	deltaTime = currentTime - lastTime;

	fCurrentTime = float( currentTime ) * 0.001f;
	fDeltaTime	 = float( deltaTime ) * 0.001f;
}

VS_INLINE const float& vsTimer::GetTime( void ) const {
	return fCurrentTime;
}

VS_INLINE const float vsTimer::GetDeltaTime( void ) const {
	return vsLib2::renderSystem->FPSCounter.GetInverseCurrent();//fDeltaTime; 
}


#endif