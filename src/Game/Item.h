#ifndef __VS_ITEM2_H__
#define __VS_ITEM2_H__

#include "..\vsLib2\vsLib2.h"


enum ItemType_t {
	ITEM_TYPE_FOOD = 1,
	ITEM_TYPE_ARMOR,
	ITEM_TYPE_MATERIAL,
	ITEM_TYPE_WEAPON,
	ITEM_TYPE_TOOL,
	ITEM_TYPE_BLOCK
};

enum {
	ITEM_WPN_MELEE = 1,		// swords, machette, etc.
	ITEM_WPN_RANGED,		// bows, bowcaster, etc.
	ITEM_WPN_GUN,			// pistol, M4, minigun, etc.
	ITEM_WPN_MINE,			// placeable mines
	ITEM_WPN_GRENADE,		// throwable weapons
	ITEM_WPN_LAUNCHER		// rocket, grenade, etc. launchers
};

enum {
	ITEM_ARMOR_HELMET = 1,
	ITEM_ARMOR_CUIRASS,
	ITEM_ARMOR_GAUNTLETS,
	ITEM_ARMOR_GREAVES,
	ITEM_ARMOR_BOOTS
};

/*
=========================================
	craftItem_t
=========================================
*/
struct craftItem_t {
	char	name[ 32 ];
	int		count;

	craftItem_t( void ) : count( 0 ) {};
	craftItem_t( char* iname, int icount ) { 
		strcpy_s<32>( name, iname ); 
		count = icount;
	};
	~craftItem_t( void ) {};
};

/*
=========================================
	ItemInfo_t
=========================================
*/
struct ItemInfo_t {
	int			id;
	ItemType_t	type;						// type of item ( food, armor, material, weapon, tool )
	vsVec2f		atlasCoord;					// texcoord to item texture atlas
	char		name[ 32 ];					// name of the item
	craftItem_t	craftMatrix[3][3];			// holds info about how this item can be crafted
	short		health;						// health of the item ( armor & tool )
	short		wpn_armor_type;				// if item is weapon or armor, holds the type of it

	// FOOD -------------------------------------
	short		food_HealthEffect;			// how food affects health points ( can be negative )
	
	// ARMOR ------------------------------------
	short		armor_Points;				// how many armor points it gives you

	// MATERIAL ---------------------------------
	// empty								// materials do not have additional properties

	// WEAPON -----------------------------------
	short		weapon_Damage;				// what damage the weapon can cause
	
	// TOOL -------------------------------------
	char		tool_HelpsWithName[ 32 ];	// which type of block can be smashed more easily
	byte		tool_HelpsWithMultiplier;	// smashing multiplier

	EMPTY_TRUCTORS( ItemInfo_t );
	MINIMUM_OPERATORS( ItemInfo_t );
};

/*
=========================================
	Item_t
=========================================
*/
struct Item_t {
	int					id;							// index to itemManager's array
	vsVec2i				inventorySlot;				// indices to inventory's array
	bool				inInventory;				// if TRUE, item does not exist in real world, just inside player inventory
	int					current_Health;				// optional: current health of the item
	const ItemInfo_t*	info;						// pointer to the item's type information
	vsVec3f				worldPosition;				// if not in inventory -> position in the world
	float				timeElapsed;				// time since spawn. item vanishes at timeElapsed >= 60.0

	EMPTY_TRUCTORS( Item_t );
	MINIMUM_OPERATORS( Item_t );
};

/*
=========================================

	vsItemManager
		Note:	Maybe it would be a good idea to use 
		   vertex buffers and arrays to render items

=========================================
*/
class vsItemManager {
public:
							vsItemManager( vsShader * shader );
							~vsItemManager( void );
	
	void					LoadItemLibrary( const char* szFileName );	// loads item infos from text file
	Item_t*					SpawnItem( const char* itemInfoName, const vsVec3f& pos );		// constructs an item based on itemInfo
	void					DeleteItem( int id );						// deletes item with id 'id'

	void					UpdateAndRenderAllItems( void );					// renders items which are not in inventory
	const ItemInfo_t*		GetItemInfoByName( const char * name ) const;		// returns item info
	const float&			GetTextureSize( void ) const;						// returns one item's texture dimensions
	const uint&				GetTextureAtlas( void ) const;						// returns id to atlas
	const char*				GetTextureAtlasFileName( void ) const;				// returns filename of atlas
	void					JumpToInventory( int itemId );				// when player gets nearby, item jumps into inventory
private:
	vsList< ItemInfo_t* >	itemInfos;									// used to create new items
	vsList< Item_t* >		spawnedItems;								// already created items

	char					textureAtlasFileName[ 64 ];					// file name of textureAtlas
	uint					textureAtlas;								// id to textureAtlas
	float					textureSize;								// 1.0f / numTexturesPerRow( Column )
	vsShader*				shader;
};


extern vsItemManager * itemManager;


VS_INLINE const float& vsItemManager::GetTextureSize( void ) const {
	return textureSize;
}

VS_INLINE const uint& vsItemManager::GetTextureAtlas( void ) const {
	return textureAtlas;
}

VS_INLINE const char* vsItemManager::GetTextureAtlasFileName( void ) const {
	return textureAtlasFileName;
}

#endif