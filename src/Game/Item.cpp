#include "Item.h"
#include "Timer.h"
#include "Player.h"
#include "Game.h"

using namespace vsLib2;
using namespace vsMath;


vsItemManager * itemManager;


vsItemManager::vsItemManager( vsShader * shader ) {
	this->shader = shader;
	shader->BindAttrib( 0, "in_Position" );
	shader->BindAttrib( 1, "in_TextureAtlasCoord" );
}

vsItemManager::~vsItemManager( void ) {
	spawnedItems.DeletePointerElements();
	spawnedItems.Free();
}


void vsItemManager::LoadItemLibrary( const char* szFileName ) {
	FILE* file;

	if ( fopen_s( &file, szFileName, "r" ) ) {
		console->Print( CMT_ERROR, "Error: Failed to load item library" );
		return;
	}

	char		line[ 128 ];
	char		szTmp[ 64 ];
	char		strItemType[ 32 ];
	vsVec2ui	dw2ItemAtlasCoord;
	ItemInfo_t* itemInfo;

	while ( !feof( file ) ) {
		fgets( line, 128, file );

		if ( strstr( line, "textureAtlas" ) ) {
			sscanf_s( line, "textureAtlas %d %s",
							&dw2ItemAtlasCoord.x,
							textureAtlasFileName, 64 );
			textureAtlas = renderSystem->LoadTexture( textureAtlasFileName, 0 );
			textureSize = 1.0f / ( float ) dw2ItemAtlasCoord.x;
		} else
		if ( strstr( line, "item" ) ) {
			itemInfos.Expand();
			itemInfo = itemInfos[ itemInfos.Length() - 1 ] = new ItemInfo_t();
			itemInfo->id = itemInfos.Length() - 1;

			sscanf_s( line, "item( %s )[ %d %d ] %s {", 
							strItemType, 32,
							&dw2ItemAtlasCoord.x,
							&dw2ItemAtlasCoord.y,
							itemInfo->name, 32 );


			if ( !strcmp( strItemType, "food" ) ) 
				itemInfo->type = ITEM_TYPE_FOOD;
			else
			if ( !strcmp( strItemType, "armor" ) ) 
				itemInfo->type = ITEM_TYPE_ARMOR;
			else
			if ( !strcmp( strItemType, "material" ) ) 
				itemInfo->type = ITEM_TYPE_MATERIAL;
			else
			if ( !strcmp( strItemType, "tool" ) ) 
				itemInfo->type = ITEM_TYPE_TOOL;
			else
			if ( !strcmp( strItemType, "weapon" ) ) 
				itemInfo->type = ITEM_TYPE_WEAPON;
			else
			if ( !strcmp( strItemType, "block" ) ) 
				itemInfo->type = ITEM_TYPE_BLOCK;

			itemInfo->atlasCoord = dw2ItemAtlasCoord.ToVec2f() * textureSize;

			while ( !strstr( line, "}" ) ) {
				fgets( line, 128, file );

				if ( strstr( line, "healthEffect" ) ) {
					sscanf_s( line, " healthEffect %d", &itemInfo->food_HealthEffect );
				}
				else
				if ( strstr( line, "helpsWith" ) )  {
					console->Print("a");
					sscanf_s( line, " helpsWith %d %s", 
								&itemInfo->tool_HelpsWithMultiplier,
								itemInfo->tool_HelpsWithName, 32 );
					console->Print("a");
				}
				else
				if ( strstr( line, "health " ) )  {
					console->Print("a");
					sscanf_s( line, " health %d", &itemInfo->health );
					console->Print("a");
				}
				else
				if ( strstr( line, "craftedAs" ) ) {
					REPEAT( 3 ) {
						fgets( line, 128, file );
						sscanf_s( line, " %d %s %d %s %d %s", 
							&itemInfo->craftMatrix[ i ][ 0 ].count,
							 itemInfo->craftMatrix[ i ][ 0 ].name, 32,
							&itemInfo->craftMatrix[ i ][ 1 ].count,
							 itemInfo->craftMatrix[ i ][ 1 ].name, 32,
							&itemInfo->craftMatrix[ i ][ 2 ].count,
							 itemInfo->craftMatrix[ i ][ 2 ].name, 32 );
					}
					fgets( line, 128, file );	// craftedAs closing '}'
				}
				else
				if ( strstr( line, "type" ) ) {
					sscanf_s( line, " type %s", szTmp, 64 );

					if ( !strcmp( szTmp, "melee" ) )
						itemInfo->wpn_armor_type = ITEM_WPN_MELEE;
					else
					if ( !strcmp( szTmp, "gun" ) )
						itemInfo->wpn_armor_type = ITEM_WPN_GUN;
					else
					if ( !strcmp( szTmp, "mine" ) )
						itemInfo->wpn_armor_type = ITEM_WPN_MINE;
					else
					if ( !strcmp( szTmp, "grenade" ) )
						itemInfo->wpn_armor_type = ITEM_WPN_GRENADE;
					else
					if ( !strcmp( szTmp, "launcher" ) )
						itemInfo->wpn_armor_type = ITEM_WPN_LAUNCHER;
					else
					if ( !strcmp( szTmp, "helmet" ) )
						itemInfo->wpn_armor_type = ITEM_ARMOR_HELMET;
					else
					if ( !strcmp( szTmp, "cuirass" ) )
						itemInfo->wpn_armor_type = ITEM_ARMOR_CUIRASS;
					else
					if ( !strcmp( szTmp, "greaves" ) )
						itemInfo->wpn_armor_type = ITEM_ARMOR_GREAVES;
					else
					if ( !strcmp( szTmp, "gauntlets" ) )
						itemInfo->wpn_armor_type = ITEM_ARMOR_GAUNTLETS;
					else
					if ( !strcmp( szTmp, "boots" ) )
						itemInfo->wpn_armor_type = ITEM_ARMOR_BOOTS;
				} else
				if ( strstr( line, "damages" ) )
					sscanf_s( line, " damages %d", &itemInfo->weapon_Damage );
				else
				if ( strstr( line, "armorPoints" ) )
					sscanf_s( line, " armorPoints %d", &itemInfo->armor_Points );
			}
		}
	}


	fclose( file );
}

Item_t* vsItemManager::SpawnItem( const char* itemInfoName, const vsVec3f& pos ) {
	Item_t* item = new Item_t();
	const ItemInfo_t* info = GetItemInfoByName ( itemInfoName );

	if ( info == NULL )
		return NULL;

	item->current_Health = info->health;
	item->id				 = spawnedItems.Length();
	item->info			 = info;
	item->inInventory	 = false;
	item->worldPosition	 = pos;
	item->timeElapsed	 = 0.0f;
	spawnedItems.Append( item );

	return item;
}

void vsItemManager::DeleteItem( int id ) {
	delete spawnedItems[ id ];
	spawnedItems.RemoveElementAt( id );
}

void vsItemManager::UpdateAndRenderAllItems( void ) {
	vsVec3f dir;

	shader->Bind( true );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, textureAtlas );
	shader->Uniform1i( "in_Texture", 0 );

	vsVec3i cell;
	float tort;
	ushort val;
	vsBox box;
	glBegin( GL_POINTS );
	REPU( spawnedItems.Length() ) {
		if ( !spawnedItems[ i ]->inInventory ) {

			spawnedItems[ i ]->timeElapsed += timer.GetDeltaTime();
			// vanish item if it's too old
			if ( spawnedItems[ i ]->timeElapsed >= 60.0f ) {
				DeleteItem( i );
				continue;
			}

			dir = player->GetPhysics()->GetCenterOfMass() - spawnedItems[ i ]->worldPosition;
			// approaching player
			if ( dir.Length() < 1.0f ) 
				spawnedItems[ i ]->worldPosition += 10.0f * dir.Normalized() * timer.GetDeltaTime();
			else {
				cell = game->world->GetCellOfPoint( spawnedItems[ i ]->worldPosition );
				tort = spawnedItems[ i ]->worldPosition.y - floorf( spawnedItems[ i ]->worldPosition.y );
				val = game->world->GetBlockOfCell( vsVec3i( cell.x, cell.y - 1, cell.z ) ).id;
				
				spawnedItems[ i ]->worldPosition.y -= 10.0f * timer.GetDeltaTime();

				if (  val != 0 && tort < 0.2f ) {
					spawnedItems[ i ]->worldPosition.y += 10.0f * timer.GetDeltaTime();
				}

				if ( spawnedItems[ i ]->timeElapsed < 1.0f )
					spawnedItems[ i ]->worldPosition.y += 0.1f;
				//else
					//spawnedItems[ i ]->worldPosition.y += 0.2f;//0.005f * Sin( timer.GetDeltaTime() * 10.0f );
				
			}

			// if item's position is inside player's radius
			if ( ( ( vsPhysics_Ellipsoid* )player->GetPhysics() )->IsPointInside( spawnedItems[ i ]->worldPosition ) ) {
				JumpToInventory( i );
			}

			glMultiTexCoord2fv( GL_TEXTURE0, spawnedItems[ i ]->info->atlasCoord.ToTypePtr() );
			glVertexAttrib2fv( 1, spawnedItems[ i ]->info->atlasCoord.ToTypePtr() );
			glVertexAttrib3fv( 0, spawnedItems[ i ]->worldPosition.ToTypePtr() );
		}
	}
	glEnd();
	shader->Bind( false );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void vsItemManager::JumpToInventory( int itemId ) {
	player->GetInventory()->AddItem( spawnedItems[ itemId ] );
}

const ItemInfo_t* vsItemManager::GetItemInfoByName( const char * name ) const {
	REPU( itemInfos.Length() )
		if ( !strcmp( itemInfos[ i ]->name, name ) )
			return itemInfos[ i ];

	return NULL;
}