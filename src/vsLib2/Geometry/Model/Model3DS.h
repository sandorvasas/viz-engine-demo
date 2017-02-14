
/*
 FIXME:
 Implement OBJTYPE_LIGHT:
	 . set type to OBJTYPE_LIGHT if it is one
     . render a light if type==OBJTYPE_LIGHT
*/
#ifndef __VS_MODEL3DS_H__
#define __VS_MODEL3DS_H__

#include "..\..\Math\Math.h"
#include "..\..\System\FileManager.h"
#include "..\..\System\List.h"
#include "..\..\System\Flag.h"
#include "..\..\System\String.h"
#include "..\..\Renderer\OpenGL\GLexp.h"
#include "..\..\Renderer\Shader.h"
#include "..\..\Renderer\Texture.h"
#include "..\..\Boundings\Frustum.h"
#include "..\..\Boundings\AABB.h"
#include "..\..\Misc\Light.h"
#include "..\Vertex.h"
#include "Model.h"

class vs3DSModel;

// flags
const unsigned int MODEL_FLAG_RENDERBASIS	= 0x00000001;
const unsigned int MODEL_FLAG_NOSHADER		= 0x00000010;
const unsigned int MODEL_FLAG_NOTEXTURE		= 0x00000100;
const unsigned int MODEL_FLAG_WIREFRAME		= 0x00001000;
const unsigned int MODEL_FLAG_COLLMODEL     = 0x00010000;
const unsigned int MODEL_FLAG_RENDERBOUNDS  = 0x00100000;

// Level of Detail
const unsigned int MODEL_LOD = 1;


#define MODEL_MAX_LIGHTS 256

/*
==================================
	Texture3DS_t
==================================
*/
struct Texture3DS_t {
	Texture3DS_t( void ) : id( 0 ) {};
	~Texture3DS_t( void ) {};
	Texture3DS_t&	operator=( const Texture3DS_t& t );

	char			FileName[ 128 ];
	float			Uoffset;
	float			Voffset;
	unsigned int	id;
};

/*
==================================
	Material3DS_t
==================================
*/
struct Material3DS_t {
	Material3DS_t( void ) {};
	~Material3DS_t( void ) {};
	Material3DS_t&	operator=( const Material3DS_t& m );

	char			Name[20];
	Texture3DS_t	tex1;
	Texture3DS_t	tex2;
	Texture3DS_t	normalmap;
	Texture3DS_t	specularmap;
	vsVec3f			diffuse;			// all properties are normalized
	vsVec3f			specular;
	vsVec3f			ambient;
	float			shininess;
	float			transparency;
};

/*
==================================
	Triangle3DS_t
==================================
*/
struct Triangle3DS_t {
	Triangle3DS_t( void ) {};
	~Triangle3DS_t( void ) {};
	Triangle3DS_t&		operator=( const Triangle3DS_t& t );

	unsigned short		a, b, c;
};


enum  {
	MODEL_OBJTYPE_MESH = 0,
	MODEL_OBJTYPE_LIGHT
};

/*
==================================
	Object3DS_t
==================================
*/
struct Object3DS_t {
							Object3DS_t( void );
							~Object3DS_t( void );

	Object3DS_t&			operator=( const Object3DS_t& o );
	void					CalculateBounds( void );

	char					Name[20];
	int						type;
	vsList<vsVertex>		Vertices;
	vsList<Triangle3DS_t>	Triangles;
	Material3DS_t*			Material;
	vsBox					bounds;
};

/*
==================================
	vs3DSModel
==================================
*/
class vs3DSModel : public vsIModel {
public: 
							vs3DSModel( void );
	virtual					~vs3DSModel( void );

	virtual int				Load( const modelLoadArgs_t& args ); 
	void					UpdateAnimation( void );

	virtual void			Render( void );
	void					RenderObject( Object3DS_t * obj  );
	void					RenderBasis( Object3DS_t * obj );
	void					RenderBasis( void );

	void					SetOrigin( const vsVec3f& newOrigin );
	void					SetAxis( const vsMat3& newAxis );
	void					Scale( const vsVec3f& v );					
	void					Translate( const vsVec3f& v );	
	void					Rotate( const vsMat3& r );

	virtual void			CalculateNormals( void );
	void					SwapNormals( void );
	void					UseShader( vsShader* shader );
	void					SetViewFrustum( vsFrustum * Frustum );
	vs3DSModel&				operator=( const vs3DSModel& m );

protected:
	void					ReadMainChunk( void );
	void					ReadEditChunk( void );
	void					ReadObjectChunk( void );
	void					ReadTrimeshChunk( void );
	void					ReadVerticesChunk( void );
	void					ReadFacesChunk( void );
	void					ReadFacesMaterialChunk( void );
	void					ReadLightChunk( void );
	void					ReadTexCoordsChunk( void );
	void					ReadMaterialChunk( void );
	Texture3DS_t			ReadTextureChunk( void );
	vsVec3f					ReadRGBColorChunk( void );
	vsVec3f					ReadTrueColorChunk( void );
	vsVec3f					ReadColorChunk( void );
	unsigned int			ReadChunkLength( void );
	void					ReadUnknownChunk( void );
	Material3DS_t*			SearchForMaterial( char* szMaterialName );

	void					ReadKeyframerChunk( void );

	void					Rotate90Model( void );
	void					CalculateBounds( void );

public:
	vsStr					path;
	vsVec3f					v3Scale;
	vsVec3f					v3Position;
	vsMat3					m3Rotation;
	vsList<Material3DS_t>	materials;
	vsList<Object3DS_t>		objects;
	bool					loaded;
	vsInputFile				File;
	vsStr					texDir;
	vsShader*				shader;
	vsFrustum*				Frustum;
	vsList<vsLight>			lights;	// also can be used as a pointer to another model's light array
	vsBox					boundingBox;
public:
	vsFlag					renderFlags;

private:
	vsVec3f					tmpOrigin;		// just used in SetOrigin() 
	vsMat3					tmpAxis;		// just used in SetAxis() 
};


inline 
Triangle3DS_t& Triangle3DS_t::operator=( const Triangle3DS_t& t ) {
	a = t.a;	b = t.b;	 c = t.c;
	return *this;
}

inline
Texture3DS_t& Texture3DS_t::operator=( const Texture3DS_t& t ) {
	for ( int i=0; i < 20; i++ )
		FileName[ i ] = t.FileName[ i ];
	id = t.id;
	Uoffset = t.Uoffset;
	Voffset = t.Voffset;
	return *this;
}

inline
Material3DS_t& Material3DS_t::operator=( const Material3DS_t& m ) {
	for ( int i=0; i < 20; i++ )
		Name[ i ] = m.Name[ i ];
	normalmap = m.normalmap;
	tex1 = m.tex1;
	tex2 = m.tex2;
	diffuse = m.diffuse;
	ambient = m.ambient;
	specular = m.specular;
	transparency = m.transparency;
	shininess = m.shininess;
	return *this;
}

inline
Object3DS_t& Object3DS_t::operator=( const Object3DS_t& o ) {
	for ( int i=0; i < 20; i++ )
		Name[ i ] = o.Name[ i ];
	Vertices = o.Vertices;
	Triangles = o.Triangles;
	Material = o.Material;
	type = o.type;
	bounds = o.bounds;
	return *this;
}

inline 
vs3DSModel& vs3DSModel::operator =( const vs3DSModel& m ) {
	v3Scale = m.v3Scale;
	v3Position = m.v3Position;
	m3Rotation = m.m3Rotation;
	materials = m.materials;
	objects = m.objects;
	loaded = m.loaded;
	File = m.File;
	texDir = m.texDir;
	shader = m.shader;
	renderFlags = m.renderFlags;
	lights = m.lights;
	Frustum = m.Frustum;
	path = m.path;
	boundingBox = m.boundingBox;
	tmpOrigin = m.tmpOrigin;
	tmpAxis = m.tmpAxis;
	return ( *this );
}

#endif