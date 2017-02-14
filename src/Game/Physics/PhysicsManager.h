#ifndef __VS_COLLISIONMANAGER_H__
#define __VS_COLLISIONMANAGER_H__

#include "..\..\System\Threading.h"
#include "..\Geometry\Sector2.h"
#include "Physics.h"
#include "RigidBody.h"
/* 
=====================================
	Ellipsoid_Sector2_TraceData_t
=====================================
*/
struct Ellipsoid_Sector2_TraceData_t {
	friend class vsPhysicsManager;
public:
	vsVec3f			in_Position;
	vsVec3f			in_Velocity;
	vsVec3f			in_Gravity;
	vsVec3f			in_Radius;
	vsSector2*		in_pSector;
	vsVec3f			out_Velocity;
	bool			out_Collided;
private:
	vsVec3f			finalPosition;
	vsVec3f			inv_radius;
	vsVec3f			lastSafePosition;
	float			nearestDistance;
	bool			foundCollision;
	bool			stuck;
	vsVec3f			sphereIntersectionPoint;
	vsVec3f			polyIntersectionPoint;
};

/* 
=====================================
	Ray_Sector2_IntersectionData_t
=====================================
*/
struct Ray_Sector2_IntersectionData_t {
	friend class vsPhysicsManager;
public:
	vsVec3f			in_Position;
	vsVec3f			in_Velocity;
	vsSector2*		in_pSector;
	vsVec3f			out_IntersectionPoint;
	float			out_IntersectionDistance;
	bool			out_Intersection;
};

/*
========================================================

	vsPhysicsManager

========================================================
*/
class vsPhysicsManager {

	friend class vsWorld;
	friend class vsConsoleProcessor;

public:
						vsPhysicsManager( void );
						~vsPhysicsManager( void );

	vsPhysics*			New( PhysicsType_t type, bool active = true ); // returns id
	void				Delete( vsPhysics* obj );
	void				Update( void );
	void				SetTesting( vsPhysics* obj, bool test = true );

//	vsPhysics*			Get( int id ) const;

	void				SetGravity( const vsVec3f& grav );
	const uint&			GetObjectCount( void ) const;
	const vsVec3f&		GetGravity( void ) const;

private:
	void	TestObjects( uint obj1, uint obj2 );
	void	Trace_Ellipsoid_Sector2( Ellipsoid_Sector2_TraceData_t * cd );
	void	Trace_Ellipsoid_Triangle( vsVec3f points[3], Ellipsoid_Sector2_TraceData_t * cd );
	void	Intersect_Ray_Sector2( Ray_Sector2_IntersectionData_t* cd );


	//void	Ellipsoid_Sector2_InternalNode( int nodeId, Ellipsoid_Sector2_TraceData_t * cd );
	void	Ellipsoid_Sector2_LeafNode( int nodeId, Ellipsoid_Sector2_TraceData_t * cd );
	void	Ellipsoid_Triangle( vsVec3f points[3], Ellipsoid_Sector2_TraceData_t * cd );

private:
	int					recursionDepth;
	vsList<vsPhysics*>	objects;
	vsList<vsPhysics*>	passiveObjects;
	vsVec3f				gravity;
public:
	vsSharedSection		ssObjects;

	vsBox	destCell;

	// Thread ----------------------------------------------------------------------------------
private:
	void				PhysicsThreadProc( void );
	static uint WINAPI	GhostPhysicsThreadProc( void* );

	HANDLE				hThread;
	uint				uiThreadId;
	//------------------------------------------------------------------------------------------
};

extern vsPhysicsManager* physicsManager;


VS_INLINE const uint& vsPhysicsManager::GetObjectCount( void ) const {
	return objects.Length();
}

VS_INLINE const vsVec3f& vsPhysicsManager::GetGravity( void ) const {
	return gravity;
}

VS_INLINE void vsPhysicsManager::SetGravity( const vsVec3f& grav ) {
	gravity = grav;
}

#endif