#include "MD5Mesh.h"
#include <stdio.h>
#include "..\..\System\FileManager.h"

#include "..\..\NspStruct.h"
using namespace vsLib2;

/*
===============================
	Constructor
===============================
*/
vsMD5Model::vsMD5Model( void ) {
	currentAnimation = NULL;
}

/*
===============================
	Destructor
===============================
*/
vsMD5Model::~vsMD5Model( void ) {
}

/*
===============================
	LoadMesh
===============================
*/
int vsMD5Model::Load( const modelLoadArgs_t& args )  {
	vsInputFile file;
	vsStr		line;
	vsStr		tmpLine;
	MD5Mesh_t*	currentMesh = NULL;
	int			tmpInt;
	int			meshX = 0;
//char		pChar[64];

	this->filename	= args.szFileName;
	this->matlib	= args.type_md5.matLib;

	LoadMaterialLibrary( matlib );

	if(file.Open((char*)args.szFileName(), "r") != VS_OK)
	{
		console->Print(CMT_ERROR, "MD5Mesh not found: %s", filename());
		return VS_FILE_NOT_FOUND;
	}

	line.Reserve( 512 );
	file.ReadLine(&line);

	if(line.Find("MD5Version 10")==-1) {
		console->Print(line());
		console->Print(CMT_ERROR, "Invalid MD5Mesh: %s", filename());
		return VS_INVALID_FILE;
	}

	while(!file.Eof())
	{
		file.ReadLine(&line);

		if(strstr(line(), "numJoints")) {
			sscanf_s(line(), "numJoints %d", &joints.Count);
			joints.Alloc( joints.Count );
		} else

		if(strstr(line(), "numMeshes")) {
			sscanf_s(line(), "numMeshes %d", &meshes.Count);
			meshes.Alloc( meshes.Count );
		} else

		if(strstr(line(), "joints {")) {
			tmpLine.Reserve(512);
			for(unsigned int i=0; i<joints.Count; ++i) {
				file.ReadLine(&tmpLine);

				joints[i].name.Reserve(64);

				sscanf_s(tmpLine(), " %s %d ( %f %f %f ) ( %f %f %f )",
									&joints[i].name, 64,
									&joints[i].parentIndex,
									&joints[i].position.x, &joints[i].position.y, &joints[i].position.z,
									&joints[i].orientation.x, &joints[i].orientation.y, &joints[i].orientation.z);
				joints[i].orientation.CalcW();	
				
			}
		} else 

		if(strstr(line(), "mesh {")) {
			currentMesh = &meshes[meshX++];	
		}
			//while(!strstr(line(), "}")) {
			//	fgets(line(), 512, pFile);

				if(strstr(line(), "shader")) {
					vsStr materialname;
					materialname.Reserve( 256 );
					sscanf_s(line(), " shader %s", materialname(), 256);
					materialname.DeleteAll('\"');
					currentMesh->material = SearchForMaterial( materialname );
				} else

				if(strstr(line(), "numverts")) {
					sscanf_s(line(), " numverts %d", &currentMesh->vertices.Count );
					currentMesh->vertices.Alloc( currentMesh->vertices.Count );
					for(unsigned int i=0; i<currentMesh->vertices.Count; ++i) {
						file.ReadLine(&line);
						sscanf_s(line(), " vert %d ( %f %f ) %d %d",
											&tmpInt, 
											&currentMesh->vertices[i].st.x,
											&currentMesh->vertices[i].st.y,
											&currentMesh->vertices[i].wStartIndex,
											&currentMesh->vertices[i].wCount );
					}
				} else

				if(strstr(line(), "numtris")) {
					sscanf_s(line(), " numtris %d", &currentMesh->triangles.Count );
					currentMesh->triangles.Alloc( currentMesh->triangles.Count );
					for(unsigned int i=0; i<currentMesh->triangles.Count; ++i) {
						file.ReadLine(&line);
						sscanf_s(line(), " tri %d %d %d %d",
											&tmpInt, 
											&currentMesh->triangles[i].vertexIndices[0],
											&currentMesh->triangles[i].vertexIndices[1],
											&currentMesh->triangles[i].vertexIndices[2] );
					}
				} else

				if(strstr(line(), "numweights")) {
					sscanf_s(line(), " numweights %d", &currentMesh->weights.Count );
					currentMesh->weights.Alloc( currentMesh->weights.Count );
					for(unsigned int i=0; i<currentMesh->weights.Count; ++i) {
						file.ReadLine(&line);
						sscanf_s(line(), " weight %d %d %f ( %f %f %f )",
											&tmpInt, 
											&currentMesh->weights[i].joint,
											&currentMesh->weights[i].bias,
											&currentMesh->weights[i].position.x,
											&currentMesh->weights[i].position.y,
											&currentMesh->weights[i].position.z );
					}
				}
			//} // while !}
		//} // mesh
	}//feof(file)

	file.Close();

	BuildBindPoseSkeleton();
	CalculateBasis();
	return VS_OK;
}

/*
===============================
	AddAnimation
===============================
*/
int vsMD5Model::AddAnimation( vsMD5Anim* anim ) {
	anim->SetModel( this );
	animations.Append( anim );
	return( animations.Length() -1 );
}

/*
===============================
	UpdateAnimation
===============================
*/
void vsMD5Model::UpdateAnimation( void ) {
}

/*
===============================
	SetCurrentAnimation
===============================
*/
void vsMD5Model::SetCurrentAnimation( unsigned int animIndex ) {
	if( animIndex < animations.Length())
		currentAnimation = animations[ animIndex ];
}

/*
===============================
	BuildBindPoseSkeleton
===============================
*/
void vsMD5Model::BuildBindPoseSkeleton( void ) {
	MD5Vertex_t		*v;
	MD5Weight_t		*w;
	vsVec3f			pos;

	for(unsigned int i=0; i<meshes.Length(); ++i) 
	{
		for(unsigned int j=0; j<meshes[i].vertices.Length(); ++j) 
		{
			v = &meshes[i].vertices[j];
			for(unsigned int k=0; k<meshes[i].vertices[j].wCount; ++k)
			{
				w = &meshes[i].weights[ v->wStartIndex + k ];
				
				pos = joints[ w->joint ].orientation * w->position;

				v->finalPosition += (joints[ w->joint ].position + pos ) * w->bias;
			}
		}
	}
}

/*
===============================
	Render
===============================
*/
void vsMD5Model::Render( void ) {
	MD5Mesh_t		*mesh;
	unsigned int	loc_tangent;

	RenderBasis();
	for(unsigned int i=0; i<meshes.Count; ++i)
	{
		mesh = &meshes[i];
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(MD5Vertex_t), &mesh->vertices[0].finalPosition);
		glNormalPointer(GL_FLOAT, sizeof(MD5Vertex_t), &mesh->vertices[0].normal);
		
		if(mesh->material) 
		{
				if(mesh->material->shader) {
					mesh->material->shader->Bind( true );

					loc_tangent = mesh->material->shader->AttribLoc("tangent");
					glEnableVertexAttribArray( loc_tangent );
					glVertexAttribPointer( loc_tangent, 3, GL_FLOAT, false, sizeof(MD5Vertex_t), &mesh->vertices[0].tangent);
				}

				//===================================
				//	Diffusemap
				//===================================
				if(mesh->material->diffusemap) 
				{
					glClientActiveTexture(GL_TEXTURE0);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, sizeof(MD5Vertex_t), &mesh->vertices[0].st );

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, mesh->material->diffusemap);
					if(mesh->material->shader)
						mesh->material->shader->Uniform1i("texDiffuse", 0);
				}
				//===================================
				//	Normalmap
				//===================================
				if(mesh->material->normalmap) 
				{
					glClientActiveTexture(GL_TEXTURE1);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, sizeof(MD5Vertex_t), &mesh->vertices[0].st );

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, mesh->material->normalmap);
					if(mesh->material->shader)
						mesh->material->shader->Uniform1i("texNormal", 1);
				}
				//===================================
				//	Specularmap
				//===================================
				if(mesh->material->specularmap) 
				{
					glClientActiveTexture(GL_TEXTURE2);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, sizeof(MD5Vertex_t), &mesh->vertices[0].st );

					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, mesh->material->specularmap);
					if(mesh->material->shader)
						mesh->material->shader->Uniform1i("texSpecular", 2);
				}
		} //mesh->material

		glDrawElements(GL_TRIANGLES, mesh->triangles.Length()*3, GL_UNSIGNED_INT, mesh->triangles() );


		if(mesh->material) 
		{
				if(mesh->material->shader) {
					mesh->material->shader->Bind( false );
					glDisableVertexAttribArray( loc_tangent );
				}

				//===================================
				//	Diffusemap
				//===================================
				if(mesh->material->diffusemap) 
				{
					glClientActiveTexture(GL_TEXTURE0);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
				//===================================
				//	Normalmap
				//===================================
				if(mesh->material->normalmap) 
				{
					glClientActiveTexture(GL_TEXTURE1);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
				//===================================
				//	Specularmap
				//===================================
				if(mesh->material->specularmap) 
				{
					glClientActiveTexture(GL_TEXTURE2);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
		} // mesh->material
	
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

/*
===============================
	CalculateBasis
===============================
*/
void vsMD5Model::CalculateBasis( void ) {
	vsVec3f			p1, p2, p3, v1, v2, N, T;
	vsVec2f			t1, t2, t3, tg1, tg2;
	vsList<vsVec3f>	tempNormals;
	vsList<vsVec3f>	tempTangents;
	UINT			k;

	for(UINT i=0; i<meshes.Length(); ++i)
	{
		tempNormals.Alloc( meshes[i].triangles.Length() );
		tempTangents.Alloc( meshes[i].triangles.Length() );
		for(UINT j=0; j<meshes[i].triangles.Length(); ++j)
		{
			p1 = meshes[i].vertices[ meshes[i].triangles[j].vertexIndices[0] ].finalPosition;
			p2 = meshes[i].vertices[ meshes[i].triangles[j].vertexIndices[1] ].finalPosition;
			p3 = meshes[i].vertices[ meshes[i].triangles[j].vertexIndices[2] ].finalPosition;

			v1 = p1 - p2;
			v2 = p3 - p2;

			N = v1.Cross( v2 );
			N.Normalize();

			t1 = meshes[i].vertices[ meshes[i].triangles[j].vertexIndices[0] ].st;
			t2 = meshes[i].vertices[ meshes[i].triangles[j].vertexIndices[1] ].st;
			t3 = meshes[i].vertices[ meshes[i].triangles[j].vertexIndices[2] ].st;

			tg1 = t2 - t1;
			tg2 = t3 - t1;
			if(tg1.y * tg2.x > tg1.x * tg2.y) 
			{
				tg1 = -tg1;
				tg2 = -tg2;
			}

			v1 = v1 * N * N.Dot(v1);
			v2 = v2 * N * N.Dot(v2);

			T = v1 * tg1.y - v2 * tg2.x;
			T.Normalize();
			
			tempNormals  [ j ]	= N;
			tempTangents [ j ]	= T;
		}


		for(UINT j=0; j<meshes[i].vertices.Length(); ++j)
		{
			N = NULLVEC3F;
			T = NULLVEC3F;
			for(UINT f=0; f<meshes[i].triangles.Length(); ++f)
			{
				if(meshes[i].triangles[f].vertexIndices[0] == j)
				{
					N += tempNormals[f];
					T += tempTangents[f];
				}
			}

			N.Normalize();
			T.Normalize();
			meshes[i].vertices[j].normal	= N;
			meshes[i].vertices[j].tangent	= T;
		}

		tempNormals.Free();
		tempTangents.Free();
	}
}

/*
===============================
	CalculateAnimatedBasis
===============================
*/
void vsMD5Model::CalculateAnimatedBasis( void ) {
}

/*
===============================
	RenderBasis
===============================
*/
void vsMD5Model::RenderBasis( void ) {
	glBegin(GL_LINES);
	for(UINT i=0; i<meshes.Length(); ++i) 
	{
		for(UINT j=0; j<meshes[i].vertices.Length(); ++j)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv( meshes[i].vertices[j].finalPosition.ToTypePtr() );
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv( (meshes[i].vertices[j].finalPosition + meshes[i].vertices[j].normal).ToTypePtr() );

			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3fv( meshes[i].vertices[j].finalPosition.ToTypePtr() );
			glColor3f(0.0f,1.0f, 0.0f);
			glVertex3fv( (meshes[i].vertices[j].finalPosition + meshes[i].vertices[j].tangent).ToTypePtr() );

		}
	}
	glEnd();
}


/*
===============================
	SetOrigin
===============================
*/
void vsMD5Model::SetOrigin( const vsVec3f& newOrigin )
{
	Translate( newOrigin - tmpOrigin );
	tmpOrigin = newOrigin;
}

/*
===============================
	SetAxis
===============================
*/
void vsMD5Model::SetAxis( const vsMat3& newAxis )
{
	Rotate( tmpAxis - newAxis ); // tyuuu fixme
	tmpAxis = newAxis;
}

/*
===============================
	Scale
===============================
*/
void vsMD5Model::Scale( const vsVec3f& s ) {
	for(unsigned int i=0; i<meshes.Length(); ++i) 
	{
		for(unsigned int j=0; j<meshes[i].vertices.Length(); ++j) 
		{
			meshes[i].vertices[j].finalPosition *= s;
		}
	}
}

/*
===============================
	Translate
===============================
*/
void vsMD5Model::Translate( const vsVec3f& t ) {
	for(unsigned int i=0; i<meshes.Length(); ++i) 
	{
		for(unsigned int j=0; j<meshes[i].vertices.Length(); ++j) 
		{
			meshes[i].vertices[j].finalPosition += t;
		}
	}
}

/*
===============================
	Rotate
===============================
*/
void vsMD5Model::Rotate( const vsMat3& t ) {

	for(unsigned int i=0; i<meshes.Length(); ++i) 
	{
		for(unsigned int j=0; j<meshes[i].vertices.Length(); ++j) 
		{
			meshes[i].vertices[j].finalPosition *= t;
			meshes[i].vertices[j].normal *= t; //THIS IS NOT CORRECT!
		}
	}
}
/*
===============================
	SearchForMaterial
===============================
*/
MD5Material_t* vsMD5Model::SearchForMaterial( const vsStr& materialName ) {
	for(unsigned int i=0; i<materials.Length(); ++i) {
		if( !strcmp(materials[i].name(),materialName()) )
			return ( &materials[i] );
	}

	return NULL;
}

/*
===============================
	LoadMaterial
===============================
*/
int vsMD5Model::LoadMaterialLibrary( const vsStr& materialName ) {
	vsInputFile file;
	vsStr		line;
	char		tmpStr[128];

	if(file.Open(matlib, "r") != VS_OK)
		return VS_FILE_NOT_FOUND;

	line.Reserve(256);

	while(!file.Eof()) {
		file.ReadLine(&line);

		if(strlen(line()) > 2) 
		{
			materials.Expand();
			materials.Last().name = line;
			materials.Last().name.CorrectLength();
			while(!strstr(line(), "}") )
			{
				file.ReadLine(&line);

				if(strstr(line(), "diffusemap")) 
				{
					sscanf_s(line(), " diffusemap %s", &tmpStr, 128);
					materials.Last().diffusemap = renderSystem->LoadTexture( tmpStr );
				} else

				if(strstr(line(), "bumpmap")) 
				{
					sscanf_s(line(), " bumpmap %s", &tmpStr, 128);
					materials.Last().normalmap = renderSystem->LoadTexture( tmpStr );
				} else

				if(strstr(line(), "specularmap")) 
				{
					sscanf_s(line(), " specularmap %s", &tmpStr, 128);
					materials.Last().specularmap = renderSystem->LoadTexture( tmpStr );
				} else

				if(strstr(line(), "shader")) 
				{
					sscanf_s(line(), " shader %s", &tmpStr, 128);
					materials.Last().shader = renderSystem->LoadShaderFromFile(tmpStr, tmpStr, 0, 0);
				} 
			}
		}
	}

	return VS_OK;
}