#pragma once

#include "../Math/Math.h"
#include "AABB.h"


//frustum WITHOUT  far plane
class vsFrustum {
public:
						vsFrustum( void );
						~vsFrustum( void );

	const vsFrustum&	operator=( const vsFrustum& b );
	bool				operator!=( const vsFrustum& b ) const;
	bool				operator==( const vsFrustum& b ) const;

	bool				IntersectPoint( const vsVec3f& point, float epsilon ) const;
	bool				IntersectSphere( const vsVec3f& point, const float& radius ) const;
	bool				IntersectBox( const vsBox& box, float epsilon ) const;
	void				Update( const vsMat4& projMatrix, const vsMat4 modelViewMatrix );

public:
	vsPlane			planes[ 6 ];

	enum {
		LEFT = 0,
		RIGHT,
		TOP,
		BOTTOM,
		ZNEAR,
		ZFAR
	};
};