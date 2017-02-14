#include "ModelManager.h"
#include <assert.h>

using namespace vsLib2;

vsModelManager * modelManager;

/* 
=====================================
	vsModelManager
=====================================
*/
vsModelManager::vsModelManager( void )
{
	// empty
	vsMaterial* defaultMaterial = new vsMaterial();
	defaultMaterial->id = 0;
	strcpy_s<128>( defaultMaterial->name, "default_material" );
	//ZeroMemory( defaultMaterial->name, 128 );

	materials.Append( defaultMaterial );
}

/* 
=====================================
	~vsModelManager
=====================================
*/
vsModelManager::~vsModelManager( void )
{
	models.DeletePointerElements();
	models.Free();

	animations.DeletePointerElements();
	animations.Free();

	materials.DeletePointerElements();
	materials.Free();
}

/* 
=====================================
	DeleteAnim
=====================================
*/
void vsModelManager::DeleteAnim( vsAnimation_MD5* anim )
{
	if ( anim ) {
		animations.RemoveElement( anim );
		delete anim;
	}
}

/* 
=====================================
	DeleteMesh
=====================================
*/
void vsModelManager::DeleteMesh( vsModel_MD5* model )
{
	if ( model ) {
		models.RemoveElement( model );
		delete model;
	}
}

/* 
=====================================
	LoadMesh
=====================================
*/
vsModel_MD5* vsModelManager::LoadMesh( const MD5ModelArgs_t& args )
{
	vsModel_MD5* model = new vsModel_MD5();
	int result;

	if ( ( result = model->LoadFromFile( args ) ) == VS_ERROR ) {
		delete model;
		console->Print( CMT_ERROR, "Failed to load mesh: %s", args.szFileName );
		return NULL;
	}

	model->id = models.Length();
	models.Append ( model );

	return model;
}

/* 
=====================================
	LoadAnim
=====================================
*/
vsAnimation_MD5* vsModelManager::LoadAnim( const char* szFileName )
{
	int n = FindAnim( szFileName );

	if ( n >= 0 )
		return animations[ n ];

	vsAnimation_MD5* anim = new vsAnimation_MD5();
	int result;

	if ( ( result = anim->LoadFromFile( szFileName ) ) == VS_ERROR ) {
		delete anim;
		console->Print( CMT_ERROR, "Failed to load anim: %s", szFileName );
		return NULL;
	}

	anim->id = animations.Length();
	animations.Append ( anim );

	return anim;
}

/* 
=====================================
	AttachAnimToMesh
=====================================
*/
int vsModelManager::AttachAnimToMesh( uint animId, uint meshId )
{
	assert( animId < animations.Length() && meshId < models.Length() );
	models[ meshId ]->AddAnimation( animations[ animId ] );

	return VS_OK;
}

/* 
=====================================
	LoadMaterials
=====================================
*/
int vsModelManager::LoadMaterials( const char * szFileName ) 
{
	FILE * f;

	if ( fopen_s( &f, szFileName, "r" ) ) {
		console->Print( "Could not load material library ( %s )", szFileName );
		return VS_ERROR;
	}

	char line[ 256 ];
	char str[ 128 ];
	int d;
	vsMaterial* mat;

	while ( !feof( f ) ) {
		fgets( line, 256, f );

		if ( strstr( line, "numMaterials" ) ) {
			sscanf_s( line, "numMaterials %d", &d );
			///materials.Resize( materials.Length() + d );
		} else
		if ( strstr( line, "material" ) ) {
			materials.Append( new vsMaterial() );
			mat = materials[ materials.Length() - 1 ];
			mat->id = materials.Length()-1;
			sscanf_s( line, "material %s {", mat->name, 128 );

			do {
				fgets( line, 256, f );

				if ( strstr( line, "diffusemap" ) ) {
					sscanf_s( line, " diffusemap %s", str, 128 );
					//if ( strcmp( str, "nope" ) )
						mat->diffuseMap = renderSystem->LoadTexture( str, 1 );
				} else
				if ( strstr( line, "bumpmap" ) ) {
					sscanf_s( line, " bumpmap %s", str, 128 );
				//	if ( strcmp( str, "nope" ) )
						mat->normalMap = renderSystem->LoadTexture( str, 1 );
				} else
				if ( strstr( line, "specularmap" ) ) {
					sscanf_s( line, " specularmap %s", str, 128 );
					//if ( strcmp( str, "nope" ) )
						mat->specularMap = renderSystem->LoadTexture( str, 1 );
				}/* else
				if ( strstr( line, "shader" ) ) {
					sscanf_s( line, "shader %s", &str, 128 );
					mat->shader = renderSystem->LoadShaderFromFile( mat->name, str, 0, 0 );
				} */
			} while( !strchr( line, '}' ) );
		}
	}

	fclose( f );

	return VS_OK;
}

/* 
=====================================
	FindMesh
=====================================
*/
int vsModelManager::FindMesh( const MD5ModelArgs_t& args )
{
	REPU ( models.Length() )
		if ( !strcmp( models[ i ]->filename, args.szFileName ) )
			return i;

	return -1;
}

/* 
=====================================
	FindAnim
=====================================
*/
int vsModelManager::FindAnim( const char* szFileName )
{
	REPU ( animations.Length() )
		if ( !strcmp( animations[ i ]->filename, szFileName ) )
			return i;

	return -1;
}

/* 
=====================================
	FindMaterial
=====================================
*/
int vsModelManager::FindMaterial( const char* szName )
{
	REPU ( materials.Length() )
		if ( !strcmp( materials[ i ]->name, szName ) )
			return i;

	return 0;
}

/* 
=====================================
	RequestMaterial
=====================================
*/
vsMaterial* vsModelManager::RequestMaterial( const char* szFileName )
{
	return materials[ FindMaterial( szFileName ) ];
}

/* 
=====================================
	PrintMaterials
=====================================
*/
void vsModelManager::PrintMaterials( void ) 
{
	REPU ( materials.Length() ) {
		console->Print( "Material: %s, d: %d, b: %d, s: %d",
						materials[ i ]->name,
						materials[ i ]->diffuseMap,
						materials[ i ]->normalMap,
						materials[ i ]->specularMap );
	}
}