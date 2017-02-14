#include "PlayerView.h"
#include "Player.h"
#include "Geometry\Sector2.h"

using namespace vsLib2;
using namespace vsSector2Texture;

/*
===========================
	vsPlayerView
===========================
*/
vsPlayerView::vsPlayerView( void )
{
/*	shader = renderSystem->LoadShaderFromFile( "shader_playerview", "data/shaders/playerview.txt", 0, 0 );

	glGenBuffers( 1, buffers );
	glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );

	glBufferData( GL_ARRAY_BUFFER, 48*sizeof( vsSimpleVertex ), NULL, GL_DYNAMIC_DRAW );
	vsSimpleVertex* vertices = ( vsSimpleVertex* ) glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

	for ( int i = 0, float x =  -0.5f; i < 40; x += 0.1f ) {
		vertices[ i ].xyz = vsVec3f( x,		-0.955f, 0.5f );
		vertices[ i++ ].st = vsVec2f( 0.0f, 1.0f );
		vertices[ i ].xyz = vsVec3f( x+0.09f, -0.955f, 0.5f );
		vertices[ i++ ].st = vsVec2f( 1.0f, 1.0f );
		vertices[ i ].xyz = vsVec3f( x+0.09f, -0.995f, 0.5f );
		vertices[ i++ ].st = vsVec2f( 1.0f, 0.0f );
		vertices[ i ].xyz = vsVec3f( x,		-0.995f, 0.5f );
		vertices[ i++ ].st = vsVec2f( 0.0f, 0.0f );
	}

	// "background"
	vertices[ 40 ].xyz = vsVec3f( -0.55f,-0.95f, 0.5f );
	vertices[ 40 ].st = vsVec2f( 0.0f, 1.0f );
	vertices[ 41 ].xyz = vsVec3f( 0.55f, -0.95f, 0.5f );
	vertices[ 41 ].st = vsVec2f( 1.0f, 1.0f );
	vertices[ 42 ].xyz = vsVec3f( 0.55f, -1.00f, 0.5f );
	vertices[ 42 ].st = vsVec2f( 1.0f, 0.0f );
	vertices[ 43 ].xyz = vsVec3f( -0.55f,-1.00f, 0.5f );
	vertices[ 43 ].st = vsVec2f( 0.0f, 0.0f );*/

	megaTexQ1 = blockLibrary->GetTextureAtlas();
	fontId = printer->NewFont( "Arial Black", 14, true, false, false, false );
}

/*
===========================
	~vsPlayerView
===========================
*/
vsPlayerView::~vsPlayerView( void )
{
}

/*
===========================
	Render
===========================
*/
void vsPlayerView::Render( void )
{
	float x;
	uint i;

	glDepthFunc( GL_ALWAYS );

	renderSystem->OrthoMode( ON );
//	shader->Bind( true );
	
	// Crosshair
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glLineWidth( 2.0f );
	glBegin( GL_LINES );
		glVertex3f( -0.02f, 0.0f, 0.1f );
		glVertex3f( +0.02f, 0.0f, 0.1f );
		glVertex3f( 0.0f, -0.02f, 0.1f );
		glVertex3f( 0.0f, +0.02f, 0.1f );
	glEnd();

	//background
	glColor4f( 0.5f, 0.5, 0.5f, 1.0f );
	glBegin( GL_QUADS );
		glVertex3f( -0.55f, -0.90f, 0.6f );
		glVertex3f(  0.55f, -0.90f, 0.6f );
		glVertex3f(  0.55f, -1.00f, 0.6f );
		glVertex3f( -0.55f, -1.00f, 0.6f );
	glEnd();

	// inventory elements
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, itemManager->GetTextureAtlas() );


	// Rendering each item -----------------------------------------------------------
	vsVec2b slot;
	Item_t * item;
	slot.y = 0;
	vsVec2f st00;
	glBegin( GL_QUADS );
	for ( x = -0.5f,  slot.x = 0; slot.x < INVENTORY_SLOT_WIDTH; slot.x++, x += 0.1f ) {
		if ( player->GetInventory()->GetActiveSlot() == slot ) 
			glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		else
			glColor4f( 0.7f, 0.7f, 0.7f, 1.0f );

		if ( item = player->GetInventory()->GetItem( slot ) ) {
			st00 = item->info->atlasCoord;
		} else
			st00 = vsVec2f( 0.0f, 14.0f * itemManager->GetTextureSize() );  

		glMultiTexCoord2fv( GL_TEXTURE0, st00.ToTypePtr() );
		glVertex3f( x,		-0.905f, 0.5f );
		
		glMultiTexCoord2fv( GL_TEXTURE0, ( st00 + vsVec2f( itemManager->GetTextureSize(), 0.0f ) ).ToTypePtr() );
		glVertex3f( x+0.09f, -0.905f, 0.5f );
		
		glMultiTexCoord2fv( GL_TEXTURE0, ( st00 + itemManager->GetTextureSize() ).ToTypePtr() );
		glVertex3f( x+0.09f, -0.995f, 0.5f );
		
		glMultiTexCoord2fv( GL_TEXTURE0, ( st00 + vsVec2f( 0.0f, itemManager->GetTextureSize() ) ).ToTypePtr() );
		glVertex3f( x,		-0.995f, 0.5f );
	}
	glEnd();
	glBindTexture( GL_TEXTURE_2D, 0 );

	// ammo
	//for ( x = -0.5f,  i = 0; i < 10 && i < b->Length(); i++, x += 0.1f )
//		printer->Print( 1, x+0.06f, -0.95f, 0.3f, vsColors::Orange, false,"%d", ( *b )[i]->ammo );


	



//	shader->Bind( false );
	renderSystem->OrthoMode( OFF );

	glDepthFunc( GL_LESS );
}
