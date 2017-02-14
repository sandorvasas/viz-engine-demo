#ifndef __GAME_H__
#define __GAME_H__


#include "..\vsLib2\vsLib2.h"
#include "World.h"
#include "Player.h"

class vsGame {

	friend class vsConsoleProcessor;

public:
	vsGame( void );
	~vsGame( void );

	void				Initialize( void );
	void				NewGame( void );
	void				Load( void );
	void				Shutdown( void );
	void				SaveToFolder( char* szFolderName );
	void				RestoreFromFolder( char* szFolderName );
	void				DeleteAllSaveFiles( void );
	void				LoadedPercent( float p );

	void				RenderToBuffer( void );
	void				RenderHUD( void );
	void				Update( void );
	void				ProcessKeys( void );
	//void				RenderToBuffer( void );

	friend static DWORD WINAPI UpdateLoadScreen( void* );
	friend static DWORD WINAPI LoaderThread( void* );


	vsShader*			GetShader( uint id )			{ return shaders[ id ]; };
	const uint&			GetTerrainAtlas( void ) const;
	const uint&			GetItemAtlas( void ) const;

public:
	vsWorld*			world;	
	playerSettings_t	playerArgs;
	// LOADING SCREEN
	float				loadPercent;
	HANDLE				hLoadThread ;
	HANDLE				hLoaderThread;
	bool				savesDeleted;

	uint				texAtlasItem;
	uint				texAtlasTerrain;
	
private:
	vsList<vsShader*>	shaders;
	
};

extern vsGame* game;


VS_INLINE const uint& vsGame::GetTerrainAtlas( void ) const {
	return texAtlasTerrain;
}

VS_INLINE const uint& vsGame::GetItemAtlas( void ) const {
	return texAtlasItem;
}

#endif