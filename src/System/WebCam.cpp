#include "WebCam.h"
#include <assert.h>

vsWebCamImageData_t imageData;

vsWebCam::vsWebCam( void )
{
}

vsWebCam::~vsWebCam( void )
{
}

int vsWebCam::Initialize( void )
{
	HRESULT hr;

	CoInitialize( NULL );

	hr = CoCreateInstance( CLSID_FilterGraph,
						  NULL,
						  CLSCTX_INPROC,
						  IID_IGraphBuilder, ( void** )&pGraph );
	
	IMediaControl* pMediaControl;
	hr = pGraph->QueryInterface( IID_IMediaControl, ( void** )&pMediaControl );

	ICreateDevEnum* pCreateDevEnum;
	hr = CoCreateInstance( CLSID_SystemDeviceEnum,
						  NULL,
						  CLSCTX_INPROC,
						  IID_ICreateDevEnum,
						  ( void** )&pCreateDevEnum );

	IEnumMoniker* pCameras;
	hr = pCreateDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory,
											   &pCameras,
											   NULL );
	IMoniker* pMoniker=NULL;
	char* name;
	while( !FAILED( hr ) )
	{
		hr = pCameras->Next( 1, &pMoniker, NULL );
		pMoniker->GetDisplayName( 0, 0, ( LPOLESTR* )&name );
		MessageBox( NULL, name, "Moniker name", MB_OK );
	}
	
	//assert( pMoniker==NULL );
	hr = pMoniker->BindToObject( 0,0, IID_IBaseFilter, ( void** )&pSource );

	hr = pGraph->AddFilter( pSource, L"WebCamSource" );

	IEnumPins* pEnumPins;
	hr = pSource->EnumPins( &pEnumPins );

	IPin* pPin;
	hr = pEnumPins->Next( 1, &pPin, NULL );
	
	pGraph->Render( pPin );

	IEnumFilters* pEnumFilters;
	hr = pGraph->EnumFilters( &pEnumFilters );

	IBaseFilter* pRendererFilter;
	hr = pEnumFilters->Next( 1, &pRendererFilter, NULL );
	hr = pRendererFilter->EnumPins( &pEnumPins );
	hr = pEnumPins->Next( 1, &pPin, NULL );

	pGraph->QueryInterface( IID_IMemInputPin, ( void** )&pMemInputPin );

	memcpy( 	&this->lastImageData, 
			&imageData, 
			sizeof( vsWebCamImageData_t ) );

	return 1;
}


void vsWebCam::Uninitalize( void )
{
	CoUninitialize();
}

void vsWebCam::CaptureImage( vsWebCamImageData_t* pImageData )
{
	memcpy( &pImageData, &lastImageData, sizeof( vsWebCamImageData_t ) );
}
