#include "Skydome.h"
#include "..\Player.h"
#include "..\Timer.h"
using namespace vsLib2;


/* 
=====================================
	vsSun
=====================================
*/
vsSun::vsSun( void ) 
{
}

/* 
=====================================
	Init
=====================================
*/
void vsSun::Init( float		distanceFromPlayer, 
				  float		size, 
				  int		numRays, 
				  vsShader* shader,
				  uint		sunTex, 
				  uint		rayTex  )
{
	this -> basePosition = vsVec3f( 0.0f, -distanceFromPlayer, 0.0f );
	this -> size	 = size;
	this -> numRays  = numRays;
	this -> shader   = shader;
	this -> sunTex	 = sunTex;
	this -> rayTex   = rayTex;

	vertices.Alloc( 4 * ( numRays + 1 ) );
	baseVertices.Alloc( 4 * ( numRays + 1 ) );

	baseVertices[ 0 ].pos = basePosition + vsVec3f( -size, 0.0f, -size );
	baseVertices[ 1 ].pos = basePosition + vsVec3f( size, 0.0f, -size );
	baseVertices[ 2 ].pos = basePosition + vsVec3f( size, 0.0f,  size );
	baseVertices[ 3 ].pos = basePosition + vsVec3f( -size, 0.0f,  size );

	for ( int i = 4; i < 4 * ( numRays + 1 ); i += 4 ) {
		baseVertices[ i ].pos   = vsVec3f( -1.1f * size, 10.0f, -size*0.02f );
		baseVertices[ i+1 ].pos = vsVec3f( -1.6f * size, 10.0f, -size*0.02f );
		baseVertices[ i+2 ].pos = vsVec3f( -1.6f * size, 10.0f,  size*0.02f );
		baseVertices[ i+3 ].pos = vsVec3f( -1.1f * size, 10.0f,  size*0.02f );
	}

	for ( int i = 0; i < 4 * ( numRays + 1 ); i += 4 ) {
		vertices[ i ].st = vsVec2f( 0.0f, 0.0f );
		vertices[ i + 1 ].st = vsVec2f( 1.0f, 0.0f );
		vertices[ i + 2 ].st = vsVec2f( 1.0f, 1.0f );
		vertices[ i + 3 ].st = vsVec2f( 0.0f, 1.0f );
	}
}

/* 
=====================================
	CreateSunGeometry
=====================================
*/
void vsSun::CreateSunGeometry( void )
{
	static const float phi = PI / 12.0f;

	vsQuaternion q, r, d;
	q.CreateFromEulerAnglesRAD( 0.0f, 0.0f, dayTime * phi );
	r.CreateFromEulerAnglesRAD( 0.0f, dayTime*5.0f * phi, 0.0f );
	d.CreateFromEulerAnglesRAD( 0.0f, phi, 0.0f );

	rayScale = 1.0f + sinf( dayTime*20.0f )*0.1f+0.1f;

	REPEAT ( 4 )
		vertices[ i ].pos = baseVertices[ i ].pos * q;

	vsVec3f playerPos = player->GetPhysics()->GetDestination();
	for ( int i = 4; i < 4 * ( numRays + 1 ); i ++ ) {
		vertices[ i ].pos = ( playerPos + basePosition + ( ( baseVertices[ i ].pos*rayScale ) * r ) ) * q;
		r *= d;
	}
}

/* 
=====================================
	Render
=====================================
*/
void vsSun::Render( float dayTime )
{
	this -> dayTime = dayTime;

	CreateSunGeometry();

	if ( !shader )
		return;

	/*shader->Bind( true );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, sunTex );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, rayTex );

	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( sunVert_t ), vertices.list );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( sunVert_t ), ( vertices.list + 12 ) );
	glDrawArrays( GL_QUADS, 0, 4+0*vertices.Length() );
	glDisableVertexAttribArray( 0 );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	shader->Bind( false );*/

	CreateSunGeometry();

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, sunTex );

	glBegin( GL_QUADS );
		REPEAT( 4 ) {
			glMultiTexCoord2fv( GL_TEXTURE0, vertices[ i ].st.ToTypePtr() );
			glVertex3fv( vertices[ i ].pos.ToTypePtr() );
		}
	glEnd();

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, rayTex );

	glBegin( GL_QUADS );
		for ( uint i = 4; i < vertices.Length(); i++ ) {
			glMultiTexCoord2fv( GL_TEXTURE0, vertices[ i ].st.ToTypePtr() );
			glVertex3fv( vertices[ i ].pos.ToTypePtr() );
		}
	glEnd();

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
}



/* 
=====================================

	vsAtmosphere

=====================================
*/
vsAtmosphere::vsAtmosphere( void )
{
	pShader = NULL;
}


/* 
=====================================
	~vsAtmosphere
=====================================
*/
vsAtmosphere::~vsAtmosphere( void )
{
	if ( pShader )
	{
		glDeleteBuffers( 2, vbo );
	}
}

/* 
=====================================
	BuildGeometry
=====================================
*/
VSRESULT vsAtmosphere::BuildGeometry( uint sides, uint slices, const vsVec3f& radius,
							   uint texture, vsShader* pShader )
{
	( slices ); ( sides ); ( radius );

	if ( !pShader ) {
		console->Print( CMT_ERROR, "vsAtmosphere::BuildGeometry(): param pShader is NULL" );
		return VS_ERROR;
	}
	
	this->pShader = pShader;
	this->pShader->BindAttrib( 0, "in_Position" );
	//this->pShader->BindAttrib( 1, "in_TexCoord0" );
	this->texture = texture;
	/*this->sides = sides;
	this->slices = slices;
	this->slices1 = slices+1;
	
	float fslices = ( float )slices;
	float fsides  = ( float )sides;
	float phi = 2.0f * PI / fslices;	// horizontal
	float tao = 0.7f * PI / fsides;		// vertical
	vsVertex V;	

	renderSystem->GenVertexBuffer( 	&vbo[VERTEX_BUFFER],
									GL_ARRAY_BUFFER,
									sizeof( vsVertex )*( sides+1 ) * ( slices1 ),
									NULL,
									GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER] );
	vsVertex* vertices = ( vsVertex* )glMapBuffer( GL_ARRAY_BUFFER,GL_WRITE_ONLY );

	int k=0;
	float i=0,j=0;
	for ( float v = 0.f,h; v<=fsides; ++v )
	for ( h = 0.f; h<=fslices; ++h )
	{
		i = h * phi;
		j = v * tao - 0.2f * PI;

		V.xyz.x = Sin( i ) * Cos( j );
		V.xyz.z = Cos( i ) * Cos( j );
		V.xyz.y = Sin( j );

		V.xyz *= radius;
				
		V.st = vsVec2f( h / fslices, v / fsides );
		vertices[k++] = V;
	}

	glUnmapBuffer( GL_ARRAY_BUFFER );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	renderSystem->GenVertexBuffer( 	&vbo[INDEX_BUFFER],
									GL_ELEMENT_ARRAY_BUFFER,
									4*sides*slices1*2,
									NULL,
									GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER] );
	uint* indices = ( uint* ) glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY );

	k=0;
	uint h, h2;
	for ( uint v = 1; v<sides+1; ++v )
	{
		if ( v%2==1 )
			for ( h = 0; h<slices1; ++h )
			{
				indices[k++] = ( v-1 ) * slices1 + h;
				indices[k++] = v * slices1 + h;	
			}
		else
			for ( h = 0; h<slices1; ++h )
			{
				h2 = slices-h;
				indices[k++] = ( v-1 ) * slices1 + h2;
				indices[k++] = v * slices1 + h2;	
			}
		
	}
	glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	*/
	sun.Init( radius.x*0.9f, radius.x*0.1f, 12, this->pShader, 
				renderSystem->LoadTexture( "data/textures/sky/sun.dds" ),
				renderSystem->LoadTexture( "data/textures/sky/sunray.dds" ) );
				
	return VS_OK;
}

/* 
=====================================
	Render
=====================================
*/
void vsAtmosphere::Render( float dayTime )
{
	if ( pShader ) {
		pShader->Bind( true );
		pShader->Uniform1f( "in_DayTime", dayTime );
	}

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );

	float s = timer.GetTime() * 0.001f;
	vsVec3f pos = player->GetPhysics()->GetDestination();
	float size = 50000.0f;
	glBegin( GL_QUADS );
		glMultiTexCoord2f( GL_TEXTURE0, s, 0.0f );
		glVertexAttrib3f( 0, pos.x - size, 1000.0f, pos.z - size );

		glMultiTexCoord2f( GL_TEXTURE0, s + 5.0f, 0.0f );
		glVertexAttrib3f( 0, pos.x + size, 1000.0f, pos.z - size );

		glMultiTexCoord2f( GL_TEXTURE0, s + 5.0f, 5.0f );
		glVertexAttrib3f( 0, pos.x + size, 1000.0f, pos.z + size );

		glMultiTexCoord2f( GL_TEXTURE0, s, 5.0f );
		glVertexAttrib3f( 0, pos.x - size, 1000.0f, pos.z + size );
	glEnd();

	/*glBindBuffer( GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER] );

	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableVertexAttribArray( 0 );
	//glEnableVertexAttribArray( 1 );
	

	glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( vsVertex ), NULL );
	//glVertexAttribPointer( 1, 2, GL_FLOAT, false, sizeof( vsVertex ), BUFFER_OFFSET( 36 ) );
	glTexCoordPointer( 3, GL_FLOAT, sizeof( vsVertex ), BUFFER_OFFSET( vsVertex::Offset_st ) );


	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER] );

	glDrawElements( 	GL_TRIANGLE_STRIP, 
					2*slices1*sides, 
					GL_UNSIGNED_INT, 
					NULL );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	
	
	//glDisableVertexAttribArray( 1 );
	glDisableVertexAttribArray( 0 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	*/

	glBindTexture( GL_TEXTURE_2D, 0 );

	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	if ( pShader )pShader->Bind( false );



	sun.Render( dayTime );
}

/* 
=====================================
	GetSunPosition
=====================================
*/
vsVec3f vsAtmosphere::GetSunPosition( float dayTime )
{
	static const float phi = PI / 12.0f;
	vsQuaternion q;
	q.CreateFromEulerAnglesRAD( 0.0f, 0.0f, dayTime * phi );

	return sun.basePosition * q;
}