#ifndef __WEBCAM_H__
#define __WEBCAM_H__

#include <Windows.h>
#include <DShow.h>

#pragma comment( lib, "strmiids.lib" )


#ifndef __webCamImageData_t__
#define __webCamImageData_t__
typedef struct vsWebCamImageData_s
{
	int				width;
	int				height;
	int				bpp;
	long			buflen;
	unsigned char*  buf;
} vsWebCamImageData_t;
#endif

extern vsWebCamImageData_t imageData;

/*
=======================================
	vsWebCamImageDataReceiver
=======================================
*/
class vsWebCamImageDataReceiver : public IMemInputPin {
public:
	vsWebCamImageDataReceiver( void ) {};
	~vsWebCamImageDataReceiver( void ) {};

	inline
	HRESULT STDMETHODCALLTYPE Receive( IMediaSample * pSample )
	{
		imageData.width = 320;
		imageData.height = 240;
		imageData.bpp = 24;
		imageData.buflen = pSample->GetActualDataLength();
		pSample->GetPointer( &imageData.buf );

		return S_OK;
	};

public:
	
};

/*
=======================================
	vsWebCam
=======================================
*/
class vsWebCam {
public:
	vsWebCam( void );
	~vsWebCam( void );
		
	int			Initialize( void );
	void		Uninitalize( void );
	void		CaptureImage( vsWebCamImageData_t* pImageData );

private:
	IPin*		GetPin( IBaseFilter* filter, PIN_DIRECTION dir );

public:
	int			imageHeight;
	int			imageWidth;
	int			bpp;

private:
	IGraphBuilder*				pGraph;
	IBaseFilter*				pSource;
	vsWebCamImageDataReceiver*	pMemInputPin;
	vsWebCamImageData_t			lastImageData;

};

#endif