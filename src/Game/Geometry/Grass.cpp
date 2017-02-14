#include "Grass.h"
#include "..\Game.h"
#include "..\Player.h"
#include "..\Blocks.h"

using namespace vsSector2Texture;

/*
===========================================

	grassVertex_t

===========================================
*/
grassVertex_s::grassVertex_s( const vsVec3f& pos, const vsVec3f& N, const vsVec2f& uv )
{
	Position = pos;
	Normal = N;
	UV = uv;
}

grassVertex_s& grassVertex_s::operator=( const grassVertex_s& r )
{
	Position = r.Position;
	UV = r.UV;
	Normal = r.Normal;
	return( *this );
}
/*
===========================================

	grassDistData_t

===========================================
*/
grassDistData_t& grassDistData_t::operator=( const grassDistData_s& r ) {
	distance = r.distance;
	batchIndex = r.batchIndex;
	return ( *this );
}

bool grassDistData_t::operator==( const grassDistData_t& r ) {
	return( distance == r.distance && batchIndex == r.batchIndex );
}

bool grassDistData_t::operator>( const grassDistData_t& r ) {
	return ( distance > r.distance );
}

bool grassDistData_t::operator<( const grassDistData_t& r ) {
	return ( distance < r.distance );
}

bool grassDistData_t::operator>=( const grassDistData_t& r ) {
	return ( distance >= r.distance );
}

bool grassDistData_t::operator<=( const grassDistData_t& r ) {
	return ( distance <= r.distance );
}

/*
===========================================

	vsGrass

===========================================
*/
vsGrass::vsGrass( void )
{
	density = 0;
	//parentSectorNode = NULL;
}

/*
===================================
	vsGrass()
===================================
*/
vsGrass::vsGrass( const grassArgs_t& args )
{
	Init( args );
}

/*
===================================
	~vsGrass()
===================================
*/
vsGrass::~vsGrass( void )
{
//	if ( world!=NULL )
	//	glDeleteBuffers( 2, buffers );

	batches.DeletePointerElements();
	batches.Free();
	batchDistances.Free();
}

/*
===================================
	Init()
===================================
*/
void vsGrass::Init( const grassArgs_t& args )
{
	density = args.density;
	scale = args.scale;
	//parentSectorNode = args.parentSectorNode;
	wind = args.wind;
	texture = args.texture;
	shader = args.shader;
	shader->BindAttrib( 0, "in_Position" );
	shader->BindAttrib( 1, "in_Normal" );
	shader->BindAttrib( 2, "in_BasePoint" );
}

/*
===================================
	Render
===================================
*/
void vsGrass::Render( void )
{
	static vsVec3f lastPlayerDest;
	static vsVec3f lastPlayerRot;
	vsVec3f playerDest = player->GetPhysics()->GetDestination();
	vsVec3f playerRot = player->GetLookingPoint( 1.0f );

	//if ( lastPlayerDest != playerDest 
	 // || lastPlayerRot != playerRot
	//  ) 
	{
		//Generate();
		if ( batches.Length() > 0 )
		{
			for ( uint i=0; i<batchDistances.Length(); ++i )
			{
				batchDistances[i].distance = ( playerDest - 
					batches[batchDistances[i].batchIndex]->vert[0].Position ).Length();
			}
		
			bool swapped;
			grassDistData_t tmpd;
			do {
				swapped = false;

				REPU ( batchDistances.Length()-1 ) {
					if ( batchDistances[i].distance < batchDistances[i+1].distance ) {

						tmpd = batchDistances[i];
						batchDistances[ i ] = batchDistances[ i + 1 ];
						batchDistances[ i + 1 ] = tmpd;

						swapped = true;
					}
				}

			} while ( swapped );
			//batchDistances.QuickSort( false );
		}
		lastPlayerDest = playerDest;
	}

	shader->Bind( true );

	grassBatch_t* b;

	glActiveTexture( GL_TEXTURE0 );
	shader->Uniform1i( "texture", 0 );
	glBindTexture( GL_TEXTURE_2D, texture );

	glBegin( GL_QUADS );
	for ( uint i=0; i<batchDistances.Length(); ++i )
	{
		if ( batchDistances[i].distance>50.0f ) 
			continue;

		b = batches[batchDistances[i].batchIndex];
		for ( uint j=0; j<12; ++j ) {
			glMultiTexCoord2fv( GL_TEXTURE0, b->vert[j].UV.ToTypePtr() );
			glVertexAttrib3fv( 2, b->basePoint.ToTypePtr() );
			glVertexAttrib3fv( 1, b->vert[j].Normal.ToTypePtr() );
			glVertexAttrib3fv( 0, b->vert[j].Position.ToTypePtr() );		
		}
	}
	glEnd();

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	shader->Bind( false );
}

/*
===================================
	Generate
===================================
*/
void vsGrass::Generate( vsSector2OTNode* n, int density )
{
	//if ( !parentSectorNode ) return;

	vsGrass::~vsGrass();

	vsVec3f p[3], N, T;
	vsVec3f v[2];

	//vsSector2* s = game->world->BindRenderSector( sectorId, true );

//	const vsSector2OTNode* n;

	//if ( !n ) return;
//	if ( s->IsGenerated() == false ) 
	//	return;
//	if ( n->vertices.Length() == 0 ) 
	//	return;
	ushort type_grass = blockLibrary->GetBlockTypeId( "grass" );
	int d1 = ( density<0 )? -density : 1;
	int d2 = ( density>0 )? density: 1;
		if ( /*n->visible &&*/ n->IsLeaf() ) 
			for ( uint j=0; j<n->indices.Length(); j+=3*d1 )
			REPEAT( d2 )
			{
				if ( n->vertices[ n->indices[ j ] ].reserved[2] == type_grass )
				{
					// Get triangle points
					p[0] = n->vertices[n->indices[j]].xyz;
					p[1] = n->vertices[n->indices[j+1]].xyz;
					p[2] = n->vertices[n->indices[j+2]].xyz;

					N = ( n->vertices[n->indices[j]].Normal +
							n->vertices[n->indices[j+1]].Normal +
							n->vertices[n->indices[j+2]].Normal ) * 0.333333f;

					v[0] = RandomLerp( p[0], p[1] );
					v[1] = RandomLerp( p[0], p[2] );

					AddNewBatch( 0.5f*( v[0]+v[1] ), N );
				}
			}

	//game->world->ReleaseRenderSector( s );
}

/*
===================================
	AddNewBatch
===================================
*/
void vsGrass::AddNewBatch( const vsVec3f& p, const vsVec3f& N )
{
	grassBatch_t* batch = new grassBatch_t();

	vsVec3f v[4];
	v[0] =  vsVec3f( -scale.x, 0.f, 0.f );
	v[1] =  vsVec3f( scale.x, 0.f, 0.f );
	v[2] =  vsVec3f( scale.x, scale.y, 0.f );
	v[3] =  vsVec3f( -scale.x, scale.y, 0.f );

//	float Rx = ACos( vsVec3f( 1.f, 0.f, 0.f ).Dot( N ) );
//	float Ry = ACos( vsVec3f( 0.f, 1.f, 0.f ).Dot( N ) );
//	float Rz = ACos( vsVec3f( 0.f, 0.f, 1.f ).Dot( N ) );

	vsQuaternion Q,  Z;
	//Q.CreateFromEulerAnglesRAD( Rx, Ry, Rz );
	Q.CreateFromEulerAnglesDEG( NULLVEC3F );

	Z.CreateFromEulerAnglesRAD( 0.f, PI*0.3333f, 0.f );

	float tv0 = 0.01f;
	float tv1 = 0.99f;
	vsVec2f t0 = vsVec2f( 0.f, tv1 );
	vsVec2f t1 = vsVec2f( 1.f, tv1 );
	vsVec2f t2 = vsVec2f( 1.f, tv0 );
	vsVec2f t3 = vsVec2f( 0.f, tv0 );

	// Generate geometry
	batch->vert[0] = grassVertex_t( p+Q*v[0], N, t0 );
	batch->vert[1] = grassVertex_t( p+Q*v[1], N, t1 );
	batch->vert[2] = grassVertex_t( p+Q*v[2], N, t2 );
	batch->vert[3] = grassVertex_t( p+Q*v[3], N, t3 );

	REPEAT( 4 ) v[i] = Z*v[i];
	batch->vert[4] = grassVertex_t( p+Q*v[0], N, t0 );
	batch->vert[5] = grassVertex_t( p+Q*v[1], N, t1 );
	batch->vert[6] = grassVertex_t( p+Q*v[2], N, t2 );
	batch->vert[7] = grassVertex_t( p+Q*v[3], N, t3 );

	REPEAT( 4 ) v[i] = Z*v[i];
	batch->vert[8]  = grassVertex_t( p+Q*v[0], N, t0 );
	batch->vert[9]  = grassVertex_t( p+Q*v[1], N, t1 );
	batch->vert[10] = grassVertex_t( p+Q*v[2], N, t2 );
	batch->vert[11] = grassVertex_t( p+Q*v[3], N, t3 );

	batch->basePoint = p;

	// Append batch
	batches.Append( batch );

	// Append distance data
	grassDistData_t gdd;
	gdd.batchIndex = batches.Length()-1;
	batchDistances.Append( gdd );
}

/*
===================================
	SetWindDirection
===================================
*/
void vsGrass::SetWindDirection( const vsVec3f& wd )
{
	wind = wd;
}

/*
===================================
	RandomLerp
===================================
*/
vsVec3f vsGrass::RandomLerp( const vsVec3f& v1, const vsVec3f& v2 )
{
	float r = float( rand() % 200 ) * 0.005f;

	return v1 * ( 1.0f - r ) + v2 * r;
}
