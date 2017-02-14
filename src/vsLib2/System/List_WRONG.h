#ifndef __VS_LIST_H__
#define __VS_LIST_H__

#include "System.h"


/*
====================================================

		Template class for handling arrays

====================================================
*/

template< class type >
class vsList {
public:

						vsList( void );
	explicit			vsList( int length );
						~vsList( void );

	const type&			operator[]( int index );
	vsList<type>&		operator=( const vsList<type>& listb );
	bool				operator==( const vsList<type>& listb );
	bool				operator!=( const vsList<type>& listb );
	void				Free( void );									// clears the list
	void				Append( const type& elem );						// appends elem to the list
	type*				Find( const type& elem );						// finds elem, and returns its pointer
	int					FindIndex( const type& elem );					// finds elem, and returns its index
	void				RemoveIndex( int index );						// removes elem at index
	void				Remove( const type& elem );						// removes elem
	//void				Insert( int to, const type& elem );				// inserts elem at "to"
	void				Resize( const unsigned int newLen );			// resizes the list
	type*				Ptr( void );									// returns pointer to list
	unsigned int		Length( void );									// returns length of list
	ulong				MemoryAllocated( void );						// returns the size of the list

	void				BubbleSort( void );
	void				ShellSort( void );
	int					QuickSort( int lo, int hi );

public:
	type*				list;
protected:
	unsigned int		len;
};


/*
================================
   vsList();
================================
*/
template< class type >
VS_INLINE
vsList< type >::vsList( void ) {
	list = NULL;
	len = 0;
}

/*
================================
   vsList()
================================
*/
template< class type >
VS_INLINE
vsList< type >::vsList( int length ) {
	//Clear();
	len = length;
	list = new type[ len ];
	ZeroMemory(list, len * sizeof(type) );
}

/*
================================
  ~vsList() 
================================
*/
template< class type >
VS_INLINE
vsList< type >::~vsList( void ) {
	Free();
}

/*
================================
   operator[]
================================
*/
template< class type >
VS_INLINE
const type& vsList< type >::operator[]( int index ) {
	static type zero;
	if(index<len)
		return list[ index ];
	return zero;
}

/*
================================
   operator=
================================
*/
template< class type >
VS_INLINE
vsList<type>& vsList< type >::operator=( const vsList<type>& listb ) {
	vsList( listb.len );

	for(unsigned int i=0; i<len; ++i) {
		list[ i ] = listb.list[ i ];
	}

	return( *this );
}

/*
================================
   operator==
================================
*/
template< class type >
VS_INLINE
bool vsList< type >::operator==( const vsList<type>& listb ) {
	
	if(len != listb.len) return false;

	if(len>0) {
		for(int i=0; i<len; ++i) {
			if( listb.list[ i ] != list[ i ] )
				return false;
		}
	}

	return true;
}

/*
================================
   operator!=
================================
*/
template< class type >
VS_INLINE
bool vsList< type >::operator!=( const vsList<type>& listb ) {
	return !( *this == listb );
}

/*
================================
   Clear()
================================
*/
template< class type >
VS_INLINE
void vsList< type >::Free( void ) {
	if( len>1 )
		delete [] list;
	else if( len==1 ) 
		delete list;

	len = 0;
	list = NULL;
}

/*
================================
   Append()
================================
*/
template< class type >
VS_INLINE
void vsList< type >::Append( const type& elem ) {
	
	/*if(len>0) {
		type* tempList = new type[ len + 1 ];

		for(int i=0; i<len; ++i) {
			tempList[ i ] = list[ i ];
		}

		tempList[ len++ ] = elem;

		if(len>1) delete [] list;
		else delete list;

		list = tempList;
	} 
	else {
		list = new type;
		list[ 0 ] = elem;
		len = 1;
	}*/

	if(len>-1) {
		Resize( len+1 );
		list[ len-1 ] = elem;
	}
}


/*
================================
   Find()
================================
*/
template< class type >
VS_INLINE
type* vsList< type >::Find( const type& elem ) {
	int i = FindIndex( elem );
	if(i > -1) {
		return &( list[ i ] );
	}
}


/*
================================
  FindIndex() 
================================
*/
template< class type >
VS_INLINE
int vsList< type >::FindIndex( const type& elem ) {

	for(int i=0; i<len; ++i) {
		if(list[ i ] == elem)
			return i;
	}

	return -1;
}

/*
================================
  RemoveIndex() 
================================
*/
template< class type >
VS_INLINE
void vsList< type >::RemoveIndex( int index ) {
	if(index >= len || len==0) return;
	len--;
	for(int i=index; i<len; ++i) {
		list[ i ] = list[ i+1 ];
	}
	Resize( len );
}

/*
================================
   Remove()
================================
*/
template< class type >
VS_INLINE
void vsList< type >::Remove( const type& elem ) {

	int i=FindIndex( elem );
	if(i>-1) {
		RemoveIndex( i );
	}
}

/*
template< class type >
VS_INLINE
void vsList< type >::Remove( const type& elem ) {
	if(len>0) {
		int i=0;
		do { ++i; } while(list[ i ] != elem || i<len);
		if( i == len-1 ) return;	// not found

		for(; i<len-1; ++i) {
			list[ i ] = list[ i+1 ];
		}

		Resize( len - 1 );
	}
}*/

/*
================================
   Resize()
================================
*/
template< class type >
VS_INLINE
void vsList< type >::Resize( const unsigned int newLen ) {
	
	if(newLen==len) return;
	if(newLen==0) Free();

	/*if(newLen>0) {
		if(len==0) {
			len = newLen;
			list = new type[ len ];
			//ZeroMemory(&list, sizeof(type)*len);
		}

		if(len>0) {
			type zero = type();
			type* tempList = new type[ newLen ];
			int smallerLen = (newLen>len)? len : newLen;

			for(int i=0; i<newLen; ++i) {
				if(i<smallerLen)
					tempList[ i ] = list[ i ];
				else 
					tempList[ i ] = zero;
			}

			if(len==1) delete list;
			else delete [] list;

			len = newLen;
			list = tempList;
			
		}
	}*/

	if(len==0 && newLen>0) 
	{
		list = new type[ newLen ];
		len = newLen;
	} 
	else 
	{
		type* temp = list;

		len = (newLen<len)? newLen : len;

		list = new type[ newLen ];

		for(unsigned int i=0; i<len; ++i)
			list[ i ] = temp[ i ];

		len = newLen;

		// may an error occur due to the below
		//if( temp ) 
		//	delete [] temp;
	}
}

/*
================================
   Ptr()
================================
*/
template< class type >
VS_INLINE
type* vsList< type >::Ptr( void ) {
	return list;
}

/*
================================
   Length()
================================
*/
template< class type >
VS_INLINE
unsigned int vsList< type >::Length( void ) {
	return len;
}

/*
================================
   MemoryAllocated()
================================
*/
template< class type >
VS_INLINE
ulong vsList< type >::MemoryAllocated( void ) {
	if(len==0) return 0;
	return sizeof(list[ 0 ]) * len;
}

/*
================================
   BubbleSort()
================================
*/
template< class type >
VS_INLINE
void vsList< type >::BubbleSort( void ) {

	int N = len-1;

	do {
		for(int i=0; i<N; ++i) {
			if( list[ i ] > list[ i+1 ]) 
				vsSystem::Swap< type >( list[ i ], list[ i+1 ] );
		}

		N--;
	} while( N > 0 );
}

/*
================================
   Shellsort()
================================
*/
template< class type >
VS_INLINE
void vsList< type >::ShellSort( void ) {
	int increment = len / 2;
	int j;
	type temp;

	while( increment > 0 ) {

		for(int i=increment; i<len; ++i) {
			j = i;
			temp = list[ i ];

			while( j >= increment && list[ j-increment ] > temp ) {
				list[ j ] = list[ j-increment ];
				j -= increment;
			}
			list[ j ] = temp;
		}

		if(increment == 2 )
			increment = 1;
		else
			increment = (int)( increment / 2.2 );
	}
}

/*
================================
   QuickSort()
================================
*/
template< class type >
VS_INLINE
int vsList< type >::QuickSort( int lo, int hi ) {
	return 0;
}

#endif