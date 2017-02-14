#ifndef __VS_MD5MESH_H__
#define __VS_MD5MESH_H__


#include "../../System/String.h"
#include "../../System/List.h"
#include "../../System/Console.h"
#include "../../Math/Math.h"
#include "../../Boundings/AABB.h"
#include "../../Boundings/Frustum.h"
#include "../../Renderer/RenderSystem.h"
#include "../../Renderer/Shader.h"
#include "MD5Anim.h"
#include "Model.h"
//#include <stdio.h>

class vsMD5Anim;

//=====================================
//	MD5Material_t
//=====================================
typedef struct MD5Material_s {
	vsStr			name;
	vsVec4f			diffuse;
	vsVec4f			ambient;
	vsVec4f			specular;
	float			shininess;
	unsigned int	diffusemap;
	unsigned int	normalmap;
	unsigned int	specularmap;
	vsShader*		shader;

	MD5Material_s( void );
	~MD5Material_s( void ) {};
	MD5Material_s&	operator=( const MD5Material_s& b );
	bool			operator==( const MD5Material_s& b );
	bool			operator!=( const MD5Material_s& b );
} MD5Material_t;

//=====================================
//	MD5Vertex_t
//=====================================
typedef struct MD5Vertex_s {
	vsVec3f			finalPosition;
	vsVec3f			normal;
	vsVec3f			tangent;
	vsVec2f			st;
	unsigned int	wStartIndex;
	unsigned int	wCount;

	MD5Vertex_s&	operator=( const MD5Vertex_s& b );
	bool			operator==( const MD5Vertex_s& b );
	bool			operator!=( const MD5Vertex_s& b );
} MD5Vertex_t;



//=====================================
//	MD5Triangle_t
//=====================================
typedef struct MD5Triangle_s {
	unsigned int	vertexIndices[3];

	MD5Triangle_s&	operator=( const MD5Triangle_s& b );
	bool			operator==( const MD5Triangle_s& b );
	bool			operator!=( const MD5Triangle_s& b );
} MD5Triangle_t;

//=====================================
//	MD5Weight_t
//=====================================
typedef struct MD5Weight_s {
	int				joint;
	float			bias;
	vsVec3f			position;

	MD5Weight_s&	operator=( const MD5Weight_s& b );
	bool			operator==( const MD5Weight_s& b );
	bool			operator!=( const MD5Weight_s& b );
} MD5Weight_t;

//=====================================
//	MD5Mesh_t
//=====================================
typedef struct MD5Mesh_s {
	vsList<MD5Vertex_t>		vertices;
	vsList<MD5Triangle_t>	triangles;
	vsList<MD5Weight_t>		weights;
	MD5Material_t*			material;

	MD5Mesh_s( void ) : material(NULL) {};
	~MD5Mesh_s( void ) {};
	MD5Mesh_s&				operator=( const MD5Mesh_s& b );
	bool					operator==( const MD5Mesh_s& b );
	bool					operator!=( const MD5Mesh_s& b );
} MD5Mesh_t;

//=====================================
//
// vsMD5Model
//
//=====================================
class vsMD5Model : public vsIModel {
	friend class vsMD5Anim;
public:
							vsMD5Model( void );
							~vsMD5Model( void );

	vsMD5Model&				operator=( const vsMD5Model& b );
	bool					operator==( const vsMD5Model& b );
	bool					operator!=( const vsMD5Model& b );

	int						Load( const modelLoadArgs_t& args );
	int						AddAnimation( vsMD5Anim* anim );
	void					UpdateAnimation( void );
	void					SetCurrentAnimation( unsigned int animIndex );
	void					BuildBindPoseSkeleton( void );
	void					Render( void );
	void					CalculateBasis( void );
	void					CalculateAnimatedBasis( void );

	void					SetOrigin( const vsVec3f& newOrigin );
	void					SetAxis( const vsMat3& newAxis );
	void					Scale( const vsVec3f& s );
	void					Translate( const vsVec3f& t );
	void					Rotate( const vsMat3& t );
	void					RenderBasis( void );

private:
	int						LoadMaterialLibrary( const vsStr& materialName );	// returns index of material
	MD5Material_t*			SearchForMaterial( const vsStr& materialName );

private:
	vsStr					filename;
	vsStr					matlib;
	vsList<MD5Material_t>	materials;
	vsList<MD5Mesh_t>		meshes;
	vsList<MD5Joint_t>		joints;
	vsMD5Anim*				currentAnimation;
	vsList<vsMD5Anim*>		animations;

private:
	vsVec3f					tmpOrigin;		// just used in SetOrigin() 
	vsMat3					tmpAxis;		// just used in SetAxis() 
};




/*
==============================
	MD5Material_s::MD5Material_s
==============================
*/
VS_INLINE MD5Material_s::MD5Material_s( void ) {
	this->diffuse		= vsVec4f( 1.0f );
	this->shininess		= 1.0f;
	this->diffusemap	= NULL;
	this->normalmap		= NULL;
	this->specularmap	= NULL;
	this->shader		= NULL;
}

/*
==============================
	MD5Material_s:: =
==============================
*/
VS_INLINE MD5Material_s& MD5Material_s::operator=( const MD5Material_s& b ) {
	this->specular		= b.specular;
	this->ambient		= b.ambient;
	this->diffuse		= b.diffuse;
	this->name			= b.name;
	this->diffusemap	= b.diffusemap;
	this->normalmap		= b.normalmap;
	this->specularmap	= b.specularmap;
	this->shininess		= b.shininess;
	this->shader		= b.shader;
	return( *this );
}

/*
==============================
	MD5Material_s:: ==
==============================
*/
VS_INLINE bool MD5Material_s::operator==( const MD5Material_s& b ) {
	return (
	this->specular		== b.specular &&
	this->ambient		== b.ambient &&
	this->diffuse		== b.diffuse &&
	this->name			== b.name &&
	this->diffusemap	== b.diffusemap &&
	this->normalmap		== b.normalmap &&
	this->specularmap	== b.specularmap &&
	this->shininess		== b.shininess &&
	this->shader		== b.shader );
}

/*
==============================
	MDMaterial_s:: !=
==============================
*/
VS_INLINE bool MD5Material_s::operator!=( const MD5Material_s& b ) {
	return!(*this==b);
}

/*
==============================
	MD5Vertex_s:: =
==============================
*/
VS_INLINE MD5Vertex_s& MD5Vertex_s::operator=( const MD5Vertex_s& b ) {
	this->finalPosition	= vsVec3f( 0.0f );
	this->normal		= vsVec3f( 0.0f );
	this->tangent		= vsVec3f( 0.0f );
	this->st			= b.st;
	this->wCount		= b.wCount;
	this->wStartIndex	= b.wStartIndex;
	return( *this );
}

/*
==============================
	MD5Vertex_s:: ==
==============================
*/
VS_INLINE bool MD5Vertex_s::operator==( const MD5Vertex_s& b ) {
	return(
	this->finalPosition	== b.finalPosition &&
	this->normal		== b.normal &&
	this->tangent		== b.tangent &&
	this->st			== b.st &&
	this->wCount		== b.wCount &&
	this->wStartIndex	== b.wStartIndex );
}

/*
==============================
	MD5Vertex_s:: !=
==============================
*/
VS_INLINE bool MD5Vertex_s::operator!=( const MD5Vertex_s& b ) {
	return!(*this==b);
}

/*
==============================
	MD5Triangle_s:: =
==============================
*/
VS_INLINE MD5Triangle_s& MD5Triangle_s::operator=( const MD5Triangle_s& b ) {
	this->vertexIndices[0]	= b.vertexIndices[0];
	this->vertexIndices[1]	= b.vertexIndices[1];
	this->vertexIndices[2]	= b.vertexIndices[2];
	return( *this );
}

/*
==============================
	MD5Triangle_s:: ==
==============================
*/
VS_INLINE bool MD5Triangle_s::operator==( const MD5Triangle_s& b ) {
	return(
	this->vertexIndices[0]	== b.vertexIndices[0] &&
	this->vertexIndices[1]	== b.vertexIndices[1] &&
	this->vertexIndices[2]	== b.vertexIndices[2] );
}

/*
==============================
	MD5Triangle_s:: !=
==============================
*/
VS_INLINE bool MD5Triangle_s::operator!=( const MD5Triangle_s& b ) {
	return!(*this==b);
}

/*
==============================
	MD5Weight_s:: =
==============================
*/
VS_INLINE MD5Weight_s& MD5Weight_s::operator=( const MD5Weight_s& b ) {
	this->bias		= b.bias;
	this->position	= b.position;
	this->joint		= b.joint;
	return( *this );
}

/*
==============================
	MD5Weight_s:: ==
==============================
*/
VS_INLINE bool MD5Weight_s::operator==( const MD5Weight_s& b ) {
	return(
	this->bias		== b.bias &&
	this->position	== b.position &&
	this->joint		== b.joint );
}

/*
==============================
	MD5Weight_s:: !=
==============================
*/
VS_INLINE bool MD5Weight_s::operator!=( const MD5Weight_s& b ) {
	return!(*this==b);
}

/*
==============================
	MD5Mesh_s:: =
==============================
*/
VS_INLINE MD5Mesh_s& MD5Mesh_s::operator=( const MD5Mesh_s& b ) {
	this->vertices		= b.vertices;
	this->triangles		= b.triangles;
	this->material		= b.material;
	this->weights		= b.weights;
	return( *this );
}

/*
==============================
	MD5Mesh_s:: ==
==============================
*/
VS_INLINE bool MD5Mesh_s::operator==( const MD5Mesh_s& b ) {
	return(
	this->vertices		== b.vertices &&
	this->triangles		== b.triangles &&
	this->material		== b.material &&
	this->weights		== b.weights );
}

/*
==============================
	MD5Mesh_s:: !=
==============================
*/
VS_INLINE bool MD5Mesh_s::operator!=( const MD5Mesh_s& b ) {
	return!(*this==b);
}

/*
==============================
	vsMD5Model:: =
==============================
*/
VS_INLINE vsMD5Model& vsMD5Model::operator=( const vsMD5Model& b ) {
	this->animations		= b.animations;
	this->joints			= b.joints;
	this->currentAnimation	= b.currentAnimation;
	this->meshes			= b.meshes;
	this->materials			= b.materials;
	this->tmpOrigin			= b.tmpOrigin;
	this->tmpAxis			= b.tmpAxis;
	return( *this );
}

/*
==============================
	vsMD5Model:: ==
==============================
*/
VS_INLINE bool vsMD5Model::operator==( const vsMD5Model& b ) {
	return(
	this->animations		== b.animations &&
	this->joints			== b.joints &&
	this->currentAnimation	== b.currentAnimation &&
	this->meshes			== b.meshes &&
	this->materials			== b.materials &&
	this->tmpOrigin			== b.tmpOrigin &&
	this->tmpAxis			== b.tmpAxis );
}

/*
==============================
	vsMD5Model:: !=
==============================
*/
VS_INLINE bool vsMD5Model::operator!=( const vsMD5Model& b ) {
	return!(*this==b);
}

#endif