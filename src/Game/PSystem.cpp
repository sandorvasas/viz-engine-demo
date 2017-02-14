#include "PSystem.h"
#include "Game.h"

vsPSystem * pSystem;


vsPMicroSystem::vsPMicroSystem( int numParticles, const vsVec3f& pos, const vsVec3f& dir, Enum microsystem_type, 
								const Texture& tex, const vsVec2f& texCoords ) 
{
	this->texture = texture;
	this->position = pos;
	this->dir = dir;
	this->type = microsystem_type;
	this->texture = tex;
	particles.Alloc( numParticles );
	dead = false;

	float p_life;
	vsVec3f p_gravity;
	bool p_coll;

	if ( type == PMICROSYSTEM_TYPE_SMOKE ) {
		p_coll = false;
		p_gravity = vsVec3f( 0.0f, 9.0f, 0.0f );
		p_life = 1.0f;
	} else
	if ( type == PMICROSYSTEM_TYPE_BLOCK ) {
		p_coll = true;
		p_gravity = vsVec3f( 0.0f, -9.0f, 0.0f );
		p_life = 0.3f;
	} else
	if ( type == PMICROSYSTEM_TYPE_RAIN ) {
		p_coll = true;
		p_gravity = vsVec3f( 0.0f, -9.0f, 0.0f );
		p_life = 10000.0f;
	}

	vsQuaternion rot;
	vsVec3f cdir;
	REPEAT( numParticles ) {
		rot.CreateFromEulerAnglesDEG( RandomDegree(), RandomDegree(), RandomDegree() );

		cdir = ( dir * rot ).Normalized();

		particles[ i ].life = p_life + ( rand() % 100 ) * 0.002f;
		particles[ i ].gravity = p_gravity;
		particles[ i ].position = pos + cdir * 0.2f;
		particles[ i ].size = 50.0f + ( rand() % 1000 ) * 0.01f;
		particles[ i ].testCollision = p_coll;
		particles[ i ].texCoords = texCoords;
		particles[ i ].acceleration = NULLVEC3F;
		particles[ i ].velocity = cdir.Normalized() * 4.0f;
	}
}

vsPMicroSystem::~vsPMicroSystem( void )
{
	particles.Free();
}



void vsPMicroSystem::UpdateAndRender( void )
{
	uint	dead_particles = 0;
	bool	collided;
	vsVec3f deltaS, acc, vel;

	game->GetShader( 6 )->Bind( true );
	
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );
	game->GetShader( 6 )->Uniform1i( "in_Texture", 0 );
	
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glPointSize( 25.0f );
	glBegin( GL_POINTS );
	REPU( particles.Length() ) {
				
		if ( particles[ i ].life <= 0.0f ) {
			dead_particles++;
			continue;
		} else
			particles[ i ].life -= timer.GetDeltaTime();

		acc = particles[ i ].acceleration + particles[ i ].gravity;
		vel = particles[ i ].velocity += acc * timer.GetDeltaTime() * 4.0f;
				
		deltaS = ( vel ) * timer.GetDeltaTime();
		collided = true;
		if ( particles[ i ].testCollision ) {
			vsChunk * chunk = game->world->GetChunkOfPoint( particles[ i ].position );

			if ( chunk ) {
				vsBox box( particles[ i ].position - 0.02f,
						   particles[ i ].position + 0.02f );
				
				collided = chunk->TraceBox( box, deltaS );
			}
		}

		if ( collided ) {
			particles[ i ].velocity = NULLVEC3F;
			particles[ i ].testCollision = false;
		} 
		else
			particles[ i ].position += deltaS;

		glMultiTexCoord2fv( GL_TEXTURE0, particles[ i ].texCoords.ToTypePtr()  );
		glVertexAttrib2fv( 1, particles[ i ].texCoords.ToTypePtr() );
		glVertexAttrib3fv( 0, particles[ i ].position.ToTypePtr() );
		
	}
	glEnd();
	glBindTexture( GL_TEXTURE_2D, 0 );

	game->GetShader( 6 )->Bind( false );

	if ( dead_particles == particles.Length()-1 )
		dead = true;
}



vsPSystem::vsPSystem( void )
{
}

vsPSystem::~vsPSystem( void )
{
	systems.DeletePointerElements();
	systems.Free();
}

void vsPSystem::AddSystem( vsPMicroSystem * system )
{
	systems.Append( system );
}

void vsPSystem::UpdateAndRender( void ) 
{
	REPU( systems.Length() ) {
		if ( systems[ i ]->IsDead() )
		{
			delete systems[ i ];
			systems.RemoveElementAt( i );
			i--;
		} else {
			systems[ i ]->UpdateAndRender();
		}
	}
}
