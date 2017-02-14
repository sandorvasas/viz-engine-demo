#include "Game.h"
#include "Player.h"
#include "WorldGenerator.h"
#include "Physics\PhysicsManager.h"
#include "..\System\ConsoleProcessor.h"
#include "Geometry\ModelManager.h"
#include "Blocks.h"
#include "Item.h"
#include "Timer.h"
#include "PSystem.h"
#include <stdio.h>

vsTimer timer;

vsGame* game;

/* 
=====================================
	vsGame
=====================================
*/
vsGame::vsGame( void )
{
	savesDeleted = false;
}

/* 
=====================================
	~vsGame
=====================================
*/
vsGame::~vsGame( void )
{
}

/* 
=====================================
	NewGame
=====================================
*/
void vsGame::Initialize( void )
{
	printer->NewFont( "Courier New", 16, true, false, false, false );

	input->SetMousePos( 	videoMode->iScreenCenterX, 
						videoMode->iScreenCenterY );

	
	//-------------------------------------------------------------------------------------------
	//	Shaders
	//-------------------------------------------------------------------------------------------
	shaders.Alloc( 7 ) ;
	shaders[ 0 ] = renderSystem->LoadShaderFromFile( "shader_grass",		"data/shaders/grass.txt", 0,0 );
	shaders[ 1 ] = renderSystem->LoadShaderFromFile( "shader_sky",		"data/shaders/sky.txt", 0, 0 );
	shaders[ 2 ] = renderSystem->LoadShaderFromFile( "shader_landscape","data/shaders/landscape.txt", 0, 0 );
	shaders[ 3 ] = renderSystem->LoadShaderFromFile( "shader_particle",	"data/shaders/particle.txt", 0, 0 );
	shaders[ 4 ] = renderSystem->LoadShaderFromFile( "shader_modelmd5",	"data/shaders/md5model.txt", 0, 0 );
	shaders[ 5 ] = renderSystem->LoadShaderFromFile( "shader_lensflare","data/shaders/lensflare.txt", 0, 0 );
	shaders[ 6 ] = renderSystem->LoadShaderFromFile( "shader_item",		"data/shaders/item.txt", 0, 0 );

	/*********************************/
	//	Allocate memory for objects
	/*********************************/
	physicsManager		= new vsPhysicsManager();
	worldGenerator		= new vsWorldGenerator();
	world				= new vsWorld();
	player				= new vsPlayer();
	modelManager		= new vsModelManager();
	itemManager			= new vsItemManager( shaders[ 6 ] );
	weaponLibrary		= new vsWeaponLibrary();
	blockLibrary		= new vsBlockLibrary();
	pSystem				= new vsPSystem();

	itemManager->LoadItemLibrary( "data/items.txt" );
	modelManager->LoadMaterials( "data/models/materials.mtr" );
	weaponLibrary->LoadLibrary( 	"data/weapons.txt" );
	blockLibrary->LoadLibrary( "data/blocks.txt" );

	if ( playerArgs.v3Origin == vsVec3f( 0.0f ) ) 
		playerArgs.v3Origin		= vsVec3f( 35.0f, 20.0f, 35.0f );

	playerArgs.fSpeed			= 6.0f;
	playerArgs.v2LookingSpeed	= vsVec2f( 0.1f );
	playerArgs.v3RotationDeg	= vsVec3f( 0.0f, 135.0f, 0.0f );
	playerArgs.bSpectating		= false;
	playerArgs.v3Radius			= vsVec3f( 0.3f, 1.72f, 0.3f );
	playerArgs.fViewDistance	= 10000.0f;
	playerArgs.pWorld			= world;
	player->Init( playerArgs );
	player->SetFieldOfView( PI * 0.6f, 0.75f, 0.01f );

	LoadedPercent( 30.0f );
}

/* 
=====================================
	NewGame
=====================================
*/
void vsGame::NewGame( void )
{
	world->Initialize();

	DeleteAllSaveFiles();
	
	// It's VERY important to save here, because spawning a player
	// requires an existing save file.
	SaveToFolder( "data/saves/user1" );

	world->SpawnPlayer();

	timer.Init();
}

/* 
=====================================
	Load
=====================================
*/
void vsGame::Load( void )
{
	RestoreFromFolder( "data/saves/user1" );

	world->SpawnPlayer();

	timer.Init();
}

/* 
=====================================
	LoadedPercent
=====================================
*/
void vsGame::LoadedPercent( float p )
{
	loadPercent = p;
	//UpdateLoadScreen( NULL );
}


/* 
=====================================
	Shutdown
=====================================
*/
void vsGame::Shutdown( void )
{
	TerminateThread( hLoaderThread, 0 );

	shaders.Free();

	delete pSystem;
	delete worldGenerator;
	delete world;
	delete player;
	delete weaponLibrary;
	delete itemManager;
	delete physicsManager;
	delete modelManager;
}


/* 
=====================================
	RenderToBuffer
=====================================
*/
void vsGame::RenderToBuffer( void )
{	
	//-------------------------------------------------------------------------------------------
	// Uniforming Shaders
	//-------------------------------------------------------------------------------------------
	REPU( shaders.Length() ) 
	{
		shaders[i]->Bind( true );
		shaders[i]->Uniform1f( "in_Time", timer.GetTime() );
		shaders[i]->Uniform3v( "in_ViewPosition", player->GetPhysics()->GetCenterOfMass() );
		shaders[i]->Uniform3v( "in_LightPosition", world->GetSunPosition() );
	//	shaders[i]->Uniform3v( "in_LightPosition",  player->GetPhysics()->GetCenterOfMass() );
	}
	glUseProgram( 0 );

	player->Look();

	/*glColor3f( 0.0f, 0.5f, 0.3f );
	glPointSize( 10.0f );
	glBegin( GL_POINTS );
	glColor3f( 1.0f, 1.0f, 1.0f );
	glVertex3fv( player->GetLookingPoint( 2.0f ).ToTypePtr() );
	glEnd();*/

	world->Render();
	itemManager->UpdateAndRenderAllItems();
	player->Render();
	//physicsManager->destCell.RenderBox( false );

	//renderSystem->SwapBuffers();
}

/* 
=====================================
	RenderHUD
=====================================
*/
void vsGame::RenderHUD( void ) 
{
	printer->PrintQueue( true );
	printer->Print( 1, -0.97f, 0.95f, 0.8f, vsColors::White, true, "FPS: %d", renderSystem->FPSCounter.GetCurrent() );
	printer->Print( 1, -0.97f, 0.9f, 0.8f, vsColors::White,	true, "Origin: %s", player->GetPhysics()->GetCenterOfMass().ToCharPtr() );
	printer->Print( 1, -0.97f, 0.85f, 0.8f, vsColors::White, true, "Rotation: %s", player->GetRotation().ToCharPtr() );

	player->GetView()->Render();
}

/* 
=====================================
	UpdatePhysics
=====================================
*/
void vsGame::Update( void )
{
	timer.Update();
	//physicsManager->Update();

	//---- player physics ----
	world->CollideWithWorld();
	// ---- eof player physics -----
	player->MoveToDestination();
	player->Update();
	world->Update();
}

/* 
=====================================
	ProcessKeys
=====================================
*/
void vsGame::ProcessKeys( void )
{
	if ( console->visible == false )
	{
		if ( input->IsKeyDown( 'W' ) )	player->GoForward();
		if ( input->IsKeyDown( 'S' ) )	player->GoBackward();
		if ( input->IsKeyDown( 'A' ) )	player->StrafeLeft();
		if ( input->IsKeyDown( 'D' ) )	player->StrafeRight();
		if ( input->IsKeyDown( 'E' ) )	player->LiftUp();
		if ( input->IsKeyDown( 'Q' ) )	player->LiftDown();
		if ( input->IsKeyDown( VK_SPACE ) )	{
			player->Jump();
			input->KeyUp( VK_SPACE );
		}

		if ( input->IsKeyDown( 'F' ) ) {
			static viewDistance_t vds[] = { VIEWDIST_TINY, VIEWDIST_SHORT, VIEWDIST_NORMAL,
											VIEWDIST_FAR, VIEWDIST_REALISTIC };
			static byte next = 0;
			world->SetViewDistance( vds[ ( ++next ) % 5 ] );
			input->KeyUp( 'F' );
		}
		
		

		// WEAPON SELECTION ================================================================
		
		if ( input->IsKeyDown( VK_LEFT ) ) {
			player->GetInventory()->PrevItem();
			input->KeyUp( VK_LEFT );
		}
		if ( input->IsKeyDown( VK_RIGHT ) ) {
			player->GetInventory()->NextItem();
			input->KeyUp( VK_RIGHT );
		}
		/*
#define chwpn( __x ) \
		if ( input->IsKeyDown( '__x' ) ) { \
			player->GetInventory()->ChangeItemTo( __x ); \
			input->KeyUp( '__x' ); } 
		
		REPEAT( 9 )
			chwpn( i );
#undef chwpn	*/
		// =================================================================================

		if ( input->IsKeyDown( 'G' ) ) {
			///world->explosion.Boom( player->GetLookingPoint( 20.0f ),
				//				  vsVec3f( 10.0f ), 30.0f, NULL );
			input->KeyUp( 'G' );
		}

		// DEBUG TREE =======================================================================
		static int age=1, branches=2;
		bool b = false;
		if ( input->IsKeyDown( VK_NUMPAD9 ) ) {
			branches++;
			b=true;
			input->KeyUp( VK_NUMPAD9 );
		}
		if ( input->IsKeyDown( VK_NUMPAD8 ) ) {
			branches--;
			b=true;
			input->KeyUp( VK_NUMPAD8 );
		}
		if ( input->IsKeyDown( VK_NUMPAD6 ) ) {
			age++;
			b=true;
			input->KeyUp( VK_NUMPAD6 );
		}
		if ( input->IsKeyDown( VK_NUMPAD5 ) ) {
			age--;
			b=true;
			input->KeyUp( VK_NUMPAD5 );
		}
	}

	if ( input->IsMouseButtonDown( MOUSE_RIGHT ) )
	{
		player->Use();
		input->MouseButtonUp( MOUSE_RIGHT );
	} else 
	if ( input->IsMouseButtonDown( MOUSE_LEFT ) )
	{	
		player->StartFiring();
		//input->MouseButtonUp( MOUSE_LEFT );
	} else {
		player->StopFiring();	
	}


	


	console->CheckInput( input->keys );
	consoleProcessor.Hz();
}

/* 
=====================================
	SaveToFolder
=====================================
*/
void vsGame::SaveToFolder( char* szFolderName )
{
	//_chdir( szFolderName );

	char szFileName[128];
	ZeroMemory( szFileName, 128 );
	sprintf_s<128>( szFileName, "%s/game.svg", szFolderName );

	vsOutputFile f;
	if ( f.Open( szFileName, "wb+" ) == VS_ERROR )
	{
		console->Print( CMT_ERROR, "Failed to save game to %s", szFileName );
		return;
	}

	player->SaveToFile( &f );
	f.Close();

	world->SaveToFolder( szFolderName );
}

/* 
=====================================
	RestoreFromFolder
=====================================
*/
void vsGame::RestoreFromFolder( char* szFolderName )
{
	char szFileName[128];
	ZeroMemory( szFileName, 128 );
	sprintf_s<128>( szFileName, "%s/game.svg", szFolderName );

	vsInputFile f;
	if ( f.Open( szFileName, "rb" ) == VS_ERROR )
	{
		console->Print( CMT_ERROR, "Failed to load game from %s. Starting new game.", szFileName );
		NewGame();
		return;
	}

	player->RestoreFromFile( &f );
	f.Close();

	world->RestoreFromFolder( szFolderName );	
}


/* 
=====================================
	DeleteAllSaveFiles
=====================================
*/
void vsGame::DeleteAllSaveFiles( void )
{
	WIN32_FIND_DATAA finddata;
	HANDLE hFile = FindFirstFile( "data/saves/user1/*", &finddata );

	char txt[ 128 ];
	while ( FindNextFile( hFile, &finddata ) )
	{
		sprintf_s<128>( txt, "data/saves/user1/%s", finddata.cFileName );
		DeleteFile( txt );
	}
	FindClose( hFile );
	RemoveDirectory( "./data/saves/user1" );
	CreateDirectory( "./data/saves/user1", NULL );

	savesDeleted = true;
}

/* 
=====================================
	LoaderThread
=====================================
*/
static DWORD WINAPI LoaderThread( void* )
{
	/*CRITICAL_SECTION s;
	vsWorld* w = game->world;
	int two = 2;

	InitializeCriticalSection( &s );
	EnterCriticalSection( &s );

	while ( 1 < two )
	{
		if ( w->requestLoad ) {

			w = game->world;

			REPU ( w->sectorLoadRequests.Length() )
			{
				w->GenLoadSector( w->sectorLoadRequests[i].sectorIndex,
								 w->sectorLoadRequests[i].coord.x,
								 w->sectorLoadRequests[i].coord.y,
								 w->sectorLoadRequests[i].coord.z );
				Sleep( 0 );
			}

			w->sectorLoadRequests.Free();
			w->FetchAllSectorNeighbors();
			w->requestLoad = false;
		}
		

		Sleep( 50 );
	}

	LeaveCriticalSection( &s );
	DeleteCriticalSection( &s );
	*/
	return 0;
}

/* 
=====================================
	UpdateLoadScreen
=====================================
*/
static DWORD WINAPI UpdateLoadScreen( void* n )
{
	UNREFERENCED_PARAMETER( n );
//	int i=true;
	//while( i ) 
	{
		//if ( n ) i=false;
	//	Beep( ( DWORD )game->loadPercent*20, 100 );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glLoadIdentity();

	printer->Print( 1, -0.05f, -0.95f, -0.98f, vsColors::White, true, "LOADING" );

	renderSystem->OrthoMode( ON );
	console->RenderTexts( -0.5f, -0.85f );
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_QUADS );
		glVertex3f( -0.4f, -0.98f, -0.98f );
		glVertex3f( -0.4f+game->loadPercent*0.01f, -0.98f, -0.98f );
		glVertex3f( -0.4f+game->loadPercent*0.01f, -0.96f, -0.98f );
		glVertex3f( -0.4f, -0.96f, -0.98f );
	glEnd();

	renderSystem->OrthoMode( OFF );
	renderSystem->SwapBuffers();

	}
	return 0;
}
