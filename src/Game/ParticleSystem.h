#ifndef __VS_PARTICLE_SYSTEM_H__
#define __VS_PARTICLE_SYSTEM_H__


#include "..\vsLib2\vsLib2.h"

class vsPlayer;
class vsParticleSubsystem;
class vsParticleSystem;


/*
==================================
	particleSystemTexPack_t
==================================
*/
typedef struct particleSystemTexPack_s {
	MINIMUM_OPERATORS( particleSystemTexPack_s );
	uint		id;
	uint	textures[5];
} particleSystemTexPack_t, pstp_t;


/* 
=====================================
	particle_t
=====================================
*/
struct particle_t {
	vsVec3f		m_origin;	// stored in buffer, offset: 0
	vsVec4f		m_color;	// stored in buffer, offset: 12
	vsVec3f		m_velocity;
	vsVec2f		m_size;// fixme: make it float
	byte		m_type;		// holds texture unit to use in shader
	float		m_distance;

	MINIMUM_OPERATORS( particle_t );
	// distance comparison for transparency sorting
	bool operator > ( const particle_t& r ) { return m_distance>r.m_distance; };
	bool operator < ( const particle_t& r ) { return m_distance<r.m_distance; };
	bool operator >= ( const particle_t& r ) { return m_distance>=r.m_distance; };
	bool operator <= ( const particle_t& r ) { return m_distance<=r.m_distance; };
};


/* 
=====================================
	particleSubSystemType_t
=====================================
*/
enum particleType_t {
	// Snowstorm
	PARTICLE_TYPE_SNOWFLAKE = 0,

	// Explosions
	PARTICLE_TYPE_SMOKE = 0,
	PARTICLE_TYPE_FIRE = 1,
	PARTICLE_TYPE_DUST = 3,

	// Misc
	PARTICLE_TYPE_SMALL = 0
};


/* 
=====================================
	particleSubsystemAttribs_t
		~ 22 bytes
=====================================
*/
struct particleSubsystemAttribs_t {
	float				m_invLifeTime;
	vsVec3f				m_scale;
	vsVec3f				m_origin;
	vsParticleSystem*	m_parent;
	ushort				m_maxNumParticles;
	vsShader*			m_shader;
	pstp_t*				m_texPack;
};

/*
==================================

	vsParticleSubsystem

	Warning note: 
		At Init()&constructor: 
			Contents of "attribs" aren't copied, it has to exist in memory
			until the particle system is dead.
==================================
*/
class vsParticleSubsystem {
	friend class vsParticleSystem;
public:
	MINIMUM_OPERATORS( vsParticleSubsystem );

					vsParticleSubsystem( void ) {};
					vsParticleSubsystem( particleSubsystemAttribs_t* attribs );
	virtual			~vsParticleSubsystem( void );

	virtual void	UpdatePhysics( void );
	virtual bool	IsDead( void ) { return( m_attribs==NULL ); };
	virtual void	Render( void );
	virtual void	InitSubsystem( particleSubsystemAttribs_t* attribs );
	virtual byte	BindTexturesAndUniformToShader( void ) = 0;	//returns num textures
	virtual void	SetupPointParameters( void ) = 0;

protected:
	virtual void	Emit( byte type, const vsVec3f& origin, const vsVec3f& velocity, const vsVec4f& color );
	virtual void	DeleteDeadParticles( void );
	virtual void	SortParticlesByDistance( const vsVec3f& respectedPoint );
	virtual void	UpdateBuffer(  byte which, bool updateWhole=true, ushort offset=0 );

protected:
	vsBox						m_bounds;
	ushort						m_numParticles;
	vsList<particle_t>			m_particles;
	vsList<ushort>				m_indices;
	particleSubsystemAttribs_t* m_attribs;
	uint						m_buffers[2];
	vsVec3f						m_lastPlayerPos;
	float						m_spriteSize;
	float						m_distance;
};

/*
===============================
	PARTICLESUBSYSTEM_CHILD
===============================
*/
#define PARTICLESUBSYSTEM_CHILD( class_name )					\
	class class_name : public vsParticleSubsystem {			\
	public:													\
		MINIMUM_OPERATORS( class_name );						\
		class_name( particleSubsystemAttribs_t* attribs )   \
			{ InitSubsystem( attribs ); Init();};	\
		void		Init( void );							\
		byte		BindTexturesAndUniformToShader( void ); \
		void		SetupPointParameters( void );			
	

/*
==================================

	vsSnowStorm

==================================
*/
PARTICLESUBSYSTEM_CHILD( vsSnowStorm )

	void		SetWind( const vsVec3f& wind );
	void		UpdatePhysics( void );

private:
	vsVec3f		m_wind;
};

/*
==================================

	vsExplosion1
	 ( rocket explosion when rocket hits something )
==================================
*/
PARTICLESUBSYSTEM_CHILD( vsExplosion1 )
	vsExplosion1( particleSubsystemAttribs_t* attribs, const vsVec3f& dir ) { 
					InitSubsystem( attribs ); 
					SetDirection( dir );
					Init();
				};
	void		SetDirection( const vsVec3f& dir ) { m_direction = dir; }

private:
	vsVec3f		m_direction;
};


/*
==================================
	vsRemoveBlockEffect
==================================
*/
PARTICLESUBSYSTEM_CHILD( vsRemoveBlockEffect )
	vsRemoveBlockEffect( particleSubsystemAttribs_t* attribs, const vsVec3f& dir ) { 
					InitSubsystem( attribs ); 
					SetDirection( dir );
					Init();
					time=0.0f;
				};
	void		SetDirection( const vsVec3f& dir ) { m_direction = dir; }
	void		UpdatePhysics( void );
private:
	vsVec3f		m_direction;
	float		time;
};


/*
==================================

	vsParticleSystem

==================================
*/
class vsParticleSystem {
	friend class vsParticleSubsystem;
public:
					vsParticleSystem( void );
					vsParticleSystem( vsShader* shader );
					~vsParticleSystem( void );

	void		Init( vsShader* shader );
	void		UpdatePhysics( void );
	void		Render( void );
	uint		GetTexture( uint index );
	uint		AddTexturePack( const pstp_t& pack );
	pstp_t*		GetTexturePackById( uint id );

	// EFFECTS - return value: id of effect ( index to m_subSystems )
	int			Explosion1( const vsVec3f& origin, const vsVec3f& scale, float lifeTime, const vsVec3f& direction, uint texPackId );
	int			SnowStorm( const vsVec3f& wind, int texture );
	int			RemoveBlockEffect( const vsVec3f& origin, const vsVec3f& scale, float lifeTime, const vsVec3f& direction, uint texPackId );

protected:
	void		SortSubsystems( void );				// sorts by distance from player

protected:
	pstp_t							m_defaultTexPack;
	vsList<vsParticleSubsystem*>	m_subSystems;
	vsList<pstp_t>					m_texPacks;
	vsList<uint>					m_textures;
	vsShader*						m_shader;
};


/* 
=====================================
	GetTexture
=====================================
*/
VS_INLINE
uint vsParticleSystem::GetTexture( uint index )
{
	return m_textures[index];
}

/* 
=====================================
	SetTexturePack
=====================================
*/
VS_INLINE
uint vsParticleSystem::AddTexturePack( const pstp_t& pack )
{
	REPU( m_texPacks.Length() )
		if ( m_texPacks[i].id == pack.id )
			return i;

	m_texPacks.Append( pack );
	return pack.id;
}

/* 
=====================================
	GetTexturePack
=====================================
*/
VS_INLINE
pstp_t* vsParticleSystem::GetTexturePackById( uint id )
{
	REPU( m_texPacks.Length() )
		if ( m_texPacks[i].id == id )
			return &m_texPacks[i];
	return NULL;
}

#endif