#ifndef __VS_AI_H__
#define __VS_AI_H__

#include "..\Geometry\Model_MD5.h"
#include "..\GameObject.h"
#include "..\..\vsLib2\vsLib2.h"


enum {
	MONSTER_ANIM_STAND = 0,
	MONSTER_ANIM_WALK = 1,
	MONSTER_ANIM_ATTACK =2
};


class vsMonster : public vsGameObject {
public:
	vsMonster( MD5ModelArgs_t args, const vsList<vsAnimation_MD5*>& anims );
	~vsMonster( void );

	void				Spawn( const vsVec3f& origin, const vsVec3f& rotation );
	void				Render( void );
	void				Update( void );
	void				BrainFunction( void );
	void				SetState( short state );

private:
	bool				visible;
	short				state;
	vsVec3f				destination;
	bool				spawned;
	float				animDeltaTime;
	vsQuaternion		baseRotation;
	vsVec3f				lastPlayerOrigin;

public:
	float				debAnimSpeed;
};

#endif