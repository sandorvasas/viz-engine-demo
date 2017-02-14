#ifndef __VS_MODELMANAGER_H__
#define __VS_MODELMANAGER_H__

#include "Model3DS.h"
#include "..\..\System\List.h"
#include "..\..\System\String.h"


class vsModelManager {
public:
	vsModelManager( void );
	~vsModelManager( void );


	vs3DSModel*				LoadClipModel3DS( const modelLoadArgs_t& args );
	vs3DSModel*				GetClipModel3DS( unsigned int index );
	vs3DSModel*				GetClipModel3DS( const vsStr& path );

	vs3DSModel*				LoadModel3DS( const modelLoadArgs_t& args );
	vs3DSModel*				GetModel3DS( unsigned int index );
	vs3DSModel*				GetModel3DS( const vsStr& path );

private:
	vsList<vs3DSModel>		lModel3DS;
	vsList<vs3DSModel>		lClipModel3DS;
};


//extern vsModelManager* modelManager;

#endif