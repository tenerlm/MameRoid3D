// platform sdk \ d3dim \ include �ֈړ�
#include <stdio.h>

#include <windows.h>
#include <wchar.h>
#include <coef.h>

#ifndef CallF
	#define WIDEN2(x) L ## x
	#define WIDEN(x) WIDEN2(x)
	#define __WFILE__ WIDEN(__FILE__)

	#define CallF(cfunc,f) if(cfunc){ DbgOut( L"file %s, line %d\r\n", __WFILE__, __LINE__ ); _ASSERT( 0 ); f; }
#endif

#ifndef DBGH
#define	DBGH
	HANDLE dbgfile = 0;

	int DbgOut( WCHAR* lpFormat, ... );

	int DbgOutB( void* lpvoid, int datsize );
	int Write2File( HANDLE hfile, char* lpFormat, ... );
	int SetTabChar( int tabnum );		
	void ErrorMessage(char* szMessage, HRESULT hr);
	int OpenDbgFile();
	int CloseDbgFile();

#else
	extern HANDLE dbgfile;

	extern int DbgOut( WCHAR* lpFormat, ... );
	extern int DbgOutB( void* lpvoid, int datsize );
	extern int Write2File( HANDLE hfile, char* lpFormat, ... );
	extern int SetTabChar( int tabnum );
	extern void ErrorMessage(char* szMessage, HRESULT hr);
	extern int OpenDbgFile();
	extern int CloseDbgFile();
#endif

