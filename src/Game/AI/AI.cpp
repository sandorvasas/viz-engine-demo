#include "AI.h"
#include "..\Game.h"
#include "..\Physics\PhysicsManager.h"
#include "..\Geometry\ModelManager.h"

#pragma region Monster

vsMonster::vsMonster( MD5ModelArgs_t args, const vsList<vsAnimation_MD5*>& anims ) 
{
	visible = false;
	state = -1;
	animDeltaTime = 0.f;
	model = modelManager->LoadMesh( args );
	if ( anims.Length() ) {
		model->AddAnimation( anims[0] );
		model->AddAnimation( anims[1] );
		model->AddAnimation( anims[2] );
		model->SetAnimation( 0, ANIM_PLAY_IMMEDIATELY|ANIM_PLAY_REPEAT );
	}

	baseRotation = model->GetRotation();

	SetPhysics( physicsManager->New( PHYSICS_PLAYER, true ) );
	physics->SetMass( 100.0f );
	physics->SetCenterOfMass( args.offset );
	( ( vsPhysics_Ellipsoid* )physics )->SetRadius( model->GetBounds().GetCenter() - model->GetBounds().points[ 0 ] ); 

	debAnimSpeed = 1.0f;
}

vsMonster::~vsMonster( void ) {
}

void vsMonster::Spawn( const vsVec3f& origin, const vsVec3f& rotation )
{
	( rotation );
	spawned = true;
	physics->SetCenterOfMass( origin );

	SetState( MONSTER_ANIM_STAND );
}
void vsMonster::Update( void ) 
{
	BrainFunction();
	physics->SetVelocity( vsVec3f( 0.0f, -1.0f, 0.0f )* renderSystem->FPSCounter.GetInverseCurrent() );
}

void vsMonster::Render( void )
{
	//	model->GetBounds().RenderBox();
	( ( vsPhysics_Ellipsoid* )physics )->SetRadius( model->GetBounds().GetCenter() - model->GetBounds().points[ 0 ] ); 
	
	vsGameObject::Render();
}

void vsMonster::BrainFunction( void )
{
	vsVec3f playerDir = player->GetPhysics()->GetDestination() - model->GetOffset();
	float dist = playerDir.Length();

	if ( dist < 20.0f ) 
	{
		if ( state != MONSTER_ANIM_ATTACK || state != MONSTER_ANIM_WALK ||
			lastPlayerOrigin != player->GetPhysics()->GetCenterOfMass() )
		{
			playerDir.y = 0.0f;
			playerDir.Normalize();
			
			vsVec3f dir = vsVec3f( model->GetDirection().x, 0.0f, model->GetDirection().z );
			dir.Normalize();
			vsQuaternion q;

		/*	if ( player->GetPhysics()->GetCenterOfMass() != lastPlayerOrigin ) {
				lastPlayerOrigin = player->GetPhysics()->GetCenterOfMass();
				q.CreateFromEulerAnglesRAD( 0.0f, 0.0f, ACos( playerDir.Dot( dir ) ) );
				model->rotation = baseRotation * q;
				model->direction = vsVec3f( 1.0f, 0.0f, 0.0f ) * model->rotation;
				model->direction.Normalize();

			}*/

			//if ( dist < 5.0f ) 
				//SetState( MONSTER_ANIM_ATTACK );
			//else
			//	SetState( MONSTER_ANIM_STAND );
		}
	} else 
	{
		//SetState( MONSTER_ANIM_STAND );
	}
}

void vsMonster::SetState( short state )
{
	if ( this->state != state )
	{
		this->state = state;

		if ( model->HasAnimation() )
			model->SetAnimation( state, ANIM_PLAY_IMMEDIATELY|ANIM_PLAY_REPEAT );

		if ( state == MONSTER_ANIM_STAND )
			console->Print( "monster is standing" );
		else
		if ( state == MONSTER_ANIM_WALK )
			console->Print( "monster is walking" );
		else
		if ( state == MONSTER_ANIM_ATTACK )
			console->Print( "monster is attacking" );
	}
}

#pragma endregion