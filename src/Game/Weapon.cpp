#include "Weapon.h"
#include "Physics\PhysicsManager.h"
#include "Geometry\ModelManager.h"
#include "Game.h"
using namespace vsLib2;

vsWeaponLibrary * weaponLibrary;

/*
====================================================================

	vsWeapon

====================================================================
*/
vsWeapon::vsWeapon( const WeaponData_t& wpndata )
{
	type			= wpndata.type;
	damage			= wpndata.damage;
	attackRange		= wpndata.attackRange;
	accuraccy		= 1.0f;
	shootingSpeed	= 3.0f;
	firingData.isFiring	= false;

	MD5ModelArgs_t margs;
	margs.scale = vsVec3f( 0.01f );
	margs.rotation = vsVec3f( 90.0f, 0.0f, 0.0f );
	margs.shader = game->GetShader( 4 );
	margs.szFileName = ( char* )wpndata.modelFileName;
	model = modelManager->LoadMesh( margs );

	model->AddAnimation( modelManager->LoadAnim( wpndata.idleAnimFileName ) );
	model->AddAnimation( modelManager->LoadAnim( wpndata.lowerAnimFileName ) );
	model->AddAnimation( modelManager->LoadAnim( wpndata.raiseAnimFileName ) );
	model->AddAnimation( modelManager->LoadAnim( wpndata.attack1AnimFileName ) );
	model->AddAnimation( modelManager->LoadAnim( wpndata.attack2AnimFileName ) );
	model->AddAnimation( modelManager->LoadAnim( wpndata.reloadAnimFileName ) );

	model->SetAnimation( WEAPON_ANIM_IDLE, ANIM_PLAY_REPEAT );

	SetPhysics( physicsManager->New( PHYSICS_ELLIPSOID ) );
	physicsManager->ssObjects.Enter();
	( ( vsPhysics_Ellipsoid* )physics )->SetRadius( vsVec3f( 0.1f, 0.2f, 0.1f ) );
	physics->SetMass( wpndata.mass );
	physicsManager->ssObjects.Leave();
}

/*
=============================================
	~vsWeapon
=============================================
*/
vsWeapon::~vsWeapon( void ) 
{

}

/*
=============================================
	StartFiring
=============================================
*/
void vsWeapon::StartFiring( const vsVec3f& at )
{
	if ( !firingData.isFiring ) {
		firingData.isFiring = true;
		firingData.direction = at;
		firingData.time = 0.0f;

		if ( model )
			model->SetAnimation( WEAPON_ANIM_ATTACK1, ANIM_PLAY_REPEAT );
	}
}

/*
=============================================
	StopFiring
=============================================
*/
void vsWeapon::StopFiring( void )
{
	if ( firingData.isFiring ) {
		firingData.isFiring = false;

		model->SetAnimation( WEAPON_ANIM_IDLE, ANIM_PLAY_REPEAT );
	}
}

/*
=============================================
	vsWeapon
=============================================
*/
void vsWeapon::Render( void )
{
	firingData.time++ ;//+= renderSystem->FPSCounter.GetInverseCurrent();

	//SetOrigin( player->GetPhysics()->GetCenterOfMass() - vsVec3f( 0.f, 0.1f, 0.f ) );
	SetOrigin( player->GetLookingPoint( -0.07f ) - vsVec3f( 0.f, 0.001f, 0.f ) );

	vsQuaternion q1, q2;
	q1.CreateFromEulerAnglesDEG( 0.0f, 0.0f, 90.0f );
	q2.CreateFromEulerAnglesDEG( -90.0f, 0.0f, 0.0f );
	model->SetRotation( player->GetAxis().Inverse()  * q2 * q1 );

	vsGameObject::Render( false );
}

/*
====================================================================

	vsWeaponLibrary

====================================================================
*/
vsWeaponLibrary::vsWeaponLibrary( void ) 
{
}

/*
=============================================
	~vsWeaponLibrary
=============================================
*/
vsWeaponLibrary::~vsWeaponLibrary( void )
{
	weaponLib.DeletePointerElements();
	weaponLib.Free();

	createdWeapons.DeletePointerElements();
	createdWeapons.Free();
}

/*
=============================================
	CreateWeapon
=============================================
*/
vsWeapon* vsWeaponLibrary::CreateWeapon( char * name )
{
	int typeId = -1;
	REPU ( weaponLib.Length() ) {
		if ( !strcmp( weaponLib[ i ]->name, name ) ) {
			typeId = ( int )i;
			break;
		}
	}

	if ( typeId != -1 ) {
		vsWeapon * wpn = new vsWeapon( *weaponLib[ typeId ] );
		wpn->id = createdWeapons.Length();
		createdWeapons.Append( wpn );
		return wpn;
	}

	return NULL;
}

/*
=============================================
	DeleteWeapon
=============================================
*/
void vsWeaponLibrary::DeleteWeapon( vsWeapon * wpn )
{
	if ( wpn ) {
		createdWeapons.RemoveElement( wpn );
		delete wpn;
	}
}

/*
=============================================
	LoadLibrary
=============================================
*/
void vsWeaponLibrary::LoadLibrary( const char* szLibraryFileName )
{
	FILE * file;

	if ( fopen_s( &file, szLibraryFileName, "r" ) ) {
		console->Print( CMT_ERROR, "Failed to load weapon library" );
		return;
	}

	char line[ 512 ];
	WeaponData_t * wpn;
	char type_name[ 32 ];

	while ( !feof( file ) ) {
		fgets( line, 512, file );

		if ( strstr( line, "weapons" ) ) {
			while ( !strstr( line, "}" ) ) {
				fgets( line, 512, file );

				if ( strstr( line, "weapon" ) ) {
					wpn = new WeaponData_t();
					sscanf_s( line, " weapon %s %s %f %f %f",
						wpn->name, 64, 
						type_name, 32,
						&wpn->attackRange,
						&wpn->damage,
						&wpn->mass );

					if ( !strcmp( type_name, "melee" ) ) {
						wpn->type = WEAPON_TYPE_MELEE;
					} else
					if ( !strcmp( type_name, "firearm" ) ) {
						wpn->type = WEAPON_TYPE_FIREARM;
					} else
					if ( !strcmp( type_name, "ranged" ) ) {
						wpn->type = WEAPON_TYPE_RANGED;
					} else
					if ( !strcmp( type_name, "mine" ) ) {
						wpn->type = WEAPON_TYPE_MINE;
					} else
					if ( !strcmp( type_name, "grenade" ) ) {
						wpn->type = WEAPON_TYPE_GRENADE;
					}

					// MODEL
					fgets( line, 512, file );
					sscanf_s( line, " %s", wpn->modelFileName, 128 );

					// IDLE ANIM
					fgets( line, 512, file );
					sscanf_s( line, " %s", wpn->idleAnimFileName, 128 );
					// LOWER ANIM
					fgets( line, 512, file );
					sscanf_s( line, " %s", wpn->lowerAnimFileName, 128 );
					// RAISE ANIM
					fgets( line, 512, file );
					sscanf_s( line, " %s", wpn->raiseAnimFileName, 128 );
					// ATTACK1 ANIM
					fgets( line, 512, file );
					sscanf_s( line, " %s", wpn->attack1AnimFileName, 128 );
					// ATTACK2 ANIM
					fgets( line, 512, file );
					sscanf_s( line, " %s", wpn->attack2AnimFileName, 128 );
					// RELOAD ANIM
					fgets( line, 512, file );
					sscanf_s( line, " %s", wpn->reloadAnimFileName, 128 );

					weaponLib.Append( wpn );
				}
			}

		}
	}


	fclose( file );
}