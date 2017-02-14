#ifndef __VS_GAME_OBJECT_H__
#define __VS_GAME_OBJECT_H__

#include "Timer.h"
#include "..\vsLib2\vsLib2.h"


class vsPhysics;
class vsModel_MD5;

/*
==================================================

	vsGameObject

==================================================
*/
class vsGameObject {
public:
								vsGameObject( void );
								~vsGameObject( void );

	virtual void				Render( bool frustumTest = true );
	
	virtual void				SetPhysics( vsPhysics* physics );
	virtual void				SetModel( vsModel_MD5* model );
	virtual void				SetVisibility( bool vis );

	virtual void				SetOrigin( const vsVec3f& o );
	/*virtual void				SetRotation( const vsVec3f& r );
	virtual void				SetRotation( const vsQuaternion& q );*/

	virtual vsPhysics*			GetPhysics( void ) const;
	virtual vsModel_MD5*		GetModel( void ) const;
	virtual const bool&			IsVisible( void ) const;

	virtual void				SaveToFile( vsOutputFile * f );
	virtual void				RestoreFromFile( vsInputFile * f );

protected:
	vsPhysics*					physics;
	vsModel_MD5*				model;
	bool						visible;
};


VS_INLINE void vsGameObject::SetPhysics( vsPhysics* physics ) {
	this->physics = physics;
}

VS_INLINE void vsGameObject::SetModel( vsModel_MD5* model ) {
	this->model = model;
}

VS_INLINE void vsGameObject::SetVisibility( bool vis ) {
	visible = vis;
}

VS_INLINE vsPhysics* vsGameObject::GetPhysics( void ) const {
	return physics;
}

VS_INLINE vsModel_MD5* vsGameObject::GetModel( void ) const {
	return model;
}

VS_INLINE const bool& vsGameObject::IsVisible( void ) const {
	return visible;
}

#endif