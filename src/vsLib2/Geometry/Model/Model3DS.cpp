#include "Model3DS.h"
#include "..\..\System\Console.h"
#include "..\..\System\Flag.h"

#include "..\..\NspStruct.h"
using namespace vsLib2;

#define MAIN3DS			0x4D4D
#define EDIT3DS			0x3D3D
#define OBJECT			0x4000
#define OBJ_TRIMESH		0x4100
#define OBJ_LIGHT		0x4600
#define VERTICES		0x4110
#define FACES			0x4120
#define FACES_MATERIAL	0x4130
#define TEXCOORDS		0x4140
#define MATERIAL		0xAFFF
#define MTEX1			0xA200
#define MTEX2			0xA33A
#define MNORMAL			0xA230
#define MSPECULAR		0xA204
#define MTEX_FILENAME	0xA300
#define MTEX_UOFFSET	0xA358
#define MTEX_VOFFSET	0xA35A
#define COL_RGB			0x0010
#define COL_TRU			0x0011
#define M_AMBIENT		0xA010
#define M_DIFFUSE		0xA020
#define M_SPECULAR		0xA030
#define M_TRANSPARENCY	0xA050
#define M_SHININESS		0xA040


#define KEYF3DS			0xB000

/*
====================================
	Object3DS_t::Constructor
====================================
*/
Object3DS_t::Object3DS_t( void ) {
	Material = NULL;
}

/*
====================================
	Object3DS_t::Destructor
====================================
*/
Object3DS_t::~Object3DS_t( void ) {
	Vertices.Free();
	Triangles.Free();
//	Indices.Free();
}

/*
====================================
	Object3DS_t::CalculateBounds()
====================================
*/
void Object3DS_t::CalculateBounds( void ) {
	vsVec3f v[] = { vsVec3f(), vsVec3f() };
	if ( Vertices.Length()>0 ) 
		bounds.FromVertices( Vertices.list, Vertices.Length() );
	else
		bounds.FromVectors( v, 2 );
}


/*
====================================
	vs3DSModel::Constructor
====================================
*/
vs3DSModel::vs3DSModel( void ) {
	v3Scale = vsVec3f( 1.0f, 1.0f, 1.0f );
	v3Position = vsVec3f( 0.0f, 0.0f, 0.0f );
	loaded = false;
	Frustum = NULL;
	shader = NULL;
}

/*
====================================
	vs3DSModel::Destructor
====================================
*/
vs3DSModel::~vs3DSModel( void ) {
	materials.Free();
	objects.Free();
}

/*
====================================
	vs3DSModel::Update()
====================================
*/
void vs3DSModel::UpdateAnimation( void ) {
	if ( !loaded ) return;
	// Animate();
	Render();
}

/*
====================================
	vs3DSModel::Render()
====================================
*/
void vs3DSModel::Render( void ) {
	if ( !loaded ) return;
	//boundingBox.RenderBox();
	//if ( ( Frustum && Frustum->IntersectBox( boundingBox, 0.002f ) )
	//	|| Frustum==NULL ) 
	{
		for ( unsigned int j = 0; j < objects.Length(); j++ ) {
			if ( Frustum ) {
				if ( Frustum->IntersectBox( objects.list[ j ].bounds, 0.002f ) ) {
					RenderObject( &objects.list[ j ] );
				}
			}
			else
				RenderObject( &objects.list[ j ] );
		}
	}
}

/*
====================================
	vs3DSModel::RenderObject()
====================================
*/
void vs3DSModel::RenderObject( Object3DS_t* obj ) {
	if ( !obj || !loaded || !obj->Triangles.Length() || !obj->Vertices.Length() ) return;
	int locTan=0;
	bool useshader =  !renderFlags.IsSet( MODEL_FLAG_NOSHADER );
	bool usetexture =  !renderFlags.IsSet( MODEL_FLAG_NOTEXTURE );

	if ( renderFlags.IsSet( MODEL_FLAG_WIREFRAME ) )
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glColor3d( 1,1,1 );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	if ( shader && useshader && obj->Material ){
		locTan = shader->AttribLoc( "tangent" );
		glEnableVertexAttribArray( locTan );
		shader->Bind( true );
		shader->Uniform4v( "material.diffuse", vsVec4f( obj->Material->diffuse,1.0 ) );
		shader->Uniform4v( "material.ambient", vsVec4f( obj->Material->ambient,1.0 ) );
		shader->Uniform4v( "material.specular", vsVec4f( obj->Material->specular,1.0 ) );

		/*char lc[32], lp[32], lr[32];
		for ( unsigned int i=0; i<lights.Length(); i++ ) {
			sprintf_s( lc, 32, "lights[%i].color\0", i );
			sprintf_s( lp, 32, "lights[%i].position\0", i );
			sprintf_s( lr, 32, "lights[%i].radius\0", i );
			shader->Uniform4v( lc, lights.list[ i ].color );
			shader->Uniform4v( lp, lights.list[ i ].position );
			shader->Uniform1f( lr, lights.list[ i ].radius );
		}*/
	}


	if ( obj->Material ) {
		if ( obj->Material->tex1.id && usetexture ) {
			glClientActiveTexture( GL_TEXTURE0 );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glTexCoordPointer( 2, GL_FLOAT, sizeof( vsVertex ), &obj->Vertices.list[0].st );
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, obj->Material->tex1.id );
			if ( shader && useshader ) shader->Uniform1i( "texDiffuse", 0 );
		}

		if ( obj->Material->normalmap.id && shader && usetexture ) {
			glClientActiveTexture( GL_TEXTURE1 );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glTexCoordPointer( 2, GL_FLOAT, sizeof( vsVertex ), &obj->Vertices.list[0].st );
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, obj->Material->normalmap.id );
			if ( shader && useshader ) shader->Uniform1i( "texNormal", 1 );
		}

		if ( obj->Material->specularmap.id && shader && usetexture ) {
			glClientActiveTexture( GL_TEXTURE2 );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glTexCoordPointer( 2, GL_FLOAT, sizeof( vsVertex ), &obj->Vertices.list[0].st );
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, obj->Material->specularmap.id );
			if ( shader && useshader ) shader->Uniform1i( "texSpecular", 2 );
		}
	}
	
	//obj->bounds.RenderBox();

	glVertexPointer( 3, GL_FLOAT, sizeof( vsVertex ), &obj->Vertices.list[0].xyz );
	glNormalPointer( GL_FLOAT, sizeof( vsVertex ), &obj->Vertices.list[0].Normal );
	if ( shader && shader->program ) {
		glVertexAttribPointer( locTan,3,GL_FLOAT, true, sizeof( vsVertex ), &obj->Vertices.list[0].Tangent );
	}
 
	glDrawElements( GL_TRIANGLES, obj->Triangles.Length()*3/MODEL_LOD, GL_UNSIGNED_SHORT, obj->Triangles.list );
//	glDrawElements( GL_TRIANGLE_STRIP, obj->Indices.Length(), GL_UNSIGNED_SHORT, obj->Indices.list );

	if ( obj->Material ) {
		if ( obj->Material->tex1.id && usetexture ) {
			glClientActiveTexture( GL_TEXTURE0 );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		if ( obj->Material->normalmap.id && shader && usetexture ) {
			glClientActiveTexture( GL_TEXTURE1 );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		if ( obj->Material->specularmap.id && shader && usetexture ) {
			glClientActiveTexture( GL_TEXTURE2 );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
	}

	if ( shader && useshader ) {
		shader->Uniform4v( "material.diffuse", NULLVEC4F );
		shader->Uniform4v( "material.ambient", NULLVEC4F );
		shader->Uniform4v( "material.specular", NULLVEC4F );
		shader->Bind( false );
		glDisableClientState( locTan );
	}
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
	
	if ( renderFlags.IsSet( MODEL_FLAG_RENDERBASIS ) )
		RenderBasis( obj );

	if ( renderFlags.IsSet( MODEL_FLAG_RENDERBOUNDS ) )
		obj->bounds.RenderBox();

	if ( renderFlags.IsSet( MODEL_FLAG_WIREFRAME ) )
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glColor3d( 1, 1, 1 );
}


/*
====================================
	vs3DSModel::RenderBasis()
====================================
*/
void vs3DSModel::RenderBasis( Object3DS_t* obj ) {
	glBegin( GL_LINES );
	for ( unsigned int k = 0; k < obj->Vertices.Length(); k++ ) {
		glColor3d( 1, 0, 0 );
		glVertex3fv( obj->Vertices.list[ k ].xyz.ToTypePtr() );
		glVertex3fv( ( obj->Vertices.list[ k ].xyz+obj->Vertices.list[ k ].Normal ).ToTypePtr() );

		glColor3d( 0, 1, 0 );
		glVertex3fv( obj->Vertices.list[ k ].xyz.ToTypePtr() );
		glVertex3fv( ( obj->Vertices.list[ k ].xyz+obj->Vertices.list[ k ].Tangent ).ToTypePtr() );
	}
	glEnd();
}

/*
====================================
	vs3DSModel::RenderBasis()
====================================
*/
void vs3DSModel::RenderBasis( void )
{
	for ( UINT i = 0; i < objects.Length(); ++i )
	{
		RenderBasis( &objects[ i ] );
	}
}

/*
====================================
	vs3DSModel::CalculateNormals()
====================================
*/
void vs3DSModel::CalculateNormals( void ) {
	if ( !loaded ) return;

	vsVertex v;
	vsList<vsVertex> temp;
	Object3DS_t* pObj;
	for ( unsigned int i=0; i<objects.Length(); i++ ) {
		pObj = &( objects.list[ i ] );
		temp.Resize( pObj->Triangles.Length() );

		for ( unsigned int j=0; j< pObj->Triangles.Length(); j++ ) {
			unsigned short a, b, c;
			vsVec3f v1, v2, pv1, pv2;
			vsVec2f t1, t2;
			a = pObj->Triangles.list[ j ].a;
			b = pObj->Triangles.list[ j ].b;
			c = pObj->Triangles.list[ j ].c;

			// calc normal
			v1 = pObj->Vertices.list[ a ].xyz - pObj->Vertices.list[ c ].xyz;
			v2 = pObj->Vertices.list[ c ].xyz - pObj->Vertices.list[ b ].xyz;
			v.Normal = v1.Cross( v2 );
			//v.Normal.Normalize();

			//calc tangent
			pv1 = v1 - v.Normal * v.Normal.Dot( v1 );
			pv2 = v2 - v.Normal * v.Normal.Dot( v2 );
			t1 = pObj->Vertices.list[ a ].st - pObj->Vertices.list[ c ].st;
			t2 = pObj->Vertices.list[ c ].st - pObj->Vertices.list[ b ].st;

			if ( t2.x + t1.y > t1.x + t2.y ) {
				t1.x = -t1.x;
				t2.x = -t2.x;
			}

			v.Tangent = ( pv1 * t2.x ) - ( pv2 * t1.x );
			//v.Tangent.Normalize();
			temp.list[ j ] = v;

		/*	pObj->Vertices.list[ a ].Normal = temp.list[ j ].Normal;
			pObj->Vertices.list[ a ].Normal.Normalize();
			pObj->Vertices.list[ a ].Tangent = temp.list[ j ].Tangent;
			pObj->Vertices.list[ a ].Tangent.Normalize();*/
		}

		vsVec3f N, T;
		int shared;
		for ( unsigned int j=0; j< pObj->Vertices.Length(); j++ ) { 
			T = N = NULLVEC3F;
			shared = 0;
			for ( unsigned int k=0; k< objects.list[ i ].Triangles.Length(); k++ ) {
				if ( pObj->Triangles.list[ k ].a == j ||
					pObj->Triangles.list[ k ].b == j ||
					pObj->Triangles.list[ k ].c == j ) {
					N += temp.list[ k ].Normal;
					T += temp.list[ k ].Tangent;
					shared++;
				}
			}
			pObj->Vertices.list[ j ].Normal = N;
			pObj->Vertices.list[ j ].Normal.Normalize();
			pObj->Vertices.list[ j ].Tangent = T;
			pObj->Vertices.list[ j ].Tangent.Normalize();
		}

	}

	temp.Free();
}

/*
====================================
	vs3DSModel::SwapNormals()
====================================
*/
void vs3DSModel::SwapNormals( void ) {
	if ( !loaded ) return;

	Object3DS_t* pObj;
	for ( unsigned int i=0; i<objects.Length(); ++i ) {
		pObj = &( objects.list[ i ] );

		for ( unsigned int j=0; j<pObj->Vertices.Length(); ++j ) {
				pObj->Vertices.list[ j ].Normal = -pObj->Vertices.list[ j ].Normal;
		}
	}
}

/*
===============================
	SetOrigin
===============================
*/
void vs3DSModel::SetOrigin( const vsVec3f& newOrigin )
{
	Translate( newOrigin - tmpOrigin );
	tmpOrigin = newOrigin;
}

/*
===============================
	SetAxis
===============================
*/
void vs3DSModel::SetAxis( const vsMat3& newAxis )
{
	Rotate( tmpAxis - newAxis ); // tyuuu fixme
	tmpAxis = newAxis;
}

/*
====================================
	vs3DSModel::Scale
====================================
*/
void vs3DSModel::Scale( const vsVec3f& v ) {
	v3Scale *= v;
	boundingBox.Scale( v );
	for ( unsigned int i=0; i<objects.Length(); ++i ) {
		for ( unsigned int j=0; j< objects.list[i].Vertices.Length(); ++j ) {
			objects.list[ i ].Vertices.list[j].xyz *= v;
			objects.list[ i ].bounds.Scale( v );
			objects.list[i].CalculateBounds();
		}
	}
}

/*
====================================
	vs3DSModel::Translate
====================================
*/
void vs3DSModel::Translate( const vsVec3f& v ) {
	v3Position += v;
	boundingBox.Translate( v );
	for ( unsigned int i=0; i<objects.Length(); ++i ) {
		for ( unsigned int j=0; j< objects.list[i].Vertices.Length(); ++j ) {
			objects.list[ i ].Vertices.list[j].xyz += v;
			objects.list[ i ].bounds.Translate( v );
			objects.list[i].CalculateBounds();
		}
	}
}

/*
====================================
	vs3DSModel::Rotate
====================================
*/
void vs3DSModel::Rotate( const vsMat3& r )
{
	m3Rotation *= r;
	boundingBox.Rotate( r );

	for ( unsigned int i=0; i<objects.Length(); ++i ) {
		for ( unsigned int j=0; j< objects.list[i].Vertices.Length(); ++j ) {
			objects.list[ i ].Vertices.list[j].xyz *= r;
			objects.list[ i ].bounds.Rotate( r );
			objects.list[i].CalculateBounds();
		}
	}
}

/*
====================================
	vs3DSModel::UseShader
====================================
*/
void vs3DSModel::UseShader( vsShader* shader ) {
	this->shader = shader;
}

/*
====================================
	vs3DSModel::SetViewFrustum
====================================
*/
void vs3DSModel::SetViewFrustum( vsFrustum * Frustum ) {
	this->Frustum = Frustum;
}

/*
====================================
	vs3DSModel::LoadFromFile
====================================
*/
int vs3DSModel::Load( const modelLoadArgs_t& args ) {
	this->texDir = args.type_3ds.texDir;

	if ( File.Open( ( char* )args.szFileName(), "rb" ) ) {
		console->Print( CMT_ERROR,"3DSModel not found: %s", args.szFileName() );
		return VS_FILE_NOT_FOUND;
	}
	vsSystem::msg32( ( char* )args.szFileName() );

	unsigned short chunkId;
	File.ReadUnsignedShort( &chunkId );
	if ( chunkId != MAIN3DS ) {
		return VS_INVALID_FILE;
	}

	ReadMainChunk();

	File.Close();

	loaded = true;
	path = args.szFileName;

	if ( args.type_3ds.rotate90 ) Rotate90Model();
	CalculateNormals();
	if ( args.type_3ds.swapNormals ) SwapNormals();

	CalculateBounds();

	return VS_OK;
}

/*
====================================
	vs3DSModel::CalculateBounds()
====================================
*/
void vs3DSModel::CalculateBounds( void ) {

	objects[ 0 ].CalculateBounds();

	vsVec3f min = objects[0].bounds.points[0];
	vsVec3f max = objects[0].bounds.points[7];

	for ( unsigned int i=0; i<objects.Length(); i++ )
	{
		objects.list[ i ].CalculateBounds();
		if ( objects[i].bounds.points[0] < min )	min = objects[i].bounds.points[0];
		if ( objects[i].bounds.points[7] > max )	max = objects[i].bounds.points[7];
	}
	boundingBox.FromMinMax( min, max );
}

/*
====================================
	vs3DSModel::ReadMainChunk
====================================
*/
void vs3DSModel::ReadMainChunk( void ) {
	unsigned int startPos = File.Tell() - 2;
	unsigned int thisLength  = ReadChunkLength();
	unsigned short chunkId;

	while( ( unsigned int )File.Tell() < startPos + thisLength ) {
		File.ReadUnsignedShort( &chunkId );

		switch( chunkId ) {
			case EDIT3DS:		ReadEditChunk();
			break;

			default: ReadUnknownChunk();
		}
	}

	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::ReadEditChunk
====================================
*/
void vs3DSModel::ReadEditChunk( void ) {
	unsigned int startPos = File.Tell() - 2;
	unsigned int thisLength   = ReadChunkLength();
	unsigned short chunkId;

	while( ( unsigned int )File.Tell() < startPos + thisLength ) {
		File.ReadUnsignedShort( &chunkId );

		switch( chunkId ) {
			case OBJECT:	ReadObjectChunk();
			break;

			case MATERIAL: if ( renderFlags.IsSet( MODEL_FLAG_COLLMODEL ) ) 
								ReadUnknownChunk(); 
						   else ReadMaterialChunk();
			break;

			default: ReadUnknownChunk();
		}
	}

	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::ReadObjectChunk
====================================
*/
void vs3DSModel::ReadObjectChunk( void ) {
	unsigned int startPos = File.Tell() - 2;
	unsigned int thisLength   = ReadChunkLength();
	unsigned short chunkId;

	objects.Resize( objects.Length()+1 );

	for ( int i = 0; i < 20; i++ ) {
		File.ReadChar( &objects.list[ objects.Length()-1 ].Name[ i ] );
		if ( objects.list[ objects.Length()-1 ].Name[ i ] == '\0' )
			break;
	}

	while( ( unsigned int )File.Tell() < startPos + thisLength ) {
		File.ReadUnsignedShort( &chunkId );

		switch( chunkId ) {
			case OBJ_TRIMESH:		ReadTrimeshChunk();
			break;

			
			case OBJ_LIGHT: ReadLightChunk();
			break;

			default: ReadUnknownChunk();
		}	
	}

	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::ReadTrimeshChunk
====================================
*/
void vs3DSModel::ReadTrimeshChunk( void ) {
	unsigned int startPos = File.Tell() - 2;
	unsigned int thisLength   = ReadChunkLength();
	unsigned short chunkId;

	while( ( unsigned int )File.Tell() < startPos + thisLength ) {
		File.ReadUnsignedShort( &chunkId );

		switch( chunkId ) {
			case VERTICES:		ReadVerticesChunk();
			break;

			case FACES: ReadFacesChunk();
			break;

			case FACES_MATERIAL:	ReadFacesMaterialChunk();
			break;

			case TEXCOORDS:	ReadTexCoordsChunk();
			break;

			default: ReadUnknownChunk();
		}
	}

	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::ReadVerticesChunk
====================================
*/
void vs3DSModel::ReadVerticesChunk( void ) {
	unsigned int startPos = File.Tell()-2;
	unsigned int thislength = ReadChunkLength();
	unsigned short vertCount;

	File.ReadUnsignedShort( &vertCount );
	objects.list[objects.Length()-1].Vertices.Resize( vertCount );

	for ( unsigned int i=0; i<vertCount; i++ )
		File.ReadVec3( &objects.list[objects.Length()-1].Vertices.list[i].xyz );

	File.Seek( startPos + thislength, SEEK_SET );
}

/*
====================================
	vs3DSModel::ReadFacesChunk
====================================
*/
void vs3DSModel::ReadFacesChunk( void ) {

	unsigned int startPos = File.Tell() - 2;
	unsigned int thisLength   = ReadChunkLength();
	unsigned short triCount, flag, chunkId;

	File.ReadUnsignedShort( &triCount );
	objects.list[ objects.Length()-1 ].Triangles.Resize( triCount );

	for ( unsigned short i=0; i<triCount; i++ ) {
		File.ReadUnsignedShort( &objects.list[objects.Length()-1].Triangles.list[i].a );
		File.ReadUnsignedShort( &objects.list[objects.Length()-1].Triangles.list[i].b );
		File.ReadUnsignedShort( &objects.list[objects.Length()-1].Triangles.list[i].c );
		File.ReadUnsignedShort( &flag );
	}

	while( ( unsigned int )File.Tell() < startPos+thisLength ) {
		File.ReadUnsignedShort( &chunkId );

		switch( chunkId ) {
			case FACES_MATERIAL:	if ( renderFlags.IsSet( MODEL_FLAG_COLLMODEL ) )
											ReadUnknownChunk();
									else	ReadFacesMaterialChunk();
			break;

			default:
				ReadUnknownChunk();
		}
	}

	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::ReadFacesMaterialChunk
====================================
*/
void vs3DSModel::ReadFacesMaterialChunk( void ) {

	unsigned int startPos = File.Tell()-2;
	unsigned int thisLength = ReadChunkLength();
	char MaterialName[20];

	for ( int i=0; i<20; i++ ) {
		File.ReadChar( &MaterialName[ i ] );
		if ( MaterialName[ i ] == '\0' ) 
			break;
	}

	objects.list[ objects.Length()-1 ].Material = SearchForMaterial( MaterialName );

	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::ReadLightChunk
====================================
*/
void vs3DSModel::ReadLightChunk( void ) {
	unsigned int startPos = File.Tell()-2;
	unsigned int thisLength = ReadChunkLength();
	unsigned short chunkId;
	unsigned char r, g, b;
	vsVec3f col=vsVec3f( 1.0f ), pos;

	if ( lights.Length()+1>MODEL_MAX_LIGHTS ) return;

	lights.Resize( lights.Length() + 1 );
	File.ReadVec3( &pos );
	lights.list[ lights.Length() - 1 ].position = vsVec4f( pos, 1.0 );

	while( ( unsigned int )File.Tell() < startPos + thisLength ) {
		File.ReadUnsignedShort( &chunkId );

		switch( chunkId ) {
			//RGB Color ( floats )
			case 0x0010: File.ReadVec3( &col );
			break;

			//24 bit Color ( bytes )
			case 0x0011: {
							File.ReadByte( &r );
							File.ReadByte( &g );
							File.ReadByte( &b );
							col = vsVec3f( 	( float )r/255.0f, 
											( float )g/255.0f, 
											( float )b/255.0f );
						 }
			break;

			// Is light on?
			case 0x4620:	File.ReadBool( &lights.list[ lights.Length() - 1 ].on );
			break;
		}
	}

	lights.list[ lights.Length() - 1 ].color = vsVec4f( col,1.0 );

	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::SearchForMaterial
====================================
*/
Material3DS_t* vs3DSModel::SearchForMaterial( char* szMaterialName ) {

	// loop through materials
	for ( unsigned int k = 0; k < materials.Length(); k++ ) {
		//if length of the strings does not match, continue
		if ( strlen( szMaterialName )!=strlen( materials.list[ k ].Name ) )
			continue;
		// loop through the name of each material
		bool found = true;
		for ( unsigned int i=0; i < strlen( szMaterialName ); i++ ) {

			if ( materials.list[ k ].Name[ i ] != szMaterialName[ i ] ) 
				found = false;
		}
		if ( found ) return ( &materials.list[ k ] );
	}

	return NULL;
}

/*
====================================
	vs3DSModel::ReadTexCoordsChunk
====================================
*/
void vs3DSModel::ReadTexCoordsChunk( void ) {

	unsigned int startPos = File.Tell()-2;
	unsigned int thisLength = ReadChunkLength();
	unsigned short texCount;

	File.ReadUnsignedShort( &texCount );

	for ( int i=0; i<texCount; i++ ) {
		File.ReadVec2( &objects.list[ objects.Length()-1 ].Vertices.list[ i ].st );
		objects.list[ objects.Length()-1 ].Vertices.list[ i ].st.y *= -1.0f;
	}
	
	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::ReadMaterialChunk
====================================
*/
void vs3DSModel::ReadMaterialChunk( void ) {

	unsigned int startPos = File.Tell() - 2, chunklen, sPos;
	unsigned int thisLength   = ReadChunkLength();
	unsigned short chunkId;

	materials.Resize( materials.Length()+1 );

	while( ( unsigned int )File.Tell() < startPos + thisLength ) {
		File.ReadUnsignedShort( &chunkId );
		File.ReadUnsignedInt( &chunklen );
		
		sPos = File.Tell() - 6;

		switch( chunkId ) {
			case 0xA000:		
								
								for ( int i=0; i<20; i++ ) {
									File.ReadChar( &materials.list[ materials.Length()-1 ].Name[ i ] );
									if ( materials.list[ materials.Length()-1 ].Name[ i ] == '\0' ) 
										break;
								}
								File.Seek( sPos + chunklen, SEEK_SET );
								
			break;

			case MTEX1:		materials.list[ materials.Length()-1 ].tex1 = ReadTextureChunk();
			break;

			case MTEX2:		materials.list[ materials.Length()-1 ].tex2 = ReadTextureChunk();
			break;

			case MNORMAL:	materials.list[ materials.Length()-1 ].normalmap = ReadTextureChunk();
			break;

			case MSPECULAR: materials.list[ materials.Length()-1 ].specularmap = ReadTextureChunk();
			break;

			case M_DIFFUSE:	File.Seek( -4, SEEK_CUR );
							materials.list[ materials.Length()-1 ].diffuse = ReadColorChunk();
							materials.list[ materials.Length()-1 ].diffuse.Clamp( vsVec3f( 0.0f ), vsVec3f( 1.0f ) );
			break;

			case M_AMBIENT:	File.Seek( -4, SEEK_CUR );
							materials.list[ materials.Length()-1 ].ambient = ReadColorChunk();
							materials.list[ materials.Length()-1 ].ambient.Clamp( vsVec3f( 0.0f ), vsVec3f( 1.0f ) );
			break;

			case M_SPECULAR:File.Seek( -4, SEEK_CUR );
							materials.list[ materials.Length()-1 ].specular = ReadColorChunk();
							materials.list[ materials.Length()-1 ].specular.Clamp( vsVec3f( 0.0f ), vsVec3f( 1.0f ) );
			break;

			default: 
				File.Seek( -4, SEEK_CUR );
				ReadUnknownChunk();
		}
	}

	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::ReadTextureChunk
====================================
*/
Texture3DS_t vs3DSModel::ReadTextureChunk( void ) {

	File.Seek( -4, SEEK_CUR );
	unsigned int startPos = File.Tell() - 2;
	unsigned int thisLength   = ReadChunkLength(), len, sPos;
	unsigned short chunkId;
	Texture3DS_t   tex;

	while( ( unsigned int )File.Tell() < startPos + thisLength ) {
		sPos = File.Tell();
		File.ReadUnsignedShort( &chunkId );
		File.ReadUnsignedInt( &len );

		switch( chunkId ) {
			case MTEX_FILENAME:	for ( int i=0; i<128; i++ ) {
									File.ReadChar( &tex.FileName[ i ] );
									if ( tex.FileName[ i ] == '\0' ) 
										break;
								}
								File.Seek( sPos + len, SEEK_SET );

			break;

		/*	case MTEX_VOFFSET:
			break;

			case MTEX_UOFFSET:
			break;*/

			default:	File.Seek( -4, SEEK_CUR );
						ReadUnknownChunk();
		}
	}

	tex.FileName[ strlen( tex.FileName )-1 ] = 'S';
	tex.FileName[ strlen( tex.FileName )-2 ] = 'D';
	tex.FileName[ strlen( tex.FileName )-3 ] = 'D';
	//tex.FileName[ strlen( tex.FileName )-4 ] = '.';
	char fullTex[512];

	sprintf_s( fullTex, 512, "%s%s", texDir, tex.FileName );
	
	tex.id = renderSystem->LoadTexture( fullTex );

	File.Seek( startPos + thisLength, SEEK_SET );

	return tex;
}

/*
====================================
	vs3DSModel::ReadKeyframerChunk
====================================
*/
void vs3DSModel::ReadKeyframerChunk( void ) {
}

/*
====================================
	vs3DSModel::ReadRGBColorChunk
====================================
*/
vsVec3f vs3DSModel::ReadRGBColorChunk( void ) {
	unsigned int startPos = File.Tell() -2;
	unsigned int thisLength = ReadChunkLength();
	vsVec3f col;

	File.ReadVec3( &col );
	//col /= 255.0f;

	File.Seek( startPos + thisLength, SEEK_SET );
	return col;
}

/*
====================================
	vs3DSModel::ReadTrueColorChunk
====================================
*/
vsVec3f vs3DSModel::ReadTrueColorChunk( void ) {
	unsigned int startPos = File.Tell() -2;
	unsigned int thisLength = ReadChunkLength();
	unsigned int r, g, b;

	File.ReadUnsignedInt( &r );
	File.ReadUnsignedInt( &g );
	File.ReadUnsignedInt( &b );

	File.Seek( startPos + thisLength, SEEK_SET );
	return vsVec3f( float( r ) / 255.0f, float( g ) / 255.0f, float( b ) / 255.0f );
}

/*
====================================
	vs3DSModel::ReadColorChunk
====================================
*/
vsVec3f vs3DSModel::ReadColorChunk( void ) {
	unsigned int startPos = File.Tell() -2;
	unsigned int thisLength = ReadChunkLength();
	unsigned short chunkId;
	vsVec3f color;

	while( unsigned int( File.Tell() ) < startPos + thisLength ) {
		File.ReadUnsignedShort( &chunkId );

		switch( chunkId ) {
			case COL_RGB:	color = ReadRGBColorChunk();
			break;

			case COL_TRU:	color = ReadTrueColorChunk();
			break;

			default: ReadUnknownChunk();
		}
	}

	File.Seek( startPos + thisLength, SEEK_SET );
	return color;
}

/*
====================================
	vs3DSModel::ReadChunkLength
====================================
*/
unsigned int vs3DSModel::ReadChunkLength( void ) {
	unsigned int len = 0;
	File.ReadUnsignedInt( &len );
	return len;
}

/*
====================================
	vs3DSModel::ReadUnknownChunk
====================================
*/
void vs3DSModel::ReadUnknownChunk( void ) {
	unsigned int startPos = File.Tell() - 2;
	unsigned int thisLength   = ReadChunkLength();
	File.Seek( startPos + thisLength, SEEK_SET );
}

/*
====================================
	vs3DSModel::Rotate90Model
====================================
*/
void vs3DSModel::Rotate90Model( void ) {
	if ( !loaded ) return;
	Object3DS_t* pObj;
	vsMat3 RM;
	RM.Rotate( -PI/2.0f, 0.0f, 0.0f );
	for ( unsigned int i=0; i< objects.Length(); i++ ) {
		pObj = &( objects.list[ i ] );
		if ( !pObj ) continue;

		for ( unsigned int j=0; j<pObj->Vertices.Length(); j++ ) {
			if ( pObj->Vertices.list[ j ].xyz != NULLVEC3F ) {
				pObj->Vertices.list[ j ].xyz = RM * pObj->Vertices.list[ j ].xyz;
			}
		}
	}
}

/*
Template for chunk reader functions:

void vs3DSModelRead*Chunk( void ) {
	unsigned int startPos = File.Tell() - 2;
	unsigned int thisLength   = ReadChunkLength();
	unsigned short chunkId;

	while( File.Tell() < startPos + thisLength ) {
		File.ReadUnsignedShort( &chunkId );

		switch( chunkId ) {
			case *:		Read*Chunk();
			break;

			default: ReadUnknownChunk();
		}
	}

	File.Seek( startPos + thisLength, SEEK_SET );
}


*/