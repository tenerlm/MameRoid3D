#ifndef XMLIOFILEH
#define XMLIOFILEH

#include <d3dx9.h>
#include <coef.h>

class CQuaternion;
class CPmCipherDll;

class CXMLIO
{
public:
	CXMLIO();
	virtual ~CXMLIO();

protected:
	virtual int InitParams();
	virtual int DestroyObjs();

	int Write2File( char* lpFormat, ... );

	int SetBuffer();
	int SetBuffer( CPmCipherDll* cipher, int dataindex );

	int Read_Int( XMLIOBUF* xmliobuf, char* startpat, char* endpat, int* dstint );
	int Read_Float( XMLIOBUF* xmliobuf, char* startpat, char* endpat, float* dstfloat );
	int Read_Vec3( XMLIOBUF* xmliobuf, char* startpat, char* endpat, D3DXVECTOR3* dstvec );
	int Read_Q( XMLIOBUF* xmliobuf, char* startpat, char* endpat, CQuaternion* dstq );
	int Read_Str( XMLIOBUF* xmliobuf, char* startpat, char* endpat, char* dststr, int arrayleng );

	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );

	int SetXmlIOBuf( XMLIOBUF* srcbuf, char* startpat, char* endpat, XMLIOBUF* dstbuf );
public:
	int m_mode;
	HANDLE m_hfile;
	XMLIOBUF m_xmliobuf;

};

#endif