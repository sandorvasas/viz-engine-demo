#ifndef __VS_TREE_H__
#define __VS_TREE_H__


#include "..\..\vsLib2\vsLib2.h"
using namespace vsLib2;

typedef struct treeParams_s{
	vsVec3f		point;
	vsVec3f		growDirection;
	vsVec3f		scale;
	int			age;
	int			numBranches;
} treeParams_t;

class vsTree {
public:
						vsTree( void );
						vsTree( const treeParams_t& p );
						~vsTree( void );

	void				Generate( const treeParams_t& p );
	void				Render( void );

private:
	void				BranchWarren( int age, const vsVec3f& origin, float len, const vsQuaternion& rot );

private:
	treeParams_t		params;
	vsList<uint>		indices;
	vsList<vsVertex>	vertices;
	uint				buffers[2];
	uint				numVert;
};

#endif