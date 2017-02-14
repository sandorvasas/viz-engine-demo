#ifndef __VS_WEAPON_H__
#define __VS_WEAPON_H__


#include "..\vsLib2\vsLib2.h"
#include "GameObject.h"


class	vsWeapon;
class	vsWeaponManager;
class	vsProjectile;

enum WeaponAnimation_t {
	WEAPON_ANIM_IDLE = 0,
	WEAPON_ANIM_LOWER,
	WEAPON_ANIM_RAISE,
	WEAPON_ANIM_ATTACK1,
	WEAPON_ANIM_ATTACK2,
	WEAPON_ANIM_RELOAD
};

enum WeaponType_t {
	WEAPON_TYPE_MELEE = 0,
	WEAPON_TYPE_FIREARM,
	WEAPON_TYPE_RANGED,
	WEAPON_TYPE_GRENADE,
	WEAPON_TYPE_MINE
};


struct FiringData_t {
	vsVec3f			direction;
	float			time;
	bool			isFiring;

	FiringData_t( void ) : isFiring( false ), time( 0.0f ) {};
	~FiringData_t( void ) {};
};


struct WeaponData_t {
	EMPTY_TRUCTORS( WeaponData_t );

	char			name[ 64 ];
	WeaponType_t	type;
	float			damage;
	float			mass;
	float			attackRange;
	char			modelFileName[ 128 ];
	char			idleAnimFileName[ 128 ];
	char			lowerAnimFileName[ 128 ];
	char			raiseAnimFileName[ 128 ];
	char			attack1AnimFileName[ 128 ];
	char			attack2AnimFileName[ 128 ];
	char			reloadAnimFileName[ 128 ];
};

/*
====================================================================

	vsWeapon

====================================================================
*/
class vsWeapon : public vsGameObject {

	friend class vsWeaponLibrary;

public:
						vsWeapon( const WeaponData_t& wpndata );
						~vsWeapon( void );

	void				StartFiring( const vsVec3f& at );
	void				StopFiring( void );
	void				Render( void );

	const float&		GetDamage( void ) const;
	const WeaponType_t&	GetType( void ) const;
	const float&		GetAccuraccy( void ) const;
	const float&		GetShootingSpeed( void ) const;
	const float&		GetAttackRange( void ) const;
	bool				IsFiring( void ) const;
	const FiringData_t& GetFiringData( void ) const;

private:
	int					id;
	FiringData_t		firingData;
	float				shootingSpeed;				// shot / second
	WeaponType_t		type;
	float				damage;
	float				accuraccy;
	float				attackRange;
};


VS_INLINE const float& vsWeapon::GetDamage( void ) const {
	return damage;
}

VS_INLINE const WeaponType_t& vsWeapon::GetType( void ) const {
	return type;
}

VS_INLINE const float& vsWeapon::GetAccuraccy( void ) const {
	return accuraccy;
}

VS_INLINE const float& vsWeapon::GetShootingSpeed( void ) const {
	return shootingSpeed;
}

VS_INLINE const float& vsWeapon::GetAttackRange( void ) const {
	return attackRange;
}

VS_INLINE bool vsWeapon::IsFiring( void ) const {
	return firingData.isFiring;
}

VS_INLINE const FiringData_t& vsWeapon::GetFiringData( void ) const {
	return firingData;
}


/*
====================================================================

	vsProjectile

====================================================================
*/
struct ProjectileData_t {
	vsStr		name;
	float		damage;
	float		explosionRadius;
	float		speed;
	float		mass;
	vsStr		modelFileName;
};

class vsProjectile : public vsGameObject {
public:
					vsProjectile( const ProjectileData_t& ptlData );
					~vsProjectile( void );

	const float&	GetDamage( void ) const;
	const float&	GetExplosionRadius( void ) const;

private:
	float			damage;
	float			explosionRadius;	
};

VS_INLINE const float& vsProjectile::GetDamage( void ) const {
	return damage;
}

VS_INLINE const float& vsProjectile::GetExplosionRadius( void ) const {
	return explosionRadius;
}

/*
====================================================================

	vsWeaponLibrary

====================================================================
*/
class vsWeaponLibrary {
public:
							vsWeaponLibrary( void );
							~vsWeaponLibrary( void );
	
	vsWeapon*				CreateWeapon( char * name );
	void					DeleteWeapon( vsWeapon * wpn );
	void					LoadLibrary( const char* szLibraryFileName );

private:
	vsList<WeaponData_t*>	weaponLib;
	vsList<vsWeapon*>		createdWeapons;
};

extern vsWeaponLibrary * weaponLibrary;

#endif