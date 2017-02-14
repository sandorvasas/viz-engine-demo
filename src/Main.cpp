#include "Main.h"
#include "WaterDemo.h"


int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	WaterDemoApplication = new vsWaterDemoApplication();

	WaterDemoApplication->Run();

	delete WaterDemoApplication;

	return 0;
}