#ifndef __VS_MEMORYALLOCATOR_H__
#define __VS_MEMORYALLOCATOR_H__

#include <malloc.h>
#include "System.h"
#include "List.h"

const int MAX_ALLOC_NUM = 1024;

/*
=====================================================

	vsMemoryAllocator

=====================================================
*/
typedef struct heap_s {
	const char*		name;
	ulong			size;
	void*			p;

	heap_s() : p( NULL ),name( NULL ), size( 0 ) {
	}

	heap_s( const char* name, ulong size, void* p ) {
		this->name = name;
		this->size = size;
		this->p = p;
	}

	heap_s& operator=( const heap_s& h ) {
		name = h.name;
		size = h.size;
		p = h.p;
		return( *this );
	}

	bool operator==( void* p ) {
		return( this->p == p );
	}
} heap_t;


class vsMemoryAllocator {
public:
						vsMemoryAllocator( void );
						~vsMemoryAllocator( void );

	void*				Alloc( size_t size, const char* name="" );
	void				Free( void* p );

public:
	vsList<heap_t>		heaps;
	uint				numAllocations;
	ulong				totalAllocatedMem;
};

extern vsMemoryAllocator memoryManager;

/*
VS_INLINE void* operator new( size_t size )
{
	return memoryManager.Alloc( size );
}

VS_INLINE void operator delete( void* p )
{
	memoryManager.Free( p );
}
*/
#endif