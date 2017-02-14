#ifndef __VS_PHYSICS_H__
#define __VS_PHYSICS_H__

#include "..\..\vsLib2\vsLib2.h"


class vsPhysics_RigidBody;
class vsSector2;

/*
===========================
	PHYSICS_OBJECT type enum
===========================
*/
enum PhysicsType_t {
	PHYSICS_UNKNOWN = 0,
	PHYSICS_ELLIPSOID,
	PHYSICS_BOX,
	PHYSICS_PLAYER,
	PHYSICS_SECTOR,
	PHYSICS_RIGIDBODY
};


/*
=============================================================================

	vsPhysics

	Lehetne template-es osztály: Nem GetType() lenne, hanem Self() v
	hasonló, és a template-je lenne a pontos típusosztályneve, és azzal térne
	vissza.

	Valami ilyesmi:

	template< class T >
	VS_INLINE T* vsPhysics::Self() {
		return ( ( T* ) this );
	}

	vsPhysics_Ellipsoid : vsPhysics<vsPhysics_Ellipsoid>

=============================================================================
*/
class vsPhysics {

	//friend class vsPhysicsManager;

public:
							vsPhysics( void )		{ type = PHYSICS_UNKNOWN; };
							~vsPhysics( void )		{};

	virtual const int		GetType( void ) const;

	virtual void			MoveToDestination( void );
	virtual void			SetMass( float mass );
	virtual void			SetCenterOfMass( const vsVec3f& CM );
	virtual void			SetVelocity( const vsVec3f& vel );

	virtual	const float&	GetMass( void ) const;
	virtual	const vsVec3f&	GetCenterOfMass( void ) const;
	virtual	const vsVec3f&	GetVelocity( void ) const;
	virtual const vsVec3f	GetDestination( void ) const;

	virtual void			AddVelocity( const vsVec3f& vel );


	virtual void			SaveToFile( vsOutputFile * f );
	virtual void			RestoreFromFile( vsInputFile * f );

protected:
	vsVec3f					centerOfMass;
	vsVec3f					velocity;
	float					mass;

	int						type;

	bool					tested;
};

/*
VS_INLINE T* vsPhysics::Self( void ) {
	return ( ( T* ) this );
}*/

VS_INLINE const int vsPhysics::GetType( void ) const { 
	return type; 
}

VS_INLINE void vsPhysics::MoveToDestination( void ) {
	centerOfMass += velocity;
}

VS_INLINE void vsPhysics::SetMass( float mass ) {
	this->mass = mass;
}

VS_INLINE void vsPhysics::SetCenterOfMass( const vsVec3f& CM ) {
	centerOfMass = CM;
}

VS_INLINE void vsPhysics::SetVelocity( const vsVec3f& vel ) {
	velocity = vel;
}

VS_INLINE const float& vsPhysics::GetMass( void ) const {
	return mass;
}

VS_INLINE const vsVec3f& vsPhysics::GetCenterOfMass( void ) const {
	return centerOfMass;
}

VS_INLINE const vsVec3f& vsPhysics::GetVelocity( void ) const {
	return velocity;
}

VS_INLINE const vsVec3f vsPhysics::GetDestination( void ) const {
	return centerOfMass + velocity;
}

VS_INLINE void vsPhysics::AddVelocity( const vsVec3f& vel ) {
	velocity += vel;
}

VS_INLINE void vsPhysics::SaveToFile( vsOutputFile * f ) {
	f->WriteVec3( & centerOfMass );
	f->WriteVec3( & velocity );
	f->WriteFloat( & mass );
}

VS_INLINE void vsPhysics::RestoreFromFile( vsInputFile * f ) {
	f->ReadVec3( & centerOfMass );
	f->ReadVec3( & velocity );
	f->ReadFloat( & mass );
}

/*
=============================================================================

	vsPhysics_Ellipsoid

=============================================================================
*/
class vsPhysics_Ellipsoid : public vsPhysics {
public:
							vsPhysics_Ellipsoid( void )		{ type = PHYSICS_ELLIPSOID; };
							~vsPhysics_Ellipsoid( void )	{};

	virtual void			SetRadius( const vsVec3f& r );
	virtual const vsVec3f&	GetRadius( void ) const;
	virtual const vsVec3f&	GetInverseRadius( void ) const;

	bool					IsPointInside( const vsVec3f& point ) const;

	virtual void			SaveToFile( vsOutputFile * f );
	virtual void			RestoreFromFile( vsInputFile * f );

protected:
	vsVec3f					radius;
	vsVec3f					invRadius;
};


VS_INLINE bool vsPhysics_Ellipsoid::IsPointInside( const vsVec3f& point ) const {
	vsVec3f p = point * invRadius;

	if ( ( p - centerOfMass * invRadius ).Length() <= 1.0f ) 
		return true;
	else
		return false;
}

VS_INLINE void vsPhysics_Ellipsoid::SetRadius( const vsVec3f& r ) {
	radius = r;
	invRadius = 1.0f / r;
}

VS_INLINE const vsVec3f& vsPhysics_Ellipsoid::GetRadius( void ) const {
	return radius;
}

VS_INLINE const vsVec3f& vsPhysics_Ellipsoid::GetInverseRadius( void ) const {
	return invRadius;
}

VS_INLINE void vsPhysics_Ellipsoid::SaveToFile( vsOutputFile * f ) {
	vsPhysics::SaveToFile( f );
	f->WriteVec3( & radius );
}

VS_INLINE void vsPhysics_Ellipsoid::RestoreFromFile( vsInputFile * f ) {
	vsPhysics::RestoreFromFile( f );
	f->ReadVec3( & radius );
}

/*
=============================================================================

	vsPhysics_Box

=============================================================================
*/
class vsPhysics_Box : public vsPhysics {
public:
							vsPhysics_Box( void )		{ type = PHYSICS_BOX; };
							~vsPhysics_Box( void )		{};

	virtual vsBox&			GetBounds( void );
	virtual void			SetBounds( const vsBox& box );


	virtual void			SaveToFile( vsOutputFile * f );
	virtual void			RestoreFromFile( vsInputFile * f );
protected:
	vsBox					bounds;
};


VS_INLINE void vsPhysics_Box::SetBounds( const vsBox& box ) {
	bounds = box;
}

VS_INLINE vsBox& vsPhysics_Box::GetBounds( void ) {
	return bounds;
}

VS_INLINE void vsPhysics_Box::SaveToFile( vsOutputFile * f ) {
	vsPhysics::SaveToFile( f );
	f->WriteVec3( & bounds.points[ 0 ] );
	f->WriteVec3( & bounds.points[ 7 ] );
}

VS_INLINE void vsPhysics_Box::RestoreFromFile( vsInputFile * f ) {
	vsPhysics::RestoreFromFile( f );
	f->ReadVec3( & bounds.points[ 0 ] );
	f->ReadVec3( & bounds.points[ 7 ] );

	bounds.FromMinMax( bounds.points[ 0 ], bounds.points[ 7 ] );
}

/*
=============================================================================

	vsPhysics_Sector

=============================================================================
*/
class vsPhysics_Sector : public vsPhysics {
public:
							vsPhysics_Sector( void )		{ type = PHYSICS_SECTOR; sector = NULL; };
							~vsPhysics_Sector( void )		{};

	virtual vsSector2*		GetSectorPtr( void );
	virtual void			SetSectorPtr( vsSector2* s );

/*	virtual bool			TryLockMemory( void );
	virtual void			LockMemory( void );
	virtual void			ReleaseMemoryLock( void );

	virtual const int		GetType( void ) const;

	virtual void			MoveToDestination( void );
	virtual void			SetMass( float mass );
	virtual void			SetCenterOfMass( const vsVec3f& CM );
	virtual void			SetVelocity( const vsVec3f& vel );
	virtual void			SetPhysicsTest( bool test );

	virtual	const float&	GetMass( void ) const;
	virtual	const vsVec3f&	GetCenterOfMass( void ) const;
	virtual	const vsVec3f&	GetVelocity( void ) const;
	virtual const vsVec3f	GetDestination( void ) const;
	virtual const bool&		GetPhysicsTest( void ) const;

	virtual void			AddVelocity( const vsVec3f& vel );


	//virtual void			SaveToFile( vsOutputFile * f );
	//virtual void			RestoreFromFile( vsInputFile * f );
*/
protected:
	vsSector2*				sector;
};

VS_INLINE vsSector2* vsPhysics_Sector::GetSectorPtr( void ) {
	return sector;
}

VS_INLINE void vsPhysics_Sector::SetSectorPtr( vsSector2* s ) {
	sector = s;
}



/*
=============================================================================

	vsPhysics_Player

=============================================================================
*/
class vsPhysics_Player : public vsPhysics_Box {
public:
							vsPhysics_Player( void )		{ type = PHYSICS_PLAYER; };
							~vsPhysics_Player( void )		{};

	virtual void			MoveToDestination( void );
};

VS_INLINE void vsPhysics_Player::MoveToDestination( void ) {
	vsPhysics::MoveToDestination();

	bounds += velocity;
	velocity = NULLVEC3F;
}

#endif