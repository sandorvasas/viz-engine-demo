#include "Physics.h"
#include "PhysicsManager.h"
#include "..\Geometry\Sector2.h"


/*
=============================================================================

	vsPhysics_Sector

=============================================================================


VS_INLINE bool vsPhysics_Sector::TryLockMemory( void ) {
	if ( sector )
		return sector->TryLockMemory();
	return false;
}

VS_INLINE void vsPhysics_Sector::LockMemory( void ) {
	if ( sector )
		sector->LockMemory();
}

VS_INLINE void vsPhysics_Sector::ReleaseMemoryLock( void ) {
	if ( sector )
		sector->ReleaseMemoryLock();
}

VS_INLINE const int vsPhysics_Sector::GetType( void ) const {
	LockMemory();
	int typp = type;
	ReleaseMemoryLock();

	return typp;
}

*/