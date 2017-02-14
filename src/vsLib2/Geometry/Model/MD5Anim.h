#ifndef __VS_MD5ANIM_H__
#define __VS_MD5ANIM_H__


#include "../../System/String.h"
#include "../../System/List.h"
#include "../../System/Console.h"
#include "../../Math/Math.h"
#include "../../Boundings/AABB.h"
#include "../../Boundings/Frustum.h"
#include "../../Renderer/RenderSystem.h"
#include "../../Renderer/Shader.h"

class vsMD5Model;

//=====================================
//	MD5Joint_t
//=====================================
typedef struct MD5Joint_s {
	vsStr			name;
	int				parentIndex;
	vsVec3f			position;
	vsQuaternion	orientation;

	MD5Joint_s&		operator=( const MD5Joint_s& b );
	bool			operator==( const MD5Joint_s& b );
	bool			operator!=( const MD5Joint_s& b );
} MD5Joint_t;

/*
===============================
	MD5AnimJoint_s
===============================
*/
typedef struct MD5AnimJoint_s {
	vsStr			name;
	int				parent;
	int				flags;
	int				startIndex;

	MD5AnimJoint_s&	operator=( const MD5AnimJoint_s& b );
	bool			operator==( const MD5AnimJoint_s& b );
	bool			operator!=( const MD5AnimJoint_s& b );
} MD5AnimJoints_t;

/*
===============================
	MD5Frame_t
===============================
*/
typedef struct MD5Frame_s {
	vsBox				bounds;
	vsList<MD5Joint_t>	skeleton;

	MD5Frame_s&			operator=( const MD5Frame_s& b );
	bool				operator==( const MD5Frame_s& b );
	bool				operator!=( const MD5Frame_s& b );
} MD5Frame_t;

/*
===============================
	vsMD5Anim
===============================
*/
class vsMD5Anim {
	friend class vsMD5Mesh;
public:
	vsMD5Anim( void );
	~vsMD5Anim( void );

	vsMD5Anim&				operator=( const vsMD5Anim& b );
	bool					operator==( const vsMD5Anim& b );
	bool					operator!=( const vsMD5Anim& b );

	int						LoadAnim( const vsStr& filename );
	//int						Update( void );
	void					SetModel( vsMD5Model* pModel );
	vsMD5Model*				GetModel( void );

private:
	int						numAnimatedComponents;
	int						numJoints;
	int						frameRate;
	int						currentFrame;
	vsList<MD5Frame_t>		frames;
	vsMD5Model*				pModel;
};



/*
==============================
	MD5Joint_s:: =
==============================
*/
VS_INLINE MD5Joint_s& MD5Joint_s::operator=( const MD5Joint_s& b ) {
	this->name			= b.name;
	this->orientation	= b.orientation;
	this->parentIndex	= b.parentIndex;
	this->position		= b.position;
	return( *this );
}

/*
==============================
	MD5Joint_s:: ==
==============================
*/
VS_INLINE bool MD5Joint_s::operator==( const MD5Joint_s& b ) {
	return(
	this->name			== b.name &&
	this->orientation	== b.orientation &&
	this->parentIndex	== b.parentIndex &&
	this->position		== b.position );
}

/*
==============================
	MD5Joint_s:: !=
==============================
*/
VS_INLINE bool MD5Joint_s::operator!=( const MD5Joint_s& b ) {
	return!(*this==b);
}

/*
===============================
	=
===============================
*/
VS_INLINE MD5AnimJoint_s& MD5AnimJoint_s::operator=( const MD5AnimJoint_s& b ) 
{
	this->name			= b.name;
	this->flags			= b.flags;
	this->parent		= b.parent;
	this->startIndex	= b.startIndex;
	return( *this );
}

/*
===============================
	==
===============================
*/
VS_INLINE bool MD5AnimJoint_s::operator==( const MD5AnimJoint_s& b )
{
	return(
	this->name			== b.name &&
	this->flags			== b.flags &&
	this->parent		== b.parent &&
	this->startIndex	== b.startIndex );
}

/*
===============================
	!=
===============================
*/
VS_INLINE bool MD5AnimJoint_s::operator!=( const MD5AnimJoint_s& b )
{
	return!(*this == b);
}

/*
===============================
	=
===============================
*/
VS_INLINE MD5Frame_s& MD5Frame_s::operator=( const MD5Frame_s& b )
{
	bounds		= b.bounds;
	skeleton	= b.skeleton;
	return(*this);
}

/*
===============================
	==
===============================
*/
VS_INLINE bool MD5Frame_s::operator==( const MD5Frame_s& b )
{
	return(
	this->bounds == b.bounds &&
	this->skeleton == b.skeleton );
}

/*
===============================
	!=
===============================
*/
VS_INLINE bool MD5Frame_s::operator!=( const MD5Frame_s& b )
{
	return!(*this == b);	
}

/*
===============================
	=
===============================
*/
VS_INLINE vsMD5Anim& vsMD5Anim::operator=( const vsMD5Anim& b )
{
	this->currentFrame			= b.currentFrame;
	this->frames				= b.frames;
	this->frameRate				= b.frameRate;
	this->numAnimatedComponents	= b.numAnimatedComponents;
	this->numJoints				= b.numJoints;
	return( *this );
}

/*
===============================
	==
===============================
*/
VS_INLINE bool vsMD5Anim::operator==( const vsMD5Anim& b ) 
{
	return(
	this->currentFrame			== b.currentFrame &&
	this->frames				== b.frames &&
	this->frameRate				== b.frameRate &&
	this->numAnimatedComponents	== b.numAnimatedComponents &&
	this->numJoints				== b.numJoints );
}

/*
===============================
	!=
===============================
*/
VS_INLINE bool vsMD5Anim::operator!=( const vsMD5Anim& b )
{
	return!(*this == b);
}

/*
===============================
	MD5Anim::SetModel()
===============================
*/
VS_INLINE void vsMD5Anim::SetModel( vsMD5Model* pModel )
{
	this->pModel = pModel;
}

/*
===============================
	MD5Anim::GetModel()
===============================
*/
VS_INLINE vsMD5Model* vsMD5Anim::GetModel( void )
{
	return( pModel );
}

#endif