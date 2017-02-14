

#ifndef __VS_ARRAY_H__
#define __VS_ARRAY_H__

#include <windows.h>
/*
template<typename T>
class vsListElement {
public:
	virtual T&		operator=( const T& ) = 0;
	virtual bool	operator==( const T& ) = 0;
	virtual bool	operator!=( const T& b ) {
		return !( *this==b );
	}
};*/

template<class T>
class vsList {
public:		
					vsList( void );
					vsList( unsigned int len );
					~vsList( void );

	void			Alloc( unsigned int count );
	void			Resize( unsigned int newCount, bool keppOld=true );
	void			Free( void );
	void			DeletePointerElements( void );
	void			ZeroAll( void );
	void			Copy( unsigned int count, T* elem );						//not tested
	void			Insert( unsigned int pos, unsigned int count, T* elem );	//not tested
	void			RemoveElementAt( unsigned int pos );
	void			RemoveElementsAt( unsigned int pos, unsigned int num );
	void			RemoveElement( const T& elem );
	void			ShiftLeftFrom( unsigned int pos, unsigned int steps );
	void			ShiftLeftRange( unsigned int from, unsigned int to, unsigned int steps );
	void			ShiftRightFrom( unsigned int pos, unsigned int steps );
	void			Append( const T& elem );
	void			Expand( void );												// expand list by one
	T&				Last( void );
	unsigned long	MemoryUsed( void ) const;									// in bytes
	bool			IsValidIndex( unsigned int i ) const;
	const unsigned int&	Length( void ) const;
	
	void			SwapElements( unsigned int index1, unsigned int index2 );
	//void			SwapElements( const T& elem1, const T& elem2 );
	void			SwapOrder( void );
	long			SimpleSearch( const T& elem );
	long			BinarySearch( const T& elem, unsigned int l, unsigned int h );
	void			QuickSort( bool ascending=true );
	void			BubbleSort( bool ascending=true );
	void			BubbleSortSegment( unsigned int segStart, unsigned int segEnd, bool ascending=true );
	void			BubbleSortPointerSegment( unsigned int segStart, unsigned int segEnd, bool ascending=true );

	vsList<T>&		operator=( const vsList<T>& a );
	bool			operator==( const vsList<T>& a );
	bool			operator!=( const vsList<T>& a );
	T&				operator[]( unsigned int index );
	const T&		operator[]( unsigned int index ) const;
	T*				operator()( void );

private:
	void			_QuickSortASC( unsigned int l, unsigned int r );
	void			_QuickSortDESC( unsigned int l, unsigned int r );

public:	// PRIVATE!
	T*				list;
	unsigned int	Count;
};

/*
===================================
	vsList()
===================================
*/
template< class T >
inline
vsList< T >::vsList( void ) {
	list = NULL;
	Count = 0;
}

/*
===================================
	vsList()
===================================
*/
template< class T >
inline
vsList< T >::vsList( unsigned int len ) {
	list = NULL;
	Count = 0;
	Alloc( len );
}

/*
===================================
	~vsList()
===================================
*/
template< class T >
inline
vsList< T >::~vsList( void ) {
	Free();
}

/*
===================================
	Alloc
===================================
*/
template< class T >
inline
void vsList< T >::Alloc( unsigned int count ) {
	if ( count == 0 ) return;
	Free();
	list = new T[ count ];
	Count = count;
	ZeroMemory( list, sizeof( T )*count );
}

/*
===================================
	Resize
===================================
*/
template< class T >
inline
void vsList< T >::Resize( unsigned int newCount, bool keepOld ) {
	if ( !list ) {
		Alloc( newCount );
		return;
	}

	if ( newCount == Count ) {
		// nothing to do
		return;
	}

	if ( newCount == 0 ) {
		Free();
		return;
	}

	T* temp = list;

	list = new T[ newCount ];

	if ( newCount < Count ) Count = newCount;
	if ( keepOld ) {
		for ( unsigned int i=0; i < Count; i++ )
			list[ i ] = temp[ i ];
	}

	Count = newCount;

//	if ( temp ) {
		delete[] temp;
//	}
}

/*
===================================
	Free
===================================
*/
template< class T >
inline
void vsList< T >::Free( void ) {
	if ( list && Count>0 )  delete[] list;
	list = NULL;
	Count = 0;
}

/*
===================================
	DeletePointerElements
===================================
*/
template< class T >
inline
void vsList< T >::DeletePointerElements( void )
{
	for ( unsigned int i = 0; i < Count; ++i )
		if ( list[ i ] != NULL ) {
				delete list[ i ];
		}
}

/*
===================================
	ZeroAll
===================================
*/
template< class T >
inline
void vsList< T >::ZeroAll( void ) {
	if ( Count )
		::ZeroMemory( list, sizeof( T )*Count );
}

/*
===================================
	Copy
===================================
*/
template< class T >
inline
void vsList< T >::Copy( unsigned int count, T* elem ) {
	Free();
	Alloc( count );
	for ( int i = 0; i < count; i++ ) {
		list[ i ] = elem[ i ];
	}
}

/*
===================================
	Insert
===================================
*/
template< class T >
inline
void vsList< T >::Insert( unsigned int pos, unsigned int count, T* elem ) {
	if ( pos < 0 ) return;
	if ( pos > GetCount() ) return;
	if ( pos == GetCount() ) {
		for ( int i = 0; i < count; i++ )
			Append( elem[ i ] );
	}

	if ( pos < Count ) {
		int ph = Count - pos;
		T* part = new T[ ph ];
		for ( int i = 0; i < ph; i++ ) {
			part[ i ] = list[ pos + i ];
		}

		Realloc( Count + count );

		for ( int i = pos; i < count; i++ ) {
			list[ i ] = elem[ i - pos ];
		}

		for ( int i = pos + count; i < Count; i++ ) {
			list[ i ] = part[ i ];
		}

		if ( sizeof( part ) > T ) delete[] part;
		else delete part;
	}
}

/*
===================================
	RemoveElementAt
===================================
*/
template< class T >
inline
void vsList<T>::RemoveElementAt( unsigned int pos )
{
	if ( Count == 1 ) {
		Free();
		return;
	}

	if ( pos<Count ) 
	{
		for ( unsigned int i=pos; i<Count-1; ++i ) 
		{
			list[i] = list[i+1];
		}
		Resize( Count - 1 );
	}
}

/*
===================================
	RemoveElementsAt
===================================
*/
template< class T >
inline
void vsList<T>::RemoveElementsAt( unsigned int pos, unsigned int num )
{
	if ( Count == 1 ) {
		Free();
		return;
	}

	unsigned int pos_num = pos+num;
	if ( pos_num<Count ) 
	{
		for ( unsigned int i=pos; i<pos_num-1; ++i ) 
		{
			list[i] = list[i+num];
		}
		Resize( Count - num );
	}
}

/*
===================================
	RemoveElement
===================================
*/
template< class T >
inline
void vsList<T>::RemoveElement( const T& elem )
{
	long index = SimpleSearch( elem );
	if ( index != -1 ) 
	{
		RemoveElementAt( ( unsigned int )index );
	}
}


/*
===================================
	ShiftLeftFrom
===================================
*/
template< class T >
inline
void vsList<T>::ShiftLeftFrom( unsigned int pos, unsigned int steps )
{
	if ( pos+steps<=Count ) 
	{
		for ( unsigned int i=pos; i<Count-steps; ++i ) 
		{
			list[i] = list[i+steps];
		}
	}
}

/*
===================================
	ShiftLeftRange
===================================
*/
template< class T >
inline
void vsList<T>::ShiftLeftRange( unsigned int from, unsigned int to, unsigned int steps ) 
{
	if ( to+steps<=Count ) 
	{
		for ( unsigned int i=pos; i<to-steps; ++i ) 
		{
			list[i] = list[i+steps];
		}
	}
}

/*
===================================
	ShiftRightFrom
	 fixme: dont tested. never, ever.
===================================
*/
template< class T >
inline
void vsList<T>::ShiftRightFrom( unsigned int pos, unsigned int steps )
{
	unsigned int pos_num = pos+steps;
	if ( pos_num<Count ) 
	{
		for ( unsigned int i=pos_num; i>pos; ++i ) 
		{
			list[i] = list[pos-steps];
		}
	}
}

/*
===================================
	Append
===================================
*/
template< class T >
inline
void vsList< T >::Append( const T& elem ) {
	Resize( Count + 1 );
	list[ Count - 1 ] = elem;
}

/*
===================================
	Expand
===================================
*/
template< class T >
inline
void vsList< T >::Expand( void ) {
	Resize( Count + 1 );
}

/*
===================================
	Last
===================================
*/
template< class T >
inline
T& vsList< T >::Last( void ) {
	return list[ Count - 1 ];
}

/*
===================================
	MemoryUsed
===================================
*/
template< class T >
inline
unsigned long vsList< T >::MemoryUsed( void ) const {
	return sizeof( T ) * Count;
}

/*
===================================
	MemoryUsed
===================================
*/
template< class T >
inline
bool vsList< T >::IsValidIndex( unsigned int i ) const {
	return( i < Count );
}
/*
===================================
	operator=
===================================
*/
template< class T >
inline
vsList< T >& vsList< T >::operator=( const vsList< T >& a ) {
	Alloc( a.Count );
	for ( unsigned int i = 0; i < a.Count; ++i )
		this->list[ i ] = a.list[ i ];

	return *this;
}

/*
===================================
	operator==
===================================
*/
template< class T >
inline
bool vsList< T >::operator==( const vsList<T>& a ) {
	if ( a.Count!=Count ) return false;
	for ( unsigned int i = 0; i < a.Count; ++i )
		if ( this->list[ i ] != a.list[ i ] )
			return false;

	return true;
}

/*
===================================
	operator!=
===================================
*/
template< class T >
inline
bool vsList< T >::operator!=( const vsList<T>& a ) {
	return!( *this==a );
}

/*
===================================
	operator[]
===================================
*/
template< class T >
inline
T& vsList< T >::operator[]( unsigned int index ) {
	if ( index > Count-1 ) 
		index = Count-1;

	return list[ index ];
}

/*
===================================
	operator[]
===================================
*/
template< class T >
inline
const T& vsList< T >::operator[]( unsigned int index ) const {
	if ( index > Count-1 ) 
		index = Count-1;

	return list[ index ];
}

/*
===================================
	operator()
===================================
*/
template< class T >
inline
T* vsList< T >::operator()( void ) {
	return list;
}

/*
===================================
	Length
===================================
*/
template< class T >
inline
const unsigned int& vsList< T >::Length( void ) const 
{
	return Count;
}

/*
===================================
	SwapElements
===================================
*/
template< class T >
inline //not tested
void vsList<T>::SwapElements( unsigned int index1, unsigned int index2 ) 
{
	T tmp = list[index1];
	list[index1] = list[index2];
	list[index2] = tmp;
}

/*
===================================
	SwapOrder
===================================
*/
template< class T >
inline //not tested
void vsList<T>::SwapOrder( void ) {
	int len2 = Count / 2;

	for ( unsigned int i=0; i<len2; ++i ) 
	{
		list[i] = list[ Count-1 - i ];
	}
}

/*
===================================
	SimpleSearch
===================================
*/
template< class T >
inline
long vsList<T>::SimpleSearch( const T& elem ) 
{
	for ( unsigned int i=0; i<Count; ++i ) 
	{
		if ( list[i] == elem )
			return( long( i ) );
	}
	return -1;
}

/*
===================================
	BinarySearch
===================================
*/
template< class T >
inline
long vsList<T>::BinarySearch( const T& elem, unsigned int l, unsigned int h ) 
{
	int mid = ( l+h )/2;

	if ( list[mid] < elem ) BinarySearch( elem, mid+1, h ); 
	else
	if ( list[mid] > elem ) BinarySearch( elem, l, mid-1 );
	else
		return mid;

	
	return mid;
}

/*
===================================
	QuickSort
===================================
*/
template< class T >
inline
void vsList<T>::QuickSort( bool ascending )
{
	if ( ascending )	_QuickSortASC( 0, Count-1 );
	else			_QuickSortDESC( 0, Count-1 );
}

/*
===================================
	_QuickSortASC
===================================
*/
template< class T >
inline
void vsList<T>::_QuickSortASC( unsigned int left, unsigned int right )
{
	unsigned int pivot = ( left+right )/2;
	unsigned int l=left, r=right;

	do {
		while( list[l]  < list[pivot] ) l++;
		while( list[r] > list[pivot] ) r--;

		if ( l <= r ) 
			SwapElements( l++, r-- );
			
	} while( l<=r );

	if ( left < r ) _QuickSortASC( left, r );
	if ( l < right ) _QuickSortASC( l, right );
}

/*
===================================
	_QuickSortDESC
===================================
*/
template< class T >
inline
void vsList<T>::_QuickSortDESC( unsigned int left, unsigned int right )
{
	unsigned int pivot = ( left+right )/2;
	unsigned int l=left, r=right;

	do {
		while( list[l] > list[pivot] ) l++;
		while( list[r] < list[pivot] ) r--;

		if ( l <= r ) 
			SwapElements( l++, r-- );
			
	} while( l<=r );

	if ( left < r ) _QuickSortDESC( left, r );
	if ( l < right ) _QuickSortDESC( l, right );
}

/*
===================================
	BubbleSort
===================================
*/
template< class T >
inline
void vsList<T>::BubbleSort( bool ascending )
{
	BubbleSortSegment( 0, Count, ascending );
}

/*
===================================
	BubbleSortSegment
===================================
*/
template< class T >
inline
void vsList<T>::BubbleSortSegment( unsigned int segStart, unsigned int segEnd, bool ascending )
{
	if ( segStart >= segEnd-1 )
		return;

	bool swapped;
	unsigned int n = 1;

	do {
		swapped = false;
									// Count-n!
		for ( unsigned int i=segStart; i < segEnd-1; ++i )
		{
			if ( ascending? list[i] > list[i+1] : list[i] < list[i+1] ) {
				SwapElements( i, i+1 );
				swapped = true;			
			}
		}
		++n;
	} while( swapped );
}

/*
===================================
	BubbleSortPointerSegment
===================================
*/
template< class T >
inline
void vsList<T>::BubbleSortPointerSegment( unsigned int segStart, unsigned int segEnd, bool ascending )
{
	if ( segStart >= segEnd-1 )
		return;

	bool swapped;
	unsigned int n = 1;

	do {
		swapped = false;
									// Count-n!
		for ( unsigned int i=segStart; i < segEnd-1; ++i )
		{
			if ( ascending? *( list[i] ) > *( list[i+1] ) : *( list[i] ) < *( list[i+1] ) ) {
				SwapElements( i, i+1 );
				swapped = true;			
			}
		}
		++n;
	} while( swapped );
}

#endif