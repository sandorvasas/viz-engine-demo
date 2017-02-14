#include "Vertex.h"

/*
==================
	def constructor
==================
*/
vsVertex::vsVertex( void )
{
	//nothing to do
	Color = vsVec4f( 1.0f );
}


vsVertex::vsVertex( const vsVec3f& xyz )
{
	this->xyz = xyz;
}

vsVertex::vsVertex( const vsVec3f& xyz, const vsVec2f& st )
{
	this->xyz = xyz;
	this->st = st;
}

/*
==================
	constructor2
==================
*/
vsVertex::vsVertex( const vsVec3f& xyz, const vsVec3f& Normal, const vsVec2f& st, const vsVec4f& Color )
{
	this->xyz = xyz;
	this->Normal = Normal;
	this->st = st;
	this->Color = Color;
}

/*
==================
	destructor
==================
*/
vsVertex::~vsVertex( void )
{
}

/*
==================
	=
==================
*/
vsVertex& vsVertex::operator=( const vsVertex& v )
{
	this->xyz = v.xyz;
	this->Normal = v.Normal;
	this->Tangent = v.Tangent;
	this->st = v.st;
	this->Color = v.Color;
	this->reserved[ 0 ] = v.reserved[ 0 ];
	this->reserved[ 1 ] = v.reserved[ 1 ];
	this->reserved[ 2 ] = v.reserved[ 2 ];
	this->reserved[ 3 ] = v.reserved[ 3 ];
//	this->stu[0] = v.stu[0];
//	this->stu[1] = v.stu[1];
//	this->stu[2] = v.stu[2];
	return( *this );
}

/*
==================
	==
==================
*/
bool vsVertex::operator==( const vsVertex& v )
{
	return( 
	this->xyz == v.xyz &&
	this->Normal == v.Normal &&
	this->Tangent == v.Tangent &&
	this->st == v.st &&
	this->Color == v.Color //&&
//	this->stu[0] == v.stu[0] &&
//	this->stu[1] == v.stu[1] &&
//	this->stu[2] == v.stu[2]
	 );
}

/*
==================
	!=
==================
*/
bool vsVertex::operator!=( const vsVertex& v )
{
	return !( *this == v );
}

/*
==================
	+=
==================
*/
vsVertex& vsVertex::operator+=( const vsVertex& v )
{
	xyz += v.xyz;
	Normal += v.Normal;
	Tangent += v.Tangent;
	st += v.st;
//	stu[0] += v.stu[0];
//	stu[1] += v.stu[1];
//	stu[2] += v.stu[2];
	Color += v.Color;

	return( *this );
}

/*
==================
	-=
==================
*/
vsVertex& vsVertex::operator-=( const vsVertex& v )
{
	xyz -= v.xyz;
	Normal -= v.Normal;
	Tangent -= v.Tangent;
	st -= v.st;
//	stu[0] -= v.stu[0];
//	stu[1] -= v.stu[1];
//	stu[2] -= v.stu[2];
	Color -= v.Color;

	return( *this );
}

/*
==================
	CalculateNormal
==================
*/
vsVertex& vsVertex::CalculateNormal( const vsVertex &neighbor1, const vsVertex &neighbor2 ) 
{
	vsVec3f v1, v2;

	v1 = xyz - neighbor1.xyz;
	v2 = xyz - neighbor2.xyz;
	Normal = v1.Cross( v2 );
	Normal.Normalize();

	return( *this );
}

/*
==================
	CalculateTangent
==================
*/
vsVertex& vsVertex::CalculateTangent( const vsVertex &neighbor1, const vsVertex &neighbor2 ) 
{
	vsVec3f v1, v2;
	vsVec3f pv1, pv2;
	vsVec2f t1, t2;

	v1 = neighbor1.xyz - this->xyz;
	v2 = neighbor2.xyz - this->xyz;
	this->Normal = v1.Cross( v2 );
	this->Normal.Normalize();

	pv1 = v1 - Normal * Normal.Dot( v1 );
	pv2 = v2 - Normal * Normal.Dot( v2 );

	t1 = neighbor1.st - this->st;
	t2 = neighbor2.st - this->st;

	if ( t2.x + t1.y > t1.x + t2.y )
	{
		t1.x = -t1.x;
		t2.x = -t2.x;
	}

	Tangent = ( pv1 * t2.x ) - ( pv2 * t1.x );
	Tangent.Normalize();

	return( *this );
}
