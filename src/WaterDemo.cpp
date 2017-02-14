#include "WaterDemo.h"
#include "System\VarStorage.h"


vsWaterDemoApplication* WaterDemoApplication;

float	vsWaterDemoApplication::appTime;


void settings_t::LoadDefaults( void )
{
	display.anisotropyLevel = 0;
	display.bitsPerPixel = 32;
	display.refreshRate = 85;
	display.screenWidth = 1024;
	display.screenHeight = 768;
	display.textureMagFilter = GL_NEAREST;
	display.textureMinFilter = GL_NEAREST;
	display.rtt = RTT_SIMPLE;
	display.bumpMapping = 1;
	display.depthOfField = 1;
	display.ambientOcclusion = 1;
	display.multiSampling = 0;
}

void settings_t::LoadFromFile( char* szFileName )
{
	FILE * f;

	if ( fopen_s( &f, szFileName, "r" ) ) {
		LoadDefaults();
		return;
	}

	char line[ 128 ];

	while ( !feof( f ) ) {
		fgets( line, 128, f );

		if ( strstr( line, "resolution" ) ) {
			sscanf_s( line, "resolution %dx%dx%d",	&display.screenWidth, 
													&display.screenHeight,
													&display.refreshRate );
		} else
		if ( strstr( line, "bpp" ) ) {
			sscanf_s( line, "bpp %d",	&display.bitsPerPixel );
		} else
		if ( strstr( line, "anisotropy" ) ) {
			sscanf_s( line, "anisotropy %d",	&display.anisotropyLevel );
		} else
		if ( strstr( line, "rendertexturetype" ) ) {
			sscanf_s( line, "rendertexturetype %d", &display.rtt );
		} else
		if ( strstr( line, "bumpmapping" ) ) {
			sscanf_s( line, "bumpmapping %d", &display.bumpMapping );
		} else
		if ( strstr( line, "multisampling" ) ) {
			sscanf_s( line, "multisampling %d", &display.multiSampling );
		} else
		if ( strstr( line, "depthoffield" ) ) {
			sscanf_s( line, "depthoffield %d", &display.depthOfField );
		} else
		if ( strstr( line, "ambientocclusion" ) ) {
			sscanf_s( line, "ambientocclusion %d", &display.ambientOcclusion );
		} else
		if ( strstr( line, "grass" ) ) {
			sscanf_s( line, "grass %d", &display.grass );
		} else
		if ( strstr( line, "particles" ) ) {
			sscanf_s( line, "particles %d", &display.grass );
		} 
	}

	display.textureMagFilter = GL_NEAREST;
	display.textureMinFilter = GL_NEAREST;

	fclose( f );
}

void settings_t::UniformDisplaySettings( vsShader* shader )
{
	shader->Bind ( true );

	shader->Uniform1i ( "in_bBumpMapping", display.bumpMapping );
	shader->Uniform1i ( "in_RttType", display.rtt );

	shader->Bind ( false );
}

//==================================
// Init()
//==================================
void vsWaterDemoApplication::Init( void )
{
	TimeBeforeInit = GetTickCount();
	WNDCLASSEX wc;
	HINSTANCE inst = GetModuleHandle( 0 );

	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof( WNDCLASSEX );
	wc.cbWndExtra = NULL;
	wc.hbrBackground = ( HBRUSH ) GetStockObject( BLACK_BRUSH );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hIcon = wc.hIconSm = LoadIcon( NULL, IDI_WINLOGO );
	wc.hInstance = inst;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = "WATERDEMOWINDOWCLASS";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	RegisterClassEx( &wc );

	vsLib2::Initialize();
		
	///RestoreFromFile( "settings.bin" ); FIXME: delete func

	settings.LoadFromFile( "graph.txt" );

	vsLib2::videoMode->Change( 	settings.display.screenWidth,
								settings.display.screenHeight,
								settings.display.bitsPerPixel,
								settings.display.refreshRate,
								true );
	
	this->window = CreateWindowEx( 	WS_EX_APPWINDOW, 
									"WATERDEMOWINDOWCLASS", 
									"Water Demo by Sonka",
									WS_POPUP | WS_CLIPSIBLINGS,
									0,
									0,
									vsLib2::videoMode->iWidth,
									vsLib2::videoMode->iHeight,
									NULL,
									NULL,
									inst,
									NULL );
	
	
	
	

	renderSettings_t rs;

	rs.OpenGLVersion = OPENGL_VERSION_2_0;
	rs.pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	rs.pfd.nVersion = 1;
	rs.pfd.cColorBits = settings.display.bitsPerPixel;
	rs.pfd.cDepthBits = ( settings.display.bitsPerPixel==32 ) ? 24 : 16;
	rs.pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	rs.pfd.iLayerType = PFD_MAIN_PLANE;
	rs.pfd.iPixelType = PFD_TYPE_RGBA;
	rs.textureSettings.anisotropyLevel = settings.display.anisotropyLevel;
	rs.textureSettings.magFilter = settings.display.textureMagFilter;
	rs.textureSettings.minFilter = settings.display.textureMinFilter;
	
	renderSystem->Initialize( &this->window, rs );
	renderSystem->SwapBuffers();

	//printer->NewFont( "Tahoma", 16, true, false, false, false );
	//printer->NewFont( "Tahoma", 16, true, false, false, false );
	console->Initialize();	

	ShowWindow( window, SW_SHOW );
	SetFocus( window );	
	//SetActiveWindow( window );
	input->SetMousePos( videoMode->iScreenCenterX, videoMode->iScreenCenterY );
	ShowCursor( false );	
	SetForegroundWindow( window );
	LockSetForegroundWindow( LSFW_LOCK );

	vsGL::LoadEXT_GL_EXT_framebuffer_object();	

	renderSystem->Enable( GL_DEPTH_TEST );
	renderSystem->Enable( GL_TEXTURE_2D );
	//renderSystem->Enable( GL_TEXTURE_3D );
	renderSystem->Enable( GL_POINT_SMOOTH );
	renderSystem->Enable( GL_POINT_SPRITE );
	renderSystem->Enable( GL_VERTEX_PROGRAM_POINT_SIZE );
	glTexEnvi( GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE );
	renderSystem->Enable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	//renderSystem->Enable( GL_CULL_FACE );
	//glCullFace( GL_BACK );

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );


	// Setup framebuffer & postprocess effects
	if ( settings.display.rtt != RTT_NO ) {

		postProcessShader = renderSystem->LoadShaderFromFile( "shader_postprocess", "data/shaders/postprocess.txt",  0, 0 );

		postProcessShader->Bind( true );
		postProcessShader->Uniform1i( "in_bDepthOfField", settings.display.depthOfField );
		postProcessShader->Uniform1i( "in_bAmbientOcclusion", settings.display.ambientOcclusion );
		postProcessShader->Bind( false );

		if ( settings.display.rtt == RTT_SIMPLE ) {
			fboSimple.Create( videoMode->iWidth, videoMode->iHeight );
			fboSimple.AttachRenderbuffer( GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24 );
			fboSimple.AttachTexture2D( GL_COLOR_ATTACHMENT0, GL_RGBA8, GL_RGBA );
			fboSimple.AttachTexture2D( GL_COLOR_ATTACHMENT1, GL_RGBA8, GL_RGBA );
			fboSimple.AttachTexture2D( GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT );
		}
	}



	/*unsigned char major, minor;
	renderSystem->GetGLVersion( major, minor );
	console->Print( "OpenGL Version: %d.%d", major, minor );*/

	vsLib2::time = 0.0f;

	game = new vsGame();
	game->Initialize();

	varStorage.Bool.Set( "nograss", !settings.display.grass );
	varStorage.Bool.Set( "renderparticles", settings.display.particles == 0 );
	
	//game->NewGame();
	game->Load();

	TimeAfterInit = GetTickCount();
	console->Print( CMT_NOTIFY, "Loading took %4.4f seconds.", float( TimeAfterInit - TimeBeforeInit )*0.001f );
}


//==================================
//	Render()
//==================================
void vsWaterDemoApplication::Render( void )
{
	if ( settings.display.rtt == RTT_SIMPLE ) {
		fboSimple.BeginRender();
		const GLenum bufs[] = { GL_COLOR_ATTACHMENT0, 
								GL_COLOR_ATTACHMENT1					
		};

		glDrawBuffers( 2, bufs );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();

		game->RenderToBuffer();

		fboSimple.EndRender();

		glClearColor( 0.f, 0.f, 0.f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();

		game->RenderHUD();
		console->Render();

		
		postProcessShader->Bind( true );
		renderSystem->OrthoMode( ON );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, fboSimple.GetTexture( 0 ) );
		postProcessShader->Uniform1i( "in_Map1", 0 );
		
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, fboSimple.GetTexture( 1 ) );
		postProcessShader->Uniform1i( "in_Map2", 1 );
		
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, fboSimple.GetTexture( 2 ) );
		postProcessShader->Uniform1i( "in_Map3", 2 );
		
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glBegin( GL_QUADS );
			glMultiTexCoord2f( GL_TEXTURE0, 0.0f, 0.0f );
			glVertex3f( -1.0f, -1.0f, 0.8f );
			glMultiTexCoord2f( GL_TEXTURE0, 1.0f, 0.0f );
			glVertex3f( 1.0f, -1.0f, 0.8f );
			glMultiTexCoord2f( GL_TEXTURE0, 1.0f, 1.0f );
			glVertex3f( 1.0f,  1.0f, 0.8f );
			glMultiTexCoord2f( GL_TEXTURE0, 0.0f, 1.0f );
			glVertex3f( -1.0f,  1.0f, 0.8f );
		glEnd();

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D,0 );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D,0 );
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D,0 );
		renderSystem->OrthoMode( OFF );	

		postProcessShader->Bind( false );
	} else
	if ( settings.display.rtt == RTT_NO ) {

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();

		game->RenderHUD();
		console->Render();

		game->RenderToBuffer();
	}
	

	renderSystem->SwapBuffers();
}

//==================================
//	RenderToBuffer()
//==================================
void vsWaterDemoApplication::RenderToBuffer( void )
{
	
}

//==================================
//	ProcessKeys()
//==================================
void vsWaterDemoApplication::ProcessKeys( void )
{
	if ( input->IsKeyDown( VK_ESCAPE ) )
		running = false;

	game->ProcessKeys();
}


//==================================
// Shutdown()
//==================================
void vsWaterDemoApplication::Shutdown( void )
{
	this->running = false;
	//renderSystem->Shutdown();
	//delete renderSystem;
	//DestroyWindow( this->window );
	//webCam.Uninitalize();
	game->Shutdown();
	delete game;
	//SaveToFile( "settings.bin" );
	vsLib2::Uninitialize();
}

//==================================
//	Update()
//==================================
void vsWaterDemoApplication::Update( void )
{
	ProcessKeys();
	game->Update();
	Render();
}

//==================================
//	Run()
//==================================
void vsWaterDemoApplication::Run( void )
{
	Init();

	this->running = true;

	MSG msg;

	while( this->running )
	{
		if ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} else
		{
			Update();		
		}
	}
	Shutdown();
}


void vsWaterDemoApplication::SaveToFile( char* szFileName )
{
	vsOutputFile f;
	f.Open( szFileName, "wb" );

	f.WriteUnsignedShort( &settings.display.screenWidth );
	f.WriteUnsignedShort( &settings.display.screenHeight );
	f.WriteUnsignedShort( &settings.display.refreshRate );
	f.WriteByte( &settings.display.bitsPerPixel );
	f.WriteInt( &settings.display.anisotropyLevel );
	f.WriteInt( &settings.display.textureMagFilter );
	f.WriteInt( &settings.display.textureMinFilter );

	f.Close();
}

void vsWaterDemoApplication::RestoreFromFile( char* szFileName )
{
	vsInputFile f;
	if ( f.Open( szFileName, "rb" )==VS_ERROR ) 
	{
		settings.LoadDefaults();
		return;
	}

	f.ReadUnsignedShort( &settings.display.screenWidth );
	f.ReadUnsignedShort( &settings.display.screenHeight );
	f.ReadUnsignedShort( &settings.display.refreshRate );
	f.ReadByte( &settings.display.bitsPerPixel );
	f.ReadInt( &settings.display.anisotropyLevel );
	f.ReadInt( &settings.display.textureMagFilter );
	f.ReadInt( &settings.display.textureMinFilter );

	f.Close();
}

//==================================
//	WndPROC()
//==================================
LRESULT CALLBACK vsWaterDemoApplication::WndProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	if ( WaterDemoApplication->running ) {
		switch( msg )
		{
			case WM_ACTIVATE:
			//case WM_SETFOCUS:
				input->SetMousePos( videoMode->iScreenCenterX, videoMode->iScreenCenterY );
				ShowCursor( false );	
				SetForegroundWindow( hwnd );
				LockSetForegroundWindow( LSFW_LOCK );
			break;
		}
		input->Win32Input( msg, wp, lp );
	}
	return DefWindowProc( hwnd, msg, wp, lp );
}