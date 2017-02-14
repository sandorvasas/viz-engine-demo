#ifndef __VS_MODEL_MD5_H__
#define __VS_MODEL_MD5_H__

#include "Material.h"
#include "..\..\vsLib2\vsLib2.h"


class vsModelMD5_t;


/* 
=====================================
	MD5Vertex_t
=====================================
*/
struct MD5Vertex_t {
	MINIMUM_OPERATORS( MD5Vertex_t );
	int		weightStart;
	int		weightCount;
	vsVec3f localNormal;
	vsVec3f localTangent;
	vsVec2f	st;
};

/* 
=====================================
	MD5Triangle_t
=====================================
*/
struct MD5Triangle_t {
	MINIMUM_OPERATORS( MD5Triangle_t );
	uint	indices[3];
};

/* 
=====================================
	MD5Weight_t
=====================================
*/
struct MD5Weight_t {
	MINIMUM_OPERATORS( MD5Weight_t );
	int		jointIndex;
	float	bias;
	vsVec3f	pos;
};

/* 
=====================================
	MD5Joint_t
=====================================
*/
struct MD5Joint_t {
	EMPTY_TRUCTORS( MD5Joint_t );
	MINIMUM_OPERATORS( MD5Joint_t );
	char			name[32];
	int				parent;
	vsVec3f			pos;
	vsQuaternion	orient;
};

/* 
=====================================
	MD5Skeleton_t
=====================================
*/
struct MD5Skeleton_t {
	MINIMUM_OPERATORS( MD5Skeleton_t );
	vsList<MD5Joint_t>	joints;

	MD5Skeleton_t( void ) {};
	~MD5Skeleton_t( void ) {joints.Free();};
};

/* 
=====================================
	MD5Mesh_t
=====================================
*/
struct MD5Mesh_t {
	friend class	vsModel_MD5;
public:
							MINIMUM_OPERATORS( MD5Mesh_t );
							MD5Mesh_t( void );
							~MD5Mesh_t( void );

	void					CalculateNormals( void );
	void					UpdateBuffers( bool justVertex = false );

	vsMaterial*				GetMaterial( void );
	void					SetMaterial( vsMaterial* material );

private:
	vsMaterial*				material;
	vsList<MD5Triangle_t>	triangles;
	vsList<MD5Vertex_t>		vertices;
	vsList<MD5Weight_t>		weights;
	vsList<vsVertex>		finalVertices;
	uint					buffers[2];				// 0: vertex 1: index

private:
	bool					buffersGenerated;
};


VS_INLINE vsMaterial* MD5Mesh_t::GetMaterial( void ) {
	return material;
}

VS_INLINE void MD5Mesh_t::SetMaterial( vsMaterial* material ) {
	this->material = material;
}

/*
=====================================
	MD5Frame_t
=====================================
*/
struct MD5Frame_t {
	MINIMUM_OPERATORS( MD5Frame_t );
	vsBox			bounds;
	vsList<float>	components;
	MD5Skeleton_t	skeleton;

	MD5Frame_t( void ) {};
	~MD5Frame_t( void ) { components.Free(); };
};


/*
=====================================
	MD5AnimJoint_t
=====================================
*/
struct MD5AnimJoint_t {
	EMPTY_TRUCTORS( MD5AnimJoint_t );
	MINIMUM_OPERATORS( MD5AnimJoint_t );
	char	name[32];
	int		parentIndex;
	int		flags;
	int		startIndex;
};

/* 
=====================================

	vsAnimation_MD5

=====================================
*/
class vsAnimation_MD5 {

	friend class vsModel_MD5;
	friend class vsModelManager;

public:					
							vsAnimation_MD5( void );
							~vsAnimation_MD5( void );
							MINIMUM_OPERATORS( vsAnimation_MD5 );
	
	const MD5Skeleton_t*	GetFrameSkeleton( int frameNum ) const;
	const float&			GetLength( void ) const;
	uint					GetFrameCount( void ) const;
	const vsVec3f&			GetTotalDelta( void ) const;
	vsVec3f					GetDelta( int frameNum ) const;

	int						LoadFromFile( const char* szFileName );

public:
	void					BuildFrameSkeleton( int frameNum );

private:
	char*					filename;

	byte					version;
	bool					loaded;
	int						numAnimatedComponents;
	float					length;
	vsList<MD5AnimJoint_t>	jointInfos;
	vsList<MD5Frame_t>		frames;
	MD5Skeleton_t			baseFrameSkeleton;
	byte					frameRate;
	vsVec3f					totalDelta;

	uint					id;			// id to modelmanager
};


/* 
=====================================
	AnimateToTime
=====================================
*/
VS_INLINE const MD5Skeleton_t* vsAnimation_MD5::GetFrameSkeleton( int frameNum ) const {
	return &frames[frameNum].skeleton;
}

/* 
=====================================
	GetFrameCount
=====================================
*/
VS_INLINE uint vsAnimation_MD5::GetFrameCount( void ) const {
	return frames.Length();
}

/* 
=====================================
	GetTotalDelta
=====================================
*/
VS_INLINE const vsVec3f& vsAnimation_MD5::GetTotalDelta( void ) const {
	return totalDelta;
}

/* 
=====================================
	GetDelta
=====================================
*/
VS_INLINE vsVec3f vsAnimation_MD5::GetDelta( int frameNum ) const
{
	vsMath::Clamp<int>( frameNum, 0, frames.Length()-2 );

	return frames[frameNum+1].skeleton.joints[0].pos - frames[frameNum].skeleton.joints[0].pos;
}

/* 
=====================================
	GetLength
=====================================
*/
VS_INLINE const float& vsAnimation_MD5::GetLength( void ) const 
{
	return length;
}



/*
=====================================
	MD5AnimPlayModeFlag_t
=====================================
*/
enum MD5AnimPlayModeFlag_t {
	ANIM_PLAY_WAIT			= 0x000001,
	ANIM_PLAY_IMMEDIATELY	= 0x000010,
	ANIM_PLAY_REPEAT		= 0x000100,
	ANIM_PLAY_ONCE			= 0x001000
};

/*
=====================================
	MD5ModelArgs_t
=====================================
*/
struct MD5ModelArgs_t {
	EMPTY_TRUCTORS( MD5ModelArgs_t );
	MINIMUM_OPERATORS( MD5ModelArgs_t );
	char*		szFileName;
	char*		szMaterialFileName;
	vsShader*	shader;
	vsVec3f		scale;
	vsVec3f		offset;
	vsVec3f		rotation;
};

/* 
=====================================

	vsModel_MD5

=====================================
*/
class vsModel_MD5 {

	friend class vsModelManager;

public:
							vsModel_MD5( void );
							~vsModel_MD5( void );

	int						LoadFromFile( const MD5ModelArgs_t& args );
	int						AddAnimation( vsAnimation_MD5 * anim );
	float					GetCurrentAnimationTime( void );
	bool					AnimationFinished( void );
	void					SetAnimation( const vsStr& animName );
	void					SetAnimation( short id, int mode );
	void					RenderAnimated( void );
	void					RenderBindpose( void );
	void					RenderSkeleton( void );
	void					Animate( float deltaTime );
	const bool				HasAnimation( void ) const;


	vsBox					GetBounds( void ) const;
	vsVec3f					GetDelta( void ) const;
	const vsVec3f&			GetOffset( void ) const;
	const vsVec3f&			GetScale( void ) const;
	const vsQuaternion&		GetRotation( void ) const;
	const vsVec3f&			GetDirection( void ) const;
	const uint&				GetId( void ) const;

	void					SetOffset( const vsVec3f& off );
	void					SetScale( const vsVec3f& sca );
	void					SetRotation( const vsQuaternion& q );
	void					SetRotation( const vsVec3f& v );

	void					Translate( const vsVec3f& t );
	void					Rotate( const vsQuaternion& q );
	void					Rotate( const vsVec3f& r );
	void					Scale( const vsVec3f& s );

private:
	void					BuildModel( MD5Skeleton_t* skeleton );
	void					Render( void );
	void					CalculateLocalNormals( MD5Skeleton_t* skeleton );

private:
	char*					filename;

	byte					version;
	bool					loaded;
	vsList<MD5Mesh_t*>		meshes;
	vsShader*				shader;
	vsBox					bounds;

	MD5Skeleton_t			bindposeSkeleton;
	vsList<vsAnimation_MD5*>animations;
	float					currentAnimTime;
	MD5Skeleton_t			currentSkeleton;
	int						currentAnimPlayMode;
	int						nextAnimPlayMode;

	short					currentAnimation;
	short					nextAnimation;

	vsVec3f					scale;
	vsVec3f					offset;
	vsQuaternion			rotation;
	vsVec3f					direction;

	uint					id;		// id to modelmanager
};


/*
=====================================
	HasAnimation
=====================================
*/
VS_INLINE const bool vsModel_MD5::HasAnimation( void ) const {
	return animations.Length() > 0;
}

/*
=====================================
	GetDelta
=====================================
*/
VS_INLINE vsVec3f vsModel_MD5::GetDelta( void ) const
{
	if ( currentAnimation < 0 ) 
		return vsVec3f();

	return animations[currentAnimation]->GetDelta( ( int )floorf( currentAnimTime * animations[currentAnimation]->frameRate ) );
}

VS_INLINE const vsVec3f& vsModel_MD5::GetOffset( void ) const {
	return offset;
}

VS_INLINE const vsVec3f& vsModel_MD5::GetScale( void ) const {
	return scale;
}

VS_INLINE const vsQuaternion& vsModel_MD5::GetRotation( void ) const {
	return rotation;
}

VS_INLINE const vsVec3f& vsModel_MD5::GetDirection( void ) const {
	return direction;
}

VS_INLINE const uint& vsModel_MD5::GetId( void ) const {
	return id;
}

VS_INLINE void vsModel_MD5::SetOffset( const vsVec3f& off ) {
	offset = off;
}

VS_INLINE void vsModel_MD5::SetScale( const vsVec3f& sca ) {
	scale = sca;
}

VS_INLINE void vsModel_MD5::SetRotation( const vsQuaternion& q ) {
	rotation = q;
}

VS_INLINE void vsModel_MD5::SetRotation( const vsVec3f& v ) {
	rotation.CreateFromEulerAnglesDEG( v );
}

VS_INLINE void vsModel_MD5::Translate( const vsVec3f& t ) {
	offset += t;
}

VS_INLINE void vsModel_MD5::Rotate( const vsQuaternion& q ) {
	rotation *= q;
}

VS_INLINE void vsModel_MD5::Rotate( const vsVec3f& r ) {
	vsQuaternion q;
	q.CreateFromEulerAnglesDEG( r );
	rotation *= q;
}

VS_INLINE void vsModel_MD5::Scale( const vsVec3f& s ) {
	scale += s;
}


#endif