#include "Tree.h"

/*
===================================
	vsTree
===================================
*/
vsTree::vsTree( void )
{
}

/*
===================================
	vsTree
===================================
*/
vsTree::vsTree( const treeParams_t& p )
{
	Generate( p );
}

/*
===================================
	~vsTree
===================================
*/
vsTree::~vsTree( void ) 
{
	indices.Free();
	vertices.Free();
	if ( buffers[0]!=0 )
		glDeleteBuffers( 2, buffers );
}

/*
===================================
	Generate
===================================
*/
void vsTree::Generate( const treeParams_t& p )
{
	vsTree::~vsTree();

	this->params.point = p.point;
	this->params.growDirection = p.growDirection;
	this->params.scale = p.scale;
	this->params.age = p.age;
	this->params.numBranches = p.numBranches;
	numVert = 0;

	float Count = 0.f;
	for ( float i=0.f; i<=( float )params.age; i++ ) 
		Count += powf( ( float )params.numBranches, i );

	vertices.Alloc( ( uint )Count*4 );

	BranchWarren( p.age, p.point, params.scale.y, vsQuaternion() );
	
	console->Print( "Tree, numvert: %d", numVert );
}

/*
===================================
	Treeize
===================================
*/
void vsTree::BranchWarren( int age, const vsVec3f& origin, float len, const vsQuaternion& rot )
{
	vsVec3f pt2 = origin+( params.growDirection*rot ).Normalized()*len;
	vertices[numVert++].xyz = origin;
	vertices[numVert++].xyz = pt2;
	vertices[numVert-1].Color = vsVec4f( 1.0f, 0.0f, 0.0f, 1.0f );
	vertices[numVert-2].Color = vsVec4f( 0.0f, 1.0f, 0.0f, 1.0f );

	if ( age > 1 )
	{
		float phi = 360.0f / ( float )params.numBranches;
		vsQuaternion Rx, Rz, Ry, R;
		Rz.CreateFromEulerAnglesDEG( 0.0f, 0.0f, 30.0f );
		Ry.CreateFromEulerAnglesDEG( 0.0f, phi, 0.0f );
		Rx.CreateFromEulerAnglesDEG( 30.0f, 0.0f, 0.0f );
		Rz.Normalize();
		Ry.Normalize();
		Rx.Normalize();

		float newLen = len * 0.6f;
		for ( int i = 0; i < params.numBranches; i++ )
		{		
			Ry.CreateFromEulerAnglesDEG( 0.0f, ( float )( i ) * phi, 0.0f );
			R = Rx * Ry * rot;
			R.Normalize();
			BranchWarren( age-1, pt2, newLen, R );
		}
	}
}

/*
===================================
	Render
===================================
*/
void vsTree::Render( void )
{
	glLineWidth( 5.0f );
	glBegin( GL_LINES );
		for ( uint i=0; i<numVert; ++i )
		{
			glColor3fv( vertices[i].Color.ToTypePtr() );
			glVertex3fv( vertices[i].xyz.ToTypePtr() );
		}
	glEnd();
}