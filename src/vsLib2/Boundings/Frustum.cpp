#include "Frustum.h"

/*
====================================
	Constructor
====================================
*/
vsFrustum::vsFrustum( void )
{
	//no code here
}

/*
====================================
	Destructor
====================================
*/
vsFrustum::~vsFrustum( void )
{
	//no code here either
}

/*
====================================
	=
====================================
*/
const vsFrustum& vsFrustum::operator=( const vsFrustum& b )
{
	planes[0] = b.planes[0];
	planes[1] = b.planes[1];
	planes[2] = b.planes[2];
	planes[3] = b.planes[3];
	planes[4] = b.planes[4];
//	planes[5] = b.planes[5];
	return( *this );
}

/*
====================================
	!=
====================================
*/
bool vsFrustum::operator!=( const vsFrustum& b ) const
{
	return!( *this==b );
}

/*
====================================
	==
====================================
*/
bool vsFrustum::operator==( const vsFrustum& b ) const
{
	return( 
	planes[0] == b.planes[0] &&
	planes[1] == b.planes[1] &&
	planes[2] == b.planes[2] &&
	planes[3] == b.planes[3] &&
	planes[4] == b.planes[4] //&&
	//planes[5] == b.planes[5]
	 );
}

/*
====================================
	IntersectPoint
====================================
*/
bool vsFrustum::IntersectPoint( const vsVec3f& point, float epsilon ) const
{
	for ( int i=0; i<5; ++i ) 
	{
		if ( planes[i].Distance( point ) < -epsilon )
			return false;
	}
	return true;
}

/*
====================================
	IntersectSphere
====================================
*/
bool vsFrustum::IntersectSphere( const vsVec3f& point, const float& radius ) const
{
	for ( int i=0; i<5; ++i ) 
	{
		if ( planes[i].Distance( point ) < -radius ) 
			return false;
	}
	return true;
}

/*
====================================
	IntersectBox
====================================
*/
bool vsFrustum::IntersectBox( const vsBox& box, float epsilon ) const
{
	vsVec3f center = box.GetCenter();
	vsVec3f halfDiagonal = box.points[7] - center;
	float d1,d2;

	for ( int i=0; i<6-1; ++i ) 
	{
		d1 = planes[i].Distance( center );
		d2 = planes[i].Normal().Abs().Dot( halfDiagonal );
		if ( d1 + d2 < epsilon )
			return false;
	}

	return true;
}

/*
====================================
	Update
====================================
*/
void vsFrustum::Update( const vsMat4& projMatrix, const vsMat4 modelViewMatrix )
{
	vsMat4 matrix = projMatrix * modelViewMatrix;

	planes[LEFT].Set( matrix[3] + matrix[0],
					  matrix[7] + matrix[4],
					  matrix[11]+ matrix[8],
					  matrix[15]+ matrix[12] );

	planes[RIGHT].Set( matrix[3] - matrix[0],
					  matrix[7] - matrix[4],
					  matrix[11]- matrix[8],
					  matrix[15]- matrix[12] );

  planes[BOTTOM].Set( matrix[3] + matrix[1],
					  matrix[7] + matrix[5],
					  matrix[11]+ matrix[9],
					  matrix[15]+ matrix[13] );

	planes[TOP].Set(  matrix[3] - matrix[1],
					  matrix[7] - matrix[5],
					  matrix[11]- matrix[9],
					  matrix[15]- matrix[13] );

	planes[ZNEAR].Set( matrix[3] + matrix[2],
					  matrix[7] + matrix[6],
					  matrix[11]+ matrix[10],
					  matrix[15]+ matrix[14] );
/*
	planes[ZFAR].Set( matrix[3] - matrix[2],
					  matrix[7] - matrix[6],
					  matrix[11]- matrix[10],
					  matrix[15]- matrix[14] );
*/
	for ( int i=0; i<5-1; ++i )
		planes[i].Normalize();
}

