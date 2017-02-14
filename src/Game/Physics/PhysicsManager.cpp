#include "PhysicsManager.h"
#include "RigidBody.h"
#include "..\..\System\VarStorage.h"
#include "..\Game.h"
#include <process.h>

using namespace vsMath;

vsPhysicsManager* physicsManager;



vsPhysicsManager::vsPhysicsManager( void )
{
	gravity = vsVec3f( 0.0f, -3.0f, 0.0f );
	ssObjects.Init();

	//hThread = ( HANDLE ) _beginthreadex( NULL, 0, &GhostPhysicsThreadProc, 0, 0, &uiThreadId );
}

vsPhysicsManager::~vsPhysicsManager( void )
{
	ssObjects.Enter();
	//objects.DeletePointerElements();
	objects.Free();

	//passiveObjects.DeletePointerElements();
	passiveObjects.Free();
	ssObjects.Leave();

	ssObjects.Delete();
}

/*
=============================================
	AddItem
=============================================
*/
vsPhysics* vsPhysicsManager::New( PhysicsType_t type, bool active )
{
	vsPhysics* obj = NULL;

	switch ( type ) {
		case PHYSICS_PLAYER:	obj = new vsPhysics_Player();		break;
		case PHYSICS_SECTOR:	obj = new vsPhysics_Sector();		break;
		case PHYSICS_ELLIPSOID: obj = new vsPhysics_Ellipsoid();	break;
		case PHYSICS_BOX:		obj = new vsPhysics_Box();			break;
		case PHYSICS_RIGIDBODY:	obj = new vsPhysics_RigidBody();	break;
	}

	if ( obj ) {
		SetTesting( obj, active );
	}

	return obj;
}

/*
=============================================
	Delete
=============================================
*/
void vsPhysicsManager::Delete( vsPhysics* obj )
{
	ssObjects.Enter();
	//if ( obj->GetPhysicsTest() ) {
		objects.RemoveElement( obj );
		//delete obj;
	//} else {
		passiveObjects.RemoveElement( obj );
		delete obj;
	//}
	ssObjects.Leave();
}

/*
=============================================
	SetAsActive
=============================================
*/
void vsPhysicsManager::SetTesting( vsPhysics* obj, bool test ) 
{
	ssObjects.Enter();
	if ( test ) {
		passiveObjects.RemoveElement( obj );
		objects.Append( obj );
	} else {
		objects.RemoveElement( obj );
		passiveObjects.Append( obj );
	}
	ssObjects.Leave();
}

/*
=============================================
	Update
=============================================
*/
void vsPhysicsManager::Update( void )
{
	if ( !ssObjects.TryEnter() )
		return;

	REPU ( objects.Length() )
	{
		//if ( objects[ i ]->GetPhysicsTest() ) 
			REPU2 ( j, objects.Length() ) 
				if ( i != j ) {
				
						TestObjects( i, j );
				}

		objects[ i ]->MoveToDestination();
	}
	ssObjects.Leave();
	//REPU ( passiveObjects.Length() )
		//passiveObjects[ i ]->MoveToDestination();
}

/*
=============================================
	TestObjects
=============================================
*/
void vsPhysicsManager::TestObjects( uint obj1, uint obj2 ) 
{
	vsPhysics* objA = objects[ obj1 ];
	vsPhysics* objB = objects[ obj2 ];

	if ( ! objA || ! objB )
		return;
	/*
	if (  	objA->GetType() == PHYSICS_PLAYER 
		&&	objB->GetType() == PHYSICS_SECTOR ) 
	{
		if ( ( ( vsPhysics_Sector* )objB )->GetSectorPtr() != NULL ) {

			vsSector2 *a, *b;
			a = game->world->GetSectorOfPoint( objA->GetCenterOfMass() );

			if ( !a )
				return;
			//	b = game->world->GetSectorOfPoint( player->GetPhysics()->GetDestination() );

			b = game->world->GetSectorOfPoint( objA->GetDestination() );
			
			if ( !b )
				return;

			vsVec3f radius = vsVec3f(0.00025f);// ((vsPhysics_Ellipsoid*)objA)->GetRadius();

			vsVec3i originCell = a->GetLocalCellOfPoint( objA->GetCenterOfMass() );
			vsVec3i destinationCell = b->GetLocalCellOfPoint( objA->GetDestination() );
			ushort value1 = a->GetVoxelDataOfLocalCell( originCell );
			ushort value2 = b->GetVoxelDataOfLocalCell( destinationCell );

			int value1_liquid = blockLibrary->GetBlockType(value1)->flags & BLOCK_FLAG_LIQUID;
			int value2_liquid = blockLibrary->GetBlockType(value2)->flags & BLOCK_FLAG_LIQUID;

			vsVec3f dest = objA->GetDestination();

			if ( originCell == destinationCell )
				return;

			if (  ( !value1 || value1_liquid )&& value2 && !value2_liquid ) {
				vsVec3i absOriginCell = a->GetVoxelOffset() + originCell;
				vsVec3i absDestinationCell = b->GetVoxelOffset() + destinationCell;
				

				vsBox boxOriginCell;
				a->GetBoxOfCell( originCell, &boxOriginCell );

				vsBox boxDestinationCell;
				b->GetBoxOfCell( destinationCell, &boxDestinationCell );

				destCell = boxDestinationCell;

				//if ( dest.y < boxDestinationCell.points[ 7 ].y )
					//dest.y = boxDestinationCell.points[ 7 ].y;


				
				
				
				if ( absDestinationCell.x > absOriginCell.x ) {
					if ( dest.x > boxDestinationCell.points[ 0 ].x - radius.x ) 
						dest.x = boxDestinationCell.points[ 0 ].x - radius.x;
						//absDestinationCell = game->world->GetCellOfPoint( dest );
				} else
					
				if ( absDestinationCell.x < absOriginCell.x ) {
					if ( dest.x < boxDestinationCell.points[ 7 ].x + radius.x )
						dest.x = boxDestinationCell.points[ 7 ].x + radius.x;
				} else
				if ( absDestinationCell.y > absOriginCell.y ) {
					if ( dest.y > boxDestinationCell.points[ 0 ].y - radius.y )
						dest.y = boxDestinationCell.points[ 0 ].y - radius.y;
				} else
				if ( absDestinationCell.y < absOriginCell.y ) {
					if ( dest.y < boxDestinationCell.points[ 7 ].y + radius.y )
						dest.y = boxDestinationCell.points[ 7 ].y + radius.y;
				} else
				if ( absDestinationCell.z > absOriginCell.z ) {
					if ( dest.z > boxDestinationCell.points[ 0 ].z - radius.z )
						dest.z = boxDestinationCell.points[ 0 ].z - radius.z;
				} else
				if ( absDestinationCell.z < absOriginCell.z ) {
					if ( dest.z < boxDestinationCell.points[ 7 ].z + radius.z )
						dest.z = boxDestinationCell.points[ 7 ].z + radius.z;
				}
					
				objA->SetVelocity( dest - objA->GetCenterOfMass() );
			}
		}
	}*/
}

/*
=============================================
	Handle_Player_Sector2
=============================================
*/
void vsPhysicsManager::Trace_Ellipsoid_Sector2( Ellipsoid_Sector2_TraceData_t * cd )
{
	vsSector2OTNode* n = cd->in_pSector->GetNodeOfPoint( cd->in_Position + cd->in_Velocity );
	
	if ( n == NULL ) {
		cd->out_Collided = false;
		cd->out_Velocity = cd->in_Velocity;
		return;
	}
	
	if ( n->vertices.Length() == 0 ) {
		cd->out_Collided = false;
		cd->out_Velocity = cd->in_Velocity;
		return;
	}


	recursionDepth = 0;
	vsVec3f originalPos = cd->in_Position;
		
	cd->inv_radius = 1.0f / cd->in_Radius;
	cd->in_Position *= cd->inv_radius;
	cd->in_Velocity *= cd->inv_radius;
	cd->lastSafePosition = cd->in_Position;
	cd->out_Collided = false;
	Ellipsoid_Sector2_LeafNode( n->Id, cd );

	// Gravity
	if ( cd->in_Gravity != vsVec3f( 0.0f ) ) {
		cd->in_Position = cd->finalPosition;
		cd->in_Velocity = cd->in_Gravity * cd->inv_radius;
		Ellipsoid_Sector2_LeafNode( n->Id, cd );
	}

	cd->out_Velocity = cd->finalPosition*cd->in_Radius - originalPos;
}

/*
=============================================
	Ellipsoid_Sector2_LeafNode
=============================================
*/
void vsPhysicsManager::Ellipsoid_Sector2_LeafNode( int nodeId, Ellipsoid_Sector2_TraceData_t* cd )
{
	if ( recursionDepth>20 )
	{
		cd->finalPosition = cd->lastSafePosition;
		//console->Print( "recursion depth >20" );
		return;
	}

	const vsSector2OTNode* n = cd->in_pSector->GetNode( nodeId );

	float		e = 0.05f;
	vsVec3f		p[3], N, cp;
	vsVec3f		origin = cd->in_Position;
	vsVec3f		velocity = cd->in_Velocity;
	vsVec3f		dest = origin + velocity;
	float		lenVelocity = velocity.Length();
	vsVec3f		unitVelocity = velocity / lenVelocity;
	vsPlane		S;
	vsVec3f		polyIPoint, sphereIPoint;
	float		d_sphereIP, d_origin;
	float		intersectionDist = 0.0f;

	cd->nearestDistance = 9999999999.0f;
	cd->foundCollision = false;
	cd->stuck = false;

	for ( uint i = 0; i < n->indices.Length(); i += 3 ) {
		p[0] = n->vertices[ n->indices[i  ] ].xyz * cd->inv_radius;
		p[1] = n->vertices[ n->indices[i+1] ].xyz * cd->inv_radius;
		p[2] = n->vertices[ n->indices[i+2] ].xyz * cd->inv_radius;

		S.FromTriangle( p[0], p[1], p[2] );
		N = S.Normal();
				
		d_origin = S.Distance( origin );
		//if ( Abs( d_origin ) > lenVelocity+1.0f )
		//	continue;

		// FINDING SPHERE INTERSECTION POINT
		sphereIPoint = origin - N*Sign( d_origin );
		d_sphereIP = S.Distance( sphereIPoint );

		// FINDING PLANE INTERSECTION POINT
		if ( d_sphereIP*d_origin < 0.0f ) {
			polyIPoint = S.ProjectOrthogonal( sphereIPoint );
			intersectionDist = ( polyIPoint - sphereIPoint ).Length();
		}
		else {
			intersectionDist = S.ProjectOblique( sphereIPoint, unitVelocity, polyIPoint );
		}

		// FINDING POLYGON INTERSECTION POINT
		// If point is outside the triangle, then polyIP = planeIP
		if ( !PointInsideTriangle( p[0],p[1],p[2], polyIPoint ) ) // edge or vertex
		{
			polyIPoint = ClosestPointOnTriangle( p[0],p[1],p[2], polyIPoint );

			intersectionDist = IntersectRaySphere( 	origin, 
													1.0f, 
													polyIPoint, 
													-unitVelocity,
													sphereIPoint );
		}

		if ( IsPointInSphere( origin, 1.0f, polyIPoint ) )
			origin = polyIPoint + N *Sign( d_origin );
			//cd->stuck = true;

		if ( intersectionDist > 0.0f && intersectionDist < lenVelocity )
		if ( intersectionDist < cd->nearestDistance ) {
			cd->nearestDistance = intersectionDist;
			cd->polyIntersectionPoint = polyIPoint;
			cd->foundCollision = true;
			cd->out_Collided = true;
			//console->Print( "Collided. iP: %s d: %4.4f", polyIPoint.ToCharPtr(), intersectionDist );
		}
	}	

	// Here comes the response step core

	if ( cd->foundCollision == false ) {
		cd->lastSafePosition = origin;
		cd->finalPosition = origin + ( unitVelocity*( lenVelocity-e ) );
		return;
	} else {
		if ( cd->stuck ) {
			cd->finalPosition = cd->lastSafePosition;
			//console->Print( "stuck" );
			return;
		}
	
		vsVec3f newPosition = origin;
		// Correction step
		if ( cd->nearestDistance >= e )
		{
			vsVec3f V = unitVelocity * ( cd->nearestDistance - e );
			newPosition = origin + V;
			//console->Print( "corrected" );
		}
	
		vsVec3f slidePlaneN = ( newPosition - cd->polyIntersectionPoint ).Normalize();
		S.Set( slidePlaneN, cd->polyIntersectionPoint );


		vsVec3f newDest = dest - slidePlaneN*( slidePlaneN.Dot( dest ) + S.d );
	
		vsVec3f newVelocity = newDest - cd->polyIntersectionPoint;

		if ( newVelocity.Length() < e ) {
			//cd->lastSafePosition = newPosition;
			cd->finalPosition = newPosition;
			return;
		}

		recursionDepth++;
		//cd->lastSafePosition = cd->in_Position;
		cd->in_Position = newPosition;
		cd->in_Velocity = newVelocity;
		Ellipsoid_Sector2_LeafNode( nodeId, cd );
	}
}

/*
=============================================
	Trace_Ellipsoid_Triangle
=============================================
*/
void vsPhysicsManager::Trace_Ellipsoid_Triangle( vsVec3f points[3], Ellipsoid_Sector2_TraceData_t * cd )
{
	vsVec3f op = cd->in_Position;
	cd->inv_radius = 1.0f / cd->in_Radius;
	cd->in_Position *= cd->inv_radius;
	cd->in_Velocity *= cd->inv_radius;
	cd->foundCollision = false;
	cd->lastSafePosition = cd->in_Position;
	recursionDepth = 0;
	Ellipsoid_Triangle( points, cd );

	cd->out_Velocity = cd->finalPosition*cd->in_Radius - op;
}

/*
=============================================
	Ellipsoid_Triangle
=============================================
*/
void vsPhysicsManager::Ellipsoid_Triangle( vsVec3f points[3], Ellipsoid_Sector2_TraceData_t * cd )
{
	float		e = 0.0005f;
	vsVec3f		N;
	vsVec3f		origin = cd->in_Position;
	vsVec3f		velocity = cd->in_Velocity;
	vsVec3f		dest = origin + velocity;
	vsVec3f		unitVelocity = velocity.Normalized();
	vsPlane		S;
	vsVec3f		polyIPoint, planeIPoint, sphereIPoint;
	float		d_dest, d_origin;
	float		intersectionDist = 0.0f;

	cd->nearestDistance = 9999999999.0f;
	cd->foundCollision = false;
	cd->stuck = false;
		
	//*******************
	// DETECTION
	//*******************

	S.FromTriangle( points[0], points[1], points[2] );
	N = S.Normal();
		
	
	d_origin = S.Distance( origin );
	sphereIPoint = origin - N*Sign( d_origin );
	d_dest = S.Distance( sphereIPoint );

	//pClassify = S.ClassifyPoint( sphereIPoint, 0.002f );

	//if ( pClassify == PLANESIDE_BACK )
		
	if ( d_dest*d_origin < 0.0f ) {
		planeIPoint = S.ProjectOrthogonal( sphereIPoint );
	}
	else {
		S.ProjectOblique( sphereIPoint, unitVelocity, planeIPoint );
	}

	polyIPoint = planeIPoint;
	intersectionDist = ( sphereIPoint - planeIPoint ).Length();

	if ( !PointInsideTriangle( points[0], points[1], points[2], polyIPoint ) )
	{
		polyIPoint = ClosestPointOnTriangle( points[0],
											points[1],
											points[2], 
											planeIPoint );
		intersectionDist = IntersectRaySphere( origin, 1.0f, polyIPoint, -unitVelocity, sphereIPoint );
	}

	glLineWidth( 5.0f );
	glColor3f( 1.0f, 0.0f, 0.0f );
	glBegin( GL_LINE_STRIP );
		
		glVertex3fv( origin.ToTypePtr() );
		glVertex3fv( sphereIPoint.ToTypePtr() );
	glEnd();
	printer->PutInQueue( 1, 0.6f, 0.95f, 0.6f, vsColors::Black, "origin: %s", origin.ToCharPtr() );
	printer->PutInQueue( 1, 0.6f, 0.90f, 0.6f, vsColors::Black, "velocity: %s", velocity.ToCharPtr() );
	printer->PutInQueue( 1, 0.6f, 0.85f, 0.6f, vsColors::Black, "sphereIP: %s", sphereIPoint.ToCharPtr() );
	printer->PutInQueue( 1, 0.6f, 0.80f, 0.6f, vsColors::Black, "planeIP: %s", planeIPoint.ToCharPtr() );
	printer->PutInQueue( 1, 0.6f, 0.75f, 0.6f, vsColors::Black, "polyIP: %s", polyIPoint.ToCharPtr() );
	printer->PutInQueue( 1, 0.6f, 0.70f, 0.6f, vsColors::Black, "IDist: %4.4f", intersectionDist );
	glColor3f( 0.0f, 0.8f, 0.0f );
	glBegin( GL_LINES );
		
		glVertex3fv( origin.ToTypePtr() );
		glVertex3fv( polyIPoint.ToTypePtr() );
	glEnd();

	if ( IsPointInSphere( origin, 1.0f, polyIPoint ) )
		cd->stuck = true;

	if ( intersectionDist > 0.0f && intersectionDist < velocity.Length() )
	if ( intersectionDist < cd->nearestDistance ) {
		cd->nearestDistance = intersectionDist;
		cd->polyIntersectionPoint = polyIPoint;
		cd->foundCollision = true;
		printer->PutInQueue( 1, 0.6f, 0.65f, 0.6f, vsColors::Black,"Collided. iP: %s d: %4.4f", polyIPoint.ToCharPtr(), intersectionDist );
		//console->Print( "Collided. iP: %s d: %4.4f", polyIPoint.ToCharPtr(), intersectionDist );
	}

	//*******************
	// RESPONSE
	//*******************
	if ( cd->foundCollision == false ) {
		cd->finalPosition = origin + ( unitVelocity*( velocity.Length()-e ) );
		cd->lastSafePosition = origin;
		return;
	} else {
		if ( cd->stuck ) {
			cd->finalPosition = cd->lastSafePosition;
			console->Print( "stuck" );
			return;
		}
	
		vsVec3f newPosition = origin;
		// Correction step
		if ( cd->nearestDistance >= e )
		{
			console->Print( "CORRECTION STEP:" );
			console->Print( "origin: %s", origin.ToCharPtr() );
			console->Print( "velocity: %s", velocity.ToCharPtr() );
			console->Print( "cd->nearestDist: %4.4f", cd->nearestDistance );
			
			vsVec3f V = unitVelocity * ( cd->nearestDistance - e );

			newPosition = origin + V;
			console->Print( "corrected" );
		}
	
		vsVec3f slidePlaneN = ( newPosition - cd->polyIntersectionPoint ).Normalize();
		S.Set( slidePlaneN, cd->polyIntersectionPoint );
		vsVec3f newDest = S.ProjectOrthogonal( dest );
		// not sure if its OK - projecting dest to S
		//vsVec3f newDest = dest - S.Distance( dest ) * slidePlaneN;

		vsVec3f newVelocity = newDest - cd->polyIntersectionPoint;

		if ( newVelocity.Length() < e ) {
			console->Print( "newVelocity.length < e" );
			cd->finalPosition = newPosition;
			return;
		}

		cd->finalPosition = newPosition + newVelocity;
	}
}

/*
=============================================
	Intersect_Ray_Sector2
=============================================
*/
void vsPhysicsManager::Intersect_Ray_Sector2( Ray_Sector2_IntersectionData_t* cd )
{
	vsSector2OTNode* n = cd->in_pSector->GetNodeOfPoint( cd->in_Position + cd->in_Velocity );
	
	if ( n == NULL ) {
		cd->out_Intersection = false;
		return;
	}
	
	if ( n->vertices.Length() == 0 ) {
		cd->out_Intersection = false;
		return;
	}

	vsVec3f		p[3];
	float		lenVelocity = cd->in_Velocity.Length();
	vsVec3f		unitVelocity = cd->in_Velocity / lenVelocity;
	vsPlane		S;
	vsVec3f		polyIPoint;
	float		intersectionDist = 0.0f;

	cd->out_Intersection = false;
	cd->out_IntersectionDistance = 9999999999.0f;

	for ( uint i = 0; i < n->indices.Length(); i += 3 ) {
		p[0] = n->vertices[ n->indices[i  ] ].xyz;
		p[1] = n->vertices[ n->indices[i+1] ].xyz;
		p[2] = n->vertices[ n->indices[i+2] ].xyz;

		S.FromTriangle( p[0], p[1], p[2] );

		// inside	
		intersectionDist = Abs( S.ProjectOblique( cd->in_Position, unitVelocity, polyIPoint ) );

		if ( !PointInsideTriangle( p[0],p[1],p[2], polyIPoint ) ) // edge or vertex
		{
			polyIPoint = ClosestPointOnTriangle( p[0],p[1],p[2], polyIPoint );

			intersectionDist = ( polyIPoint-cd->in_Position ).Length();
		}

		if ( intersectionDist < lenVelocity+0.2f &&
			intersectionDist < cd->out_IntersectionDistance ) {
			cd->out_IntersectionPoint = polyIPoint;
			cd->out_Intersection = true;
			cd->out_IntersectionDistance = intersectionDist;
		}
	}	
}

/*
=============================================
	PhysicsThreadProc
=============================================
*/
void vsPhysicsManager::PhysicsThreadProc( void )
{
	while ( 1 ) {
	//	Update();
		Sleep( 20 );
	}
}

/*
=============================================
	GhostPhysicsThreadProc
=============================================
*/
uint WINAPI vsPhysicsManager::GhostPhysicsThreadProc( void* )
{
	physicsManager->PhysicsThreadProc();
	_endthreadex( 0 );
	return 0;
}