#ifndef __VS_ENG_CONSOLE_H__
#define __VS_ENG_CONSOLE_H__

#include "List.h"
#include "System.h"
#include "../Renderer/Printer.h"
#include "..\Renderer\Colors.h"
#include "String.h"


const int MAX_CONSOLE_LINES = 500;
const int MAX_VISIBLE_CONSOLE_LINES = 17;

const vsVec4f CMT_LOG	= vsColors::White;
const vsVec4f CMT_ERROR	= vsColors::Red;
const vsVec4f CMT_NOTIFY= vsColors::Green;


/*
============================================

	consoleMsg_t

============================================
*/
class consoleMsg_t {
public:
	consoleMsg_t( void ) {};
	~consoleMsg_t( void ) {};

	consoleMsg_t&		operator=( const consoleMsg_t& b );
	bool				operator==( const consoleMsg_t& b );
	bool				operator!=( const consoleMsg_t& b );

public:
	vsStr				text;
	vsVec4f				type;
};


/*
============================================

	vsCmdFunc_t

============================================
*/
template<typename returnType, typename type>
class vsCmdFunc_t {

	typedef returnType ( *pCmdFunc )( const type& );

public:
	vsCmdFunc_t( void ) {};
	~vsCmdFunc_t( void ) {};

	/*
	============================================
		operator =
	============================================
	*/
	vsCmdFunc_t<returnType, type>& operator=( const vsCmdFunc_t<returnType, type>& b ) {
		func = b.func;
		return( *this );
	}

	/*
	============================================
		operator ==
	============================================
	*/
	bool operator==( const vsCmdFunc_t<returnType, type>& b ) {
		return( func == b.func );
	}

	/*
	============================================
		operator !=
	============================================
	*/
	bool operator!=( const vsCmdFunc_t<returnType, type>& b ) {
		return( func != b.func );
	}

	/*
	============================================
		Set
	============================================
	*/
	void Set( pCmdFunc function ) {
		this->func = function;
	}

	/*
	============================================
		Call
	============================================
	*/
	returnType Call( const type& param ) {
		//if ( typeid( returnType )!=void )
			return func( param );
		//else
			//func( param );
	}

public:
	pCmdFunc	func;
};

//typedef vsCmdFunc_t<void, void>		vsCmdFuncVoid_t;
typedef vsCmdFunc_t<void, int>		vsCmdFuncInt_t;
typedef vsCmdFunc_t<void, float>	vsCmdFuncFloat_t;
typedef vsCmdFunc_t<void, bool>		vsCmdFuncBool_t;
typedef vsCmdFunc_t<void, vsVec2f>	vsCmdFuncVec2_t;
typedef vsCmdFunc_t<void, vsVec3f>	vsCmdFuncVec3_t;
typedef vsCmdFunc_t<void, vsVec4f>	vsCmdFuncVec4_t;


enum cmdType_t {
	CMD_TYPE_VOID=0,
	CMD_TYPE_INT,
	CMD_TYPE_BOOL,
	CMD_TYPE_FLOAT,
	CMD_TYPE_VEC2,
	CMD_TYPE_VEC3,
	CMD_TYPE_VEC4
};

/*
============================================

	vsCmd_t

============================================
*/
class vsCmd_t {
public:
	vsCmd_t( void );
	~vsCmd_t( void ) {};

	vsCmd_t&			operator=( const vsCmd_t& b );
	bool				operator==( const vsCmd_t& b );
	bool				operator!=( const vsCmd_t& b );

	//void				CreateVoid( char* name, void ( *func )( void ) );
	void				CreateInt( char* name, void ( *func )( const int& ) );
	void				CreateBool( char* name, void ( *func )( const bool& ) );
	void				CreateFloat( char* name, void ( *func )( const float& ) );
	void				CreateVec2( char* name, void ( *func )( const vsVec2f& ) );
	void				CreateVec3( char* name, void ( *func )( const vsVec3f& ) );
	void				CreateVec4( char* name, void ( *func )( const vsVec4f& ) );
	void				Do( char* format );

public:
	char				name[ 32 ];
	cmdType_t			type;
private:

//	vsCmdFuncVoid_t		voidFunc;
	vsCmdFuncInt_t		intFunc;
	vsCmdFuncFloat_t	floatFunc;
	vsCmdFuncBool_t		boolFunc;
	vsCmdFuncVec2_t		vec2Func;
	vsCmdFuncVec3_t		vec3Func;
	vsCmdFuncVec4_t		vec4Func;
};


/*
============================================

	vsConsole

============================================
*/
class vsConsole {
public:
							vsConsole( void );
							~vsConsole( void );
	void					Initialize( bool logToFile = true );
	void					Clear( void );
	void					Print( const vsVec4f& color, char* msg, ... );
	void					Print( char* msg, ... );
	void					Toggle( void ) { visible = !visible; };
	void					Render( void );
	void					ApplyTexture( unsigned int texture );
	void					SetFont( const char* fontname, int size, bool bold, bool italic );
	void					AddCommand( const vsCmd_t& cmd );
	void					ScrollUp( void );
	void					ScrollDown( void );
	void					DisablePrinting( void );
	void					EnablePrinting( void );
	void					CheckInput( bool* keys );
	void					RenderTexts( float x, float y );
	bool					GetLine( vsStr& cmd );

private:
	void					ProcessUserInput( void );
	void					LogToFile( char* text );
	void					NewLine( const vsStr& text, const vsVec4f& cmtype );

protected:
	vsList<vsCmd_t>			cmds;
	consoleMsg_t			lines[MAX_CONSOLE_LINES];
	int						numLines;
//	vsPrinter				printer;
	unsigned int			texture;
	int						scroll;
	int						scrollTop;
	bool					printingEnabled;
	vsStr					userInput;
	vsStr					getline;
	bool					logToFile;
	bool					bfont;
	int						fontId;

public:
	bool					visible;
};

#endif