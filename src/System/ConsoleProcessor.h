#ifndef __VS_CONSOLE_PROCESSOR_H__
#define __VS_CONSOLE_PROCESSOR_H__

#include "..\vsLib2\vsLib2.h"


/*
====================================================================

	vsConsoleProcessor

====================================================================
*/
class vsConsoleProcessor {
private:
	vsStr cmd;

public:
	vsConsoleProcessor( void ) {};
	~vsConsoleProcessor( void ) {};

	void Hz( void );


};

extern vsConsoleProcessor consoleProcessor;

#endif