#include "MemoryAllocator.h"
#include <stdio.h>
#include <Windows.h>

vsMemoryAllocator::vsMemoryAllocator( void ) 
{
	numAllocations = 0;
	totalAllocatedMem = 0;
}

vsMemoryAllocator::~vsMemoryAllocator( void ) 
{
}

void* vsMemoryAllocator::Alloc( size_t size, const char* name ) 
{
	void* p = malloc( size );
	
	if ( strlen( name ) ) 
	{
		heaps.Append( heap_t( name, size, p ) );
	}
	else {
		heaps.Append( heap_t( "unknown", size, p ) );
	}

	++numAllocations;
	totalAllocatedMem += ( ulong ) size;

	return p;
}

void vsMemoryAllocator::Free( void* p )
{
	--numAllocations;
	totalAllocatedMem -= ( ulong ) 1;
	
	uint i=0;
	while( heaps[i].p != p && i<heaps.Length() ) i++;

	if ( i<heaps.Length() )
		heaps.RemoveElementAt( i );

	free( p );
}