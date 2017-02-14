
#ifndef __CR2_AABB_H__
#define __CR2_AABB_H__
//FIXME: make it axis-aligned

#include "../Math/Math.h"
#include "../Geometry/Vertex.h"


enum boxFaces_t {
	BOX_FACE_X_POS = 0,
	BOX_FACE_X_NEG,
	BOX_FACE_Y_POS,
	BOX_FACE_Y_NEG,
	BOX_FACE_Z_POS,
	BOX_FACE_Z_NEG
};

class vsBox {
public:
					vsBox( void );
					vsBox( const vsVec3f& min, const vsVec3f& max );
					~vsBox( void );

	vsBox&			operator=( const vsBox& box );
	bool			operator==( const vsBox& box ) const;
	bool			operator!=( const vsBox& box ) const;
	vsBox			operator*( float f ) const;
	vsBox			operator*( const vsVec3f& v ) const;
	vsBox			operator*( const vsQuaternion& v ) const;
	vsBox			operator/( const vsVec3f& v ) const;
	vsBox			operator/( float ) const;
	vsBox			operator+( const vsVec3f& v ) const;
	vsBox			operator+( float f ) const;
	vsBox			operator-( const vsVec3f& v ) const;
	vsBox			operator-( float ) const;
	vsBox&			operator*=( const vsVec3f& v );
	vsBox&			operator*=( const vsQuaternion& v );
	vsBox&			operator*=( const float f );
	vsBox&			operator/=( const vsVec3f& v );
	vsBox&			operator/=( float v );
	vsBox&			operator+=( const vsVec3f& v );
	vsBox&			operator+=( const float f );
	vsBox&			operator-=( const vsVec3f& v );
	vsBox&			operator-=( const float f );

	void			FromVectors( vsVec3f * vectors, long count );		// create box from a list of vectors
	void			FromVertices( vsVertex * vert, long count );		// create box from a list of vertices
	void			FromMinMax( const vsVec3f& min, const vsVec3f& max ); // create box from min & max points

	void			Translate( const vsVec3f& t );
	void			Scale( const vsVec3f& s );
	void			Rotate( const vsMat3& r );

	void			RenderBox( bool lines = true );
	vsVec3f			GetCenter( void ) const;
	vsVec3f			GetExtents( void ) const;
	vsVec3f			GetClosestPoint( const vsVec3f& pt ) const;
	int				GetFaceOfSurfacePoint( const vsVec3f& pt ) const;

	bool			IsSphereInside( const vsVec3f& center, float radius ) const;
	bool			IsEllipsoidInside( const vsVec3f& center, const vsVec3f& radius ) const;
	bool			IsPointInside( const vsVec3f& point ) const;
	bool			IsBoxInside( const vsBox& box ) const;
	vsVec3f			OverlapBox( const vsBox& box ) const;
	bool			IntersectRay( const vsVec3f& rayOrigin, const vsVec3f& rayVector, vsVec3f& nearPt, vsVec3f& farPt );
	bool			IntersectRay( const vsVec3f& rayOrigin, const vsVec3f& rayVector, float& t_near, float& t_far );

	void			SetRenderColor( const vsVec4f& color );

public:
	vsVec3f			points[ 8 ];
	vsVec4f			renderColor;
};

VS_INLINE void vsBox::SetRenderColor( const vsVec4f& color ) {
	renderColor = color;
}

#endif