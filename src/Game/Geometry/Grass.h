#ifndef __VS_GRASS_H__
#define __VS_GRASS_H__


#include "..\..\vsLib2\vsLib2.h"
//#include "Sector2.h"

class vsPlayer;
class vsWorld;
class vsSector2OTNode;
class vsSector2;

/*
====================
	grassArgs_t
====================
*/
typedef struct grassArgs_s {
	//vsSector2OTNode*	parentSectorNode;
	vsShader*			shader;
	uint				texture;
	int					density;
	vsVec2f				scale;
	vsVec3f				wind;
} grassArgs_t;

/*
====================
	grassVertex_t
====================
*/
typedef struct grassVertex_s {
	vsVec3f		Position;
	vsVec3f		Normal;
	vsVec2f		UV;

	grassVertex_s( void ) {};
	grassVertex_s( const vsVec3f& pos, const vsVec3f& N, const vsVec2f& uv );
	grassVertex_s& operator=( const grassVertex_s& r );
} grassVertex_t;

/*
====================
	grassBatch_t
====================
*/
typedef struct grassBatch_s {
	grassVertex_t	vert[12];
	vsVec3f			basePoint;

	grassBatch_s( void ) {};
} grassBatch_t;

/*
====================
	grassDistData_t
====================
*/
typedef struct grassDistData_s {
	float	distance;
	int		batchIndex;

	grassDistData_s& operator=( const grassDistData_s& r );
	bool operator==( const grassDistData_s& r ); 
	//bool operator!=( const grassDistData_s& r ); 
	bool operator>( const grassDistData_s& r );
	bool operator<( const grassDistData_s& r );
	bool operator>=( const grassDistData_s& r );
	bool operator<=( const grassDistData_s& r );
} grassDistData_t;

/*
===========================================

	vsGrass

===========================================
*/
class vsGrass {
public:
							vsGrass( void );
							vsGrass( const grassArgs_t& args );
							~vsGrass( void );

	void					Init( const grassArgs_t& args );
	void					Render( void );
	void					SetWindDirection( const vsVec3f& wd );
	void					Generate( vsSector2OTNode* n, int density );

private:
	void					AddNewBatch( const vsVec3f& p, const vsVec3f& N );			
	vsVec3f					RandomLerp( const vsVec3f& v1, const vsVec3f& v2 );

private:
	int						density;
	uint					texture;
	//vsSector2OTNode*		parentSectorNode;
	vsVec3f					wind;
	vsVec2f					scale;
	vsShader*				shader;

	vsList<grassBatch_t*>	batches;
	vsList<grassDistData_t> batchDistances;
};

#endif