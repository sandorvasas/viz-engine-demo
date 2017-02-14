#ifndef __VS_RIGIDBODY_H__
#define __VS_RIGIDBODY_H__

#include "Physics.h"
#include "..\..\vsLib2\vsLib2.h"


class vsPhysics_RigidBody : public vsPhysics {
public:
							vsPhysics_RigidBody( void )		 { type = PHYSICS_RIGIDBODY; };
							~vsPhysics_RigidBody( void ) {};
};

#endif