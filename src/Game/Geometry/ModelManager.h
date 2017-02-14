#ifndef __VS_MODELMANAGER_H__
#define __VS_MODELMANAGER_H__

#include "Material.h"
#include "Model_MD5.h"
#include "..\Physics\Physics.h"
#include "..\..\vsLib2\vsLib2.h"

/*
==================================================

	vsModelManager

==================================================
*/
class vsModelManager {
public:
								vsModelManager( void );
								~vsModelManager( void );

	void						DeleteAnim( vsAnimation_MD5* anim );
	void						DeleteMesh( vsModel_MD5* model );

	int							LoadMaterials( const char* materials );
	vsModel_MD5*				LoadMesh( const MD5ModelArgs_t& args );
	vsAnimation_MD5*			LoadAnim( const char* szFileName );
	int							AttachAnimToMesh( uint animId, uint meshId );
	vsMaterial*					RequestMaterial( const char* szFileName );
	void						PrintMaterials( void );

private:
	int							FindMesh( const MD5ModelArgs_t& args );
	int							FindAnim( const char* szFileName );
	int							FindMaterial( const char* szName );

private:
	vsList< vsModel_MD5* >		models;
	vsList< vsAnimation_MD5* >	animations;
	vsList< vsMaterial* >		materials;
};

extern vsModelManager * modelManager;



#endif