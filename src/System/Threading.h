#ifndef __VS_THREADING_H__
#define __VS_THREADING_H__

#include "..\vsLib2\vsLib2.h"
#include <windows.h>


class vsSharedSection {
public:
						vsSharedSection( void ) {};
						~vsSharedSection( void ) {};

	void				Init( void ) ;
	void				Delete( void ) ;
	void				Enter( void ) ;
	ulong				TryEnter( void ) ;
	void				Leave( void );

private:
	CRITICAL_SECTION	cs;
};

/*
===========================
	Init
===========================
*/
VS_INLINE void vsSharedSection::Init( void )  {
	InitializeCriticalSection( &cs );
}

/*
===========================
	Delete
===========================
*/
VS_INLINE void vsSharedSection::Delete( void )  {
	DeleteCriticalSection( &cs );
}

/*
===========================
	Enter
===========================
*/
VS_INLINE void vsSharedSection::Enter( void )  {
	EnterCriticalSection( &cs );
}

/*
===========================
	TryEnter
===========================
*/
VS_INLINE ulong vsSharedSection::TryEnter( void )  {
	return TryEnterCriticalSection( &cs );
}

/*
===========================
	Leave
===========================
*/
VS_INLINE void vsSharedSection::Leave( void )  {
	LeaveCriticalSection( &cs );
}

#endif