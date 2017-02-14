#ifndef __VS_VARSTORAGE_H__
#define __VS_VARSTORAGE_H__

#pragma warning( disable:4100 )
#pragma warning( disable:4101 )
#pragma warning( disable:4706 )
#pragma warning( disable:4189 )

#include "..\vsLib2\vsLib2.h"


/*
================================
	vsVar
================================
*/
template< class T >
class vsVar {
public:
	vsStr	name;
	T		value;

	vsVar<T>& operator=( const vsVar<T>& r ) {
		name = r.name;
		value = r.value;
		return( *this );
	}

	bool operator==( const vsVar<T>& r ) {
		return( r.name == name && r.value == value );
	}

	bool operator!=( const vsVar<T>& r ) {
		return!( *this==r );
	}
};

/*
====================================================================

	vsVarList
	
====================================================================
*/
template< class T >
class vsVarList {
public:
						vsVarList( void );
						~vsVarList( void );
	T					operator[]( const vsStr& name );
	void				Set( const vsStr& name, T value );
	T					Get( const vsStr& name );
	bool				Stored( const vsStr& name );
private:
	vsList<vsVar<T>>	list;
	uint				realNumStored;
};

/*
===================================
	vsVarList::vsVarList
===================================
*/
template< class T > VS_INLINE
vsVarList<T>::vsVarList( void )
{
	realNumStored = 0;
	list.Alloc( 32 );
}

/*
===================================
	vsVarList::~vsVarList
===================================
*/
template< class T > VS_INLINE
vsVarList<T>::~vsVarList( void )
{
	list.Free();
}

/*
===================================
	vsVarList::operator[]
===================================
*/
template< class T > VS_INLINE
T vsVarList<T>::operator[]( const vsStr& name ) 
{
	return Get( name );
}

/*
===================================
	vsVarList::Set
===================================
*/
template< class T > VS_INLINE
void vsVarList<T>::Set( const vsStr& name, T value ) 
{
	if ( Stored( name ) ) {
		for ( uint i=0; i<realNumStored; ++i )
			if ( list[i].name == name ) {
				list[i].value = value;
				break;
			}
	} else {
		if ( list.Length() <= realNumStored )
			list.Expand();
		list[realNumStored].name = name;
		list[realNumStored].value = value;
		realNumStored++;
	}
}

/*
===================================
	vsVarList::Get
===================================
*/
template< class T > VS_INLINE
T vsVarList<T>::Get( const vsStr& name )
{
	for ( uint i=0; i<realNumStored; ++i )
		if ( list[i].name == name )
			return list[i].value;
	return T( 0 );
}

/*
===================================
	vsVarList::Stored
===================================
*/
template< class T > VS_INLINE
bool vsVarList<T>::Stored( const vsStr& name )
{
	for ( uint i=0; i<list.Length(); ++i )
		//if ( list[i].name.string != NULL )
			if ( list[i].name == name )
				return true;
	return false;
}


/*
====================================================================

	vsVarStorage
	
====================================================================
*/
class vsVarStorage {
public:
	vsVarList<bool>		Bool;
	vsVarList<vsVec3f>	Vec3f;
	vsVarList<float>	Float;
	vsVarList<int>		Int;
};

extern vsVarStorage varStorage;

#endif