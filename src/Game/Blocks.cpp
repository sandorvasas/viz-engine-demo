#include "Blocks.h"
#include "Geometry\ModelManager.h"


vsBlockLibrary * blockLibrary;

vsBlockLibrary::vsBlockLibrary( void ) 
{
}

vsBlockLibrary::~vsBlockLibrary( void )
{
	blocks.DeletePointerElements();
	blocks.Free();
}

void vsBlockLibrary::LoadLibrary( const char* szFileName )
{
	FILE* file;

	if ( fopen_s( &file, szFileName, "r" ) ) {
		vsLib2::console->Print( CMT_ERROR, "Error: could not load block library." );
		return;
	}

	blocktype_t ** block;
	char line[ 128 ];
	char szName[ 64 ];
	int i;
	while ( !feof( file ) )
	{
		fgets( line, 128, file );

		if ( strstr( line, "textureAtlas" ) ) {
			while ( !strstr( line, "}" ) ) {
				fgets( line, 128, file );

				if ( strstr( line, "size" ) ) {
					sscanf_s( line, " size %d", &i );
					textureSize = 1.0f / ( float ) i;
				} else
				if ( strstr( line, "diffusemap" ) ) {
					sscanf_s( line, " diffusemap %s", szName, 64 );
					textureAtlas = vsLib2::renderSystem->LoadTexture( szName, 0 );
				} else
				if ( strstr( line, "bumpmap" ) ) {
					sscanf_s( line, " bumpmap %s", szName, 64 );
					textureAtlasBump = vsLib2::renderSystem->LoadTexture( szName, 0 );
				}
			}
		} else
		if ( strstr( line, "block" ) ) {
			blocks.Expand();

			block = &blocks[ blocks.Length() - 1 ];
			*block = new blocktype_t();

			sscanf_s( line, "block %s", ( *block )->name, 32 );

			while ( !strstr( line, "}" ) ) {
				fgets( line, 128, file );

				if ( strstr( line, "tc_top" ) ) 
					sscanf_s( line, " tc_top %d,%d", &( *block )->tcTop.x, &( *block )->tcTop.y );
				else
				if ( strstr( line, "tc_bottom" ) ) 
					sscanf_s( line, " tc_bottom %d,%d", &( *block )->tcBottom.x, &( *block )->tcBottom.y );
				else
				if ( strstr( line, "tc_side" ) ) {
					sscanf_s( line, " tc_side %d,%d", &( *block )->tcSide.x, &( *block )->tcSide.y );
					( *block )->tcFront = ( *block )->tcSide;
				}
				else
				if ( strstr( line, "tc_front" ) ) 
					sscanf_s( line, " tc_front %d,%d", &( *block )->tcFront.x, &( *block )->tcFront.y );
				else
				if ( strstr( line, "weight" ) ) 
					sscanf_s( line, " weight %f", &( *block )->weight );
				else
				if ( strstr( line, "life" ) ) 
					sscanf_s( line, " life %d", &( *block )->life );
				else
				if ( strstr( line, "damages" ) ) 
					sscanf_s( line, " damages %d", &( *block )->damages );
				else
				if ( strstr( line, "liquid" ) ) 
					( *block )->flags |= BLOCK_FLAG_LIQUID;
				else
				if ( strstr( line, "transparent" ) ) 
					( *block )->flags |= BLOCK_FLAG_TRANSPARENT;
				else
				if ( strstr( line, "eatable" ) ) 
					( *block )->flags |= BLOCK_FLAG_EATABLE;
				else
				if ( strstr( line, "nonsolid" ) ) 
					( *block )->flags |= BLOCK_FLAG_NONSOLID;
				else
				if ( strstr( line, "drops" ) ) {
					( *block )->flags |= BLOCK_FLAG_DROPS_ITEM;

						sscanf_s( line, " drops %d%% %d %s", 
									&( *block )->dropChance,
									&( *block )->dropItemCount,
									&( *block )->dropItemName, 32 );
				}
				else
				if ( strstr( line, "model" ) ) {
					sscanf_s( line, " model %s", szName, 64 );
					( *block )->modelargs = new MD5ModelArgs_t();
					( *block )->modelargs->scale = vsVec3f( 1.0f );
					( *block )->modelargs->shader = vsLib2::renderSystem->GetShaderByName( "shader_modelmd5" );
					( *block )->modelargs->szFileName = szName;
				}
				else 
				if ( strstr( line, "anim" ) ) {
					sscanf_s( line, " anim %s", ( *block )->animFileName, 64 );
				}
			}
		}
	}

	SortBlocksByName();
}

ushort vsBlockLibrary::GetBlockTypeId( const char* blockTypeName ) const
{
	for ( uint i = 0; i < blocks.Length(); i++ )
		if ( !strcmp( blockTypeName, blocks[ i ]->name ) )
			return ( ushort )i+1;

	return NULL;
}

const blocktype_t* vsBlockLibrary::GetBlockType( const char * blockTypeName ) const
{
	for ( uint i = 0; i < blocks.Length(); i++ )
		if ( !strcmp( blockTypeName, blocks[ i ]->name ) )
			return blocks[ i ];

	return NULL;
}

const blocktype_t* vsBlockLibrary::GetBlockType( ushort blockTypeId ) const
{
	return blocks[ blockTypeId - 1 ];
}

const blocktype_t* vsBlockLibrary::GetBlockType( const block_t& block ) const
{
	return blocks[ block.id - 1 ];
}

void vsBlockLibrary::SortBlocksByName( void )
{
	bool swapped;
	( swapped );
	//fixme & binarysearch
	return ;
	do {
		swapped = false;

		//if ( strlen

	} while ( swapped );
}