#include "ModelManager.h"	
#include "..\..\system\Console.h"
#include "..\..\NspStruct.h"
using namespace vsLib2;

/*
==================================
	Constructor
==================================
*/
vsModelManager::vsModelManager( void )
{
}

/*
==================================
	Destructor
==================================
*/
vsModelManager::~vsModelManager( void ) {
}

/*
==================================
	LoadModel3DS
==================================
*/
vs3DSModel* vsModelManager::LoadModel3DS( const modelLoadArgs_t& args )
{
	vs3DSModel* i = GetModel3DS(args.szFileName );

	//if(i == NULL) {
		lModel3DS.Expand();
		lModel3DS.Last().Load( args );
		console->Print(CMT_LOG, "Model loaded: %s", args.szFileName());
	//}
	//else {
		//console->Print(CMT_NOTIFY, "i->path: %s", i->path());
		lModel3DS.Append( *i );
		//console->Print(CMT_NOTIFY, "lModel3DS[%d].path: %s", lModel3DS.Length()-1, lModel3DS.Last().path());
	//}

	return( &lModel3DS.Last() );
}

/*
==================================
	GetModel3DS()
==================================
*/
vs3DSModel* vsModelManager::GetModel3DS( unsigned int index )
{
	if(index<lModel3DS.Length())
		return( &lModel3DS[ index ] );
	else
		return NULL;
}

/*
==================================
	GetModel3DS()
==================================
*/
vs3DSModel* vsModelManager::GetModel3DS( const vsStr& path ) 
{
	unsigned int i = 0;
	
	while( (i<lModel3DS.Length()) && (lModel3DS[i].path != path)) ++i;

	if(i < lModel3DS.Length() )
		return( &lModel3DS[ i ] ); 
	else return NULL;
}

/*
==================================
	LoadClipModel3DS
==================================
*/
vs3DSModel* vsModelManager::LoadClipModel3DS( const modelLoadArgs_t& args )
{
	vs3DSModel* i = GetClipModel3DS(args.szFileName);

	if(i == NULL) {
		lClipModel3DS.Expand();
		lClipModel3DS.Last().Load( args );
		console->Print(CMT_LOG, "ClipModel loaded: %s", args.szFileName());
	}
	else {
		//console->Print(CMT_NOTIFY, "i->path: %s", i->path());
		lClipModel3DS.Append( *i );
		//console->Print(CMT_NOTIFY, "lClipModel3DS[%d].path: %s", lClipModel3DS.Length()-1, lClipModel3DS.Last().path());
	}

	return( &lClipModel3DS.Last() );
}

/*
==================================
	GetClipModel3DS()
==================================
*/
vs3DSModel* vsModelManager::GetClipModel3DS( unsigned int index )
{
	if(index<lClipModel3DS.Length())
		return( &lClipModel3DS[ index ] );
	else
		return NULL;
}

/*
==================================
	GetClipModel3DS()
==================================
*/
vs3DSModel* vsModelManager::GetClipModel3DS( const vsStr& path ) 
{
	unsigned int i = 0;
	
	while( (i<lClipModel3DS.Length()) && (lClipModel3DS[i].path != path)) ++i;

	if(i < lClipModel3DS.Length() )
		return( &lModel3DS[ i ] ); 
	else return NULL;
}