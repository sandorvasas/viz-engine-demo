#include "MarchingCubes.h"
#include "MC_Tables.h"



vsMarchingCubes::vsMarchingCubes( void )
{
	voxelData = NULL;
}

vsMarchingCubes::~vsMarchingCubes( void )
{
	if ( voxelData )
	{
		for ( uint x=0, y; x<size; ++x )
		{
			for ( y=0; y<size; ++y )
				delete [] voxelData[x][y];
			delete [] voxelData[x];
		} 
		delete [] voxelData;
	}
}

void vsMarchingCubes::Generate( const marchingCubesArgs_t& a )
{
	size = a.voxelDataSize;
	isolevel = a.isolevel;
	gridSize = a.gridSize;
	shader = a.shader;

	// allocate memory for voxels
	voxelData = new byte**[size];
	for ( uint x=0, y; x<size; ++x ) {
		voxelData[x] = new byte*[size];
		for ( y=0; y<size; ++y )
			voxelData[x][y] = new byte[size];
	}

	GenerateSampleVoxelData();
	GenerateIsosurface();

	shader->BindAttrib( 0, "in_Position" );
	shader->BindAttrib( 1, "in_Normal" );
}

void vsMarchingCubes::GenerateSampleVoxelData( void )
{
	int a, b, c;
	a=b=c=0;
	for ( uint x=0, y, z; x<size; ++x )
	{
		if ( x % 2 == 0 ) a = 1-a;
		//if ( x == byte( size/2 + int( sinf( x/size * 3.14f*2 ) * ( size/2 ) ) ) )
			// a = 1-a;
		for ( y=0; y<size; ++y )
		{
			if ( y % 2 == 0 ) b = 1-b;
			//if ( y == byte( size/2 + int( sinf( y/size * 3.14f*2 ) * ( size/2 ) ) ) )
			 //b = 1-b;
			for ( z=0; z<size; ++z )
			{
				//if ( z % 2 == 0 ) c = 1-c;
				if ( ( y>=size/4 && x>=size/4 && z>=size/4 )
				&& ( y<=size/4*3 && x<=size/4*3 && z<=size/4*3 ) )
				//if ( a && b && c )
				//if ( z % 2 && x % 2 && y % 2 )
				//if ( x > int( sinf( y ) * 5 + 5 ) 
					//&& y == int( cosf( z ) * 5 + 5 ) )
				{
					voxelData[x][y][z] = 255;
				} else
				{
					voxelData[x][y][z] = 0;
				}
			}
		}
	}
}

void vsMarchingCubes::GenerateIsosurface( void )
{
	marchingCubesGridCell_t cell;
	uint xg, yg, zg;
	float fx, fy, fz, fxg, fyg, fzg;
	float scale = 3.2f;

	numVertices=0;
	vertices.Alloc( ( size/gridSize ) *( size/gridSize ) *( size/gridSize ) * 3 );
	console->Print( "vertex alloc size: %d", vertices.Length() );

	for ( uint x=0,y,z; x<size; x+=gridSize ) {
	for ( y=0; y<size; y+=gridSize ) {
	for ( z=0; z<size; z+=gridSize ) {
		
		zg = z+gridSize < size ? z+gridSize : z;
		yg = y+gridSize < size ? y+gridSize : y;
		xg = x+gridSize < size ? x+gridSize : x;

		
		fx = ( float )x * scale;
		fy = ( float )y * scale;
		fz = ( float )z * scale;
		fxg = ( float )xg * scale;
		fyg = ( float )yg * scale;
		fzg = ( float )zg * scale;

		cell.vertices[7] = vsVec3f( fx, fy, fz );
		cell.values[7] = voxelData[x][y][z];

		cell.vertices[6] = vsVec3f( fxg, fy, fz );
		cell.values[6] = voxelData[xg][y][z];

		cell.vertices[4] = vsVec3f( fx, fy, fzg );
		cell.values[4] = voxelData[x][y][zg];

		cell.vertices[5] = vsVec3f( fxg, fy, fzg );
		cell.values[5] = voxelData[xg][y][zg];

		cell.vertices[3] = vsVec3f( fx, fyg, fz );
		cell.values[3] = voxelData[x][yg][z];

		cell.vertices[0] = vsVec3f( fx, fyg, fzg );
		cell.values[0] = voxelData[x][yg][zg];

		cell.vertices[2] = vsVec3f( fxg, fyg, fz );
		cell.values[2] = voxelData[xg][yg][z];

		cell.vertices[1] = vsVec3f( fxg, fyg, fzg );
		cell.values[1] = voxelData[xg][yg][zg];

		Polygonize( cell );
	}
	}
	}
	
	vertices.Resize( numVertices );
	console->Print( "real vertex: %d", numVertices );
	//REPEAT( numVertices )
		//console->Print( "%s", vertices[i].xyz.ToCharPtr() );
}

int vsMarchingCubes::Polygonize( const marchingCubesGridCell_t& cell )
{
	int cubeIndex = 0;
	// 12 éle van a kockának, ezért lehet max. 12 vertex
	vsVec3f vert[12];

	// x < isolevel -> x is below the meshes surface, aka it's inside
	for ( int j=0, k=1; j<8; ++j, k*=2 ) 
		if ( cell.values[j] < isolevel ) cubeIndex |= k;


	if ( edgeTable[cubeIndex] == 0 )
		return 0;

	for ( int j=0, k=1; j<12; ++j, k*=2 )
		if ( edgeTable[cubeIndex] & k ) vert[j] = InterpolateVertex( cell, j );

	vsVec3f scale = vsVec3f( 3.0f );
	vsVec3f offset = vsVec3f( 0.0f, 200.0f, 0.0f );
	for ( int i=0;i<triVertLenTable[cubeIndex]; i+=3 )
	{
		vertices[numVertices++].xyz = vert[ triTable[cubeIndex][i] ]*scale+offset;

		vertices[numVertices++].xyz = vert[ triTable[cubeIndex][i+1] ]*scale+offset;

		vertices[numVertices++].xyz = vert[ triTable[cubeIndex][i+2] ]*scale+offset;

		vertices[numVertices-3].CalculateNormal( 	vertices[numVertices-2], 
													vertices[numVertices-1] );
		vertices[numVertices-2].Normal  = vertices[numVertices-3].Normal;
		//vertices[numVertices-2].Tangent = vertices[numVertices-3].Tangent;
		vertices[numVertices-1].Normal  = vertices[numVertices-3].Normal;
		//vertices[numVertices-1].Tangent = vertices[numVertices-3].Tangent;
	}

	vsVec3f n;
	/*for ( uint i=0, j; i<vertices.Length()-1; ++i )
	{
		for ( j=i+1; j<vertices.Length(); ++j )
		{
			if ( vertices[i].xyz == vertices[j].xyz )
			{
				n = vertices[i].Normal + vertices[j].Normal;
				vertices[j].Normal = vertices[i].Normal = n.Normalize();
			}
		}
	}*/
	//trinumTable();
	return 0;
}

// visszaadja, hogy az isolevel hányad részén áll az ab élnek
vsVec3f vsMarchingCubes::InterpolateVertex( const marchingCubesGridCell_t& c, int edgeIndex )
{
	float fval1 = float( c.values[edgeVertices[edgeIndex][0]] );
	float fval2 = float( c.values[edgeVertices[edgeIndex][1]] );
	float fisolevel = float( isolevel );
	vsVec3f a = c.vertices[edgeVertices[edgeIndex][0]];
	vsVec3f b = c.vertices[edgeVertices[edgeIndex][1]];

	return a +  ( ( b-a ) / ( fval2-fval1 ) ) * ( fisolevel - fval1 );
}

void vsMarchingCubes::RenderVoxelData( void )
{
	vsVec3f v;

	glPointSize( 10.0f );
	glColor3f( 0.0f, 1.0f, 0.0f );
	glBegin( GL_POINTS );
	for ( uint x=0,y,z; x<size; x++ ) 
		for ( y=0; y<size; y++ ) 
			for ( z=0; z<size; z++ ) 
			{
				if ( voxelData[x][y][z]>0 ) {
					v = vsVec3f( ( float )x, ( float )y+200.f, ( float )z ) * 0.2f;
					glVertex3fv( v.ToTypePtr() );
				}
			}

	glEnd();
	glColor3f( 1.0f, 1.0f, 1.0f );
}

void vsMarchingCubes::Render( void )
{
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glLineWidth( 2.0f );
	//glEnableClientState( GL_VERTEX_ARRAY );
	//glVertexPointer( 3, GL_FLOAT, sizeof( vsVertex ), vertices.list );

	shader->Bind( true );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );

	glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( vsVertex ), vertices.list );
	glVertexAttribPointer( 1, 2, GL_FLOAT, true, sizeof( vsVertex ), ( &vertices.list[0].Normal ) );

	glDrawArrays( GL_TRIANGLES, 0, vertices.Length() );

	glDisableVertexAttribArray( 0 );
	glDisableVertexAttribArray( 1 );
	shader->Bind( false );

	//glDisableClientState( GL_VERTEX_ARRAY );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}
