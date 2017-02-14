

#ifndef __CR2_LIGHT_H__
#define __CR2_LIGHT_H__

#include "../Math/Math.h"

class vsLight {
public:
					vsLight( void );
					~vsLight( void ) {};

	vsLight&		operator=( const vsLight& l );
	bool			operator==( const vsLight& l );
	bool			operator!=( const vsLight& l );

	void			Zero( void );
	void			SetColor( const vsVec4f& color );
	void			SetPosition( const vsVec3f& position );
	void			SetType( int directional=1 );

	vsVec3f			GetPosition3( void );
	const vsVec4f&	GetPosition4( void ) const;
	vsVec3f			GetColor3( void );
	const vsVec4f&	GetColor4( void ) const;
	bool			IsDirectional( void ) const;

public:
	vsVec4f			color;
	//vsVec4f			specular;
	//vsVec4f			ambient;		
	vsVec4f			position;
	float			radius;
	bool			on;
};

inline 
vsLight::vsLight( void ) {
	color = vsVec4f( 1.0f );
	position = vsVec4f( 0.0f );
	radius = 1.0f;
	on = true;
}

inline
vsLight& vsLight::operator=( const vsLight& l ) {
	color = l.color;
	position = l.position;
	radius = l.radius;
	on = l.on;

	return( *this );
}

inline
bool vsLight::operator==( const vsLight& l ) {
	if ( 	color!=l.color ||
		position!=l.position ||
		radius!=l.radius ||
		on!=l.on ) return false;

	return true;
}

inline
bool vsLight::operator!=( const vsLight& l )
{
	return!( *this==l );
}

inline
void vsLight::Zero( void ) 
{
	position = NULLVEC4F;
	radius = 1.0f;
	on = false;
	color = NULLVEC4F;
}

inline
void vsLight::SetColor( const vsVec4f& color )
{
	this->color = color;
}

inline
void vsLight::SetPosition( const vsVec3f& position )
{
	this->position = vsVec4f( position, this->position.w );
}

inline
void vsLight::SetType( int directional )
{
	position.w = float( directional );
}

inline
vsVec3f vsLight::GetPosition3( void )
{
	return( vsVec3f( position.x, position.y, position.z ) );
}

inline
const vsVec4f& vsLight::GetPosition4( void ) const
{
	return position;
}

inline
vsVec3f vsLight::GetColor3( void )
{
	return vsVec3f( color.x, color.y, color.z );
}

inline
const vsVec4f& vsLight::GetColor4( void ) const
{
	return color;
}

inline
bool vsLight::IsDirectional( void ) const
{
	return( position.w > 0.0f );
}

#endif