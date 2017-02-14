#ifndef __NSP_STRUCT_H__
#define __NSP_STRUCT_H__

#include "Renderer\RenderSystem.h"
#include "Renderer\Printer.h"
#include "Renderer\VideoMode.h"
#include "Misc\Input.h"
#include "System\Console.h"
#include "Renderer\Font.h"

/*
class vsModelManager;
class vsRenderSystem;
class vsPrinter;
class vsVideoMode;
class vsInput;
class vsConsole;
*/
namespace vsLib2
{
	/*namespace Boundings
	{
		class vsAABB;
		class vsFrustum;
	};

	namespace Geometry
	{
		namespace Model
		{
			class vsMD5Anim;
			class vsMD5Model;			
			class vs3DSModel;
			class vsIModel;
		} 

		class vsVertex;
	};*/

	/*class vsAABB;
	class vsFrustum;
	class vsMD5Anim;
	class vsMD5Model;			
	class vs3DSModel;
	class vsIModel; 
	class vsVertex;
	namespace Math;
	class vsPlane;
	class vsMat2;
	class vsMat3;
	class vsMat4;
	class vsQuaternion;
	class vsVec2f;
	class vsVec3f;
	class vsVec4f;*/
	

	extern vsRenderSystem*		renderSystem;
	extern vsInput*				input;
	extern vsConsole*			console;
	extern vsVideoMode*			videoMode;
	extern vsPrinter*			printer;
	extern vsPrinter2*			printer2;
	extern float				time;

	/*
	===================================
		Initialize
	===================================
	*/
	inline void Initialize( void )
	{
		printer			= new vsPrinter();
		printer2		= new vsPrinter2();
		console			= new vsConsole();
		renderSystem	= new vsRenderSystem();
		input			= new vsInput();
		videoMode		= new vsVideoMode();
	}

	/*
	===================================
		Uninitialize
	===================================
	*/
	inline void Uninitialize( void )
	{
		delete printer;
		delete printer2;
		delete console;
		delete renderSystem;
		delete input;
		delete videoMode;
	}
}

#endif