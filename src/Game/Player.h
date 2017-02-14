#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Physics\Physics.h"
#include "..\vsLib2\vsLib2.h"
#include "Weapon.h"
#include "PlayerView.h"
#include "Item.h"
#include "Geometry\Sector2.h"
#include "Blocks.h"

using namespace vsLib2;

struct Item_t;
class vsWorld;

const float PLAYER_MAX_BLOCKING_DISTANCE = 3.5f;
const byte INVENTORY_SLOT_WIDTH = 10;
const byte INVENTORY_SLOT_HEIGHT = 5;

/* 
=====================================
	InventorySlot_t
=====================================
*/
struct InventorySlot_t {
	vsList<Item_t*>	items;
};

/* 
=====================================
	vsInventory
=====================================
*/
class vsInventory {
public:
							vsInventory( vsPlayer * owner );
							~vsInventory( void );

	void					ChangeActiveSlot( const vsVec2b& slot );
	void					AddItem( Item_t* object );
//	void					AddItems( Item_t* object );
	void					RemoveItem( const vsVec2b& slot );
	void					RemoveItems( const vsVec2b& slot, uint count );
	void					NextItem( void );
	void					PrevItem( void );
	Item_t*					GetItem( const vsVec2b& slot ) const;
	const vsVec2b&			GetActiveSlot( void ) const;
	const vsList<Item_t*>*	GetSlot( const vsVec2b& slot ) const;

	void					RestoreFromFile( vsInputFile* f );
	void					SaveToFile( vsOutputFile* f );

public:
	InventorySlot_t			slots[ INVENTORY_SLOT_WIDTH ][ INVENTORY_SLOT_HEIGHT ];
	vsVec2b					activeSlot;
	vsPlayer*				owner;
};

/* 
=====================================
	ChangeItemTo
=====================================
*/
VS_INLINE void vsInventory::ChangeActiveSlot( const vsVec2b& slot ) {
	activeSlot = slot;
}

/* 
=====================================
	GetItem
=====================================
*/
VS_INLINE Item_t* vsInventory::GetItem( const vsVec2b& slot ) const {
	if ( slots[ slot.x ][ slot.y ].items.Length() )
		return slots[ slot.x ][ slot.y ].items[ 0 ];
	else 
		return NULL;
}

/* 
=====================================
	GetActiveSlot
=====================================
*/
VS_INLINE const vsVec2b& vsInventory::GetActiveSlot( void ) const {
	return activeSlot;
}

/* 
=====================================
	playerSettings_t
=====================================
*/
typedef struct playerSettings_s {
	vsVec3f			v3Origin;
	vsVec3f			v3RotationDeg;
	vsVec2f			v2LookingSpeed;
	vsVec3f			v3Radius;
	float			fSpeed;
	float			fViewDistance;
	bool			bSpectating;
	vsWorld*		pWorld;
} playerSettings_t;



/* 
=====================================
	Weapon_t
=====================================
*/
struct Weapon_t {
public:
	typedef struct {
		vsVec3f xyz;
		vsVec3f stu;
	} vertex;

	FiringData_t	firing;
	WeaponType_t	type;
	float			damage;
	Item_t*			itemPtr;
	vsQuaternion	rotation;
	vsVec3f			translation;

private:
	uint			texture;
	uint			buffers[ 2 ];

public:
	Weapon_t( void ) {
		texture = 0;
		buffers[ 0 ] = 0;
	};
	~Weapon_t( void ) {};

	void	GenerateMesh( Item_t* item );
	void	Render( void );
};

/*
===============================================

	vsPlayer

===============================================
*/
class vsPlayer {

	friend class vsConsoleProcessor;
	friend class vsPlayerView;

public:
	vsPlayer( void ) : spectating( true ) {};
	~vsPlayer( void );

	void				Init( const playerSettings_t& st );
	void				Update( void );
	void				UpdateLookCell( void );
	void				Look( void );
	void				Render( void );
	void				GoForward( void );
	void				GoBackward( void );
	void				StrafeLeft( void );
	void				StrafeRight( void );
	void				LiftUp( void );
	void				LiftDown( void );
	void				MoveToDirection( const vsVec3f& dir );
	void				SetFieldOfView( float fov, float aspect, float nearPlane );

	void				Jump( void );
	void				SetJumpHeight( float jumpHeight );
	void				Use( void );
	void				MoveToDestination( void );

	vsVec3f				GetLookingPoint( float distFromEye ) const;
	vsPlayerView*		GetView( void ) const;
	const vsVec3f&		GetViewDirection( void ) const;
	vsPhysics*			GetPhysics( void ) const;
	const vsQuaternion&	GetAxis( void ) const;
	const vsVec3f&		GetRotation( void ) const;
	vsInventory*		GetInventory( void ) const;
	const vsFrustum*	GetFrustum( void ) const;

	const bool&			IsSpectating( void ) const;
	const bool&			IsJumping( void ) const;
	const bool&			IsClipping( void ) const;
	void				Clip( bool clip = true );

	bool				OriginChanged( /* not working */ ) { return originChanged; };

	// state control
	void				StartFiring( void );
	void				StopFiring( void );
	void				AutoPutBlock( void );
	void				AutoRemoveBlock( void );
	void				PutBlock( const vsVec3i& cell,const block_t& block );
	void				RemoveBlock( void );


	void				SaveToFile( vsOutputFile* f );
	void				RestoreFromFile( vsInputFile* f );

private:
	vsPhysics*			physics;
	vsQuaternion		axis;
	vsVec3f				spectatingDirection;
	vsVec3f				viewDirection;
	vsVec3f				walkingDirection;
	vsVec3f				strafingDirection;
	vsVec3f				rotation;			// degrees
	vsVec2f				lookingSpeed;
	vsVec3f				eyePos;
	float				speed;
	bool				spectating;
	bool				originChanged;		// true if player has moved
	float				viewDistance;
	vsFrustum			frustum;
	vsWorld*			world;
	float				steps;

	bool				clip;

	bool				firing;
	bool				canPutBlock;

	vsVec3i				lastHitCell;
	vsVec3i				hitCell;
	float				hitCellMaxLife;
	float				hitCellLife;

	vsVec3i				originCell;
	vsVec3i				lookCell;
	vsSector2*			sectorOfLookCell;
	vsVec3f				lookCellF;
	vsBox				lookBox;

	uint				particleSysTexPackId;
	vsInventory*		inventory;
	vsPlayerView*		view;

	Weapon_t			wpn;
	
	bool				jumping;
	float				curJumpHeight;
	float				jumpHeight;

public:
	bool				in_air;
};

extern vsPlayer* player;


/* 
=====================================
	GoForward
=====================================
*/
VS_INLINE void vsPlayer::GoForward( void ) 
{
	physics->AddVelocity( walkingDirection );
}

/* 
=====================================
	GoBackward
=====================================
*/
VS_INLINE void vsPlayer::GoBackward( void ) 
{
	physics->AddVelocity( -walkingDirection );
}

/* 
=====================================
	StrafeLeft
=====================================
*/
VS_INLINE void vsPlayer::StrafeLeft( void ) 
{
	physics->AddVelocity( -strafingDirection );
}

/* 
=====================================
	StrafeRight
=====================================
*/
VS_INLINE void vsPlayer::StrafeRight( void ) 
{
	physics->AddVelocity( strafingDirection );
}

/* 
=====================================
	LiftUp
=====================================
*/
VS_INLINE void vsPlayer::LiftUp( void )
{
	physics->AddVelocity( vsVec3f( 0.0f, speed * timer.GetDeltaTime(), 0.0f ) );
}

/* 
=====================================
	LiftDown
=====================================
*/
VS_INLINE void vsPlayer::LiftDown( void )
{
	physics->AddVelocity( vsVec3f( 0.0f, -speed * timer.GetDeltaTime(), 0.0f ) );
}

/* 
=====================================
	MoveToDirection
=====================================
*/
VS_INLINE void vsPlayer::MoveToDirection( const vsVec3f& dir )
{
	physics->AddVelocity( dir );
}

/* 
=====================================
	GetLookingPoint
=====================================
*/
VS_INLINE vsVec3f vsPlayer::GetLookingPoint( float distFromEye ) const
{
	return( physics->GetCenterOfMass() + viewDirection*distFromEye );
}

/* 
=====================================
	GetView
=====================================
*/
VS_INLINE vsPlayerView* vsPlayer::GetView( void ) const {
	return view;
}

/* 
=====================================
	GetView
=====================================
*/
VS_INLINE const vsVec3f& vsPlayer::GetViewDirection( void ) const {
	return viewDirection;
}

/* 
=====================================
	GetPhysics
=====================================
*/
VS_INLINE vsPhysics* vsPlayer::GetPhysics( void ) const {
	return physics;
}

/* 
=====================================
	GetRotation
=====================================
*/
VS_INLINE const vsVec3f& vsPlayer::GetRotation( void ) const {
	return rotation;
}

/* 
=====================================
	GetInventory
=====================================
*/
VS_INLINE vsInventory* vsPlayer::GetInventory( void ) const {
	return inventory;
}

/* 
=====================================
	GetFrustum
=====================================
*/
VS_INLINE const vsFrustum* vsPlayer::GetFrustum( void ) const {
	return &frustum;
}

/* 
=====================================
	GetAxis
=====================================
*/
VS_INLINE const vsQuaternion& vsPlayer::GetAxis( void ) const {
	return axis;
}

/* 
=====================================
	SetJumpHeight
=====================================
*/
VS_INLINE void vsPlayer::SetJumpHeight( float jumpHeight ) {
	this->jumpHeight = jumpHeight;
}

/* 
=====================================
	IsJumping
=====================================
*/
VS_INLINE const bool& vsPlayer::IsJumping( void ) const {
	return jumping;
}

/* 
=====================================
	IsJumping
=====================================
*/
VS_INLINE const bool& vsPlayer::IsSpectating( void ) const {
	return spectating;
}

/* 
=====================================
	MoveToDestination
=====================================
*/
VS_INLINE void vsPlayer::MoveToDestination( void ) {
	if ( physics->GetVelocity().LengthSqr() > 0.0f && !spectating && !in_air )	
		steps +=  30.0f * timer.GetDeltaTime();

	physics->MoveToDestination();
}

#endif