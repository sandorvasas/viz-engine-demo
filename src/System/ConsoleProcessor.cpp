#include "ConsoleProcessor.h"
#include <stdio.h>
#include "VarStorage.h"
#include "..\Game\Game.h"
#include "..\Game\Player.h"
#include "..\Game\World.h"
#include "..\Game\Geometry\Sector2.h"
#include "..\Game\Physics\PhysicsManager.h"
#include "..\Game\Geometry\ModelManager.h"

vsConsoleProcessor consoleProcessor;
vsVarStorage varStorage; 


void vsConsoleProcessor::Hz( void ) 
	{
		if ( !console->GetLine( cmd ) ) 
			return;

		if ( cmd == "wireframe" )
			varStorage.Bool.Set( "wireframe", !varStorage.Bool["wireframe"] );
		else
		if ( strstr( cmd.ToCharPtr(), "setorigin" ) ) {
			float x, y, z;
			sscanf_s( cmd.ToCharPtr(), "setorigin %f %f %f", &x, &y, &z );
			player->GetPhysics()->SetCenterOfMass( vsVec3f( x,y,z ) );		
		}
		else
		if ( strstr( cmd.ToCharPtr(), "setspeed" ) ) {
			float x, y, z;
			sscanf_s( cmd.ToCharPtr(), "setspeed %f", &x );
			player->speed = x*0.001f;
		}
		else
		if ( strstr( cmd.ToCharPtr(), "setlightpos" ) ) {
			float x, y, z;
			sscanf_s( cmd.ToCharPtr(), "setlightpos %f %f %f", &x, &y, &z );
			game->world->lightOffset = vsVec3f( x,y,z );		
		} else
		if ( strstr( cmd.ToCharPtr(), "pb" ) ) {
			int x;
			sscanf_s( cmd.ToCharPtr(), "pb %d", &x );
			varStorage.Int.Set( "putblock_type", x );
		} else
		if ( strstr( cmd.ToCharPtr(), "monsteranim" ) ) {
			short x, y;
			sscanf_s( cmd.ToCharPtr(), "monsteranim %d %d", &x, &y );
			game->world->monsters[x]->SetState( y );	
		} else
		if ( strstr( cmd.ToCharPtr(), "monsteranimspeed" ) ) {
			int y;
			float x;
			sscanf_s( cmd.ToCharPtr(), "monsteranimspeed %d %f", &y, &x );
			game->world->monsters[y]->debAnimSpeed = x * 0.001f;	
		} else
		if ( strstr( cmd.ToCharPtr(), "monsterspawn" ) ) {
			float x, y, z;
			int i;
			sscanf_s( cmd.ToCharPtr(), "monsterspawn %d %f %f %f", &i, &x, &y, &z );
			game->world->monsters[ i ]->Spawn( vsVec3f( x,y,z ), vsVec3f() );	
		} else
		if ( cmd == "bl" ) {
			game->world->lightOffset = player->GetPhysics()->GetCenterOfMass();
		}
		else
		if ( cmd == "num physics objects" || cmd=="npo" ) {
			console->Print( "%d", physicsManager->GetObjectCount() );
		}
		else
		if ( cmd == "rendernormals true" ) {
			varStorage.Bool.Set( "rendernormals", true );
		}
		else
		if ( cmd == "rendernormals false" ) {
			varStorage.Bool.Set( "rendernormals", false );
		}
		else
		if ( cmd == "noclip" ) {
//			player->GetPhysics()->SetPhysicsTest( !player->GetPhysics()->GetPhysicsTest() );
	//		console->Print( CMT_NOTIFY, "Noclip %s", ( player->GetPhysics()->GetPhysicsTest() )? "ON" : "OFF" );
		}
		else
		if ( cmd == "spectate" ) {
			player->spectating = !player->spectating;
		}
		else
		if ( cmd == "rendertree true" ) {
			varStorage.Bool.Set( "rendertree", true );
		}
		else
		if ( cmd == "rendertree false" ) {
			varStorage.Bool.Set( "rendertree", false );
		} 
		else
		if ( cmd == "nograss" ) {
			varStorage.Bool.Set( "nograss", !varStorage.Bool["nograss"] );
		}
		else
		if ( cmd == "rendersnow true" ) {
			varStorage.Bool.Set( "dontrendersnow", false );
		}
		else
		if ( cmd == "rendersnow false" ) {
			varStorage.Bool.Set( "dontrendersnow", true );
		}
		else
		if ( cmd == "renderparticles true" ) {
			varStorage.Bool.Set( "disableparticles", false );
		}
		else
		if ( cmd == "renderparticles false" ) {
			varStorage.Bool.Set( "disableparticles", true );
		}
		else
		if ( cmd == "delete saves" ) {
			game->DeleteAllSaveFiles();
		} else
		if ( cmd == "newgame" ) {
			game->NewGame();
		} else
		if ( cmd == "loadgame" ) {
			game->Load();
		} else
		if ( cmd == "spawn" ) {
			if ( game->savesDeleted )
				game->SaveToFolder( "data/saves/user1" );
			else
				game->RestoreFromFolder( "data/saves/user1" );
			game->world->SpawnPlayer();
		} else
		if ( cmd == "list materials" ) {
			modelManager->PrintMaterials();
		}
		else
		if ( cmd != "clear" )
			console->Print( CMT_ERROR, "Invalid command" );
	}