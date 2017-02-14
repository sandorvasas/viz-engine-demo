#ifndef __IMODEL_H__
#define __IMODEL_H__


struct loadArgs3DS_t
{
	 vsStr		texDir;
	 bool		rotate90;
	 bool		swapNormals;
};

struct loadArgsMD5_t 
{
	vsStr		matLib;
};

struct modelLoadArgs_t 
{
	vsStr			szFileName;
	loadArgs3DS_t	type_3ds;
	loadArgsMD5_t	type_md5;
};


class vsIModel {
public:

	virtual int						Load( const modelLoadArgs_t& args ) = 0;
	virtual void					Render( void ) = 0;
	virtual void					UpdateAnimation( void ) = 0;

	virtual void					SetOrigin( const vsVec3f& newOrigin ) = 0;
	virtual void					SetAxis( const vsMat3& newAxis ) = 0;
	virtual void					Scale( const vsVec3f& s ) = 0;
	virtual void					Translate( const vsVec3f& delta ) = 0;
	virtual void					Rotate( const vsMat3& delta ) = 0;
	virtual void					RenderBasis( void ) = 0;
};

#endif