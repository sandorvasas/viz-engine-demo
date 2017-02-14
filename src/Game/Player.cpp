#include "Player.h"
#include "World.h"
#include "..\System\VarStorage.h"
#include "PSystem.h"
#include "Game.h"
#include "Physics\PhysicsManager.h"
#include <assert.h>

using namespace vsLib2;

vsPlayer* player;

/* 
=====================================
	Weapon_t::GenerateMesh()
=====================================
*/
void Weapon_t::GenerateMesh( Item_t* item )
{
	if ( !buffers[ 0 ] ) {

		texture = _LoadTexture( itemManager->GetTextureAtlasFileName(),
								GL_NEAREST,
								GL_NEAREST_MIPMAP_NEAREST,
								0,
								0,
								4 );

		glGenBuffers( 2, buffers );

		byte indices[ 24 ] = {
			0, 1, 2, 3,
			4, 5, 6, 7,
			0, 3, 7, 4,
			1, 2, 6, 5,
			0, 1, 5, 4,
			2, 3, 7, 6
		};

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[ 1 ] );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 24, indices, GL_STATIC_DRAW );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		glBindBuffer( GL_ARRAY_BUFFER, buffers[ 0 ] );
		glBufferData( GL_ARRAY_BUFFER, 8 * sizeof( vsSimpleVertex ), NULL, GL_DYNAMIC_DRAW );
	} else {
		glBindBuffer( GL_ARRAY_BUFFER, buffers[ 0 ] );
	}

	vertex * vert = ( vertex * ) glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

	float one_over_16 = 1.0f / 16.0f;
	vert[ 0 ].xyz = translation + vsVec3f( -1.0f, -1.0f, -one_over_16 ) * rotation;
	vert[ 1 ].xyz = translation + vsVec3f(  1.0f, -1.0f, -one_over_16 ) * rotation;
	vert[ 2 ].xyz = translation + vsVec3f(  1.0f,  1.0f, -one_over_16 ) * rotation;
	vert[ 3 ].xyz = translation + vsVec3f( -1.0f,  1.0f, -one_over_16 ) * rotation;

	vert[ 4 ].xyz = translation + vsVec3f( -1.0f, -1.0f,  one_over_16 ) * rotation;
	vert[ 5 ].xyz = translation + vsVec3f(  1.0f, -1.0f,  one_over_16 ) * rotation;
	vert[ 6 ].xyz = translation + vsVec3f(  1.0f,  1.0f,  one_over_16 ) * rotation;
	vert[ 7 ].xyz = translation + vsVec3f( -1.0f,  1.0f,  one_over_16 ) * rotation;

	vsVec3f off = vsVec3f( item->info->atlasCoord.x, 
						   item->info->atlasCoord.y,
						   0.0f );
	one_over_16 = 1.0f;
	float size = itemManager->GetTextureSize() * 0.f + 1.f;
	vert[ 0 ].stu = off + vsVec3f( 0.0f, 0.0f, 0.0f );
	vert[ 1 ].stu = off + vsVec3f( size, 0.0f, 0.0f );
	vert[ 2 ].stu = off + vsVec3f( size, size, 0.0f );
	vert[ 3 ].stu = off + vsVec3f( 0.0f, size, 0.0f );
	vert[ 4 ].stu = off + vsVec3f( 0.0f, 0.0f, one_over_16 );
	vert[ 5 ].stu = off + vsVec3f( size, 0.0f, one_over_16 );
	vert[ 6 ].stu = off + vsVec3f( size, size, one_over_16 );
	vert[ 7 ].stu = off + vsVec3f( 0.0f, size, one_over_16 );
	
	glUnmapBuffer( GL_ARRAY_BUFFER );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

/* 
=====================================
	Weapon_t::Render
=====================================
*/
void Weapon_t::Render( void ) {
	
	if ( !buffers[ 0 ] )
		return;

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glBindBuffer( GL_ARRAY_BUFFER, buffers[ 0 ] );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[ 1 ] );

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 2 * sizeof( vsVec3f ), NULL );

	glActiveTexture( GL_TEXTURE0 );
	renderSystem->Disable( GL_TEXTURE_2D );
	renderSystem->Enable( GL_TEXTURE_3D );
	glBindTexture( GL_TEXTURE_3D, texture );
	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 3, GL_FLOAT, 2 * sizeof( vsVec3f ), BUFFER_OFFSET( sizeof( vsVec3f ) ) );

	glDrawElements( GL_QUADS, 24, GL_UNSIGNED_BYTE, NULL );

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
	renderSystem->Disable( GL_TEXTURE_3D );
	glBindTexture( GL_TEXTURE_3D, 0 );
	renderSystem->Enable( GL_TEXTURE_2D );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

/* 
=====================================
	vsInventory
=====================================
*/
vsInventory::vsInventory( vsPlayer * owner )
{
	this->owner = owner;
}

/* 
=====================================
	~vsInventory
=====================================
*/
vsInventory::~vsInventory( void )
{
	REPEAT( INVENTORY_SLOT_WIDTH )
	REPEAT2( j, INVENTORY_SLOT_HEIGHT )
		slots[ i ][ j ].items.Free();
}

/* 
=====================================
	AddItem
=====================================
*/
void vsInventory::AddItem( Item_t* item )
{
	vsVec2i firstEmptySlot = vsVec2i( -1 );

	REPEAT( INVENTORY_SLOT_WIDTH )
	REPEAT2( j, INVENTORY_SLOT_HEIGHT )
		if ( slots[ i ][ j ].items.Length() == 0 ) {
			if ( firstEmptySlot == vsVec2i( -1 ) ) 
				firstEmptySlot = vsVec2i( i, j );
		} else 
		if ( slots[ i ][ j ].items[ 0 ]->info->id == item->info->id &&
			 slots[ i ][ j ].items.Length() < 64 ) {
			slots[ i ][ j ].items.Append( item );
			item->inventorySlot = vsVec2i( i, j );
			item->inInventory = true;
			console->Print( "item added to %s [ num: %d ]", vsVec2i( i, j ).ToCharPtr(),  slots[ i ][ j ].items.Length());
			return;
		}

	if ( firstEmptySlot != vsVec2i( -1 ) ) {
		slots[ firstEmptySlot.x ][ firstEmptySlot.y ].items.Append( item );
		item->inventorySlot = firstEmptySlot;
		item->inInventory = true;
		console->Print( "item added to %s", firstEmptySlot.ToCharPtr() );
	}
}

/* 
=====================================
	RemoveItem
=====================================
*/
void vsInventory::RemoveItem( const vsVec2b& slot )
{
	if ( slots[ slot.x ][ slot.y ].items.Length() > 0 ) {
		itemManager->DeleteItem( slots[ slot.x ][ slot.y ].items[ slots[ slot.x ][ slot.y ].items.Length() - 1 ]->id );
		slots[ slot.x ][ slot.y ].items.Resize( slots[ slot.x ][ slot.y ].items.Length() - 1 );
	}
}

/* 
=====================================
	RemoveItems
=====================================
*/
void vsInventory::RemoveItems( const vsVec2b& slot, uint count )
{
	if ( slots[ slot.x ][ slot.y ].items.Length() > 0 ) {
		uint num = 0;
		for ( uint i = 0; i < count && i < slots[ slot.x ][ slot.y ].items.Length(); i++, num++ ) {
			itemManager->DeleteItem( slots[ slot.x ][ slot.y ].items[ slots[ slot.x ][ slot.y ].items.Length() - 1 ]->id );
		}
		
		slots[ slot.x ][ slot.y ].items.RemoveElementsAt( 0, num );
	}
}

/* 
=====================================
	NextItem
=====================================
*/
void vsInventory::NextItem( void )
{
	activeSlot.x++;
	if ( activeSlot.x >= INVENTORY_SLOT_WIDTH ) {
		activeSlot.x = 0;
		/*activeSlot.y++;

		if ( activeSlot.y == INVENTORY_SLOT_HEIGHT ) {
			activeSlot.y = 0;
		}*/
	}
}

/* 
=====================================
	PrevItem
=====================================
*/
void vsInventory::PrevItem( void )
{
	if ( activeSlot.x == 0 )
		activeSlot.x = INVENTORY_SLOT_WIDTH - 1;
	else
		activeSlot.x--;
}


/* 
=====================================
	FIXME RestoreFromFile FIXME 
=====================================
*/
void vsInventory::RestoreFromFile( vsInputFile* f )
{
	uint len;
	f->ReadUnsignedInt( &len );
	
	//items.Alloc( len );

//	REPU( items.Length() ) {
		//items[ i ] = itemManager->SpawnItem( 
	//}

	/*f->ReadUnsignedInt( &weapons.Count );
	
	REPU( weapons.Length() ) {
		weapons[ i ]->SaveToFile( f );
	}*/

	f->ReadByte( &activeSlot.x );
	f->ReadByte( &activeSlot.y );
}

/* 
=====================================
	SaveToFile
=====================================
*/
void vsInventory::SaveToFile( vsOutputFile* f )
{
	uint len;
	f->WriteUnsignedInt( &len );
	/*
	REPU( items.Length() ) {
		items[ i ] = new pit_t( 0,0 );
		f->WriteInt( &items[ i ]->itemTypeId );
		f->WriteInt( &items[ i ]->ammo );	
	}*/

	f->WriteByte( &activeSlot.x );
	f->WriteByte( &activeSlot.y );
}

/*
===============================================



	vsPlayer



===============================================
*/

vsPlayer::~vsPlayer( void ) {
	delete inventory;
	delete view;
	physicsManager->Delete( physics );
}

/* 
=====================================
	Init
=====================================
*/
void vsPlayer::Init( const playerSettings_t& st ) 
{	
	physics = physicsManager->New( PHYSICS_PLAYER, true );

	( ( vsPhysics_Player* )( physics ) )->GetBounds().FromMinMax( 
					st.v3Origin - st.v3Radius * 0.5f,
				    st.v3Origin + st.v3Radius * 0.5f );

	physics->SetMass( 75.0f );
	physics->SetCenterOfMass( st.v3Origin );
	
	rotation			= st.v3RotationDeg;
	speed				= st.fSpeed;
	lookingSpeed		= st.v2LookingSpeed;
	spectating			= st.bSpectating;
	walkingDirection	= NULLVEC3F;
	strafingDirection	= NULLVEC3F;
	viewDistance		= st.fViewDistance;
	world				= st.pWorld;
	steps				= 0.f;
	originChanged		= true;
	firing				= false;
	hitCellLife			= 100.0f;
	clip				= true;
	jumpHeight			= 1.5f;
	jumping				= false;
	in_air				= true;

	inventory	= new vsInventory( this );
	view		= new vsPlayerView();

	particleSysTexPackId = 0;
	lookBox.SetRenderColor( vsColors::Black );
}

/* 
=====================================
	UpdatePhysics
=====================================
*/
void vsPlayer::Update( void ) 
{
	originCell = world->GetCellOfPoint( physics->GetCenterOfMass() );


	if ( jumping ) {
		float deltaJump = jumpHeight /** (( curJumpHeight / jumpHeight ) * PI * 0.5f )*/ * 10.0f * timer.GetDeltaTime();
		curJumpHeight += deltaJump;
		physics->AddVelocity( vsVec3f( 0.f, deltaJump, 0.f ) );
		if ( curJumpHeight >= jumpHeight ) {
			jumping = false;
		}
	}

	originChanged = physics->GetVelocity().LengthSqr() > 0.f;
	
	vsVec2f deltaMousePos = input->GetMouseDeltaSinceLast();
	if ( deltaMousePos != vsVec2f() )
	{
		rotation.y += deltaMousePos.x * lookingSpeed.y;
		rotation.x += deltaMousePos.y * lookingSpeed.x;
		vsMath::Clamp( rotation.x, -90.0f, 90.0f );
		if ( rotation.y > 360.0f ) rotation.y -= 360.0f;
		if ( rotation.y <   0.0f ) rotation.y += 360.0f;
	}
	input->SetMousePos( videoMode->GetScreenCenter() );

	UpdateLookCell();

	vsQuaternion qx, qy;
	qx.CreateFromEulerAnglesDEG( rotation.x, 0.0f, 0.0f );
	qy.CreateFromEulerAnglesDEG( 0.0f, rotation.y, 0.0f );

	axis = ( qx*qy );
	vsMat4 M;

	M = ( qy*qx ).ToMat4();
	viewDirection.x = M[8];
	viewDirection.z = -M[10];
	viewDirection.y = qx.ToMat4()[9];

	vsBox box = ( ( vsPhysics_Player* )physics )->GetBounds();
	float height = box.points[ 7 ].y - box.points[ 0 ].y;
	
	eyePos = physics->GetCenterOfMass() + vsVec3f( 0.f, height*0.45f+0.05f*( vsMath::Sin( steps ) ), 0.0f );

	if ( spectating )
	{
		walkingDirection = viewDirection;
		M = qy.ToMat4();
		strafingDirection = vsVec3f( M[10], 0.0f, M[8] );
		
		//axis.Translate( -eyePos );
	} 
	else
	{
		M = qy.ToMat4();
		walkingDirection.x = M[8];
		walkingDirection.y = 0.0f;
		walkingDirection.z = -M[10];
		strafingDirection = vsVec3f( -walkingDirection.z, 0.0f, walkingDirection.x );

		//axis.Translate( -eyePos );
	}
	walkingDirection.Normalize();
	walkingDirection *= speed * timer.GetDeltaTime();
	strafingDirection.Normalize();
	strafingDirection *= speed * timer.GetDeltaTime();

	if ( wpn.firing.isFiring && game->world->GetBlockOfCell( hitCell ).id != 0 ) {
		hitCellLife -= 2.5f;
		game->world->SetCellLife( hitCell, hitCellLife / hitCellMaxLife );
	}

	if ( hitCellLife <= 0.0f ) {
		AutoRemoveBlock();
		hitCellLife = 100.0f;//game->world->GetCellLife( hitCell ) * hitCellMaxLife;
	}
	/*if ( weapon ) {
		if ( weapon->IsFiring() ) {
			//if ( fmod( weapon->GetFiringData().time, 1.0f / weapon->GetShootingSpeed() ) ) {
			if ( fmod( weapon->GetFiringData().time, 4 ) == 0.0f ) {
				hitCellLife -= weapon->GetDamage();	
			}

			if ( hitCellLife <= 0.0f ) 
			{
				vsSector2 * s = game->world->GetSectorOfPoint( lookCellF );
				if ( s ) {
					s->RemoveBlock( ( lookCell - s->GetVoxelOffset() ).ToVec3ui() );
					// inventory->add item
				}
			}
		}
	}*/
}

/* 
=====================================
	Look
=====================================
*/
void vsPlayer::Look( void )
{
	vsMat4 mat = axis.ToMat3().ToMat4();
	mat.Translate( -eyePos );

	glMatrixMode( GL_MODELVIEW );
	renderSystem->LoadMatrix( &mat );

	frustum.Update( renderSystem->GetProjectionMatrix(), mat );

//	vsMat4 m;
//	m.Rotate( -45.0f, 0.0f, 0.0f );
//	frustum.Update( renderSystem->GetProjectionMatrix(), m );
}

/* 
=====================================
	SetFieldOfView
=====================================
*/
void vsPlayer::SetFieldOfView( float fov, float aspect, float nearPlane ) 
{
	vsMat4 m;
	m.SetupInfProjMatrix( fov, aspect, nearPlane );
	glMatrixMode( GL_PROJECTION );
	renderSystem->LoadMatrix( &m );
	glMatrixMode( GL_MODELVIEW );
}

/* 
=====================================
	AutoPutBlock
=====================================
*/
void vsPlayer::AutoPutBlock( void ) {

	vsVec3f nearPt, farPt;

	lookBox.IntersectRay( eyePos, viewDirection * 20.0f, nearPt, farPt );
	int face = lookBox.GetFaceOfSurfacePoint( nearPt );

	vsVec3i putCell = lookCell;

	switch ( face ) {
	case BOX_FACE_X_POS: putCell.x++; break;
	case BOX_FACE_Y_POS: putCell.y++; break;
	case BOX_FACE_Z_POS: putCell.z++; break;
	case BOX_FACE_X_NEG: putCell.x--; break;
	case BOX_FACE_Y_NEG: putCell.y--; break;
	case BOX_FACE_Z_NEG: putCell.z--; break;
	}

	if ( lookCell == putCell )
		return;

	PutBlock( putCell, block_t( ( ushort )varStorage.Int["putblock_type"]+3, 0 ) );
}

/* 
=====================================
	AutoRemoveBlock
=====================================
*/
void vsPlayer::AutoRemoveBlock( void ) {
	pSystem->AddSystem( 
		new vsPMicroSystem( 
			24, 
			lookBox.GetCenter(), 
			vsVec3f( 0.0f, 10.0f, 0.0f ),
			//(physics->GetDestination() - lookBox.GetCenter() ).Normalized(),
			PMICROSYSTEM_TYPE_BLOCK, 
			blockLibrary->GetTextureAtlas(), 
			blockLibrary->GetBlockType( game->world->GetBlockOfCell( lookCell ).id )->tcSide.ToVec2f() * blockLibrary->GetTextureSize() ) );
	RemoveBlock();
}

/* 
=====================================
	PutBlock
=====================================
*/
void vsPlayer::PutBlock( const vsVec3i& cell, const block_t& block )
{
//	if ( !canPutBlock ) return;

	vsVec3f pointf;
	vsChunk* s = world->GetChunkOfCell( cell );
		
	if ( !s ) 
		return;

	if ( s->GetBlockOfCell( cell ).id == 0 )
	{
		s->PutBlock( cell, block );
	}
}

/* 
=====================================
	RemoveBlock
=====================================
*/
void vsPlayer::RemoveBlock( void )
{
	//if ( !canPutBlock ) return;
	game->world->ssChunks.Enter();
	vsChunk * chunk = game->world->GetChunkOfCell( hitCell );

	//console->Print("removeblock %s", hitCell.ToCharPtr());

	if ( chunk ) {
		chunk->RemoveBlock( hitCell );
	}
	game->world->ssChunks.Leave();
/*
	vsSector2* s = world->BindSectorOfCell( lookCell, true );

	if ( s == NULL ) {
		world->ReleaseRenderSector( s );
		return;
	}

	ushort vox = s->RemoveBlock( s->GetLocalCellOfAbsoluteCell( lookCell ) );
	world->ReleaseRenderSector( s );

		/*
	if ( !particleSysTexPackId ) {
		pstp_t tp;
		tp.id = 2;
		tp.textures[0] = renderSystem->LoadTexture( "data/textures/particles/02.dds" );
		particleSysTexPackId = world->particleSystem->AddTexturePack( tp );
	}

	world->particleSystem->RemoveBlockEffect( 
		lookBox.GetCenter(),
		s->GetCellDimensions()*5.0f,
		30.0f,
		vsVec3f( 0.0f, -1.0f, 0.0f ).Normalized(),
		particleSysTexPackId );*/
	/*
	if ( vox == 0 )
		return;

	bool found = false;
	REPU( inventory->blocks.Length() ) {
		if ( inventory->GetBlockType( i )->type == vox ) {
			inventory->GetBlockType( i )->ammo++;
			found = true;
			break;
		}
	} 
	if ( !found ) {
		pbt_t* pbt = new pbt_t( vox, 1 );
		inventory->AddBlockType( pbt );
		inventory->ChangeBlockTypeTo( 0 );
	}*/
}

/* 
=====================================
	UpdateLookCell
=====================================
*/
void vsPlayer::Use( void ) 
{
	block_t block = game->world->GetBlockOfCell( lookCell );
	const blocktype_t* blocktype = blockLibrary->GetBlockType( block.id );
	if ( !strcmp( blocktype->name, "jack_o_lantern" ) ) {
		RemoveBlock();

		PutBlock( lookCell, block_t( blockLibrary->GetBlockTypeId( "jack_o_lantern_lit" ), block.extra ) );
	}
	else
	if ( !strcmp( blocktype->name, "jack_o_lantern_lit" ) ) {
		RemoveBlock();
		PutBlock( lookCell, block_t( blockLibrary->GetBlockTypeId( "jack_o_lantern" ), block.extra ) );
	}
	else
		AutoPutBlock();
}

/* 
=====================================
	UpdateLookCell
=====================================
*/
void vsPlayer::UpdateLookCell( void )
{/*
	vsChunk* s;
	vsVec3i lc, blockAt;
	

	vsVec3f lookPointf;
	ushort vox;

	s = world->GetChunkOfCell( originCell );

	if ( !s ) 
		return;

	float m = 1.0f;
	float step = 0.3f;
	byte two = 2;
	do {
		m += step;
		lookPointf = physics->GetCenterOfMass() + viewDirection.Normalized()*m;
		
		if ( m < PLAYER_MAX_BLOCKING_DISTANCE ) 
		{
			lc = world->GetCellOfPoint( lookPointf );

			s = world->GetChunkOfCell( lc );
			if ( !s ) {
			//	canPutBlock = false;
				return;
			}
			
			vox = s->GetBlockOfCell( lc ).id;
			if ( vox != 0 ) 
			{
				lookCellF = lookPointf;
				lookCell = lc;
				canPutBlock=true;

				// bounding box of lookCell
				world->GetBoxOfCell( lc, &lookBox );

				break;
			}
		} else {
			//canPutBlock=false;
			canPutBlock = false;
			//lookCell = vsVec3i( 0 );//world->GetCellOfPoint( lookPointf );
			break;
		}
	} while ( 1 < two );*/

	vsVec3f origin = eyePos;
	vsVec3i cell;
	vsBox	box, minBox;
	int		x, y, z;
	float	tmp1, tmp2, min_time = 10000.0f;


	canPutBlock = false;

	for ( x = -4; x < 4; x++ )
	for ( y = -4; y < 4; y++ )
	for ( z = -4; z < 4; z++ ) {
		cell = origin.ToVec3i() + vsVec3i( x, y, z );
		if ( game->world->GetChunkOfCell( cell ) ) 
			if ( game->world->GetBlockOfCell( cell ).id != 0 && 
				 game->world->GetBlockOfCell( cell ).id != blockLibrary->GetBlockTypeId( "still_water" ) ) 
			{
				game->world->GetBoxOfCell( cell, &box );
				if ( box.IntersectRay( origin, viewDirection*20.0f, tmp1, tmp2 ) ) {
					if ( tmp1 < min_time ) {
						min_time = tmp1;
						lookCell = cell;
						lookCellF = box.GetCenter();
						canPutBlock=true;
						// bounding box of lookCell
						world->GetBoxOfCell( cell, &lookBox );
					}
				}
			}
	}

	if ( hitCell != lookCell ) {
		game->world->SetCellLife( hitCell, 1.0f );
		hitCell = lookCell;
		hitCellMaxLife = hitCellLife = ( float ) blockLibrary->GetBlockType( game->world->GetChunkOfCell( hitCell )->GetBlockOfCell( hitCell ).id )->life;
	}
}

/* 
=====================================
	Render
=====================================
*/
void vsPlayer::Render( void )
{
	if ( canPutBlock ) 
	{
		//	vsSector2* s = world->BindSectorOfCell( lookCell );
			//if ( s ) {
				/*vsVec3ui localCell = s->GetLocalCellOfAbsoluteCell( lookCell );
				if ( s->GetVoxelDataOfLocalCell( localCell ) == 0 )
						glColor3f( 0.0f, 1.0f, 0.0f );
					else*/
						glColor3f( 1.0f, 0.0f, 0.0f );

				//world->ReleaseRenderSector( s );

				glDepthFunc( GL_LEQUAL );
				glUseProgram( 0 );
				glColor4f( 0.0f, 0.0f, 0.0f, hitCellLife*0.01f );
				glLineWidth( 2.0f );
				lookBox.RenderBox( true );
			//}
	}
	view->Render();

	Item_t* citem;
	if ( citem = inventory->GetItem( inventory->GetActiveSlot() ) ) {
		wpn.translation = GetLookingPoint( 2.0f );
		wpn.GenerateMesh( citem );
		wpn.Render();
	}
}

/* 
=====================================
	IsClipping
=====================================
*/
const bool& vsPlayer::IsClipping( void ) const {
	return clip;
}

/* 
=====================================
	Clip
=====================================
*/
void vsPlayer::Clip( bool clip ) {
	this->clip = clip;
	physicsManager->SetTesting( physics, clip );
}

/* 
=====================================
	StartFiring
=====================================
*/
void vsPlayer::StartFiring( void )
{
	if ( !wpn.firing.isFiring ) {
		wpn.firing.isFiring = true;
	}
}

/* 
=====================================
	StopFiring
=====================================
*/
void vsPlayer::StopFiring( void )
{
	if ( wpn.firing.isFiring ) {
		wpn.firing.isFiring = false;
		game->world->SetCellLife( hitCell, 1.0f );
		hitCellLife = hitCellMaxLife;
	}
}

/* 
=====================================
	Jump
=====================================
*/
void vsPlayer::Jump( void ) 
{
	if ( !jumping && !in_air ) {
		curJumpHeight = 0.0f;
		jumping = true;
	}
}

/* 
=====================================
	SaveToFile
=====================================
*/
void vsPlayer::SaveToFile( vsOutputFile* f )
{
	f->WriteVec4( ( vsVec4f* )&axis );
	f->WriteVec3( &spectatingDirection );
	f->WriteVec3( &walkingDirection );
	f->WriteVec3( &strafingDirection );
	f->WriteVec3( &rotation );
	f->WriteVec2( &lookingSpeed );
	f->WriteFloat( &speed );
	f->WriteBool( &spectating );
	f->WriteFloat( &viewDistance );
//	inventory->SaveToFile( f );
	physics->SaveToFile( f );
}

/* 
=====================================
	RestoreFromFile
=====================================
*/
void vsPlayer::RestoreFromFile( vsInputFile* f )
{
	f->ReadVec4( ( vsVec4f* )&axis );
	f->ReadVec3( &spectatingDirection );
	f->ReadVec3( &walkingDirection );
	f->ReadVec3( &strafingDirection );
	f->ReadVec3( &rotation );
	f->ReadVec2( &lookingSpeed );
	f->ReadFloat( &speed );
	f->ReadBool( &spectating );
	f->ReadFloat( &viewDistance );

	//inventory->RestoreFromFile( f );
	physics->RestoreFromFile( f );
}
