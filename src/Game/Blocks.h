#ifndef __VS_BLOCKS_H__
#define __VS_BLOCKS_H__


#include "..\vsLib2\vsLib2.h"
#include "Geometry\ModelManager.h"


enum blockflags_t {
	BLOCK_FLAG_EATABLE		= 0x00000001,
	BLOCK_FLAG_LIQUID		= 0x00000010,
	BLOCK_FLAG_FLAMMABLE	= 0x00000100,
	BLOCK_FLAG_NONSOLID		= 0x00001000,
	BLOCK_FLAG_DROPS_ITEM	= 0x00010000,
	BLOCK_FLAG_TRANSPARENT	= 0x00100000
};

struct blocktype_t {
	char			name[ 32 ];				// name of the block
	vsVec2ui		tcTop;					// TOP texture coords
	vsVec2ui		tcBottom;				// BOTTOM texture coords
	vsVec2ui		tcSide;					// SIDE texture coords
	vsVec2ui		tcFront;				// FRONT texture coords
	float			weight;
	uint			life;
	ushort			damages;				// like lava, cactus, etc
	int				flags;

	MD5ModelArgs_t*	modelargs;				// optional: used if block is NONSOLID
	char			animFileName[ 64 ];		// optional: used if block is NONSOLID

	char			dropItemName[ 32 ];		// optional: if block drops item, it's the name of it
	int				dropChance;				// optional: chance ( 0..100 ) to drop the item
	int				dropItemCount;			// optional: number of items to drop

	blocktype_t( void ) {
		weight = 0.0f;
		life = 0;
		damages = 0;
		flags = 0;
		modelargs = NULL;
	}
};


enum blockextra_t {
	BLOCK_DIR_NORTH = 5,
	BLOCK_DIR_SOUTH = 6,
	BLOCK_DIR_WEST  = 7,
	BLOCK_DIR_EAST  = 8
};

struct block_t {
	ushort		id;
	byte		extra;
	byte		ambient;
//	int			vertexIndex;
//	byte		vertexCount;

	block_t( ushort _id, byte _extra ) : id( _id ), extra( _extra ) {};
	EMPTY_TRUCTORS( block_t );
	MINIMUM_OPERATORS( block_t );
};

class vsBlockLibrary {
public:
							vsBlockLibrary( void );
							~vsBlockLibrary( void );

	void					LoadLibrary( const char* szFileName );
	ushort					GetBlockTypeId( const char* blockTypeName ) const;
	const blocktype_t*		GetBlockType( const char * blockTypeName ) const;
	const blocktype_t*		GetBlockType( ushort blockTypeId ) const;
	const blocktype_t*		GetBlockType( const block_t& blockTypeId ) const;

	const uint&				GetTextureAtlas( void ) const;
	const uint&				GetTextureAtlasBump( void ) const;
	const float&			GetTextureSize( void ) const;

private:
	void					SortBlocksByName( void );

private:
	vsList<blocktype_t*>	blocks;

	uint					textureAtlas;
	uint					textureAtlasBump;
	float					textureSize;
};

extern vsBlockLibrary * blockLibrary;



VS_INLINE const uint& vsBlockLibrary::GetTextureAtlas( void ) const {
	return textureAtlas;
}

VS_INLINE const uint& vsBlockLibrary::GetTextureAtlasBump( void ) const {
	return textureAtlasBump;
}

VS_INLINE const float& vsBlockLibrary::GetTextureSize( void ) const {
	return textureSize;
}

#endif