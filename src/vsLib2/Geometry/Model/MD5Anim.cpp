#include "MD5Anim.h"
#include "../../System/FileManager.h"

/*
===============================
	vsMD5Anim
===============================
*/
vsMD5Anim::vsMD5Anim( void ) 
{
	frameRate				= 0;
	currentFrame			= 0;
	pModel					= NULL;
	numAnimatedComponents	= 0;
}

/*
===============================
	~vsMD5Anim
===============================
*/
vsMD5Anim::~vsMD5Anim( void ) 
{
}

/*
===============================
	LoadAnim
===============================
*/
int vsMD5Anim::LoadAnim( const vsStr& filename )
{
	vsInputFile		file;
	vsStr			line;


	if(file.Open(filename, "r") != VS_OK)
		return VS_FILE_NOT_FOUND;

	line.Reserve( 512 );

	while(!file.Eof())
	{
		file.ReadLine( &line );

		if(strstr(line(), "numFrames"))
		{
			sscanf_s(line(), "numFrames", &frames.Count);
			frames.Alloc( frames.Count );
		} else

		if(strstr(line(), "numJoints"))
		{
			sscanf_s(line(), "numJoints", &numJoints);
		} else

		if(strstr(line(), "numAnimatedComponents"))
		{
			sscanf_s(line(), "numAnimatedComponents", &numAnimatedComponents);
		} //else
	}

	file.Close();

	return VS_OK;
}