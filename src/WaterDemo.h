#ifndef __WATER_DEMO_H__
#define __WATER_DEMO_H__

#include "vsLib2\vsLib2.h"
#include "Game\Game.h"
#include "System\WebCam.h"
using namespace vsLib2;

enum rttType_t{
	RTT_NO = 0,
	RTT_SIMPLE,
	RTT_MSAA,
	RTT_CSAA
};

enum AAType_t {
	ANTIALIAS_NONE = 0,
	ANTIALIAS_MSAA,
	ANTIALIAS_CSAA
};


typedef struct settings_s 
{
	struct {
		ushort		screenWidth;
		ushort		screenHeight;
		ushort		refreshRate;
		byte		bitsPerPixel;
		int			textureMagFilter;
		int			textureMinFilter;
		int			anisotropyLevel;
		rttType_t	rtt;
		int			ambientOcclusion;
		int			depthOfField;
		int			bumpMapping;
		ushort		textureQualiy;
		int			multiSampling;
		int			grass;
		int			particles;
	} display;

	struct {
	} game;

	void			LoadDefaults( void );
	void			LoadFromFile( char* szFileName );
	void			UniformDisplaySettings( vsShader* shader );
} settings_t;


class vsWaterDemoApplication {
	friend class vsConsoleProcessor;
public:
	vsWaterDemoApplication() {  game = NULL; };
	~vsWaterDemoApplication() { /*Shutdown();*/ };

	void		Run( void );

private:
	void		Init( void );
	void		Shutdown( void );
	void		Update( void );
	void		Render( void );
	void		RenderToBuffer( void );
	void		ProcessKeys( void );

	void		SaveToFile( char* szFileName );
	void		RestoreFromFile( char* szFileName );

	static LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp );
	
private:
	bool					running;
	HWND					window;
	static	float			appTime;

	settings_t				settings;
	//vsGame*					game;
	vsShader*				postProcessShader;
	vsWebCam				webCam;
	vsFramebuffer			fboSimple;
	vsFramebuffer			fboMultisample;

	uint					TimeBeforeInit;		// used to calc loading time
	uint					TimeAfterInit;
};

extern vsWaterDemoApplication* WaterDemoApplication;

#endif