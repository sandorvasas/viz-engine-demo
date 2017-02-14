#include "World.h"
#include "Player.h"
#include "Physics\CollisionManager.h"
#include <new>
using namespace vsMath;

vsWorld::vsWorld( void )
{
	ZeroMemory(offsetmap, MAP_W*MAP_H*4);
	numSectors = 0;
	saveFileName = "";

//	storage = new byte[sizeof(vsSector)*53];
	REPEAT(9) 
		//renderSectors[i] = NULL;
		renderSectors[i] = new  vsSector();
}

vsWorld::~vsWorld( void )
{
	REPEAT(9) {
	//	renderSectors[i]->vsSector::~vsSector();	
		delete renderSectors[i];
	}
	//delete [] storage;
}

void vsWorld::Initialize( const worldArgs_t& args )
{
	
	this->player = args.pPlayer;
	this->viewDistance = player->viewDistance;
	shaders.Alloc(3);
	shaders[0] = renderSystem->LoadShaderFromFile("shader_default", "data/shaders/default.txt", 0,0);
	shaders[1] = renderSystem->LoadShaderFromFile("shader_sky", "data/shaders/sky.txt", 0, 0);
	shaders[2] = renderSystem->LoadShaderFromFile("shader_landscape", "data/shaders/landscape.txt", 0, 0);

	
	sky.BuildGeometry(8,8, 
					  vsVec3(10000.0f), 
					  renderSystem->LoadTexture("data/textures/cloudsclear02.dds"),
					  shaders[1]);
	
	player->origin.y += 50.0f;

	sectorArgs.generateNew	= args.bNewWorld;
	sectorArgs.player		= args.pPlayer;
	sectorArgs.scale		= args.v3SectorScale;
	sectorArgs.size			= args.nSectorSize;
	sectorArgs.terrainType	= args.eTerrainType;
	sectorArgs.texScale		= args.v3SectorTexScale;
	sectorArgs.translation	= args.v3SectorTranslation;
	sectorArgs.shader		= shaders[2];

	glFogf(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, vsVec3(0.878f, 1.0f, 1.0f).ToFloatPtr());
	glFogf(GL_FOG_START, viewDistance-5.0f);
	glFogf(GL_FOG_END, viewDistance);
	glEnable(GL_FOG);
	
}

void vsWorld::SpawnPlayer( void )
{
	LocatePlayer();
	SetZone(curX, curY);
}

/*
===========================
	SetZone()
===========================
*/
// If a sector's id != 0, it is evident that the world was restored from file.
// Is the above true? Answer: No. There's no such thing as a sector id anymore.
void vsWorld::SetZone( byte x, byte y )
{
	
	if(!strlen(saveFileName)) 
	{
		console->Print(CMT_ERROR, "vsWorld::SetZone() says: Save file could not be found.");
		return;
	}

	curX = x;
	curY = y;
	console->Print(CMT_LOG, "curX: %d curY: %d", curX, curY);
	
	REPEAT(9) 
		if(renderSectors[i]) 
		{
			delete renderSectors[i];
			renderSectors[i] = NULL;
		}
	

	// CENTER
	GenLoadSector(4, x, y);
	// FRONT
	GenLoadSector(1, x, y-1);
	// BACK
	GenLoadSector(7, x, y+1);
	// LEFT
	GenLoadSector(3, x-1, y);
	// RIGHT
	GenLoadSector(5, x+1, y);
	// FRONT-LEFT
	GenLoadSector(0, x-1, y-1);
	// FRONT-RIGHT
	GenLoadSector(2, x+1, y-1);
	// BACK-RIGHT
	GenLoadSector(8, x+1, y+1);
	// BACK-LEFT
	GenLoadSector(6, x-1, y+1);

	REPEAT(9)
	{
		//if(!rs_used[i])
		//	delete lastRenderSectors[i];

		rs_used[i] = false;
	}
}

void vsWorld::GenLoadSector( byte index, byte x, byte y )
{
	if(x>0 && x<MAP_W && y>0 && y<MAP_H)
	{
		if(offsetmap[x][y] == 0)
			GenNewRenderSector(index, x, y);
		else
			LoadNewRenderSector(index, x, y);
		/*{
			renderSectors[index] = RestoreSectorFromFile(x, y);
			sectorArgs.generateNew = false;
			renderSectors[index]->Generate(sectorArgs);
		}*/
	}
}

void vsWorld::GenNewRenderSector( byte index, byte x, byte y )
{
	vsSector *neighbors[4];
	REPEAT(4) neighbors[i] = NULL;

	++numSectors;
	// Ha van FRONT szomszédja
	if(y>0)if(offsetmap[x][y-1] != 0)
		neighbors[SECTOR_NEIGHBOR_FRONT] = LoadSector(x, y-1);
			
	//Ha van BACK szomszédja
	if(y<MAP_H-1)if(offsetmap[x][y+1] != 0)
		neighbors[SECTOR_NEIGHBOR_BACK] = LoadSector(x, y+1);
		
	//Ha van LEFT szomszédja
	if(x>0)if(offsetmap[x-1][y] != 0)
		neighbors[SECTOR_NEIGHBOR_LEFT] = LoadSector(x-1, y);

	//Ha van RIGHT szomszédja
	if(x<MAP_W-1)if(offsetmap[x+1][y] != 0)
		neighbors[SECTOR_NEIGHBOR_RIGHT] = LoadSector(x+1, y);

	REPEAT(4) sectorArgs.neighbors[i] = neighbors[i];
	sectorArgs.coordX = x;
	sectorArgs.coordY = y;
	sectorArgs.generateNew = true;

	//New renderSector gen 1
//	if(renderSectors[index]) 
//		delete renderSectors[index];
	renderSectors[index] = new vsSector(sectorArgs);
	
	bool notE;
	REPEAT(4) 
	{
	/*	notE = true;

		REPEAT2(j, 9)
		if(//renderSectors[j] == neighbors[i] /*||
		   lastRenderSectors[j] == neighbors[i])
			notE = false;

		if(notE)
		*/	if(neighbors[i]) {
			//neighbors[i]->Destroy();
			delete neighbors[i];
		//	sectorArgs.neighbors[i]->vsSector::~vsSector();
		}
	}
			
	offsetmap[x][y] = SaveSectorToFile(index);
}

void vsWorld::LoadNewRenderSector( byte index, byte x, byte y )
{
	if(renderSectors[index])
		delete renderSectors[index];
	renderSectors[index] = RestoreSectorFromFile(x, y);//LoadSector(x, y, true);
	REPEAT(4) sectorArgs.neighbors[i] = NULL;
	sectorArgs.generateNew = false;
	renderSectors[index]->Generate(sectorArgs);
}

vsSector* vsWorld::LoadSector( byte x, byte y, bool forRendering )
{
	UNREFERENCED_PARAMETER(forRendering);
/*
	REPEAT(9)
		if(lastRenderSectors[i])
			if(lastRenderSectors[i]->coordX == x &&
			   lastRenderSectors[i]->coordY == y)
			{
				rs_used[i] = true;
				return lastRenderSectors[i];
			}
*/
	return RestoreSectorFromFile(x, y);
}

void vsWorld::SaveToFile( vsOutputFile* f )
{
	saveFileName = f->filename;

	f->WriteByte(&curX);
	f->WriteByte(&curY);
	f->WriteVec3(&sectorArgs.translation);
	f->WriteVec3(&sectorArgs.scale);
	f->WriteVec3(&sectorArgs.texScale);
	f->WriteUnsignedInt(&sectorArgs.size);
	f->WriteByte(&sectorArgs.terrainType);

	REPEAT(9)
		if(renderSectors[i]) 
			if(renderSectors[i]->coordY>-1 && renderSectors[i]->coordX>-1)	
				renderSectors[i]->SaveToFile(f);
}

/*
===========================================================
	Saves a newly generated sector to the end of the file.
===========================================================
*/
uint vsWorld::SaveSectorToFile( byte renderSectorIndex )
{
	if(saveFileName && renderSectors[renderSectorIndex])
	{
		vsOutputFile f;
		if(f.Open(saveFileName, "ab+") != VS_OK) 
			return 0;

		f.Seek(0, SEEK_END);
		uint off = f.Tell();
		renderSectors[renderSectorIndex]->SaveToFile(&f);

		f.Close();

		console->Print("vsWorld::SaveSectorToFile() = %d", off);
		return off;
	}
	console->Print(CMT_ERROR, "vsWorld::SaveSectorToFile() FAILED");
	return 0;
}

void vsWorld::RestoreFromFile( vsInputFile* f, vsPlayer* player )
{
	console->Print(CMT_LOG, "Restoring whole world from file");
	this->player = player;
	//load sectors and gen landscape from it
	this->saveFileName = f->fileName;

	f->ReadByte(&curX);
	f->ReadByte(&curY);
	f->ReadVec3(&sectorArgs.translation);
	f->ReadVec3(&sectorArgs.scale);
	f->ReadVec3(&sectorArgs.texScale);
	f->ReadUnsignedInt(&sectorArgs.size);
	f->ReadByte(&sectorArgs.terrainType);

	uint Offset = 0;
	int cx, cy;
	numSectors=0;
	while(!f->Eof())
	{
		Offset = (uint)f->Tell();
		f->ReadInt(&cx);
		f->ReadInt(&cy);
		f->Seek(Offset + 2*sizeof(int)+1+5*sizeof(vsVec3)+sizeof(uint)+8+4*sectorArgs.size*sectorArgs.size, SEEK_SET);
		offsetmap[cx][cy] = Offset;
		++numSectors;
	}

	worldArgs_t wa;
	wa.bNewWorld			= false;
	wa.eTerrainType			= sectorArgs.terrainType;
	wa.nSectorSize			= sectorArgs.size;
	wa.pPlayer				= player;
	wa.v3SectorScale		= sectorArgs.scale;
	wa.v3SectorTexScale		= sectorArgs.texScale;
	wa.v3SectorTranslation	= sectorArgs.translation;
	Initialize(wa);
}

/*
===============================
	RestoreSectorFromFile
		Returns a NEW sector from file or NULL
===============================
*/
vsSector* vsWorld::RestoreSectorFromFile( byte x, byte y )
{ 
	if(x<0 || x>=MAP_W || y<0 || y>=MAP_H)
		return NULL;
	if(offsetmap[x][y] == 0) 
		return NULL;

	console->Print("Restoring sector from file");
	
	if(strlen(saveFileName)>0)
	{
		static vsInputFile f;
		if(f.Open(saveFileName, "rb") != VS_OK)
			return NULL;

		// New renderSector gen 2
		static vsSector* s =NULL;
		if(s) delete s;
		s = new vsSector();

		f.Seek(offsetmap[x][y], SEEK_SET);
		
		s->RestoreFromFile(&f);

		f.Close();
		console->Print("Restored");
		return s;
	}
	return NULL;
}

void vsWorld::LocatePlayer( void )
{ //FIXME: középreigazítás nincs beleszámolva!!!!!!
	vsVec3 dest = player->origin + player->deltaMovement;
	float x = dest.x / sectorArgs.scale.x;
	float z = dest.z / sectorArgs.scale.z;

	short sx = 17+(short)( (x>0.f)? ceil(x) : ceil(x));
	short sz = 17+(short)( (z>0.f)? ceil(z) : ceil(z));

	sx = Clamp<short>(sx, 0, 32);
	sz = Clamp<short>(sz, 0, 32);

	curX = (byte)sx; 
	curY = (byte)sz;
}

void vsWorld::Render( void )
{
	shaders[0]->Bind(true);
	shaders[0]->Uniform3v("lightPos", player->origin);
	shaders[0]->Uniform1f("time", vsLib2::time);
	shaders[0]->Bind(false);
	shaders[1]->Bind(true);
	shaders[1]->Uniform1f("time", vsLib2::time);
	shaders[1]->Bind(false);
	shaders[2]->Bind(true);
	shaders[2]->Uniform1f("time", vsLib2::time);
	shaders[2]->Bind(false);

	glLineWidth(3.0f);
	glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(10000.0f, 0.0f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 10000.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 10000.0f);
	glEnd();

	sky.Render();
	
	REPEAT(9)
		if(renderSectors[i] != NULL)
		if(renderSectors[i]->coordX>-1)
			renderSectors[i]->Render();
			
	/*glPointSize(5.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for(float i=0.0f; i<20.0f; ++i)
	for(float j=0.0f; j<20.0f; ++j)
		glVertex3f(i, 0.0f, j);
	glEnd();*/
	//glColor3f(1.0f, 1.0f, 1.0f);
}

void vsWorld::UpdatePhysics( void )
{
	//player->deltaMovement.y -= 0.2f;
	if(renderSectors[4] != NULL)
	if(renderSectors[4]->coordX!=-1 &&
	   renderSectors[4]->coordY!=-1)
		collisionManager->Handle_Player_Landscape(player, renderSectors[4]);
	
	/*PSLD_t psld = sectors[currentSector]->WhereDidPlayerLeave();
	if( psld != PLAYER_IS_HERE)
	{
		/*
		sectorArgs.neighbors[0] = (psld==PLAYER_WENT_BACK)? sectors[currentSector] : NULL;
		sectorArgs.neighbors[1] = (psld==PLAYER_WENT_LEFT)? sectors[currentSector] : NULL;
		sectorArgs.neighbors[2] = (psld==PLAYER_WENT_FRONT)? sectors[currentSector] : NULL;
		sectorArgs.neighbors[3] = (psld==PLAYER_WENT_RIGHT)? sectors[currentSector] : NULL;
		currentSector = 1-currentSector;
		console->Print("Generating new sector");
		//sectors[currentSector]->Destroy();
		delete sectors[currentSector];
		sectors[currentSector] = new vsSector();
		sectors[currentSector]->Generate(sectorArgs);*/
	/*
	PSLD_t psld = renderSectors[4]->WhereDidPlayerLeave();
	if(psld != PLAYER_IS_HERE)
		switch(psld)
		{
			case PLAYER_WENT_BACK:
				console->Print("PLAYER_WENT_BACK");
				SetZone(curX, curY+1);
			break;

			case PLAYER_WENT_FRONT:
				console->Print("PLAYER_WENT_FRONT");
				SetZone(curX, curY-1);
			break;

			case PLAYER_WENT_LEFT:
				console->Print("PLAYER_WENT_LEFT");
				SetZone(curX-1, curY);
			break;

			case PLAYER_WENT_RIGHT:
				console->Print("PLAYER_WENT_RIGHT");
				SetZone(curX+1, curY);
			break;
		}*/

	static byte lastX = curX, lastY = curY;
	LocatePlayer();
	if(lastX != curX || lastY != curY)
	{
		lastX = curX;
		lastY = curY;
		SetZone(curX, curY);
	}
}

