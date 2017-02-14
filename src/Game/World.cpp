#include "World.h"
#include "Player.h"
#include "WorldGenerator.h"
#include "Physics\PhysicsManager.h"
#include "Geometry\Grass.h"
#include "Geometry\Sector2.h"
#include "Geometry\ModelManager.h"
#include "..\System\VarStorage.h"
#include "PSystem.h"
#include "Game.h"
#include <process.h>

using namespace vsMath;



/*
===========================
	Constructor
===========================
*/
vsWorld::vsWorld( void )
{
	numChunks = 0;
	saveFolder = "";
	playerSpawned = false;
	request.available = false;
	eofChunkLoaderThread = false;
	gravity = vsVec3f( 0.0f, -9.8f, 0.0f );
	ssChunks.Init();
}

/*
===========================
	Destructor
===========================
*/
vsWorld::~vsWorld( void )
{
	eofChunkLoaderThread = true;

	Sleep( 100 );

	ssChunks.Enter();
	REPU ( chunks.Length() )
	{
		if ( chunks[ i ]->Modified() )
			chunks[ i ]->SaveToFolder( saveFolder );
	}
	ssChunks.Leave();

	chunks.DeletePointerElements();
	chunks.Free();

	inactiveChunks.DeletePointerElements();
	inactiveChunks.Free();

	monsters.DeletePointerElements();
	monsters.Free();

	delete particleSystem;

	ssChunks.Delete();
}

/*
===========================
	Initialize()
===========================
*/
void vsWorld::Initialize(  void  )
{
	// View distance----------------------------------------------------------------------------------
	SetViewDistance( VIEWDIST_TINY );

	//-------------------------------------------------------------------------------------------
	//	Sky
	//-------------------------------------------------------------------------------------------
	sky.BuildGeometry( 8,8, 
					  vsVec3f( 1000.0f ), 
					  renderSystem->LoadTexture( "data/textures/clouds.dds" ),
					  game->GetShader( 1 ) );

	//-------------------------------------------------------------------------------------------
	//	World Generator Parameters
	//-------------------------------------------------------------------------------------------
	worldParams_t wgp;
	wgp.seaLevel = 10;
	worldGenerator->SetParameters( wgp );

	//-------------------------------------------------------------------------------------------
	curChunkCoords = GetChunkCoordOfPoint( player->GetPhysics()->GetCenterOfMass() );

	lightOffset = vsVec3f( 35.0f, 30.0f, 35.0f );
	//-------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------
	//	Monsters
	//-------------------------------------------------------------------------------------------
	monsters.Alloc( 2 );
	MD5ModelArgs_t margs; 

	// HELL KNIGHT
	vsList< vsAnimation_MD5* > hKnightAnim( 3 );
	hKnightAnim[ 0 ] = //modelManager->LoadAnim( "data/models/hellknight/anim/stand.md5anim" );
	hKnightAnim[ 1 ] = //modelManager->LoadAnim( "data/models/hellknight/anim/walk7.md5anim" );
	hKnightAnim[ 2 ] = modelManager->LoadAnim( "data/models/test1anim.md5anim" );
	margs.szFileName = "data/models/test1.md5mesh";
	margs.shader	 = game->GetShader( 4 );
	margs.rotation	 = vsVec3f( -90.0f, 0.0f, 0.0f );
	margs.scale		 = vsVec3f( 0.03f );
	margs.offset	 = vsVec3f( 0.0f, 0.0f, 0.0f );
	monsters[0]		 = new vsMonster( margs, hKnightAnim );
	monsters[0]->Spawn( vsVec3f( 310.8f, 3.5f, 300.8f ), NULLVEC3F );
	hKnightAnim.Free();

	// FATTY
	vsList< vsAnimation_MD5* > zFattyAnim( 3 );
	zFattyAnim[ 0 ]  = modelManager->LoadAnim( "data/models/zfat/anim/idle1.md5anim" );
	zFattyAnim[ 1 ]  = modelManager->LoadAnim( "data/models/zfat/anim/walk1.md5anim" );
	zFattyAnim[ 2 ]  = modelManager->LoadAnim( "data/models/zfat/anim/attack3.md5anim" );
	margs.szFileName = "data/models/zfat/zfat.md5mesh";
	margs.shader	 = game->GetShader( 4 );
	margs.rotation	 = vsVec3f( -90.0f, 0.0f, 60.0f );
	margs.scale		 = vsVec3f( 0.03f );
	margs.offset	 = vsVec3f( 0.0f, 0.0f, 0.0f );
	monsters[1]		 = new vsMonster( margs, zFattyAnim );
	monsters[1]->Spawn( vsVec3f( 303.4f, 2.5f, 303.8f ), NULLVEC3F );
	zFattyAnim.Free();

	//-------------------------------------------------------------------------------------------
	//	Particle System
	//-------------------------------------------------------------------------------------------
	particleSystem = new vsParticleSystem( game->GetShader( 3 ) );

	uint _tmp334 = renderSystem->LoadTexture( "data/textures/particles/snowflake.dds" );

	pstp_t etp;
	etp.id = 1;
	etp.textures[0] = renderSystem->LoadTexture( "data/textures/particles/01.dds" );
	etp.textures[1] = renderSystem->LoadTexture( "data/textures/particles/05.dds" );
	etp.textures[2] = renderSystem->LoadTexture( "data/textures/particles/04.dds" );
	particleSystem->AddTexturePack( etp );

	//particleSystem->SnowStorm( NULLVEC3F, _tmp334 );

	//-------------------------------------------------------------------------------------------
	// DAY TIME
	//-------------------------------------------------------------------------------------------
	dayTime = 8.0f;

	printer2->NewFont( "font1", renderSystem->LoadTexture( "data/textures/fonts/font1.dds" ), 40.0f );


	//-------------------------------------------------------------------------------------------
	//	Lens Flare
	//-------------------------------------------------------------------------------------------
	uint lf_tex[ 4 ] = {
		renderSystem -> LoadTexture( "data/textures/particles/03.dds" ),
		renderSystem -> LoadTexture( "data/textures/particles/07.dds" ),
		renderSystem -> LoadTexture( "data/textures/particles/08.dds" ),
		renderSystem -> LoadTexture( "data/textures/particles/09.dds" )
	};
	fx_lensFlare.Initialize( lf_tex, game->GetShader( 5 ) );

	//-------------------------------------------------------------------------------------------
	// Debugging Stuff
	//-------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------
	// Default weapon
	//-------------------------------------------------------------------------------------------
//	player->SetWeapon( weaponLibrary->CreateWeapon( "fists" ) );


	//-------------------------------------------------------------------------------------------
	// Chunk Loader Thread Init
	//-------------------------------------------------------------------------------------------
	uint tid;
	chunkLoaderThread = ( HANDLE )_beginthreadex( NULL, 0, &GhostChunkLoaderThreadProc, NULL, 0, &tid );
	//-------------------------------------------------------------------------------------------

	saveFolder = "data/saves/user1/";
}

/*
===========================
	Render()
===========================
*/
void vsWorld::Render( void )
{
	if ( !playerSpawned ) 
		return;

	static int numvisible, numvisibleleaf;
	printer->PutInQueue( 1, -0.97f, 0.50f, 0.1f, vsColors::Green, "Time: %2.2f", dayTime );
	//printer->PutInQueue( 1, -0.97f, 0.8f, 0.8f, vsColors::White,"Local cell: %s", GetSectorOfPoint( player->GetPhysics()->GetCenterOfMass() )->GetLocalCellOfPoint( player->GetPhysics()->GetCenterOfMass() ).ToCharPtr() );
	printer->PutInQueue( 1, -0.97f, 0.75f, 0.8f, vsColors::White,"Absolute cell: %s", GetCellOfPoint( player->GetPhysics()->GetCenterOfMass() ).ToCharPtr() );
	printer->PutInQueue( 1, -0.97f, 0.70f, 0.8f, vsColors::Black, "numVisibleNodes: %d", numvisible );
	printer->PutInQueue( 1, -0.97f, 0.65f, 0.8f, vsColors::Black, "numVisibleLeafNodes: %d", numvisibleleaf );
	printer->PutInQueue( 1, -0.97f, 0.55f, 0.8f, vsColors::Purple, "sector Coords: %s", curChunkCoords.ToCharPtr() );
	//printer->PutInQueue( 1, -0.97f, 0.55f, 0.8f, vsColors::Purple, "sectorIndex: %s", );

	//printer2->Print( 0, 0.0f, 10.0f, vsColors::Green, "Hello Soncraft!" );

	//glClearColor( 0.529411765f, 0.807843137f, 0.980392157f, 1.0f );
	glClearColor( 0.8f, 0.8f, 0.8f, 1.0f );

	//-------------------------------------------------------------------------------------------
	// Space Basis Lines
	//-------------------------------------------------------------------------------------------
	glLineWidth( 3.0f );
	glBegin( GL_LINES );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 10000.0f, 0.0f, 0.0f );

		glColor3f( 0.0f, 1.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 10000.0f, 0.0f );

		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 10000.0f );
	glEnd();

	//-------------------------------------------------------------------------------------------
	// Light
	//-------------------------------------------------------------------------------------------
	glPointSize( 10.0f );
	glColor3f( 1.0f, 0.0f, 0.0f );
	glBegin( GL_POINTS );
		glVertex3fv( lightPos.ToTypePtr() );
	glEnd();

	//-------------------------------------------------------------------------------------------
	// Rendering everything
	//-------------------------------------------------------------------------------------------
	sky.Render( dayTime );

	REPU ( monsters.Length() ) 
		monsters[ i ]->Render();
	
	numvisible = numvisibleleaf = 0;

	ssChunks.Enter();
		REPU ( chunks.Length() ) {
			chunks[ i ]->Render();
		}
	ssChunks.Leave();
	



	//-------------------------------------------------------------------------------------------
	// Transparent things
	//-------------------------------------------------------------------------------------------
	//particleSystem->Render();
	pSystem->UpdateAndRender();

	fx_lensFlare.Render( sky.GetSunPosition( dayTime ) );
}

/*
===========================
	SortChunks
===========================
*/
void vsWorld::SortChunks( void )
{
	if ( !chunks.Length() )
		return;

	vsList<float>	distances( chunks.Length() );

	REPU( chunks.Length() ) {
		distances[ i ] = ( player->GetPhysics()->GetDestination() - chunks[ i ]->GetBox().GetCenter() ).Length();
	}

	uint i, j = 0;
	float ftmp;
	vsChunk* ctmp;
	bool swapped;

	do {
		swapped = false;

		for ( i = 0; i < chunks.Length()-1; i++ ) {
			if ( distances[ i ] < distances[ i + 1 ] ) {
				vsSystem::Swap( distances[ i ], distances[ i + 1 ] );
				vsSystem::Swap( chunks[ i ], chunks[ i + 1 ] );
				swapped = true;
			}
		}
	} while( swapped );

	REPU( chunks.Length() )
		if ( chunks[ i ]->GetChunkCoords() == curChunkCoords )
			curChunkIndex = i;
}

/*
===========================
	Update()
===========================
*/
void vsWorld::Update( void )
{
	static vsVec3i last = GetChunkCoordOfPoint( player->GetPhysics()->GetDestination() );

	vsVec3i c = GetChunkCoordOfPoint( player->GetPhysics()->GetDestination() );

	if ( last != c )
	{
		last = c;
		curChunkCoords = c;
		request.available  = true;
	}

	if ( !playerSpawned ) 
		return;

	lightPos = lightOffset;
	particleSystem->UpdatePhysics();


	REPU ( monsters.Length() ) 
		monsters[ i ]->Update();

	dayTime = fmod( dayTime+0.1f*renderSystem->FPSCounter.GetInverseCurrent(), 24.0f );
	
}

/*
===========================
	SpawnPlayer
===========================
*/
void vsWorld::SpawnPlayer( void )
{
	curChunkCoords = GetChunkCoordOfPoint( player->GetPhysics()->GetDestination() );
	request.available  = true;

	playerSpawned = true;
}

/*
===========================
	SetZone()
===========================
*/
void vsWorld::SetZone( const vsVec3i& zone )
{
	static uint TimeBeforeSetZone, TimeAfterSetZone;

	TimeBeforeSetZone = GetTickCount();

	if ( !strlen( saveFolder ) ) 
	{
		console->Print( CMT_ERROR, "vsWorld::SetZone() says: Save file could not be found." );
		return;
	}

	curChunkCoords = zone;

	int k = 0;
	vsVec3f bmin, bmax;
	vsVec3i off;
	for ( int x = -chunkViewDist / 2; x <= chunkViewDist / 2; x++ )
	for ( int y = -chunkViewDist / 2; y <= chunkViewDist / 2; y++ )
	for ( int z = -chunkViewDist / 2; z <= chunkViewDist / 2; z++ ) {
		off = vsVec3i( x, y, z );
		bmin = ( ( curChunkCoords + off ) * chunkDim ).ToVec3f();
		chunkOffBoxes[ k ].box = vsBox( bmin, bmin + chunkDim.ToVec3f() );
		chunkOffBoxes[ k ].loaded = false;
		chunkOffBoxes[ k++ ].offset = off;
	}
		
	chunkOffBoxes.BubbleSort( true );

	//-------------------------------------------------------------------------------------------
	// Generating New Chunks
	//-------------------------------------------------------------------------------------------
	ssChunks.Enter();

	REPU ( chunks.Length() ) {
		inactiveChunks.Append ( chunks[ i ] );
	}
	chunks.Free();
	int indx;
	bool main;
	for ( uint i = 0; i < numChunks; ++i ) 
	{
		main = ( i == 0 );

		// searches among inactive render sectors
		indx = FindInactiveChunk( curChunkCoords + chunkOffBoxes[ i ].offset );
		
		if ( indx != -1 ) {
			chunks.Append( inactiveChunks[ indx ] );
			inactiveChunks[ indx ] = NULL;
		} else {	
			request.chunkLoad.Append( chunkRequest_t( main, curChunkCoords + chunkOffBoxes[ i ].offset ) );
		}
		
		chunkOffBoxes[ i ].loaded = true;
	}
	SortChunks();
	ssChunks.Leave();

	allChunksLoaded = false;

	vsChunk* tmp;
	REPU( request.chunkLoad.Length() ) {
		tmp = new vsChunk();
		LoadChunk( tmp,  request.chunkLoad[ i ].coord );

		ssChunks.Enter();
			chunks.Append( tmp );
			SortChunks();
		ssChunks.Leave();
	}
	request.chunkLoad.Free();
	//-------------------------------------------------------------------------------------------
	// Saving Chunks
	//-------------------------------------------------------------------------------------------
	REPU( inactiveChunks.Length() )
		if ( inactiveChunks[ i ] ) 
			if ( inactiveChunks[ i ]->Modified() ) 
						inactiveChunks[ i ]->SaveToFolder( saveFolder );				
		
	//-------------------------------------------------------------------------------------------
	inactiveChunks.DeletePointerElements();
	inactiveChunks.Free();
	//-------------------------------------------------------------------------------------------

	
	TimeAfterSetZone = GetTickCount();
	console->Print( "Zone loading took %5.4f", float( TimeAfterSetZone - TimeBeforeSetZone )*0.001f );
}


/*
===========================
	LoadChunk
===========================
*/
void vsWorld::LoadChunk( vsChunk* chunk, const vsVec3i& coords )
{
	bool _new = ( MapAt( coords ) == 0 );

	chunk->Generate( coords, ( _new ) ? NULL : saveFolder );

	if ( _new )	
		chunk->SaveToFolder( saveFolder );
}

/*
===========================
	CollideWithWorld
===========================
*/
void vsWorld::CollideWithWorld( void ) 
{
	ssChunks.Enter();
	vsChunk * chunk;
	vsVec3f velocity = player->GetPhysics()->GetVelocity();
	//chunk->TraceBox( ((vsPhysics_Player*)player->GetPhysics())->GetBounds(), velocity );	

	if ( !player->IsJumping() && !player->IsSpectating() )
		velocity += gravity * timer.GetDeltaTime();

	byte collInfo = TraceBox( ((vsPhysics_Player*)player->GetPhysics())->GetBounds(), velocity );

	player->in_air = !( collInfo & COLLISION_INFO_ONGROUND );
	player->GetPhysics()->SetVelocity( velocity );
	ssChunks.Leave();
}

/*
===========================
	TraceBox
===========================
*/
byte vsWorld::TraceBox( const vsBox& box, vsVec3f& velocity )
{
	struct traceBoxInfo_t {
		float	timeOfIntersection;		// time of first intersection
		vsVec3f	nearPt;					// point of first intersection
		vsBox	box;					// bounding box
		vsVec3f	modifiedVelocity;

		MINIMUM_OPERATORS( traceBoxInfo_t );
		EMPTY_TRUCTORS( traceBoxInfo_t );
	};

	vsList<traceBoxInfo_t> boxes;
	vsVec3f origin = box.GetCenter();
	vsVec3f extents = box.GetExtents();

	vsVec3i cell;
	int x, y, z;
	for ( x = -1; x < 2; x++ )
	for ( y = -1; y < 2; y++ )
	for ( z = -1; z < 2; z++ ) {
		cell = origin.ToVec3i() + vsVec3i( x, y, z );
		if ( GetChunkOfCell( cell ) ) 
			if ( GetBlockOfCell( cell ).id != 0 && 
				 GetBlockOfCell( cell ).id != blockLibrary->GetBlockTypeId( "still_water" ) ) {
				boxes.Expand();
				game->world->GetBoxOfCell( cell, &boxes[ boxes.Length() - 1 ].box );
				boxes[ boxes.Length() - 1 ].timeOfIntersection = ( boxes[ boxes.Length() - 1 ].box.GetCenter() - box.GetCenter() ).Length();
			}
	}
	if ( ! boxes.Length() ) {
		return COLLISION_INFO_NOT_COLLIDED;
	}

	bool swapped;
	do {
		swapped = false;
		REPU( boxes.Length() - 1 )
			if ( boxes[ i ].timeOfIntersection > boxes[ i + 1 ].timeOfIntersection )  {
				vsSystem::Swap( boxes[ i ], boxes[ i + 1 ] );
				swapped = true;
			}
	} while ( swapped );

	vsBox destBox = box + velocity;
	vsVec3f minVelocity = velocity;
	byte info;

	REPU( boxes.Length() ) {
		boxes[ i ].modifiedVelocity = velocity;
		if ( boxes[ i ].box.IsBoxInside( destBox ) ) 
		{
			if ( velocity.x < 0.0f && destBox.points[ 0 ].x < boxes[ i ].box.points[ 7 ].x )
				boxes[ i ].modifiedVelocity.x = boxes[ i ].box.points[ 7 ].x - box.points[ 0 ].x;

			if ( velocity.x > 0.0f && destBox.points[ 7 ].x > boxes[ i ].box.points[ 0 ].x )
				boxes[ i ].modifiedVelocity.x = boxes[ i ].box.points[ 0 ].x - box.points[ 7 ].x;

			if ( velocity.y < 0.0f && destBox.points[ 0 ].y < boxes[ i ].box.points[ 7 ].y ) {
				boxes[ i ].modifiedVelocity.y = boxes[ i ].box.points[ 7 ].y - box.points[ 0 ].y;
				info |= COLLISION_INFO_ONGROUND;
			}

			if ( velocity.y > 0.0f && destBox.points[ 7 ].y > boxes[ i ].box.points[ 0 ].y )
				boxes[ i ].modifiedVelocity.y = boxes[ i ].box.points[ 0 ].y - box.points[ 7 ].y;

			if ( velocity.z < 0.0f && destBox.points[ 0 ].z < boxes[ i ].box.points[ 7 ].z )
				boxes[ i ].modifiedVelocity.z = boxes[ i ].box.points[ 7 ].z - box.points[ 0 ].z;

			if ( velocity.z > 0.0f && destBox.points[ 7 ].z > boxes[ i ].box.points[ 0 ].z )
				boxes[ i ].modifiedVelocity.z = boxes[ i ].box.points[ 0 ].z - box.points[ 7 ].z;

			if ( Abs( boxes[ i ].modifiedVelocity.x ) < Abs( minVelocity.x ) ) {
				minVelocity.x = boxes[ i ].modifiedVelocity.x;
				info |= COLLISION_INFO_COLLIDED;
			}

			if ( Abs( boxes[ i ].modifiedVelocity.y ) < Abs( minVelocity.y ) ) {
				minVelocity.y = boxes[ i ].modifiedVelocity.y;
				info |= COLLISION_INFO_COLLIDED;
			}

			if ( Abs( boxes[ i ].modifiedVelocity.z ) < Abs( minVelocity.z ) ) {
				minVelocity.z = boxes[ i ].modifiedVelocity.z;
				info |= COLLISION_INFO_COLLIDED;
			}

			velocity = minVelocity;
			destBox = box + velocity;
		}
	}	

	velocity = minVelocity * 0.9f;

	boxes.Free();

	return info;
}

/*
===========================
	SetCellLife
===========================
*/
void vsWorld::SetCellLife( const vsVec3i& cell, float life ) {
	game->world->ssChunks.Enter();
	vsChunk * chunk = game->world->GetChunkOfCell( cell );

	if ( chunk ) {
		chunk->SetCellLife( cell, life );
	}
	game->world->ssChunks.Leave();
}

/*
===========================
	GetCellLife
===========================
*/
float vsWorld::GetCellLife( const vsVec3i& cell ) {
	float life = 0.0f;
	game->world->ssChunks.Enter();
	vsChunk * chunk = game->world->GetChunkOfCell( cell );

	if ( chunk ) {
		life = chunk->GetCellLife( cell );
	}

	game->world->ssChunks.Leave();
	return life;
}

/*
===========================
	GetBlockOfCell
===========================
*/
block_t vsWorld::GetBlockOfCell( const vsVec3i& cell ) const {
	block_t value = block_t( 1, 0 );
	game->world->ssChunks.Enter();
	vsChunk * chunk = game->world->GetChunkOfCell( cell );

	if ( chunk ) {
		value = chunk->GetBlockOfCell( cell );
	}

	game->world->ssChunks.Leave();
	return value;
}

/*
===========================
	SaveToFolder
===========================
*/
void vsWorld::SaveToFolder( char* szFolderName )
{
	saveFolder = szFolderName;

	char szFileName[128];
	ZeroMemory( szFileName, 128 );
	sprintf_s<128>( szFileName, "%s/world.svg", szFolderName );

	vsOutputFile f;
	f.Open( szFileName, "wb+" );
	f.WriteInt( &curChunkCoords.x );
	f.WriteInt( &curChunkCoords.y );
	f.WriteInt( &curChunkCoords.z );
	
	worldGenerator->SaveToFile( &f );

	f.Close();
}

/*
===========================
	RestoreFromFile
		Also Initializes
===========================
*/
void vsWorld::RestoreFromFolder( char* szFolderName )
{
	console->Print( CMT_LOG, "Restoring whole world from file" );
	//load sectors and gen landscape from it
	this->saveFolder = szFolderName;

	char szFileName[128];
	ZeroMemory( szFileName, 128 );
	sprintf_s<128>( szFileName, "%s/world.svg", szFolderName );

	vsInputFile f;
	if ( f.Open( szFileName, "rb+" ) == VS_ERROR )  {
		console->Print( CMT_ERROR, "Failed to load world from %s", szFolderName );
		return;
	}

	f.ReadInt( &curChunkCoords.x );
	f.ReadInt( &curChunkCoords.y );
	f.ReadInt( &curChunkCoords.z );
	worldGenerator->RestoreFromFile( &f );
	f.Close();

		
	this->vsWorld::Initialize();
}

/*
===========================
	OpenMapAt
===========================
*/
FILE* vsWorld::OpenMapAt( const vsVec3i& coords )
{
	char fname[128];
	sprintf_s<128>( fname, "%s/%d_%d_%d.vws", saveFolder, coords.x, coords.y, coords.z );
	FILE* f;
	fopen_s( &f, fname, "rb" );

	return f;
}

/*
===========================
	MapAt
===========================
*/
uint vsWorld::MapAt( const vsVec3i& coords )
{
	FILE* f;

	char fname[128];
	
	sprintf_s<128>( fname, "%s/%d_%d_%d.vws", saveFolder, coords.x, coords.y, coords.z );
	
	fopen_s( &f, fname, "rb" );

	if ( f == NULL )
		return 0;
	
	fclose( f );

	return 1;
}

/*
===========================
	ChunkLoaderProc
===========================
*/
void vsWorld::ChunkLoaderProc( void )
{
	vsChunk* tmp;

	while ( 1 )
	{
		if ( request.available ) {

			SetZone( curChunkCoords );

			request.available = false;
		}

		if ( eofChunkLoaderThread ) {
			return;
		}

		/*if ( !allChunksLoaded )
			REPU( chunkOffBoxes.Length() ) {

				if ( !chunkOffBoxes[ i ].loaded ) {
					allChunksLoaded = false;

					if ( player->GetFrustum()->IntersectBox( chunkOffBoxes[ i ].box, 0.002f ) ) 
					{
						tmp = new vsChunk();
						LoadChunk( tmp,  curChunkCoords + chunkOffBoxes[ i ].offset );

						ssChunks.Enter();
							chunks.Append( tmp );
							SortChunks();
						ssChunks.Leave();

						chunkOffBoxes[ i ].loaded = true;
						allChunksLoaded = true;
					}
				}
			}*/

		Sleep( 50 );
	}
}

/*
===========================
	GhostChunkLoaderThreadProc
===========================
*/
uint WINAPI vsWorld::GhostChunkLoaderThreadProc( void* )
{
	game->world->ChunkLoaderProc();
	_endthreadex( 0 );
	return 0;
}

/*
===========================
	SetViewDistance
===========================
*/
void vsWorld::SetViewDistance( const viewDistance_t& vd )
{
	chunkViewDist = vd;
	numChunks = chunkViewDist * chunkViewDist * chunkViewDist;
	chunkOffBoxes.Alloc( numChunks );
	SetZone( GetChunkCoordOfPoint( player->GetPhysics()->GetCenterOfMass() ) );
}