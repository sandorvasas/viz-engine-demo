#ifndef __PSYSTEM_H__
#define __PSYSTEM_H__

#include "..\vsLib2\vsLib2.h"

struct Particle_t {
	vsVec3f		position;
	vsVec3f		velocity;
	vsVec3f		acceleration;
	vsVec3f		gravity;
	float		life;
	float		size;
	vsVec2f		texCoords;
	bool		testCollision;

	MINIMUM_OPERATORS( Particle_t );
};

enum {
	PMICROSYSTEM_TYPE_SMOKE = 1,			// smoke raises up, no collision detection
	PMICROSYSTEM_TYPE_BLOCK,				// block pieces fall down, CD on
	PMICROSYSTEM_TYPE_RAIN					// rain falls from the sky, CD on
};


class vsPMicroSystem {
	friend class vsPSystem;
public:
	vsPMicroSystem( int numParticles, const vsVec3f& pos, const vsVec3f& dir, Enum microsystem_type, 
					const Texture& tex, const vsVec2f& texCoords );
	~vsPMicroSystem( void );

	void				UpdateAndRender( void );
	bool				IsDead( void ) { return dead; };

private:
	float				RandomDegree( void );

protected:
	bool				dead;
	uint				texture;
	vsVec3f				position;
	vsVec3f				dir;
	vsList<Particle_t>	particles;
	Enum				type;
};

VS_INLINE float vsPMicroSystem::RandomDegree( void )
{
	return ( rand() % 60 - 30 );
}

class vsPSystem {
public:
	vsPSystem( void );
	~vsPSystem( void );

	void				AddSystem( vsPMicroSystem * system );
	void				UpdateAndRender( void );

private:
	vsList<vsPMicroSystem*>	systems;
};

extern vsPSystem * pSystem;

#endif