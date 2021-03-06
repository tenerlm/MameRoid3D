//#include <stdafx.h>

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>

#include <crtdbg.h>

#include <mqomaterial.h>

#include <TexBank.h>

extern CTexBank* g_texbank;


CMQOMaterial::CMQOMaterial()
{
	InitParams();
}
CMQOMaterial::~CMQOMaterial()
{
	DestroyObjs();
}

int CMQOMaterial::SetParams( int srcno, D3DXVECTOR4 srcsceneamb, char* srcchar, int srcleng )
{
	//"肌色" col(1.000 0.792 0.651 1.000) dif(0.490) amb(0.540) emi(0.530) spc(0.020) power(0.00)


//DbgOut( L"check!!!: mqomat : SetParams : %d, %s, %d\r\n", srcno, srcchar, srcleng );

	materialno = srcno;

	sceneamb = srcsceneamb;//!!!!!!!!!!!!!!!!!

	char pat[12][20] = 
	{
		"col(",
		"dif(",
		"amb(",
		"emi(",
		"spc(",
		"power(",
		"tex(",
		"alpha(",
		"bump(",
		"shader(",
		"vcol(",
		"\"" //！！！tex, alpha, bumpよりも後でないとだめ。
	};

	int pos = 0;
	int stepnum;
	int ret;

	while( pos < srcleng ){
		while( (pos < srcleng) &&  
		( (*(srcchar + pos) == ' ') || (*(srcchar + pos) == '\t') )
		){
		pos++;
		}

		int cmp;
		int patno, patleng;

		int isfind = 0;

		for( patno = 0; patno < 12; patno++ ){
			if( isfind == 1 )
				break;

			patleng = (int)strlen( pat[patno] );
			if( srcleng - pos >= patleng ){
				cmp = strncmp( pat[patno], srcchar + pos, patleng );
				if( cmp == 0 ){
					isfind = 1;
					switch( patno ){
					case 0:
						ret = SetCol( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 1:
						ret = SetDif( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 2:
						ret = SetAmb( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 3:
						ret = SetEmi( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 4:
						ret = SetSpc( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 5:
						ret = SetPower( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 6:
						ret = SetTex( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 7:
						ret = SetAlpha( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 8:
						ret = SetBump( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 9:
						ret = SetShader( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 10:
						ret = SetVcolFlag( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					case 11:
						//DbgOut( L"MQOMaterial : SetParams : SetName %d\r\n", pos );
						ret = SetName( srcchar, pos, srcleng, &stepnum );
						if( ret )
							return ret;
						break;
					default:
						_ASSERT( 0 );
						return 1;
						break;
					}

				}
			}
		}

		if( isfind == 1 ){
			pos += stepnum;
		}else{
			pos++;
		}

	}

	ret = ConvParamsTo3F();
	_ASSERT( !ret );


	return 0;
}

int CMQOMaterial::ConvParamsTo3F()
{
	dif4f.w = col.w;
	dif4f.x = col.x * dif;
	dif4f.y = col.y * dif;
	dif4f.z = col.z * dif;

//	amb3f.x = col.x * amb;
//	amb3f.y = col.y * amb;
//	amb3f.z = col.z * amb;

	amb3f.x = sceneamb.x * amb;
	amb3f.y = sceneamb.y * amb;
	amb3f.z = sceneamb.z * amb;

	emi3f.x = col.x * emi;
	emi3f.y = col.y * emi;
	emi3f.z = col.z * emi;

	spc3f.x = col.x * spc;
	spc3f.y = col.y * spc;
	spc3f.z = col.z * spc;

	return 0;
}


int CMQOMaterial::InitParams()
{	
	materialno = -1;
	ZeroMemory ( name, 256 );

	col.w = 1.0f;
	col.x = 1.0f;
	col.y = 1.0f;
	col.z = 1.0f;

	dif = 1.0f;
	amb = 0.25f;
	emi = 0.0f;
	spc = 0.0f;
	power = 0.0f;

	ZeroMemory ( tex, 256 );
	ZeroMemory ( alpha, 256 );
	ZeroMemory ( bump, 256 );

	//next = 0;


	shader = 3;

	vcolflag = 0;

	dif4f.w = 1.0f;
	dif4f.x = 1.0f;
	dif4f.y = 1.0f;
	dif4f.z = 1.0f;

	amb3f.x = 0.25f;
	amb3f.y = 0.25f;
	amb3f.z = 0.25f;

	emi3f.x = 0.0f;
	emi3f.y = 0.0f;
	emi3f.z = 0.0f;

	spc3f.x = 0.0f;
	spc3f.y = 0.0f;
	spc3f.z = 0.0f;

	transparent = 0;
	texrule = 0;//!!!!!!!!!
	blendmode = 0;

	uanime = 0.0f;
	vanime = 0.0f;


	curtexname = 0;

	alphatest = 1;
	alphaval = 8;

	glowmult[0] = 1.0f;
	glowmult[1] = 1.0f;
	glowmult[2] = 1.0f;

	orgalpha = 1.0f;

	m_texid = -1;

	m_convnamenum = 0;
	m_ppconvname = 0;

	return 0;
}

int CMQOMaterial::DestroyObjs()
{
	if( m_convnamenum > 0 ){
		int nameno;
		for( nameno = 0; nameno < m_convnamenum; nameno++ ){
			char* delconv;
			delconv = *( m_ppconvname + nameno );
			if( delconv )
				free( delconv );
		}

		free( m_ppconvname );
		m_ppconvname = 0;
		m_convnamenum = 0;
	}

	return 0;
}

int CMQOMaterial::SetName( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//"肌色"
	int step = 1;

	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"')

// "の後に)またはスペースが続いたときのみ終端とする。
//		!( (*(srcchar + pos + step) == '\"') && ( (*(srcchar + pos + step + 1) == ')' ) || (*(srcchar + pos + step + 1) == ' ' ) ) ) 

	){
		step++;
	}

	if( ((step - 1) < 256) && ((step - 1) > 0) ){
		ZeroMemory( name, sizeof( char ) * 256 );
		strncpy_s( name, 256, srcchar + pos + 1, step - 1 );
		name[step -1] = 0;
	}

//	DbgOut( L"check!!! : mqomat : SetName %s, %d, %d\r\n", srcchar, pos, srcleng );
//	DbgOut( L"check!!! : mqomat : SetName %d, %s\r\n", step, name );


	if( pos + step < srcleng )
		*stepnum = step + 1;
	else
		*stepnum = step;


	return 0;
}
int CMQOMaterial::SetCol( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//col(1.000 0.792 0.651 1.000)
	int ret;
	int step;
	int cnt;
	float dstfloat;
	*stepnum = 4;
	for( cnt = 0; cnt < 4; cnt++ ){		
		ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
		if( ret )
			return ret;

		switch( cnt ){
		case 0:
			col.x = dstfloat;
			break;
		case 1:
			col.y = dstfloat;
			break;
		case 2:
			col.z = dstfloat;
			break;
		case 3:
			col.w = dstfloat;
			orgalpha = dstfloat;
			break;
		default:
			break;
		}

		*stepnum += step;
	}

	(*stepnum)++;//最後の）の分

	return 0;
}


int CMQOMaterial::SetDif( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//dif(0.490) 
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;
	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	dif = dstfloat;

	dif4f.w = col.w;
	dif4f.x = col.x * dif;
	dif4f.y = col.y * dif;
	dif4f.z = col.z * dif;


	*stepnum += step + 1;

	return 0;
}
int CMQOMaterial::SetAmb( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//amb(0.540) 
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	amb = dstfloat;

//	amb3f.x = col.x * amb;
//	amb3f.y = col.y * amb;
//	amb3f.z = col.z * amb;

	amb3f.x = sceneamb.x * amb;
	amb3f.y = sceneamb.y * amb;
	amb3f.z = sceneamb.z * amb;

	*stepnum += step + 1;
	return 0;
}
int CMQOMaterial::SetEmi( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//emi(0.530)
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	emi = dstfloat;

	emi3f.x = col.x * emi;
	emi3f.y = col.y * emi;
	emi3f.z = col.z * emi;


	*stepnum += step + 1;

	return 0;
}
int CMQOMaterial::SetSpc( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//spc(0.020) 
	int step;
	float dstfloat;
	*stepnum = 4;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	spc = dstfloat;

	spc3f.x = col.x * spc;
	spc3f.y = col.y * spc;
	spc3f.z = col.z * spc;

	*stepnum += step + 1;

	return 0;
}
int CMQOMaterial::SetPower( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//power(0.00)
	int step;
	float dstfloat;
	*stepnum = 6;
	int ret;

	ret = GetFloat( &dstfloat, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	power = dstfloat;

	*stepnum += step + 1;

	return 0;
}

int CMQOMaterial::SetShader( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//shader(3)
	int step;
	int dstint;
	*stepnum = 7;
	int ret;

	ret = GetInt( &dstint, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	shader = dstint;

	*stepnum += step + 1;

	return 0;
}

int CMQOMaterial::SetVcolFlag( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//vcol(1)
	int step;
	int dstint;
	*stepnum = 5;
	int ret;

	ret = GetInt( &dstint, srcchar, pos + *stepnum, srcleng, &step ); 
	if( ret )
		return ret;

	vcolflag = dstint;

	*stepnum += step + 1;

	return 0;
}

int CMQOMaterial::SetTex( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//tex("sakana.jpg")
	int step = 5;
	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"') ){
		step++;
	}

	if( (step - 5 < 256) && (step - 5 > 0) ){
		strncpy_s( tex, 256, srcchar + pos + 5, step - 5 );
		tex[step -5] = 0;
	}

	if( pos + step < srcleng )
		*stepnum = step + 2;//　")の分
	else
		*stepnum = step;


	return 0;
}
int CMQOMaterial::SetAlpha( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//alpha("sakana.jpg")
	int step = 7;
	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"') ){
		step++;
	}

	if( (step - 7 < 256) && (step - 7 > 0) ){
		strncpy_s( alpha, 256, srcchar + pos + 7, step - 7 );
		alpha[step -7] = 0;
	}

	if( pos + step < srcleng )
		*stepnum = step + 2;//　")の分
	else
		*stepnum = step;

	return 0;
}
int CMQOMaterial::SetBump( char* srcchar, int pos, int srcleng, int* stepnum )
{
	//bump("sakana.jpg")
	int step = 6;
	while( (pos + step < srcleng) && (*(srcchar + pos + step) != '\"') ){
		step++;
	}

	if( (step - 6 < 256) && (step - 6 > 0) ){
		strncpy_s( bump, 256, srcchar + pos + 6, step - 6 );
		bump[step -6] = 0;
	}

	if( pos + step < srcleng )
		*stepnum = step + 2;//　")の分
	else
		*stepnum = step;

	return 0;
}

int CMQOMaterial::GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int flstart;
	int flend;
	int step = 0;

	while( (pos + step < srcleng) && 
	( (*(srcchar + pos + step) == ' ') || (*(srcchar + pos + step) == '\t') )
	//( *(srcchar + pos + step) ) 
	){
		step++;
	}

	flstart = pos+ step;

	while( (pos + step < srcleng) && 
		( isdigit( *(srcchar + pos + step) ) || (*(srcchar + pos + step) == '.') || (*(srcchar + pos + step) == '-') )
	){
		step++;
	}
	flend = pos + step;

	char tempchar[256];
	if( flend - flstart < 256 ){
		strncpy_s( tempchar, 256, srcchar + flstart, flend - flstart );
		tempchar[flend - flstart] = 0;
		*dstfloat = (float)atof( tempchar );
	}else{
		_ASSERT( 0 );
		*dstfloat = 0.0f;
	}

	*stepnum = step;

	return 0;

}

int CMQOMaterial::GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum )
{
	int flstart;
	int flend;
	int step = 0;

	while( (pos + step < srcleng) && 
	( (*(srcchar + pos + step) == ' ') || (*(srcchar + pos + step) == '\t') )
	//( *(srcchar + pos + step) ) 
	){
		step++;
	}

	flstart = pos+ step;

	while( (pos + step < srcleng) && 
		( isdigit( *(srcchar + pos + step) ) || (*(srcchar + pos + step) == '-') )
	){
		step++;
	}
	flend = pos + step;

	char tempchar[256];
	if( flend - flstart < 256 ){
		strncpy_s( tempchar, 256, srcchar + flstart, flend - flstart );
		tempchar[flend - flstart] = 0;
		*dstint = atoi( tempchar );
	}else{
		_ASSERT( 0 );
		*dstint = 0;
	}

	*stepnum = step;

	return 0;
}



int CMQOMaterial::Dump()
{
	WCHAR wname[256] = {0};
	WCHAR wtex[256] = {0};
	WCHAR walpha[256] = {0};
	WCHAR wbump[256] = {0};

	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, name, 256, wname, 256 );
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, tex, 256, wtex, 256 );
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, alpha, 256, walpha, 256 );
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, bump, 256, wbump, 256 );


	DbgOut( L"MQOMaterial %d : name %s\r\n", materialno, wname );
	DbgOut( L"\tcol : r %f, g %f, b %f, a %f\r\n", col.x, col.y, col.z, col.w );
	DbgOut( L"\tdif %f\r\n", dif );
	DbgOut( L"\tamb %f\r\n", amb );
	DbgOut( L"\temi %f\r\n", emi );
	DbgOut( L"\tspc %f\r\n", spc );
	DbgOut( L"\tspc3f %f, %f, %f\r\n", spc3f.x, spc3f.y, spc3f.z );
	DbgOut( L"\tpower %f\r\n", power );
	DbgOut( L"\ttex %s\r\n", wtex );
	DbgOut( L"\talpha %s\r\n", walpha );
	DbgOut( L"\tbump %s\r\n", wbump );
	DbgOut( L"\r\n" );
	return 0;
}

int CMQOMaterial::IsSame( CMQOMaterial* compmat, int compnameflag, int cmplevel )
{
	//compnameflag == 0 の時は、名前は比較しない。
	/***
	int materialno;
	char name[256];

	ARGBF col;
	float dif;
	float amb;
	float emi;
	float spc;
	float power;
	char tex[256];
	char alpha[256];
	char bump[256];
	***/

	int cmp;

	if( compnameflag ){
		cmp = strcmp( name, compmat->name );
		if( cmp )
			return 0;
	}
	
	if( shader != compmat->shader )
		return 0;

	if( (col.w != compmat->col.w) || (col.x != compmat->col.x) || (col.y != compmat->col.y) || (col.z != compmat->col.z) )
		return 0;

	if( dif != compmat->dif )
		return 0;

	if (amb != compmat->amb )
		return 0;

	if( emi != compmat->emi )
		return 0;

	if( spc != compmat->spc )
		return 0;

	if( power != compmat->power )
		return 0;

	if( *tex && *(compmat->tex) ){
		cmp = strcmp( tex, compmat->tex );
		if( cmp )
			return 0;
	}else{
		if( (*tex != 0) || (*(compmat->tex) != 0) )
			return 0;
	}

	if( *alpha && *(compmat->alpha) ){
		cmp = strcmp( alpha, compmat->alpha );
		if( cmp )
			return 0;
	}else{
		if( (*alpha != 0) || (*(compmat->alpha) != 0) )
			return 0;
	}

	if( *bump && *(compmat->bump) ){
		cmp = strcmp( bump, compmat->bump );
		if( cmp )
			return 0;
	}else{
		if( (*bump != 0) || (*(compmat->bump) != 0) )
			return 0;
	}

	if( cmplevel == 0 ){
		if( (dif4f.w != compmat->dif4f.w) || (dif4f.x != compmat->dif4f.x) || (dif4f.y != compmat->dif4f.y) || (dif4f.z != compmat->dif4f.z) ){
			return 0;
		}

		if( (amb3f.x != compmat->amb3f.x) || (amb3f.y != compmat->amb3f.y) || (amb3f.z != compmat->amb3f.z) ){
			return 0;
		}

		if( (emi3f.x != compmat->emi3f.x) || (emi3f.y != compmat->emi3f.y) || (emi3f.z != compmat->emi3f.z) ){
			return 0;
		}

		if( (spc3f.x != compmat->spc3f.x) || (spc3f.y != compmat->spc3f.y) || (spc3f.z != compmat->spc3f.z) ){
			return 0;
		}
	}


	return 1;
}

int CMQOMaterial::GetColorrefDiffuse( COLORREF* dstdiffuse )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( dif4f.x * 255.0f );
	tempg = (unsigned char)( dif4f.y * 255.0f );
	tempb = (unsigned char)( dif4f.z * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstdiffuse = RGB( tempr, tempg, tempb );

	return 0;
}
int CMQOMaterial::GetColorrefSpecular( COLORREF* dstspecular )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( spc3f.x * 255.0f );
	tempg = (unsigned char)( spc3f.y * 255.0f );
	tempb = (unsigned char)( spc3f.z * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstspecular = RGB( tempr, tempg, tempb );

	return 0;
}
int CMQOMaterial::GetColorrefAmbient( COLORREF* dstambient )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( amb3f.x * 255.0f );
	tempg = (unsigned char)( amb3f.y * 255.0f );
	tempb = (unsigned char)( amb3f.z * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstambient = RGB( tempr, tempg, tempb );

	return 0;
}
int CMQOMaterial::GetColorrefEmissive( COLORREF* dstemissive )
{
	unsigned char tempr, tempg, tempb;
	tempr = (unsigned char)( emi3f.x * 255.0f );
	tempg = (unsigned char)( emi3f.y * 255.0f );
	tempb = (unsigned char)( emi3f.z * 255.0f );

	tempr = max( 0, tempr );
	tempr = min( 255, tempr );
	tempg = max( 0, tempg );
	tempg = min( 255, tempg );
	tempb = max( 0, tempb );
	tempb = min( 255, tempb );

	*dstemissive = RGB( tempr, tempg, tempb );

	return 0;
}

int CMQOMaterial::CreateTexture( CPmCipherDll* cipher, WCHAR* dirname )
{

	WCHAR wname[256] = {0};

	if( tex[0] ){
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, tex, 256, wname, 256 );

		g_texbank->AddTex( cipher, dirname, wname, transparent, D3DPOOL_DEFAULT, 0, &m_texid );
	}

	return 0;
}


int CMQOMaterial::CreateTexture( WCHAR* dirname )
{
	SetCurrentDirectory( dirname );
	
	WCHAR wname[256] = {0};

	if( tex[0] ){
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, tex, 256, wname, 256 );

		g_texbank->AddTex( dirname, wname, transparent, D3DPOOL_DEFAULT, 0, &m_texid );
	}

	return 0;
}

int CMQOMaterial::AddConvName( char** ppname )
{
	*ppname = 0;

	m_convnamenum++;

	m_ppconvname = (char**)realloc( m_ppconvname, sizeof( char* ) * m_convnamenum );
	if( !m_ppconvname ){
		DbgOut( L"mqomaterial : AddConvName : ppconvname alloc error !!!\r\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)strlen( name );

	char* newname;
	newname = (char*)malloc( sizeof( char ) * leng + 10 );
	if( !newname ){
		DbgOut( L"mqomaterial : AddConvName : newname alloc error !!!\r\n" );
		_ASSERT( 0 );
		return 1;
	}

	int addno;
	addno = m_convnamenum - 1;

	if( addno >= 1 ){
		sprintf_s( newname, leng + 10, "%s%02d", name, addno );
	}else{
		strcpy_s( newname, leng + 10, name );
	}


	*( m_ppconvname + m_convnamenum - 1 ) = newname;

	*ppname = newname;

	return 0;
}

CMQOMaterial CMQOMaterial::operator= (const CMQOMaterial &m)
{
	materialno = m.materialno;
	strcpy_s( name, 256, m.name );
	col = m.col;
	dif = m.dif;
	amb = m.amb;
	emi = m.emi;
	spc = m.spc;
	power = m.power;
	texnum = m.texnum;
	strcpy_s( tex, 256, m.tex );
	strcpy_s( alpha, 256, m.alpha );
	strcpy_s( bump, 256, m.bump );

	vcolflag = m.vcolflag;
	shader = m.shader;
	dif4f = m.dif4f;
	amb3f = m.amb3f;
	emi3f = m.emi3f;
	spc3f = m.spc3f;
	sceneamb = m.sceneamb;

	transparent = m.transparent;
	texrule = m.texrule;
	blendmode = m.blendmode;

	uanime = m.uanime;
	vanime = m.vanime;

///
	curtexname = m.curtexname;

	alphatest = m.alphatest;
	alphaval = m.alphaval;

	MoveMemory( glowmult, m.glowmult, sizeof( float ) * 3 );
	orgalpha = m.orgalpha;

	m_texid = m.m_texid;

//	int m_convnamenum;
//	char** m_ppconvname;



	return *this;
}

CMQOMaterial CMQOMaterial::operator* (float srcw) const
{
	CMQOMaterial m;

	m.materialno = m.materialno;
	strcpy_s( m.name, 256, name );
	m.col.x *= srcw;
	m.col.y *= srcw;
	m.col.z *= srcw;
	m.col.w *= srcw;
	m.dif *= srcw;
	m.amb *= srcw;
	m.emi *= srcw;
	m.spc *= srcw;
	m.power *= srcw;
	m.texnum = texnum;
	strcpy_s( m.tex, 256, tex );
	strcpy_s( m.alpha, 256, alpha );
	strcpy_s( m.bump, 256, bump );

	m.dif4f.x *= srcw;
	m.dif4f.y *= srcw;
	m.dif4f.z *= srcw;
	m.dif4f.w *= srcw;
	m.amb3f.x *= srcw;
	m.amb3f.y *= srcw;
	m.amb3f.z *= srcw;
	m.emi3f.x *= srcw;
	m.emi3f.y *= srcw;
	m.emi3f.z *= srcw;
	m.spc3f.x *= srcw;
	m.spc3f.y *= srcw;
	m.spc3f.z *= srcw;
	m.sceneamb.x *= srcw;
	m.sceneamb.y *= srcw;
	m.sceneamb.z *= srcw;
	m.sceneamb.w *= srcw;

	m.transparent = transparent;
	m.texrule = texrule;
	m.blendmode = blendmode;

	m.uanime *= srcw;
	m.vanime *= srcw;

	m.curtexname = curtexname;

	m.alphatest = alphatest;
	m.alphaval = alphaval;

	MoveMemory( m.glowmult, glowmult, sizeof( float ) * 3 );
	m.orgalpha = orgalpha;

	return m;
}

CMQOMaterial CMQOMaterial::operator*= (float srcw)
{
	*this = *this * srcw;
	return *this; 
}
