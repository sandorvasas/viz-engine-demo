#include "FX.h"
#include "..\Player.h"

using namespace vsLib2;
using namespace vsColors;

/*
===========================
	vsLensFlare
===========================
*/
vsLensFlare::vsLensFlare( void )
{
	shader = NULL;
	buffer = 0;
}

/*
===========================
	~vsLensFlare
===========================
*/
vsLensFlare::~vsLensFlare( void )
{
	if ( buffer )
		glDeleteBuffers( 1, &buffer );
}

/*
===========================
	Initialize
===========================
*/
void vsLensFlare::Initialize( uint textures[ 4 ], vsShader* shader )
{
	this->textures[ 0 ] = textures[ 0 ];
	this->textures[ 1 ] = textures[ 1 ];
	this->textures[ 2 ] = textures[ 2 ];
	this->textures[ 3 ] = textures[ 3 ];
	this->shader = shader;

	shader->BindAttrib( 0, "in_Position" );
	shader->BindAttrib( 1, "in_TexUnit" );
	shader->BindAttrib( 2, "in_Color" );
	shader->Bind( true );
	shader->Uniform1f( "in_SpriteSize", 150.0f );
	shader->Bind( false );
}

/*
===========================
	Render
===========================
*/
void vsLensFlare::Render( const vsVec3f& lightSource )
{
	if ( !player->GetFrustum()->IntersectPoint( lightSource, 0.002f ) )
		return;

	Calculate( lightSource );

	//glBindBuffer( GL_ARRAY_BUFFER, buffer );

	glDisable( GL_DEPTH_TEST );
	
	shader->Bind( true );


	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, textures[ 0 ] );
	shader->Uniform1i( "in_Diffuse1", 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, textures[ 1 ] );
	shader->Uniform1i( "in_Diffuse2", 1 );

	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, textures[ 2 ] );
	shader->Uniform1i( "in_Diffuse3", 2 );

	glActiveTexture( GL_TEXTURE3 );
	glBindTexture( GL_TEXTURE_2D, textures[ 3 ] );
	shader->Uniform1i( "in_Diffuse4", 3 );

	//glClientActiveTexture( GL_TEXTURE0 );
	//glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );
	
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, 7*sizeof( float ), NULL );
	glVertexAttribPointer( 1, 1, GL_FLOAT, false, 7*sizeof( float ), BUFFER_OFFSET( 3*sizeof( float ) ) );
	glVertexAttribPointer( 2, 3, GL_FLOAT, false, 7*sizeof( float ), BUFFER_OFFSET( 4*sizeof( float ) ) );

	glDrawArrays( GL_POINTS, 0, 7 );

	glDisableVertexAttribArray( 2 );
	glDisableVertexAttribArray( 1 );
	glDisableVertexAttribArray( 0 );
	

	REPEAT( 4 ) {
		glActiveTexture( GL_TEXTURE0 + i );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glEnable( GL_DEPTH_TEST );

	shader->Bind( false );
}

/*
===========================
	Calculate
===========================
*/
void vsLensFlare::Calculate( const vsVec3f& lightSource )
{
	if ( !buffer ) {
		glGenBuffers( 1, &buffer );

		glBindBuffer( GL_ARRAY_BUFFER, buffer );
	
		glBufferData( 	GL_ARRAY_BUFFER, 
						7 * ( 3 + 1 + 3 ) * sizeof( float ), 
						NULL, 
						GL_DYNAMIC_DRAW );
	} else {
		glBindBuffer( GL_ARRAY_BUFFER, buffer );
	}

	float* vert = ( float * ) glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

	float	texUnits[ 7 ] = { 0.0f, 2.0f, 3.0f, 2.0f, 1.0f, 3.0f, 2.0f };
	float	percents[ 7 ] = { 1.0f, 1.0f, 0.012f, 0.008f, 0.002f, 0.0f, -0.002f };
	vsVec4f colors[ 7 ] = { White, White, Orange, Cyan, Blue, Green, Red };
	vsVec3f	v			  = ( lightSource - player->GetLookingPoint( 4.0f ) );
	vsVec3f tmp;

	for ( int i = 0, k = 0; i < 7; i ++ ) {
		tmp = player->GetLookingPoint( 4.0f ) + v * percents[ i ];
		vert[ k++ ] = tmp.x;
		vert[ k++ ] = tmp.y;
		vert[ k++ ] = tmp.z;
		vert[ k++ ] = texUnits[ i ];
		vert[ k++ ] = colors[ i ].x;
		vert[ k++ ] = colors[ i ].y;
		vert[ k++ ] = colors[ i ].z;
	}

	glUnmapBuffer( GL_ARRAY_BUFFER );
}
