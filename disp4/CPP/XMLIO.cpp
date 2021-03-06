#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <XMLIO.h>
#include <quaternion.h>

#define DBGH
#include <dbg.h>

#include <string>

#include <PmCipherDll.h>

CXMLIO::CXMLIO()
{
	InitParams();
}

CXMLIO::~CXMLIO()
{
	DestroyObjs();
}

int CXMLIO::InitParams()
{
	m_mode = 0;
	ZeroMemory( &m_xmliobuf, sizeof( XMLIOBUF ) );
	m_hfile = INVALID_HANDLE_VALUE;

	return 0;
}

int CXMLIO::DestroyObjs()
{
	if( m_hfile != INVALID_HANDLE_VALUE ){
		if( m_mode == XMLIO_WRITE ){
			FlushFileBuffers( m_hfile );
			SetEndOfFile( m_hfile );
		}
		CloseHandle( m_hfile );
		m_hfile = INVALID_HANDLE_VALUE;
	}
	
	if( m_xmliobuf.buf ){
		free( m_xmliobuf.buf );
		m_xmliobuf.buf = 0;
	}

	m_mode = 0;
	ZeroMemory( &m_xmliobuf, sizeof( XMLIOBUF ) );
	m_hfile = INVALID_HANDLE_VALUE;

	return 0;
}

int CXMLIO::Write2File( char* lpFormat, ... )
{
	if( m_hfile == INVALID_HANDLE_VALUE ){
		return 0;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	char outchar[XMLIOLINELEN];
			
	ZeroMemory( outchar, XMLIOLINELEN );

	va_start( Marker, lpFormat );
	ret = vsprintf_s( outchar, XMLIOLINELEN, lpFormat, Marker );
	va_end( Marker );

	if( ret < 0 )
		return 1;

	wleng = (unsigned long)strlen( outchar );
	WriteFile( m_hfile, outchar, wleng, &writeleng, NULL );
	if( wleng != writeleng ){
		return 1;
	}
	
	return 0;
}

int CXMLIO::SetBuffer( CPmCipherDll* cipher, int dataindex )
{
	PNDPROP prop;
	ZeroMemory( &prop, sizeof( PNDPROP ) );


	CallF( cipher->GetProperty( dataindex, &prop ), return 1 );

	if( prop.sourcesize <= 0 ){
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( prop.sourcesize + 1 ) );
	if( !newbuf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( prop.sourcesize + 1 ) );

	int getsize = 0;
	CallF( cipher->Decrypt( prop.path, (unsigned char*)newbuf, prop.sourcesize, &getsize ), return 1 );

	if( getsize != prop.sourcesize ){
		_ASSERT( 0 );
		return 1;
	}

	m_xmliobuf.buf = newbuf;
	m_xmliobuf.pos = 0;
	m_xmliobuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, "</XMLIO>" );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = prop.sourcesize;
	}
	m_xmliobuf.bufleng = validleng;


	return 0;
}

int CXMLIO::SetBuffer()
{
	DWORD sizehigh;
	DWORD bufleng;
	bufleng = GetFileSize( m_hfile, &sizehigh );
	if( bufleng < 0 ){
		_ASSERT( 0 );
		return 1;
	}
	
	if( sizehigh != 0 ){
		_ASSERT( 0 );
		return 1;
	}

	char* newbuf;
	newbuf = (char*)malloc( sizeof( char ) * ( bufleng + 1 ) );
	if( !newbuf ){
		_ASSERT( 0 );
		return 1;
	}
	ZeroMemory( newbuf, sizeof( char ) * ( bufleng + 1 ) );


	DWORD rleng, readleng;
	rleng = bufleng;
	ReadFile( m_hfile, (void*)newbuf, rleng, &readleng, NULL );
	if( rleng != readleng ){
		_ASSERT( 0 );

		free( newbuf );
		return 1;
	}

	m_xmliobuf.buf = newbuf;
	m_xmliobuf.pos = 0;
	m_xmliobuf.isend = 0;


	int validleng;
	char* endptr;
	endptr = strstr( newbuf, "</XMLIO>" );
	if( endptr ){
		validleng = (int)( endptr - newbuf );
	}else{
		validleng = bufleng;
	}
	m_xmliobuf.bufleng = validleng;


	return 0;
}

int CXMLIO::Read_Int( XMLIOBUF* xmliobuf, char* startpat, char* endpat, int* dstint )
{
	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)xmliobuf->bufleng) ){
		_ASSERT( 0 );
		return 1;
	}


	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetInt( dstint, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}
int CXMLIO::Read_Float( XMLIOBUF* xmliobuf, char* startpat, char* endpat, float* dstfloat )
{
	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)xmliobuf->bufleng) ){
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		_ASSERT( 0 );
		return 1;
	}


	int stepnum = 0;
	ret = GetFloat( dstfloat, srcchar, 0, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}
int CXMLIO::Read_Vec3( XMLIOBUF* xmliobuf, char* startpat, char* endpat, D3DXVECTOR3* dstvec )
{
	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)xmliobuf->bufleng) ){
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		_ASSERT( 0 );
		return 1;
	}

	float xval = 0.0f;
	float yval = 0.0f;
	float zval = 0.0f;

	int srcpos = 0;
	int stepnum = 0;
	ret = GetFloat( &xval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &yval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &zval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	dstvec->x = xval;
	dstvec->y = yval;
	dstvec->z = zval;


	return 0;
}
int CXMLIO::Read_Q( XMLIOBUF* xmliobuf, char* startpat, char* endpat, CQuaternion* dstq )
{
	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
		_ASSERT( 0 );
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)xmliobuf->bufleng) ){
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= 256) ){
		_ASSERT( 0 );
		return 1;
	}

	float xval = 0.0f;
	float yval = 0.0f;
	float zval = 0.0f;
	float wval = 0.0f;

	int srcpos = 0;
	int stepnum = 0;
	ret = GetFloat( &xval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &yval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &zval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	srcpos += stepnum;
	stepnum = 0;
	ret = GetFloat( &wval, srcchar, srcpos, srcleng, &stepnum );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}


	dstq->x = xval;
	dstq->y = yval;
	dstq->z = zval;
	dstq->w = wval;

	return 0;
}
int CXMLIO::Read_Str( XMLIOBUF* xmliobuf, char* startpat, char* endpat, char* dststr, int arrayleng )
{
	int ret;
	char* startptr;
	startptr = strstr( xmliobuf->buf + xmliobuf->pos, startpat );
	if( !startptr ){
		return 1;
	}
	char* endptr;
	endptr = strstr( xmliobuf->buf + xmliobuf->pos, endpat );
	if( !endptr || (endptr <= startptr) ){
		_ASSERT( 0 );
		return 1;
	}

	int endpatpos;
	endpatpos = (int)( endptr - xmliobuf->buf );
	if( (endpatpos <= 0) || (endpatpos >= (int)xmliobuf->bufleng) ){
		_ASSERT( 0 );
		return 1;
	}

	char* srcchar = startptr + (int)strlen( startpat );
	int srcleng = (int)( endptr - srcchar );
	if( (srcleng <= 0) || (srcleng >= arrayleng) ){
		_ASSERT( 0 );
		return 1;
	}

	ret = GetName( dststr, arrayleng, srcchar, 0, srcleng );
	if( ret ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CXMLIO::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );

	strncpy_s( tempchar, 256, srcchar + pos, srcleng );

	*dstint = atoi( tempchar );

	*stepnum = srcleng;


	return 0;
}
int CXMLIO::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	char* startptr = 0;
	char* endptr = 0;
	startptr = srcchar + pos;
	int poscnt = 0;
	while( *startptr && ( (isspace( *startptr ) != 0) || (*startptr == ',') ) && (poscnt < srcleng) ){
		startptr++;
		poscnt++;
	}

	endptr = startptr;
	while( *endptr && ( ((*endptr >= '0') && (*endptr <= '9')) || (*endptr == '.') || (*endptr == '-')) && (poscnt < srcleng) ){
		endptr++;
		poscnt++;
	}
	char tempchar[256];
	ZeroMemory( tempchar, sizeof( char ) * 256 );

	int leng;
	leng = endptr - startptr;
	if( leng <= 0 ){
		_ASSERT( 0 );
		return 1;
	}

	strncpy_s( tempchar, 256, startptr, leng );

	*dstfloat = (float)atof( tempchar );

	*stepnum = poscnt;

	return 0;
}
int CXMLIO::GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng )
{
	strncpy_s( dstchar, dstleng, srcchar + pos, srcleng );
	*( dstchar + srcleng ) = 0;

	return 0;
}

int CXMLIO::SetXmlIOBuf( XMLIOBUF* srcbuf, char* startpat, char* endpat, XMLIOBUF* dstbuf )
{
	char* startptr = 0;
	char* endptr = 0;
	startptr = strstr( srcbuf->buf + srcbuf->pos, startpat );
	endptr = strstr( srcbuf->buf + srcbuf->pos, endpat );

	int spatlen = (int)strlen( startpat );
	startptr += spatlen;

	if( !startptr || !endptr ){
		_ASSERT( 0 );
		return 1;
	}

	int epatlen;
	epatlen = (int)strlen( endpat );

	int chkendpos;
	chkendpos = (int)( endptr + epatlen - srcbuf->buf );
	if( (chkendpos >= srcbuf->bufleng) || (endptr < startptr) ){
		_ASSERT( 0 );
		return 1;
	}

	srcbuf->pos = chkendpos;

	dstbuf->buf = startptr;
	dstbuf->bufleng = (int)( endptr - startptr );
	dstbuf->pos = 0;
	dstbuf->isend = 0;

	return 0;
}
