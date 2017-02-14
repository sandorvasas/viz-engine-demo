#include "AABB.h"			
#include "../Renderer/OpenGL/GLexp.h"
#include "..\Math\Math.h"

using namespace vsMath;

vsBox::vsBox( void ) {
	renderColor = vsVec4f( 1.0f );
}

vsBox::vsBox( const vsVec3f& min, const vsVec3f& max )
{
	FromMinMax( min, max );
}

vsBox::~vsBox( void ) {
}

vsBox& vsBox::operator=( const vsBox& box ) {
	memcpy( points, box.points, sizeof( points ) );
	return *this;
}

bool vsBox::operator==( const vsBox& box ) const {
	for ( int i=0; i<8; i++ ) {
		if ( points[i] != box.points[i] )
			return false;
	}
	
	 return ( true );
}

bool vsBox::operator!=( const vsBox& box ) const {
	return !( this->operator==( box ) );
}

vsBox vsBox::operator*( const vsQuaternion& v ) const
{
	vsBox a=*this;

	REPEAT( 9 )
		a.points[i] *= v;

	return( a );
}

vsBox vsBox::operator*( const vsVec3f& v ) const
{
	vsBox a=*this;

	REPEAT( 9 )
		a.points[i] *= v;

	return( a );
}

vsBox vsBox::operator*( float f ) const
{
	vsBox a=*this;

	REPEAT( 9 )
		a.points[i] *= f;

	return( a );
}

vsBox vsBox::operator/( const vsVec3f& v ) const
{
	vsBox a=*this;

	REPEAT( 9 )
		a.points[i] /= v;

	return( a );
}

vsBox vsBox::operator/( float f ) const
{
	vsBox a=*this;

	REPEAT( 9 )
		a.points[i] /= f;

	return( a );
}

vsBox vsBox::operator+( const vsVec3f& v ) const
{
	vsBox a = *this;
	REPEAT( 9 )
		a.points[i] += v;
	return ( a );
}

vsBox vsBox::operator+( float f ) const
{
	vsBox a = *this;
	REPEAT( 9 )
		a.points[i] += f;
	return ( a );
}

vsBox vsBox::operator-( const vsVec3f& v ) const
{
	vsBox a = *this;
	REPEAT( 9 )
		a.points[i] -= v;
	return ( a );
}

vsBox vsBox::operator-( float f ) const
{
	vsBox a = *this;
	REPEAT( 9 )
		a.points[i] -= f;
	return ( a );
}

vsBox& vsBox::operator+=( const vsVec3f& v )
{
	*this = *this + v;
	return *this;
}

vsBox& vsBox::operator+=( float f )
{
	*this = *this + f;
	return *this;
}

vsBox& vsBox::operator-=( const vsVec3f& v )
{
	*this = *this - v;
	return *this;
}

vsBox& vsBox::operator-=( float f )
{
	*this = *this - f;
	return *this;
}

vsBox& vsBox::operator*=( const vsVec3f& v )
{
	*this = *this * v;
	return *this;
}

vsBox& vsBox::operator*=( const vsQuaternion& v )
{
	*this = *this * v;
	return *this;
}

vsBox& vsBox::operator*=( float f )
{
	*this = *this * f;
	return *this;
}

vsBox& vsBox::operator/=( const vsVec3f& v )
{
	*this = *this / v;
	return *this;
}

vsBox& vsBox::operator/=( float f )
{
	*this = *this / f;
	return *this;
}

void vsBox::FromVectors( vsVec3f * vectors, long count ) {
	vsVec3f min, max;

	min = vectors[ 0 ];
	max = vectors[ 0 ];

	for ( long i=0; i < count;  i++ ) {
		if ( vectors[ i ].x > max.x ) max.x = vectors[ i ].x;
		if ( vectors[ i ].y > max.y ) max.y = vectors[ i ].y;
		if ( vectors[ i ].z > max.z ) max.z = vectors[ i ].z;

		if ( vectors[ i ].x < min.x ) min.x = vectors[ i ].x;
		if ( vectors[ i ].y < min.y ) min.y = vectors[ i ].y;
		if ( vectors[ i ].z < min.z ) min.z = vectors[ i ].z;
	}

	FromMinMax( min, max );
}

void vsBox::FromVertices( vsVertex * vert, long count ) {
	vsVec3f min, max;

	min = vert[ 0 ].xyz;
	max = vert[ 0 ].xyz;

	for ( long i=0; i < count;  i++ ) {
		if ( vert[ i ].xyz.x > max.x ) max.x = vert[ i ].xyz.x;
		if ( vert[ i ].xyz.y > max.y ) max.y = vert[ i ].xyz.y;
		if ( vert[ i ].xyz.z > max.z ) max.z = vert[ i ].xyz.z;

		if ( vert[ i ].xyz.x < min.x ) min.x = vert[ i ].xyz.x;
		if ( vert[ i ].xyz.y < min.y ) min.y = vert[ i ].xyz.y;
		if ( vert[ i ].xyz.z < min.z ) min.z = vert[ i ].xyz.z;
	}

	FromMinMax( min, max );
}

void vsBox::FromMinMax( const vsVec3f& min, const vsVec3f& max ) {
	points[ 0 ] = min;
	points[ 1 ] = vsVec3f( max.x, min.y, min.z );
	points[ 2 ] = vsVec3f( max.x, max.y, min.z );
	points[ 3 ] = vsVec3f( min.x, max.y, min.z );
	points[ 4 ] = vsVec3f( min.x, max.y, max.z );
	points[ 5 ] = vsVec3f( min.x, min.y, max.z );
	points[ 6 ] = vsVec3f( max.x, min.y, max.z );
	points[ 7 ] = max;
}

void vsBox::Translate( const vsVec3f& t )
{ 
	for ( int i=0; i<8; ++i ) 
		points[i] += t;
}

void vsBox::Scale( const vsVec3f& s )
{
	for ( int i=0; i<8; ++i ) 
		points[i] *= s;
}

void vsBox::Rotate( const vsMat3& r )
{
	for ( int i=0; i<8; ++i ) 
		points[i] *= r;
}

void vsBox::RenderBox( bool lines ) 
{
	glColor4fv( renderColor.ToTypePtr() );

	if ( lines ) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		//glLineWidth( 1.0f );
			glBegin( GL_LINE_LOOP );
					glVertex3fv( points[ 0 ].ToTypePtr() );
					glVertex3fv( points[ 1 ].ToTypePtr() );
					glVertex3fv( points[ 6 ].ToTypePtr() );
					glVertex3fv( points[ 5 ].ToTypePtr() );
			glEnd();
			glBegin( GL_LINE_LOOP );
					glVertex3fv( points[ 2 ].ToTypePtr() );
					glVertex3fv( points[ 3 ].ToTypePtr() );
					glVertex3fv( points[ 4 ].ToTypePtr() );
					glVertex3fv( points[ 7 ].ToTypePtr() );
			glEnd();
			glBegin( GL_LINES );
					glVertex3fv( points[ 0 ].ToTypePtr() );
					glVertex3fv( points[ 3 ].ToTypePtr() );

					glVertex3fv( points[ 1 ].ToTypePtr() );
					glVertex3fv( points[ 2 ].ToTypePtr() );

					glVertex3fv( points[ 4 ].ToTypePtr() );
					glVertex3fv( points[ 5 ].ToTypePtr() );

					glVertex3fv( points[ 6 ].ToTypePtr() );
					glVertex3fv( points[ 7 ].ToTypePtr() );
			glEnd();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	} else {

		static const byte indices[] = {
			0, 1, 6, 5, // min y
			2, 3, 4, 7, // max y
			0, 1, 2, 3,	// min z
			4, 5, 6, 7,	// max z
			0, 3, 4, 5,
			7, 6, 2, 1
		};

		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, sizeof( vsVec3f ), points );
		glDrawElements( GL_QUADS, 24, GL_UNSIGNED_BYTE, indices );
		glDisableClientState( GL_VERTEX_ARRAY );
	}
}

vsVec3f vsBox::GetCenter( void ) const 
{
	return ( points[0] + points[7] ) * 0.5f;
}

vsVec3f	vsBox::GetExtents( void ) const
{
	return ( points[ 7 ] - GetCenter() );
}

vsVec3f	vsBox::GetClosestPoint( const vsVec3f& pt ) const 
{
	vsVec3f point = pt;

	point.x = Clamp( point.x, points[ 0 ].x, points[ 7 ].x );
	point.y = Clamp( point.y, points[ 0 ].y, points[ 7 ].y );
	point.z = Clamp( point.z, points[ 0 ].z, points[ 7 ].z );

	return point;
}

/*
================================
GetFaceOfSurfacePoint
	Works most of the time, but not on edges
================================
*/
int vsBox::GetFaceOfSurfacePoint( const vsVec3f& pt ) const
{
	if ( pt.x == points[ 0 ].x ) return BOX_FACE_X_NEG;
	if ( pt.x == points[ 7 ].x ) return BOX_FACE_X_POS;
	if ( pt.y == points[ 0 ].y ) return BOX_FACE_Y_NEG;
	if ( pt.y == points[ 7 ].y ) return BOX_FACE_Y_POS;
	if ( pt.z == points[ 0 ].z ) return BOX_FACE_Z_NEG;
	if ( pt.z == points[ 7 ].z ) return BOX_FACE_Z_POS;
	return -1;
}

bool vsBox::IsSphereInside( const vsVec3f& center, float radius ) const
{
/*
	!!!!!!!!! fixme
*/
	return false;
}

bool vsBox::IsEllipsoidInside( const vsVec3f& center, const vsVec3f& radius ) const
{
	vsVec3f invRadius = 1.0f / radius;
	vsVec3f v1 = ( ( center-points[0] ) * invRadius ).Abs();
	vsVec3f v2 = ( ( center-points[7] ) * invRadius ).Abs();
	
	float minDist = 0.0f;

	if ( v1.x < minDist ) minDist = v1.x;
	if ( v1.y < minDist ) minDist = v1.y;
	if ( v1.z < minDist ) minDist = v1.z;

	if ( v2.x < minDist ) minDist = v2.x;
	if ( v2.y < minDist ) minDist = v2.y;
	if ( v2.z < minDist ) minDist = v2.z;
	
	if ( center<points[7] && center>points[0] && minDist>=1.0f )
		return true;

	return false;
	//FIXME not working correctly
}

bool vsBox::IsPointInside( const vsVec3f& point ) const
{
	if ( point>points[0] && point<points[7] )
		return true;
	
	return false;
}

bool vsBox::IsBoxInside( const vsBox& box ) const 
{
	vsVec3f overlap = OverlapBox( box );
	return ( overlap.x < 0.0f && overlap.y < 0.0f && overlap.z < 0.0f );
}

/*
=======================
	OverlapBox
		Returns overlap
			Any component of overlap is POSITIVE if box is overlapping
			All components of overlap are NEGATIVE if box is not overlapping
=======================
*/
vsVec3f vsBox::OverlapBox( const vsBox& box ) const 
{
	vsVec3f ext1 = GetExtents();
	vsVec3f ext2 = box.GetExtents();
	vsVec3f center1 = GetCenter();
	vsVec3f center2 = box.GetCenter();

	vsVec3f dist = ( center1 - center2 ).Abs();
	vsVec3f sumext = ext1 + ext2;

	return dist - sumext;
	
}

bool vsBox::IntersectRay( const vsVec3f& rayOrigin, const vsVec3f& rayVector, float& t_near, float& t_far )
{
	float Tnear = -99999999999999.9f;
	float Tfar = 99999999999999.9f;
	float T1, T2;
	vsVec3f invRayVector = 1.0f / rayVector;

	// X planes
	if ( rayVector.x == 0 ) {
		if ( rayOrigin.x < points[ 0 ].x || rayOrigin.x > points[ 7 ].x )
			return false;
	} else {
		T1 = ( points[ 0 ].x - rayOrigin.x ) * invRayVector.x;
		T2 = ( points[ 7 ].x - rayOrigin.x ) * invRayVector.x;

		if ( T1 > T2 ) vsSystem::Swap( T1, T2 );
		if ( T1 > Tnear ) Tnear = T1;
		if ( T2 < Tfar ) Tfar = T2;

		if ( Tnear > Tfar ) return false;
		if ( Tfar < 0.0f ) return false;
	}

	// Y planes
	if ( rayVector.y == 0 ) {
		if ( rayOrigin.y < points[ 0 ].y || rayOrigin.y > points[ 7 ].y )
			return false;
	} else {
		T1 = ( points[ 0 ].y - rayOrigin.y ) * invRayVector.y;
		T2 = ( points[ 7 ].y - rayOrigin.y ) * invRayVector.y;

		if ( T1 > T2 ) vsSystem::Swap( T1, T2 );
		if ( T1 > Tnear ) Tnear = T1;
		if ( T2 < Tfar ) Tfar = T2;

		if ( Tnear > Tfar ) return false;
		if ( Tfar < 0.0f ) return false;
	}

	// Z planes
	if ( rayVector.z == 0 ) {
		if ( rayOrigin.z < points[ 0 ].z || rayOrigin.z > points[ 7 ].z )
			return false;
	} else {
		T1 = ( points[ 0 ].z - rayOrigin.z ) * invRayVector.z;
		T2 = ( points[ 7 ].z - rayOrigin.z ) * invRayVector.z;

		if ( T1 > T2 ) vsSystem::Swap( T1, T2 );
		if ( T1 > Tnear ) Tnear = T1;
		if ( T2 < Tfar ) Tfar = T2;

		if ( Tnear > Tfar ) return false;
		if ( Tfar < 0.0f ) return false;
	}

	t_near = Tnear;
	t_far = Tfar;
	return true;
}

bool vsBox::IntersectRay( const vsVec3f& rayOrigin, const vsVec3f& rayVector, vsVec3f& nearPt, vsVec3f& farPt )
{
	float t_near, t_far;
	if ( IntersectRay( rayOrigin, rayVector, t_near, t_far ) ) {
		nearPt = rayOrigin + rayVector * t_near;
		farPt = rayOrigin + rayVector * t_far;
		return true;
	}

	return false;
}