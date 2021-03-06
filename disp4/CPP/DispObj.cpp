#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <memory.h>

#include <windows.h>
#include <crtdbg.h>

#include <DispObj.h>

#include <mqomaterial.h>

#include <polymesh3.h>
#include <ExtLine.h>

#include <coef.h>

#define DBGH
#include <dbg.h>

#include <TexBank.h>
#include <TexElem.h>

#include <InfBone.h>

#include <MQOObject.h>

#include <map>
#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

extern int g_dbgflag;
extern CTexBank* g_texbank;

extern ID3DXEffect*		g_pEffect;
extern D3DXHANDLE g_hRenderBoneL0;
extern D3DXHANDLE g_hRenderBoneL1;
extern D3DXHANDLE g_hRenderBoneL2;
extern D3DXHANDLE g_hRenderBoneL3;
extern D3DXHANDLE g_hRenderNoBoneL0;
extern D3DXHANDLE g_hRenderNoBoneL1;
extern D3DXHANDLE g_hRenderNoBoneL2;
extern D3DXHANDLE g_hRenderNoBoneL3;
extern D3DXHANDLE g_hRenderLine;

extern D3DXHANDLE g_hdiffuse;
extern D3DXHANDLE g_hambient;
extern D3DXHANDLE g_hspecular;
extern D3DXHANDLE g_hpower;
extern D3DXHANDLE g_hemissive;
extern D3DXHANDLE g_hMeshTexture;

extern int	g_nNumActiveLights;


CDispObj::CDispObj()
{
	InitParams();
}
CDispObj::~CDispObj()
{
	DestroyObjs();
}
int CDispObj::InitParams()
{
	m_hasbone = 0;

	m_pdev = 0;
	m_pm3 = 0;//外部メモリ
	
	m_declbone = 0;
	m_declnobone = 0;
	m_declline = 0;

    m_VB = 0;
	m_InfB = 0;
	m_IB = 0;

	return 0;
}
int CDispObj::DestroyObjs()
{
	if( m_declbone ){
		m_declbone->Release();
		m_declbone = 0;
	}
	if( m_declnobone ){
		m_declnobone->Release();
		m_declnobone = 0;
	}
	if( m_declline ){
		m_declline->Release();
		m_declline = 0;
	}

	if( m_VB ){
		m_VB->Release();
		m_VB = 0;
	}

	if( m_InfB ){
		m_InfB->Release();
		m_InfB = 0;
	}

	if( m_IB ){
		m_IB->Release();
		m_IB = 0;
	}

	return 0;
}

int CDispObj::CreateDispObj( LPDIRECT3DDEVICE9 pdev, CPolyMesh3* pm3, int hasbone )
{
	DestroyObjs();

	m_hasbone = hasbone;

	m_pdev = pdev;
	m_pm3 = pm3;

	CallF( CreateDecl(), return 1 );
	CallF( CreateVBandIB(), return 1 );

	return 0;
}

int CDispObj::CreateDispObj( LPDIRECT3DDEVICE9 pdev, CExtLine* extline )
{
	DestroyObjs();

	m_hasbone = 0;

	m_pdev = pdev;
	m_extline = extline;

	CallF( CreateDecl(), return 1 );
	CallF( CreateVBandIBLine(), return 1 );

	return 0;
}


int CDispObj::CreateDecl()
{
	D3DVERTEXELEMENT9 declbone[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//normal[3]
		{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

		//uv
		{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		//weight[4]
		{ 1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },

		//boneindex[4]
		{ 1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },


		D3DDECL_END()

	};
	D3DVERTEXELEMENT9 declnobone[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		//normal[3]
		{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

		//uv
		{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		D3DDECL_END()

	};
	D3DVERTEXELEMENT9 declline[] = {
		//pos[4]
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

		D3DDECL_END()

	};


	HRESULT hr;
	hr = m_pdev->CreateVertexDeclaration( declbone, &m_declbone );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}
	hr = m_pdev->CreateVertexDeclaration( declnobone, &m_declnobone );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}
	hr = m_pdev->CreateVertexDeclaration( declline, &m_declline );
	if( hr != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

int CDispObj::CopyDispV( CPolyMesh3* pm3 )
{
	m_pm3 = pm3;

	if( !m_VB || !pm3->m_dispv ){
		_ASSERT( 0 );
		return 1;
	}

	HRESULT hr;
	PM3DISPV* pv;
	hr = m_VB->Lock( 0, sizeof( PM3DISPV ) * m_pm3->m_optleng, (void**)&pv, 0 );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}
	memcpy( pv, m_pm3->m_dispv, sizeof( PM3DISPV ) * m_pm3->m_optleng );
	m_VB->Unlock();

	return 0;
}

int CDispObj::CreateVBandIB()
{
	HRESULT hr;

	UINT elemleng, infleng;
	DWORD curFVF;

	elemleng = sizeof( PM3DISPV );
	infleng = sizeof( PM3INF );
	curFVF = 0;

	hr = m_pdev->CreateVertexBuffer( m_pm3->m_optleng * elemleng,
		D3DUSAGE_WRITEONLY, curFVF,
		D3DPOOL_MANAGED,
		//D3DPOOL_DEFAULT,
		&m_VB, NULL );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}
	if( m_hasbone ){
		hr = m_pdev->CreateVertexBuffer( m_pm3->m_optleng * infleng,
			D3DUSAGE_WRITEONLY, curFVF,
			D3DPOOL_MANAGED,
			//D3DPOOL_DEFAULT,
			&m_InfB, NULL );
		if( FAILED(hr) ){
			_ASSERT( 0 );
			return 1;
		}
	}
	hr = m_pdev->CreateIndexBuffer( m_pm3->m_facenum * 3 * sizeof(int),
		0,
		D3DFMT_INDEX32, 
		D3DPOOL_MANAGED,
		&m_IB, NULL );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}

//////////
	PM3DISPV* pv;
	hr = m_VB->Lock( 0, sizeof( PM3DISPV ) * m_pm3->m_optleng, (void**)&pv, 0 );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}
	memcpy( pv, m_pm3->m_dispv, sizeof( PM3DISPV ) * m_pm3->m_optleng );
	m_VB->Unlock();


	if( m_hasbone ){
		PM3INF* pi;
		hr = m_InfB->Lock( 0, sizeof( PM3INF ) * m_pm3->m_optleng, (void**)&pi, 0 );
		if( FAILED(hr) ){
			_ASSERT( 0 );
			return 1;
		}
		int vno;
		for( vno = 0; vno < m_pm3->m_optleng; vno++ ){
			PM3INF* curpm3inf = pi + vno;
			N3P* curn3p = 0;
			curn3p = m_pm3->m_n3p + vno;
			int orgvno = curn3p->pervert->vno;
			_ASSERT( orgvno < m_pm3->m_orgpointnum );

			CInfBone* curib = m_pm3->m_infbone + orgvno;

			int ieno;
			for( ieno = 0; ieno < 4; ieno++ ){
				if( curib->m_infelem[ieno].boneno >= 0 ){
					_ASSERT( curib->m_infelem[ieno].boneno < MAXBONENUM );

					curpm3inf->boneindex[ieno] = (float)( curib->m_infelem[ieno].boneno );
					curpm3inf->weight[ieno] = curib->m_infelem[ieno].dispinf;
					_ASSERT( (curpm3inf->weight[ieno] >= 0.0f) && (curpm3inf->weight[ieno] <= 1.0f) );
				}else{
					curpm3inf->boneindex[ieno] = 0.0f;
					curpm3inf->weight[ieno] = 0.0f;
				}
			}
		}
		m_InfB->Unlock();
	}

	int* pIndices;
	hr = m_IB->Lock( 0, m_pm3->m_facenum * 3 * sizeof(int), (void**)&pIndices, 0 );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}
	memcpy( pIndices, m_pm3->m_dispindex, m_pm3->m_facenum * 3 * sizeof( int ) );
	m_IB->Unlock();

	return 0;
}

int CDispObj::CreateVBandIBLine()
{
	HRESULT hr;

	UINT elemleng;
	DWORD curFVF;

	elemleng = sizeof( EXTLINEV );
	curFVF = 0;

	hr = m_pdev->CreateVertexBuffer( m_extline->m_linenum * 2 * elemleng,
		D3DUSAGE_WRITEONLY, curFVF,
		D3DPOOL_MANAGED,
		//D3DPOOL_DEFAULT,
		&m_VB, NULL );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}
	hr = m_pdev->CreateIndexBuffer( m_extline->m_linenum * 2 * sizeof(int),
		0,
		D3DFMT_INDEX32, 
		D3DPOOL_MANAGED,
		&m_IB, NULL );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}

//////////
	EXTLINEV* lv;
	hr = m_VB->Lock( 0, sizeof( EXTLINEV ) * m_extline->m_linenum * 2, (void**)&lv, 0 );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}
	memcpy( lv, m_extline->m_linev, sizeof( EXTLINEV ) * m_extline->m_linenum * 2 );
	m_VB->Unlock();

	int* pIndices;
	hr = m_IB->Lock( 0, m_extline->m_linenum * 2 * sizeof(int), (void**)&pIndices, 0 );
	if( FAILED(hr) ){
		_ASSERT( 0 );
		return 1;
	}
	int i;
	for( i = 0; i < (m_extline->m_linenum * 2); i++ ){
		*( pIndices + i ) = i;
	}
	m_IB->Unlock();

	return 0;
}



int CDispObj::RenderNormal( int lightflag, D3DXVECTOR4 diffusemult, CMQOObject* morphbaseobj )
{
	if( !m_pm3 ){
		return 0;
	}
	if( m_pm3->m_createoptflag == 0 ){
		return 0;
	}



	HRESULT hr;
	int blno;
	for( blno = 0; blno < m_pm3->m_optmatnum; blno++ ){
		MATERIALBLOCK* currb = m_pm3->m_matblock + blno;

		CMQOMaterial* curmat;
		if( morphbaseobj == 0 ){
			curmat = currb->mqomat;
		}else{
			curmat = morphbaseobj->m_morphmaterial[ currb->materialno ];
		}
		if( !curmat ){
			_ASSERT( 0 );
			return 1;
		}

		D3DXVECTOR4 diffuse;
		diffuse.w = curmat->dif4f.w * diffusemult.w;
		diffuse.x = curmat->dif4f.x * diffusemult.x;
		diffuse.y = curmat->dif4f.y * diffusemult.y;
		diffuse.z = curmat->dif4f.z * diffusemult.z;

		hr = g_pEffect->SetValue( g_hdiffuse, &diffuse, sizeof( D3DXVECTOR4 ) );
		_ASSERT( !hr );
		hr = g_pEffect->SetValue( g_hambient, &(curmat->amb3f), sizeof( D3DXVECTOR3 ) );
		_ASSERT( !hr );
		hr = g_pEffect->SetValue( g_hspecular, &(curmat->spc3f), sizeof( D3DXVECTOR3 ) );
		_ASSERT( !hr );
		hr = g_pEffect->SetFloat( g_hpower, curmat->power );
		_ASSERT( !hr );
		hr = g_pEffect->SetValue( g_hemissive, &(curmat->emi3f), sizeof( D3DXVECTOR3 ) );
		_ASSERT( !hr );


		if( diffuse.w <= 0.99999f ){
			m_pdev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		}else{
			m_pdev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		}


		if( m_hasbone ){
			if( lightflag != 0 ){
				switch( g_nNumActiveLights ){
				case 1:
					hr = g_pEffect->SetTechnique( g_hRenderBoneL1 );
					_ASSERT( !hr );
					break;
				case 2:
					hr = g_pEffect->SetTechnique( g_hRenderBoneL2 );
					_ASSERT( !hr );
					break;
				case 3:
					hr = g_pEffect->SetTechnique( g_hRenderBoneL3 );
					_ASSERT( !hr );
					break;
				default:
					_ASSERT( 0 );
					hr = g_pEffect->SetTechnique( g_hRenderBoneL1 );
					_ASSERT( !hr );
					break;
				}
			}else{
				hr = g_pEffect->SetTechnique( g_hRenderBoneL0 );
				_ASSERT( !hr );
			}
		}else{
			if( lightflag != 0 ){
				switch( g_nNumActiveLights ){
				case 1:
					hr = g_pEffect->SetTechnique( g_hRenderNoBoneL1 );
					_ASSERT( !hr );
					break;
				case 2:
					hr = g_pEffect->SetTechnique( g_hRenderNoBoneL2 );
					_ASSERT( !hr );
					break;
				case 3:
					hr = g_pEffect->SetTechnique( g_hRenderNoBoneL3 );
					_ASSERT( !hr );
					break;
				default:
					_ASSERT( 0 );
					hr = g_pEffect->SetTechnique( g_hRenderNoBoneL1 );
					_ASSERT( !hr );
					break;
				}
			}else{
				hr = g_pEffect->SetTechnique( g_hRenderNoBoneL0 );
				_ASSERT( !hr );
			}
		}

		LPDIRECT3DTEXTURE9 disptex = 0;
		if( curmat->m_texid >= 0 ){
			map<int,CTexElem*>::iterator finditr;
			finditr = g_texbank->m_texmap.find( curmat->m_texid );
			if( finditr != g_texbank->m_texmap.end() ){
				CTexElem* curte;
				curte = finditr->second;
				if( curte ){
					disptex = curte->m_ptex;
				}
			}
		}else{
			disptex = 0;
		}

		int passno;
		if( disptex ){
			passno = 0;
		}else{
			passno = 1;
		}

		hr = g_pEffect->SetTexture( g_hMeshTexture, disptex );
		_ASSERT( !hr );
		
	/////////////
		HRESULT hres;
		UINT cPasses;
		hres = g_pEffect->Begin(&cPasses, 0);
		if( hres != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->BeginPass( passno );
		if( hres != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}
		int curnumprim;
		curnumprim = currb->endface - currb->startface + 1;
		if( m_hasbone ){
			m_pdev->SetVertexDeclaration( m_declbone );
			m_pdev->SetStreamSource( 0, m_VB, 0, sizeof(PM3DISPV) );
			m_pdev->SetStreamSource( 1, m_InfB, 0, sizeof(PM3INF) );
			m_pdev->SetIndices( m_IB );
		}else{
			m_pdev->SetVertexDeclaration( m_declnobone );
			m_pdev->SetStreamSource( 0, m_VB, 0, sizeof(PM3DISPV) );
			m_pdev->SetIndices( m_IB );
		}

		hres = m_pdev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
											0,
											0,
											m_pm3->m_optleng,
											currb->startface * 3,
											curnumprim
											);

		hres = g_pEffect->EndPass();
		if( hres != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}

		hres = g_pEffect->End();
		if( hres != D3D_OK ){
			_ASSERT( 0 );
			return 1;
		}
	}

	return 0;
}

int CDispObj::RenderLine( D3DXVECTOR4 diffusemult )
{
	if( !m_extline ){
		return 0;
	}
	if( m_extline->m_linenum <= 0 ){
		return 0;
	}

	HRESULT hr;

	D3DXVECTOR4 diffuse;
	diffuse.w = m_extline->m_color.w * diffusemult.w;
	diffuse.x = m_extline->m_color.x * diffusemult.x;
	diffuse.y = m_extline->m_color.y * diffusemult.y;
	diffuse.z = m_extline->m_color.z * diffusemult.z;

	hr = g_pEffect->SetValue( g_hdiffuse, &diffuse, sizeof( D3DXVECTOR4 ) );
	_ASSERT( !hr );

	hr = g_pEffect->SetTechnique( g_hRenderLine );
	_ASSERT( !hr );
		
	/////////////
	HRESULT hres;
	UINT cPasses;
	hres = g_pEffect->Begin(&cPasses, 0);
	if( hres != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	hres = g_pEffect->BeginPass( 0 );
	if( hres != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}
	int curnumprim;
	curnumprim = m_extline->m_linenum;
	m_pdev->SetVertexDeclaration( m_declline );
	m_pdev->SetStreamSource( 0, m_VB, 0, sizeof(EXTLINEV) );
	m_pdev->SetIndices( m_IB );
	hres = m_pdev->DrawIndexedPrimitive( D3DPT_LINELIST,
										0,
										0,
										m_extline->m_linenum * 2,
										0,
										curnumprim
										);

	hres = g_pEffect->EndPass();
	if( hres != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	hres = g_pEffect->End();
	if( hres != D3D_OK ){
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



