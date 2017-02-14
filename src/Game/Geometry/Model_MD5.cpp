#include "Model_MD5.h"
#include "..\..\System\VarStorage.h"
#include "ModelManager.h"

using namespace vsLib2;
using namespace vsMath;

#pragma region vsAnimation_MD5

/* 
=====================================
	vsAnimation_MD5
=====================================
*/
vsAnimation_MD5::vsAnimation_MD5( void )
{
	length = 0.0f;
	numAnimatedComponents = 0;
	frameRate = 0;
	loaded = false;
	filename = NULL;
}

/* 
=====================================
	~vsAnimation_MD5
=====================================
*/
vsAnimation_MD5::~vsAnimation_MD5( void )
{
	jointInfos.Free();
	frames.Free();
}

/* 
=====================================
	LoadFromFile
=====================================
*/
int vsAnimation_MD5::LoadFromFile( const char* szFileName )
{
	FILE* f;
	if ( fopen_s( &f, szFileName, "r" ) ) {
		console->Print( CMT_ERROR, "Failed to load MD5 animation: %s", szFileName );
		return VS_ERROR;
	}


	char line[512];
	int p, q, curJoint;
	vsVec3f min, max;

	while( !feof( f ) )
	{
		fgets( line, 512, f );

		if ( strstr( line, "MD5Version" ) ) {
			sscanf_s( line, "MD5Version %d", &version );
		} else

		if ( strstr( line, "numFrames" ) ) {
			sscanf_s( line, "numFrames %d", &p );
			frames.Alloc( p );
		} else

		if ( strstr( line, "numAnimatedComponents" ) ) {
			sscanf_s( line, "numAnimatedComponents %d", &numAnimatedComponents );
		} else

		if ( strstr( line, "frameRate" ) ) {
			sscanf_s( line, "frameRate %d", &frameRate );
		} else

		if ( strstr( line, "numJoints" ) ) {
			sscanf_s( line, "numJoints %d", &p );
			jointInfos.Alloc( p );
			baseFrameSkeleton.joints.Alloc( p );
		} else

		if ( strstr( line, "hierarchy {" ) ) {
			REPU( jointInfos.Length() )
			{
				fgets( line, 512, f );
				sscanf_s( line, " %s %d %d %d",
					&jointInfos[i].name, 32,
					&jointInfos[i].parentIndex,
					&jointInfos[i].flags,
					&jointInfos[i].startIndex );
			}
		} else

		if ( strstr( line, "bounds {" ) ) {
			REPU( frames.Length() )
			{
				fgets( line, 512, f );
				sscanf_s( line, " ( %f %f %f ) ( %f %f %f )",
					&min.x, &min.y, &min.z,
					&max.x, &max.y, &max.z );
				frames[i].bounds.FromMinMax( min, max );
			}
		} else

		if ( strstr( line, "baseframe {" ) ) {
			REPU( jointInfos.Length() )
			{
				fgets( line, 512, f );

				sscanf_s( line, " ( %f %f %f ) ( %f %f %f )",
					&baseFrameSkeleton.joints[i].pos.x,
					&baseFrameSkeleton.joints[i].pos.y,
					&baseFrameSkeleton.joints[i].pos.z,
					&baseFrameSkeleton.joints[i].orient.x,
					&baseFrameSkeleton.joints[i].orient.y,
					&baseFrameSkeleton.joints[i].orient.z );
				baseFrameSkeleton.joints[i].orient.CalcW();
			}
		} else

		if ( strstr( line, "frame" ) && strchr( line, '{' ) ) {
			sscanf_s( line, "frame %d {", &p );
			
			frames[p].components.Alloc( numAnimatedComponents );

			for ( int i=0; i<numAnimatedComponents; ++i )
				fscanf_s( f, "%f", &frames[p].components[i] );
			BuildFrameSkeleton( p );
		} 
	}

	// animation length
	length = ( frames.Length()-1 ) * ( 1.0f/frameRate );

	// total movement delta
	int j = 0;
	if ( !numAnimatedComponents ) {
		totalDelta = NULLVEC3F;
	} else {

		if ( jointInfos[ 0 ].flags & 1 ) {
			for ( uint i = 0; i < frames.Length(); i++ ) {
				frames[ i ].components[ jointInfos[ 0 ].startIndex ] -= baseFrameSkeleton.joints[ 0 ].pos.x;
			}
			totalDelta.x = frames[ frames.Length() - 1 ].components[ j ];
			j++;
		} else 
			totalDelta.x = 0.0f;

		if ( jointInfos[ 0 ].flags & 2 ) {
			for ( uint i = 0; i < frames.Length(); i++ ) {
				frames[ i ].components[ jointInfos[ 0 ].startIndex + j ] -= baseFrameSkeleton.joints[ 0 ].pos.y;
			}
			totalDelta.y = frames[ frames.Length() - 1 ].components[ j ];
			j++;
		} else 
			totalDelta.y = 0.0f;


		if ( jointInfos[ 0 ].flags & 4 ) {
			for ( uint i = 0; i < frames.Length(); i++ ) {
				frames[ i ].components[ jointInfos[ 0 ].startIndex + j ] -= baseFrameSkeleton.joints[ 0 ].pos.z;
			}
			totalDelta.z = frames[ frames.Length() - 1 ].components[ j ];
		} else
			totalDelta.z = 0.0f;
	}

	fclose( f );
	loaded = true;
	filename = ( char* )szFileName;
	return VS_OK;
}

/* 
=====================================
	BuildFrameSkeleton
=====================================
*/
void vsAnimation_MD5::BuildFrameSkeleton( int frameNum )
{
	MD5Frame_t* f = &frames[frameNum];

	f->skeleton.joints.Alloc( baseFrameSkeleton.joints.Length() );

	MD5Joint_t* parentJ;
	vsVec3f pos;
	vsQuaternion orient;
	int j=0;

	REPU( f->skeleton.joints.Length() ) 
	{
		pos = baseFrameSkeleton.joints[i].pos;
		orient = baseFrameSkeleton.joints[i].orient;

		j=0;
		if ( jointInfos[i].flags & 1 ) {
			pos.x = f->components[ jointInfos[i].startIndex + j++ ];
		}
		if ( jointInfos[i].flags & 2 ) {
			pos.y = f->components[ jointInfos[i].startIndex + j++ ];
		}
		if ( jointInfos[i].flags & 4 ) {
			pos.z = f->components[ jointInfos[i].startIndex + j++ ];
		}
		if ( jointInfos[i].flags & 8 ) {
			orient.x = f->components[ jointInfos[i].startIndex + j++ ];
		}
		if ( jointInfos[i].flags & 16 ) {
			orient.y = f->components[ jointInfos[i].startIndex + j++ ];
		}
		if ( jointInfos[i].flags & 32 ) {
			orient.z = f->components[ jointInfos[i].startIndex + j++ ];
		}
		orient.CalcW();

		strcpy_s<32>( f->skeleton.joints[i].name, jointInfos[i].name );
		f->skeleton.joints[i].parent = jointInfos[i].parentIndex;

		if ( jointInfos[ i ].parentIndex < 0 ) {
			f->skeleton.joints[ i ].pos = pos;
			f->skeleton.joints[ i ].orient = orient;
		} else {
			parentJ = &f->skeleton.joints[ jointInfos[i].parentIndex ];

			pos *= parentJ->orient;
			f->skeleton.joints[ i ].pos = parentJ->pos + pos;
			f->skeleton.joints[ i ].orient = parentJ->orient * orient;
			f->skeleton.joints[ i ].orient.Normalize();
		}
	}
}

#pragma endregion
#pragma region MD5Mesh_t
/* 
=====================================


	MD5Mesh_t


=====================================
*/
MD5Mesh_t::MD5Mesh_t( void ) 
{
	buffersGenerated = false;
	material = NULL;
}

/* 
=====================================
	~MD5Mesh_t
=====================================
*/
MD5Mesh_t::~MD5Mesh_t( void ) 
{
	triangles.Free(); 
	vertices.Free(); 
	weights.Free();
}

/* 
=====================================
	UpdateBuffers
=====================================
*/
void MD5Mesh_t::UpdateBuffers( bool justVertex )
{
	if ( buffersGenerated ) {
		glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
		glBufferSubData( GL_ARRAY_BUFFER, 0,
						sizeof( vsVertex ) * finalVertices.Length(),
						finalVertices.list );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		if ( !justVertex ) {
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[1] );
			glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0,
							sizeof( uint ) * triangles.Length()*3,
							triangles.list );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}	
	} else {
		glGenBuffers( 2, buffers );
		glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
		glBufferData( 	GL_ARRAY_BUFFER, 
						sizeof( vsVertex ) * finalVertices.Length(),
						finalVertices.list,
						GL_DYNAMIC_DRAW );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[1] );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
						sizeof( uint ) * triangles.Length()*3,
						triangles.list,
						GL_DYNAMIC_DRAW );	
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		buffersGenerated = true;
	}
}

/* 
=====================================
	CalculateLocalJointNormals
=====================================
*/
void MD5Mesh_t::CalculateNormals( void ) 
{
	vsVertex *a, *b, *c;
	vsList<vsVec3f>	faceNormals;
	vsList<vsVec3f>	faceTangents;

	faceNormals.Alloc( triangles.Length() );
	faceTangents.Alloc( triangles.Length() );

	REPU2( j, triangles.Length() ) 
	{
		a = &finalVertices[triangles[j].indices[0]];
		b = &finalVertices[triangles[j].indices[1]];
		c = &finalVertices[triangles[j].indices[2]];

		a->CalculateTangent( *b, *c );
		faceNormals[j] = a->Normal;
		faceTangents[j] = a->Tangent;
	}

	vsVec3f N, T;
	REPU( finalVertices.Length() ) {
		N = T = NULLVEC3F;
		REPU2( j, triangles.Length() ) {
			if ( finalVertices[triangles[j].indices[0]].xyz == finalVertices[i].xyz ||
			   finalVertices[triangles[j].indices[1]].xyz == finalVertices[i].xyz ||
			   finalVertices[triangles[j].indices[2]].xyz == finalVertices[i].xyz )
			{
				N -= faceNormals[j];
				T -= faceTangents[j];
			}
		}

		finalVertices[i].Normal = N.Normalized();
		finalVertices[i].Tangent = T.Normalized();
	}

	faceNormals.Free();
	faceTangents.Free();
}

#pragma endregion 

#pragma region vsModel_MD5
/* 
=====================================


	vsModel_MD5


=====================================
*/
vsModel_MD5::vsModel_MD5( void )
{
	loaded = false;
	shader = NULL;
	currentAnimation = -1;
	currentAnimPlayMode = 0;
	nextAnimation = -1;
	nextAnimPlayMode = 0;
	filename = NULL;
}

/* 
=====================================
	~vsModel_MD5
=====================================
*/
vsModel_MD5::~vsModel_MD5( void )
{
	//animations.DeletePointerElements();
	//animations.Free();
	meshes.DeletePointerElements();
	meshes.Free();
}

/* 
=====================================
	LoadFromFile
=====================================
*/
int vsModel_MD5::LoadFromFile( const MD5ModelArgs_t& args )
{
	FILE* f;
	if ( fopen_s( &f, args.szFileName, "r" ) ) {
		console->Print( CMT_ERROR, "Failed to load MD5Mesh: %s", args.szFileName );
		return VS_ERROR;
	}

	char line[512], *c;
	int p, q, r, s;
	int curMesh=0, curJoint=0;
	MD5Mesh_t* mesh;

	while( !feof( f ) ) {
		fgets( line, 512, f );

		if ( strstr( line, "MD5Version" ) ) {
			sscanf_s( line, "MD5Version %d", &version );
		} else

		if ( strstr( line, "numMeshes" ) ) {
			sscanf_s( line, "numMeshes %d", &p );
			meshes.Alloc( p );

		} else

		if ( strstr( line, "numJoints" ) ) {
			sscanf_s( line, "numJoints %d", &p );
			bindposeSkeleton.joints.Alloc( p );
		} else

		if ( strstr( line, "joints {" ) ) {
			curJoint=0;
			while( !strstr( line, "}" ) ) {
				fgets( line, 512, f );
				
				sscanf_s( line, " %s %d ( %f %f %f ) ( %f %f %f )",
					&bindposeSkeleton.joints[curJoint].name, 32,
					&bindposeSkeleton.joints[curJoint].parent,
					&bindposeSkeleton.joints[curJoint].pos.x,
					&bindposeSkeleton.joints[curJoint].pos.y,
					&bindposeSkeleton.joints[curJoint].pos.z,
					&bindposeSkeleton.joints[curJoint].orient.x,
					&bindposeSkeleton.joints[curJoint].orient.y,
					&bindposeSkeleton.joints[curJoint].orient.z );
				bindposeSkeleton.joints[curJoint].orient.CalcW();

				curJoint++;
			}
		} else

		if ( strstr( line, "mesh {" ) ) {
			meshes[curMesh] = new MD5Mesh_t();
			mesh = meshes[curMesh];
			while( !strstr( line, "}" ) ) {
				fgets( line, 512, f );

				// Material
				if ( strstr( line, "shader" ) ) {
					char materialName[256];
					sscanf_s( line, " shader %s", &materialName, 256 );
					// cut "s at the beginning and at the end

					materialName[ strlen( materialName )-1 ] = '\0';
					mesh->material = modelManager->RequestMaterial( strchr( materialName, materialName[1] ) );
				} else
				// Vertices
				if ( strstr( line, "numverts" ) ) {
					sscanf_s( line, " numverts %d", &p );
					mesh->vertices.Alloc( p );
					for ( uint i=0; i<mesh->vertices.Length(); ++i ) {
						fgets( line, 512, f );
						sscanf_s( line, " vert %d ( %f %f ) %d %d",
							&q,
							&mesh->vertices[i].st.x,
							&mesh->vertices[i].st.y,
							&mesh->vertices[i].weightStart,
							&mesh->vertices[i].weightCount );
					}
				} else
				// TRIANGLES
				if ( strstr( line, "numtris" ) ) {
					sscanf_s( line, " numtris %d", &p );
					mesh->triangles.Alloc( p );
					for ( uint i=0; i<mesh->triangles.Length(); ++i ) {
						fgets( line, 512, f );
						sscanf_s( line, " tri %d %d %d %d",
							&q,
							&mesh->triangles[i].indices[0],
							&mesh->triangles[i].indices[1],
							&mesh->triangles[i].indices[2] );
					}
				} else
				// WEIGHTS
				if ( strstr( line, "numweights" ) ) {
					sscanf_s( line, " numweights %d", &p );
					mesh->weights.Alloc( p );
					for ( uint i=0; i<mesh->weights.Length(); ++i ) {
						fgets( line, 512, f );
						sscanf_s( line, " weight %d %d %f ( %f %f %f )",
							&q,
							&mesh->weights[i].jointIndex,
							&mesh->weights[i].bias,
							&mesh->weights[i].pos.x,
							&mesh->weights[i].pos.y,
							&mesh->weights[i].pos.z );
					}
				}
			} // END MESH

			curMesh++;
		}
	}

	shader = args.shader;

	shader->BindAttrib( 0, "in_Position" );
	shader->BindAttrib( 1, "in_Normal" );
	shader->BindAttrib( 2, "in_Tangent" );

	scale = args.scale;
	offset = args.offset;
	rotation.CreateFromEulerAnglesDEG( args.rotation );
	direction = ( vsVec3f( 1.0f, 0.0f, 0.0f ) * rotation ).Normalize();

	//BuildModel( &bindposeSkeleton, true );

	fclose( f );

	loaded = true;
	filename = ( char* )args.szFileName;
		
	return VS_OK;
}

/* 
=====================================
	Render
=====================================
*/
void vsModel_MD5::Render( void )
{
	if ( !shader || !loaded ) return;
	shader->Bind( true );
	REPU( meshes.Length() ) 
	{
		glBindBuffer( GL_ARRAY_BUFFER, meshes[i]->buffers[0] );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, meshes[i]->material->diffuseMap );
		shader->Uniform1i( "in_DiffuseMap", 0 );

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, meshes[i]->material->normalMap );
		shader->Uniform1i( "in_NormalMap", 1 );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, meshes[i]->material->specularMap );
		shader->Uniform1i( "in_SpecularMap", 2 );

		glClientActiveTexture( GL_TEXTURE0 );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, sizeof( vsVertex ), BUFFER_OFFSET( vsVertex::Offset_st ) );
		
		glEnableVertexAttribArray( 0 );
		glEnableVertexAttribArray( 1 );
		glEnableVertexAttribArray( 2 );
		
		
		glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( vsVertex ), 0 );
		glVertexAttribPointer( 1, 3, GL_FLOAT, false, sizeof( vsVertex ), BUFFER_OFFSET( vsVertex::Offset_Normal ) );
		glVertexAttribPointer( 2, 3, GL_FLOAT, false, sizeof( vsVertex ), BUFFER_OFFSET( vsVertex::Offset_Tangent ) );
		
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, meshes[i]->buffers[1] );
		
		glDrawElements( GL_TRIANGLES, 
					   meshes[i]->triangles.Length()*3, 
					   GL_UNSIGNED_INT, 
					   NULL );
		
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		glDisableVertexAttribArray( 0 );
		glDisableVertexAttribArray( 1 );
		glDisableVertexAttribArray( 2 );

		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE1 );	
		glBindTexture( GL_TEXTURE_2D, 0 );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
	shader->Bind( false );

	if ( varStorage.Bool["rendernormals"] ) 
	{
		glBegin( GL_LINES );
		REPU( meshes.Length() ) 
		{
			REPU2( j, meshes[i]->finalVertices.Length() ) {
				glColor4f( 1.0f, 0.0f, 1.0f, 1.0f );
				glVertex3fv( meshes[i]->finalVertices[j].xyz.ToTypePtr() );
				glColor4f( 0.0f, 0.8f, 0.0f, 1.0f );
				glVertex3fv( ( meshes[i]->finalVertices[j].xyz+meshes[i]->finalVertices[j].Normal*scale ).ToTypePtr() );
			}
			
		}
		glEnd();
	}


	// Offset
	/*glPointSize( 10.0f );
	glColor4f( 1.0f, 0.0f, 1.0f, 1.0f );
	glBegin( GL_POINTS );
	glVertex3fv( offset.ToTypePtr() );
	glEnd();*/
}

/* 
=====================================
	RenderSkeleton
=====================================
*/
void vsModel_MD5::RenderSkeleton( void )
{
	glLineWidth( 10.0f );
	glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );

	glBegin( GL_LINES );
		for ( uint i=0; i<currentSkeleton.joints.Length(); ++i ) 
		{
			if ( currentSkeleton.joints[i].parent!=-1 ) {
				glVertex3fv( currentSkeleton.joints[i].pos.ToTypePtr() );
				glVertex3fv( currentSkeleton.joints[currentSkeleton.joints[i].parent].pos.ToTypePtr() );
			}
		}
	glEnd();
}

/* 
=====================================
	RenderBindpose
=====================================
*/
void vsModel_MD5::RenderBindpose( void )
{
	BuildModel( &bindposeSkeleton );
	Render();
}

/* 
=====================================
	RenderAnimated
=====================================
*/
void vsModel_MD5::RenderAnimated( void )
{ 
	BuildModel( ( currentAnimation==-1 )? &bindposeSkeleton : &currentSkeleton );
	Render();
}

/* 
=====================================
	Animate
=====================================
*/
void vsModel_MD5::Animate( float deltaTime )
{
	if ( currentAnimation == -1 )
		return;

	vsAnimation_MD5* anim = animations[ currentAnimation ];
	float animLen = anim->GetLength();
	

	if ( ( currentAnimTime < animLen && currentAnimTime + deltaTime > animLen ) ) {
		offset += ( anim->GetTotalDelta() * scale * rotation ); 

		if ( currentAnimPlayMode & ANIM_PLAY_ONCE ) {
			currentAnimation = nextAnimation;
			currentAnimPlayMode = nextAnimPlayMode;
			nextAnimation = -1;
			nextAnimPlayMode = -1;
		}
	}
		  //|| ( nextAnimPlayMode & ANIM_PLAY_IMMEDIATELY )
	//	  )
	//{
		currentAnimTime = fmod( currentAnimTime + deltaTime, animLen );
	const MD5Skeleton_t * skeleton1 = anim->GetFrameSkeleton( ( int )floorf( currentAnimTime * anim->frameRate ) );
	const MD5Skeleton_t * skeleton2;

		

		//anim = animations[ currentAnimation ];
	//	skeleton2 = anim->GetFrameSkeleton( ( int )floorf( currentAnimTime * anim->frameRate )  );	
	//} else {
		//currentAnimTime += deltaTime;
		skeleton2 = anim->GetFrameSkeleton( ( int )ceilf( currentAnimTime * anim->frameRate ) );
//	}

	float f = ( currentAnimTime * anim->frameRate ) - floorf( currentAnimTime * anim->frameRate );

	if ( currentSkeleton.joints.Length() != skeleton1->joints.Length() )
		currentSkeleton.joints.Alloc( skeleton1->joints.Length() );

	REPU ( skeleton1->joints.Length() ) 
	{
		currentSkeleton.joints[i].pos.Lerp( skeleton1->joints[i].pos, 
											skeleton2->joints[i].pos, 
											f );

		currentSkeleton.joints[i].orient.Slerp( skeleton1->joints[i].orient, 
												skeleton2->joints[i].orient, 
												f );
	}
}

/* 
=====================================
	BuildBindposeSkeleton
=====================================
*/
void vsModel_MD5::BuildModel( MD5Skeleton_t* skeleton )
{
	MD5Mesh_t* m;
	MD5Weight_t* w;
	MD5Joint_t* joint;
	vsVec3f rotatedWeightPos;

	bool localNorms = false;

	REPU( meshes.Length() ) 
	{
		m = meshes[i];
		if ( !m->buffersGenerated )
			m->finalVertices.Alloc( m->vertices.Length() );

		REPU2( j, m->vertices.Length() ) 
		{
			m->finalVertices[j].xyz = NULLVEC3F;
			//m->finalVertices[j].Normal = m->vertices[j].localNormal;
			//m->finalVertices[j].Tangent = m->vertices[j].localTangent;

			for ( int k = m->vertices[j].weightStart; 
				k < m->vertices[j].weightStart+m->vertices[j].weightCount; 
				++k ) 
			{
				w=&m->weights[k];
				joint = &( skeleton->joints[w->jointIndex] );
				rotatedWeightPos = joint->orient * w->pos;
				m->finalVertices[j].xyz += ( joint->pos + rotatedWeightPos ) * w->bias;
				//m->finalVertices[j].Normal *= joint->orient;
				//m->finalVertices[j].Tangent *= joint->orient;
			}
			m->finalVertices[j].xyz = rotation * m->finalVertices[j].xyz*scale + offset;
			m->finalVertices[j].st = m->vertices[j].st;
		}
		if ( !m->buffersGenerated ) {
			m->CalculateNormals();
			localNorms = true;
		}
			m->UpdateBuffers();	
	}
	if ( localNorms )
		CalculateLocalNormals( &bindposeSkeleton );
}

/* 
=====================================
	AddAnimation
		Returns animation id
=====================================
*/
int vsModel_MD5::AddAnimation( vsAnimation_MD5 * anim )
{
	animations.Append( anim );
	return animations.Length()-1;
}

/* 
=====================================
	SetAnimation
=====================================
*/
void vsModel_MD5::SetAnimation( const vsStr& animName )
{
}

/* 
=====================================
	SetAnimation
=====================================
*/
void vsModel_MD5::SetAnimation( short id, int mode  )
{
//	if ( mode & ANIM_PLAY_IMMEDIATELY ) {
		//currentAnimation = id;
	//	currentAnimPlayMode = 
//	} else if ( mode & ANIM_PLAY_WAIT ) {
	/*if ( currentAnimation > 0 ) {
		nextAnimation = id;
		nextAnimPlayMode = mode;
	} else */
	{
		currentAnimation = id;
		currentAnimPlayMode = mode;
		nextAnimation = -1;
		nextAnimPlayMode = -1;
	}
	//}

//	if ( mode & ANIM_PLAY_REPEAT ) {
//
//	}
}

/* 
=====================================
	GetCurrentAnimationTime
=====================================
*/
float vsModel_MD5::GetCurrentAnimationTime( void )
{
	return currentAnimTime;
}

/* 
=====================================
	AnimationFinished
=====================================
*/
bool vsModel_MD5::AnimationFinished( void )
{
	if ( !currentAnimation )
		return false;

	return( currentAnimTime >= animations[currentAnimation]->GetLength() );
}

/*
=====================================
	GetBounds
=====================================
*/
vsBox vsModel_MD5::GetBounds( void ) const
{
	if ( currentAnimation > -1 ) {
		vsAnimation_MD5 * anim = animations[ currentAnimation ];
		return anim->frames[ ( int )( currentAnimTime * anim->frameRate )].bounds*rotation*scale+offset;
	}

	return bounds*rotation*scale+offset;
}

/*
=====================================
	CalculateLocalNormals
=====================================
*/
void vsModel_MD5::CalculateLocalNormals( MD5Skeleton_t* skeleton  ) 
{
	MD5Mesh_t * m;
	REPU( meshes.Length() )
	{
		m = meshes[i];
		REPU2( j, m->vertices.Length() )
		{
			m->vertices[j].localNormal = m->finalVertices[j].Normal;
			m->vertices[j].localTangent = m->finalVertices[j].Tangent;

			for ( int k=m->vertices[j].weightStart;
				k<m->vertices[j].weightCount+m->vertices[j].weightStart;
				++k )
			{
				m->vertices[j].localNormal *= skeleton->joints[m->weights[k].jointIndex].orient.Inverse();
				m->vertices[j].localTangent *= skeleton->joints[m->weights[k].jointIndex].orient.Inverse();
			}
		}
	}
}




#pragma endregion