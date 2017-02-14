#include "ParticleSystem.h"
#include "Player.h"
#include "..\System\VarStorage.h"
#include "Physics\\PhysicsManager.h"


using namespace vsLib2;

#pragma region PARTICLE SUBSYSTEM
/*
=======================================================================


	vsParticleSubsystem


=======================================================================
*/
vsParticleSubsystem::vsParticleSubsystem( particleSubsystemAttribs_t* attribs )
{
	InitSubsystem( attribs );
}

/* 
=====================================
	~vsParticleSubsystem
=====================================
*/
vsParticleSubsystem::~vsParticleSubsystem( void )
{
	if ( m_attribs != NULL )
		delete m_attribs;

	if ( m_numParticles > 0 )
		glDeleteBuffers( 2, m_buffers );

	m_particles.Free();
	m_indices.Free();
}

/* 
=====================================
	Init
=====================================
*/
void vsParticleSubsystem::InitSubsystem( particleSubsystemAttribs_t* attribs )
{
	m_numParticles = 0;

	m_attribs = attribs;
	m_particles.Alloc( m_attribs->m_maxNumParticles );
	m_indices.Alloc( m_attribs->m_maxNumParticles );

	m_bounds.FromMinMax( m_attribs->m_origin - m_attribs->m_scale,
						 m_attribs->m_origin + m_attribs->m_scale );

	glGenBuffers( 2, m_buffers );
	// vertices & colors & texUnits
	glBindBuffer( GL_ARRAY_BUFFER, m_buffers[0] );
	glBufferData( GL_ARRAY_BUFFER, 
				 m_attribs->m_maxNumParticles*( sizeof( vsVec3f )+sizeof( vsVec4f )+1 ), 
				 NULL, 
				 GL_DYNAMIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	// indices
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffers[1] );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
				 m_attribs->m_maxNumParticles*sizeof( ushort ), 
				 NULL, 
				 GL_DYNAMIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
/* 
=====================================
	UpdatePhysics
=====================================
*/
void vsParticleSubsystem::UpdatePhysics( void )
{
	if ( m_attribs == NULL ) return;

	REPU( m_numParticles ) 
	{
		m_particles[i].m_color.w -= m_attribs->m_invLifeTime;
		m_particles[i].m_origin += m_particles[i].m_velocity;	
	}

	DeleteDeadParticles();
	UpdateBuffer( 0 );
	UpdateBuffer( 1 );
}

/* 
=====================================
	Emit
=====================================
*/
void vsParticleSubsystem::Emit( byte type, const vsVec3f& origin, const vsVec3f& velocity, const vsVec4f& color )
{
	if ( m_attribs == NULL ) return;

	if ( m_numParticles+1 > m_attribs->m_maxNumParticles ) 
		return;

	m_particles[m_numParticles].m_origin = origin;
	m_particles[m_numParticles].m_velocity = velocity;
	m_particles[m_numParticles].m_type = type;
	m_particles[m_numParticles].m_size = m_attribs->m_scale.ToVec2f();
	m_particles[m_numParticles].m_color = vsVec4f( color.x, color.y, color.z, 1.0f );
	m_indices[m_numParticles] = m_numParticles;

	UpdateBuffer( 0, false, m_numParticles );	// vertices ( m_origin )
	UpdateBuffer( 1, false, m_numParticles );	// colors	( m_color )
	UpdateBuffer( 2, false, m_numParticles );	// indices
	UpdateBuffer( 3, false, m_numParticles );	// texunits ( m_type )

	m_numParticles++;
}

/* 
=====================================
	DeleteDeadParticles
=====================================
*/
void vsParticleSubsystem::DeleteDeadParticles( void )
{
	REPU( m_numParticles )
		if ( m_particles[i].m_color.w <= 0.0f ) {
			m_particles.ShiftLeftFrom( i, 1 );
			if ( !--m_numParticles ) {
				delete m_attribs;
				m_attribs = NULL;
			}
		}
	UpdateBuffer( 3 );
}

/* 
=====================================
	SortParticlesByDistance
=====================================
*/
void vsParticleSubsystem::SortParticlesByDistance( const vsVec3f& respectedPoint )
{
	if ( !m_attribs || !m_numParticles ) return;

	REP( ushort, m_numParticles ) {
			m_particles[i].m_distance = ( m_particles[i].m_origin - respectedPoint ).Length();
			m_indices[i] = i;
	}

	bool swapped;
	ushort tmp;
	do {
		swapped = false;
									// Count-n-1!
		for ( ushort i=0; i<m_numParticles-1; ++i )
		{
			if ( m_particles[m_indices[i]].m_distance < m_particles[m_indices[i+1]].m_distance ) {
				tmp = m_indices[i+1];
				m_indices[i+1] = m_indices[i];
				m_indices[i] = tmp;
				swapped = true;			
			}
		}
	} while( swapped );

	UpdateBuffer( 2, true );
}

/* 
=====================================
	Render
=====================================
*/
void vsParticleSubsystem::Render( void )
{
	if ( !m_attribs ) 
		return;

	if ( !m_attribs->m_shader || !m_numParticles )
		return;

	//if ( player->OriginChanged() )
		SortParticlesByDistance( player->GetPhysics()->GetCenterOfMass() );
	
	m_attribs->m_shader->Bind( true );
	m_attribs->m_shader->Uniform1f( "in_SpriteSize", m_spriteSize );

	// WARNING: CALL TO PURE VIRTUAL FUNCTIONS!
	SetupPointParameters();
	byte numTextures = BindTexturesAndUniformToShader();

	glBindBuffer( GL_ARRAY_BUFFER, m_buffers[0] );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );

	uint colorOffset = m_attribs->m_maxNumParticles*sizeof( vsVec3f );
	uint texUnitOffset = colorOffset+m_attribs->m_maxNumParticles*sizeof( vsVec4f );

	glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( vsVec3f ), BUFFER_OFFSET( 0 ) );
	glVertexAttribPointer( 1, 4, GL_FLOAT, false, sizeof( vsVec4f ), BUFFER_OFFSET( colorOffset ) );
	glVertexAttribPointer( 2, 1, GL_UNSIGNED_BYTE, false, 1, BUFFER_OFFSET( texUnitOffset ) );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffers[1] );
	glDrawElements( GL_POINTS, m_numParticles, GL_UNSIGNED_SHORT, NULL );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	glDisableVertexAttribArray( 0 );
	glDisableVertexAttribArray( 1 );
	glDisableVertexAttribArray( 2 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	REP( byte, numTextures ) {
		glActiveTexture( GL_TEXTURE0+i );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	m_attribs->m_shader->Bind( false );

	

	/*glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, sizeof( particle_t ), m_particles.list );

	glDrawElements( GL_POINTS, m_numIndices, GL_UNSIGNED_SHORT, m_indices.list );
	glDisableClientState( GL_VERTEX_ARRAY );
	*/
	/*glBegin( GL_POINTS );
		REP( ushort, m_numIndices )
				glVertex3fv( m_particles[m_indices[i]].m_origin.ToTypePtr() );
	glEnd();*/
}

/* 
=====================================
	UpdateBuffer
		0: VERTEX
		1: COLOR
		2: INDEX
		3: TEXUNIT
=====================================
*/
void vsParticleSubsystem::UpdateBuffer( byte which, bool updateWhole, ushort offset )
{
	if ( !m_attribs ) return;

	switch( which ) {
	case 0: 
		{ // VERTEX BUFFER
			glBindBuffer( GL_ARRAY_BUFFER, m_buffers[0] );

			vsVec3f* pos = ( vsVec3f* )glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

			if ( updateWhole ) 
				for ( ushort i=0; i<m_numParticles; ++i )
					pos[i] = m_particles[i].m_origin;
			else 
				pos[offset] = m_particles[offset].m_origin;

			glUnmapBuffer( GL_ARRAY_BUFFER );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}
	break;
	case 1: // COLOR BUFFER
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_buffers[0] );

			byte* ptr = ( byte* )glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
			ptr += ( m_attribs->m_maxNumParticles * sizeof( vsVec3f ) );
			
			vsVec4f* color = ( vsVec4f* )ptr;
			
			if ( updateWhole ) 
				for ( ushort i=0; i<m_numParticles; ++i )
					color[i] = m_particles[i].m_color;
			else 
				color[offset] = m_particles[offset].m_color;

			glUnmapBuffer( GL_ARRAY_BUFFER );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}
	break;
	case 2: // INDEX BUFFER
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffers[1] );
		if ( updateWhole )
			glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 
							0, 
							m_numParticles * sizeof( ushort ),
							m_indices.list );
		else 
			glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 
							offset * sizeof( ushort ), 
							sizeof( ushort ),
							&m_indices.list[offset] );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	break;
	case 3: // TEXUNIT BUFFER
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_buffers[0] );

			byte* ptr = ( byte* )glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
			ptr += ( m_attribs->m_maxNumParticles * ( sizeof( vsVec3f )+sizeof( vsVec4f ) ) );
			
			byte* texUnits = ptr;
			
			if ( updateWhole ) {
				for ( ushort i=0; i<m_numParticles; ++i )
					texUnits[i] = m_particles[i].m_type;
			} else 
				texUnits[offset] = m_particles[offset].m_type;

			glUnmapBuffer( GL_ARRAY_BUFFER );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}
	break;
	}
}

#pragma endregion

#pragma region SNOWSTORM
/*
=======================================================================


	vsSnowStorm


=======================================================================
*/
/* 
=====================================
	SetWind
=====================================
*/
void vsSnowStorm::SetWind( const vsVec3f& wind )
{
	m_wind = wind;
}

/* 
=====================================
	Init
=====================================
*/
void vsSnowStorm::Init( void )
{
	REPU( m_attribs->m_maxNumParticles )
		Emit( PARTICLE_TYPE_SNOWFLAKE,
			 player->GetPhysics()->GetCenterOfMass() + vsVec3f(  float( rand()%450-225 )*0.1f,
										10.f+float( rand()%20 )*0.2f,
										float( rand()%450-225 )*0.1f ),
			 physicsManager->GetGravity() * float( rand()%100+40 )*0.01f,
			 vsVec4f( 1.0f ) );
}

/* 
=====================================
	UpdatePhysics
=====================================
*/
void vsSnowStorm::UpdatePhysics( void )
{
	REPU( m_numParticles ) {
		m_particles[i].m_origin += m_particles[i].m_velocity+m_wind;	
		if ( m_particles[i].m_origin.y < player->GetPhysics()->GetCenterOfMass().y - 10.0f )
			m_particles[i].m_origin = player->GetPhysics()->GetCenterOfMass() + 
									vsVec3f( float( rand()%450-225 )*0.1f,
											 10.0f+float( rand()%20 )*0.2f,
											 float( rand()%450-225 )*0.1f );
	}
	DeleteDeadParticles();
	UpdateBuffer( 0 );
	UpdateBuffer( 1 );
	UpdateBuffer( 3 );
}

/* 
=====================================
	BindTexturesAndUniformShader
=====================================
*/
byte vsSnowStorm::BindTexturesAndUniformToShader( void )
{
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_attribs->m_parent->GetTexture( PARTICLE_TYPE_SNOWFLAKE ) );
	m_attribs->m_shader->Uniform1i( "diff0", 0 );

	return 1;
}

/* 
=====================================
	SetupPointParameters
=====================================
*/
void vsSnowStorm::SetupPointParameters( void )
{
	//glPointSize( 25.0f );
	m_spriteSize = 250.0f;
	//glPointParameterf( GL_POINT_FADE_THRESHOLD_SIZE, 10.0f );
	//glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION, vsVec3f( 0.f, 1.f, 0.f ).ToTypePtr() );
}
#pragma endregion

#pragma region EXPLOSION1
/*
=======================================================================


	vsExplosion1


=======================================================================
*/
/* 
=====================================
	Init
=====================================
*/
void vsExplosion1::Init( void )
{
	ushort part3 = m_attribs->m_maxNumParticles / 3;
	vsVec3f scale = m_attribs->m_scale;
	vsVec3f pVel;
	vsQuaternion angle;
	
	byte maxAngle = 75;
	byte halfMaxAngle = maxAngle / 2;

	for ( uint i=0; i<part3; ++i ) 
	{
		angle.CreateFromEulerAnglesDEG( float( rand()%maxAngle-halfMaxAngle ),
										float( rand()%maxAngle-halfMaxAngle ),
										float( rand()%maxAngle-halfMaxAngle ) );

		pVel = ( m_direction*angle ).Normalized() * float( rand()%100 ) * 0.003f;

		Emit( 	PARTICLE_TYPE_SMOKE, 
				m_attribs->m_origin, 
				pVel, 
				vsColors::MediumGrey );
	}

	maxAngle = 20;
	halfMaxAngle = maxAngle / 2;

	for ( ushort i=part3; i<part3*2; ++i ) {
		angle.CreateFromEulerAnglesDEG( float( rand()%maxAngle-halfMaxAngle ),
										float( rand()%maxAngle-halfMaxAngle ),
										float( rand()%maxAngle-halfMaxAngle ) );

		pVel = ( m_direction*angle ).Normalized() * float( rand()%100 ) * 0.001f;

		Emit( 	PARTICLE_TYPE_FIRE, 
				m_attribs->m_origin, 
				pVel, 
				vsColors::Orange );
	}

	/*	Emit( 	PARTICLE_TYPE_DUST, 
				m_attribs->m_origin+pPos, 
				pVel*6.0f, 
				vsColors::DarkKhaki );*/

}

/* 
=====================================
	BindTexturesAndUniformShader
=====================================
*/
byte vsExplosion1::BindTexturesAndUniformToShader( void )
{
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_attribs->m_texPack->textures[0] );
	m_attribs->m_shader->Uniform1i( "diff0", 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, m_attribs->m_texPack->textures[1] );
	m_attribs->m_shader->Uniform1i( "diff1", 1 );

	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, m_attribs->m_texPack->textures[2] );
	m_attribs->m_shader->Uniform1i( "diff2", 2 );

	return 3;
}

/* 
=====================================
	SetupPointParameters
=====================================
*/
void vsExplosion1::SetupPointParameters( void )
{
	m_spriteSize = m_attribs->m_scale.x * 80.0f;
//	glPointSize( 60.0f );
//	glPointParameterf( GL_POINT_FADE_THRESHOLD_SIZE, 60.0f );
//	glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION, vsVec3f( 0.f, 10.f, 0.f ).ToTypePtr() );
}

#pragma endregion


#pragma region REMOVE_BLOCK_EFFECT
/* 
=====================================
	Init
=====================================
*/
void vsRemoveBlockEffect::Init( void )
{
	ushort part3 = m_attribs->m_maxNumParticles / 3;
	vsVec3f scale = m_attribs->m_scale;
	vsVec3f pVel;
	vsQuaternion angle;
	
	byte maxAngle = 80;
	byte halfMaxAngle = maxAngle / 2;

	for ( uint i=0; i<m_attribs->m_maxNumParticles; ++i ) 
	{
		angle.CreateFromEulerAnglesDEG( float( rand()%maxAngle-halfMaxAngle ),
										float( rand()%maxAngle-halfMaxAngle ),
										float( rand()%maxAngle-halfMaxAngle ) );

		pVel = ( m_direction*angle ).Normalized() * float( rand()%100 ) * 0.003f;

		Emit( 	PARTICLE_TYPE_SMALL, 
				m_attribs->m_origin, 
				pVel, 
				vsColors::DarkKhaki );
	}

}

/* 
=====================================
	BindTexturesAndUniformShader
=====================================
*/
byte vsRemoveBlockEffect::BindTexturesAndUniformToShader( void )
{
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_attribs->m_texPack->textures[0] );
	m_attribs->m_shader->Uniform1i( "diff0", 0 );

	return 1;
}

/* 
=====================================
	SetupPointParameters
=====================================
*/
void vsRemoveBlockEffect::SetupPointParameters( void )
{
	m_spriteSize = m_attribs->m_scale.x * 40.0f;
//	glPointSize( 60.0f );
//	glPointParameterf( GL_POINT_FADE_THRESHOLD_SIZE, 60.0f );
//	glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION, vsVec3f( 0.f, 10.f, 0.f ).ToTypePtr() );
}

/* 
=====================================
	UpdatePhysics
=====================================
*/
void vsRemoveBlockEffect::UpdatePhysics( void )
{
	REPU( m_numParticles ) {
		m_particles[i].m_origin += m_particles[i].m_velocity;	
		if ( m_particles[i].m_origin.y < player->GetPhysics()->GetCenterOfMass().y - 10.0f )
			m_particles[i].m_origin = player->GetPhysics()->GetCenterOfMass() + 
									vsVec3f( float( rand()%450-225 )*0.1f,
											 10.0f+float( rand()%20 )*0.2f,
											 float( rand()%450-225 )*0.1f );

		m_particles[i].m_velocity.y = sinf( m_attribs->m_invLifeTime * PI*0.5f )*0.1f;
	}
	DeleteDeadParticles();
	UpdateBuffer( 0 );
	UpdateBuffer( 1 );
	UpdateBuffer( 3 );
}
#pragma endregion

#pragma region PARTICLE SYSTEM
/*
=======================================================================


	vsParticleSystem


=======================================================================
*/
vsParticleSystem::vsParticleSystem( void )
{
	m_shader = NULL;
	//m_numSubSystems = 0;
	//m_subSystems.Alloc( 200 );
}

/* 
=====================================
	vsParticleSystem
=====================================
*/
vsParticleSystem::vsParticleSystem( vsShader* shader )
{
	Init( shader );
}

/* 
=====================================
	~vsParticleSystem
=====================================
*/
vsParticleSystem::~vsParticleSystem( void )
{
	m_subSystems.DeletePointerElements();
	m_subSystems.Free();
}

/* 
=====================================
	Init
=====================================
*/
void vsParticleSystem::Init( vsShader* shader )
{
	m_shader = shader;
	m_textures.Alloc( 16 );
	
	shader->BindAttrib( 0, "in_Position" );
	shader->BindAttrib( 1, "in_Color" );
	shader->BindAttrib( 2, "in_TexUnit" );
	//shader->Bind( true );
	
	//shader->Bind( false );
}

/* 
=====================================
	UpdatePhysics
=====================================
*/
void vsParticleSystem::UpdatePhysics( void )
{
	REPU( m_subSystems.Length() ) {
		m_subSystems[i]->UpdatePhysics();
	}

	REPU( m_subSystems.Length() )
		if ( m_subSystems[i]->IsDead() ) {
			delete m_subSystems[i];
			m_subSystems.RemoveElementAt( i );
		}
}

/* 
=====================================
	Render
=====================================
*/
void vsParticleSystem::Render( void )
{
	SortSubsystems();

	REPU( m_subSystems.Length() )
		m_subSystems[i]->Render();
}

/* 
=====================================
	SortSubSystems
=====================================
*/
void vsParticleSystem::SortSubsystems( void ) 
{
	if ( !m_subSystems.Length() )
		return;

	// Calculate distances from viewer
	REPU( m_subSystems.Length() )
		if ( !m_subSystems[i]->IsDead() )
			m_subSystems[i]->m_distance = ( m_subSystems[i]->m_attribs->m_origin - player->GetPhysics()->GetCenterOfMass() ).Length();

	bool swapped;
	vsParticleSubsystem* tmp;
	do {
		swapped = false;
									// Count-n-1!
		for ( ushort i=0; i<m_subSystems.Length()-1; ++i )
		{
			if ( m_subSystems[i]->m_distance < m_subSystems[i+1]->m_distance ) {
				tmp = m_subSystems[i+1];
				m_subSystems[i+1] = m_subSystems[i];
				m_subSystems[i] = tmp;
				swapped = true;			
			}
		}
	} while( swapped );
}

/* 
=====================================
	Explosion1
=====================================
*/
int vsParticleSystem::Explosion1( const vsVec3f& origin, const vsVec3f& scale, float lifeTime, const vsVec3f& direction, uint texPackId )
{
//	if ( varStorage.Bool["disableparticles"] ) return -1;

	// create new subsystem
	particleSubsystemAttribs_t* psa = new particleSubsystemAttribs_t();
	psa->m_invLifeTime = 1.0f/lifeTime;
	psa->m_origin = origin;
	psa->m_parent = this;
	psa->m_maxNumParticles = 16;
	psa->m_scale = scale;
	psa->m_shader = m_shader;
	pstp_t* tp = GetTexturePackById( texPackId );
	psa->m_texPack = ( tp==NULL )? &m_defaultTexPack : tp;

	m_subSystems.Append( new vsExplosion1( psa ) );

	return m_subSystems.Length()-1;
}

/* 
=====================================
	SnowStorm
=====================================
*/
int vsParticleSystem::SnowStorm( const vsVec3f& wind, int texture )
{
//	if ( varStorage.Bool["disableparticles"] ) return -1;

	// create new subsystem
	particleSubsystemAttribs_t* psa = new particleSubsystemAttribs_t();
	psa->m_invLifeTime = 0.0f;
	psa->m_origin = player->GetPhysics()->GetCenterOfMass();
	psa->m_parent = this;
	psa->m_maxNumParticles = 64;
	psa->m_scale = vsVec3f( 100.0f, 100.0f, 100.0f );
	psa->m_shader = m_shader;
	m_subSystems.Append( new vsSnowStorm( psa ) );

	// append texture
	if ( texture!=-1 )
		m_textures[PARTICLE_TYPE_SNOWFLAKE] = ( uint )texture;

	return m_subSystems.Length()-1;
}

/* 
=====================================
	RemoveBlockEffect
=====================================
*/
int vsParticleSystem::RemoveBlockEffect( const vsVec3f& origin, const vsVec3f& scale, float lifeTime, const vsVec3f& direction, uint texPackId )
{
//	if ( varStorage.Bool["disableparticles"] ) return -1;

	// create new subsystem
	particleSubsystemAttribs_t* psa = new particleSubsystemAttribs_t();
	psa->m_invLifeTime = 1.0f/lifeTime;
	psa->m_origin = origin;
	psa->m_parent = this;
	psa->m_maxNumParticles = 16;
	psa->m_scale = scale;
	psa->m_shader = m_shader;
	pstp_t* tp = GetTexturePackById( texPackId );
	psa->m_texPack = ( tp==NULL )? &m_defaultTexPack : tp;
	m_subSystems.Append( new vsExplosion1( psa, direction ) );

	return m_subSystems.Length()-1;
}


#pragma endregion