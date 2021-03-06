#ifndef MQOOBJECTH
#define MQOOBJECTH

#include <stdio.h>
#include <stdarg.h>

#include <coef.h>
//#include <basedat.h>

#define DBGH
#include <dbg.h>

#include <d3dx9.h>

#include <string>
#include <map>
using namespace std;


class CMQOFace;
class CMQOMaterial;
class CPolyMesh3;
class CExtLine;
class CDispObj;
class CBone;
class CInfBone;
class CMorphKey;

class CMQOObject
{
public:
	CMQOObject();
	~CMQOObject();

	int SetParams( char* srcchar, int srcleng );
	
	int SetVertex( int* vertnum, char* srcchar, int srcleng );
	int SetPointBuf( int vertno, char* srcchar, int srcleng );
	int SetPointBuf( unsigned char* srcptr, int srcnum );

	int SetFace( int* facenum, char* srcchar, int srcleng );
	int SetFaceBuf( int faceno, char* srcchar, int srcleng, int materialoffset );

	int CreateColor();
	int SetColor( char* srcchar, int srcleng );

	int Dump();

	int MakePolymesh3( LPDIRECT3DDEVICE9 pdev, map<int,CMQOMaterial*>& srcmat );
	int MakeExtLine();
	int MakeDispObj( LPDIRECT3DDEVICE9 pdev, map<int,CMQOMaterial*>& srcmat, int hasbone );
//	int MakeExtLine( map<int,CMQOMaterial*>& srcmat );

	int HasPolygon();
	int HasLine();

	int MultMat( D3DXMATRIX multmat );
	int Shift( D3DXVECTOR3 shiftvec );
	int Multiple( float multiple );
	int MultVertex();

	int InitFaceDirtyFlag();

	int SetMikoBoneIndex3();
	int SetMikoBoneIndex2();
	int CheckSameMikoBone();
	int GetTopLevelMikoBone( CMQOFace** pptopface, int* topnumptr, int maxnum );
	int SetTreeMikoBone( CMQOFace* srctopface );
	int CheckLoopedMikoBoneReq( CMQOFace* faceptr, int* isloopedptr, int* jointnumptr );
	int SetMikoBoneName( map<int, CMQOMaterial*> &srcmaterial );
	int SetMikoFloatBoneName();

	int GetMaterialNoInUse( int* noptr, int arrayleng, int* getnumptr );
	int GetFaceInMaterial( int matno, CMQOFace** ppface, int arrayleng, int* getnumptr );
	int CollisionLocal_Ray( D3DXVECTOR3 startlocal, D3DXVECTOR3 dirlocal );

	int MakeXBoneno2wno( int arrayleng, int* boneno2wno, int* infnumptr );
	int GetSkinMeshHeader( int leng, int* maxpervert, int* maxperface );
	int MakeXBoneInfluence( map<int, CBone*>& bonelist, int arrayleng, int bonenum, int* boneno2wno, BONEINFLUENCE* biptr );

	int UpdateMorph( LPDIRECT3DDEVICE9 pdev, int hasbone, int srcmotid, double srcframe, map<int, CMQOMaterial*>& srcmaterial );
	int DeleteMorphMotion( int srcmotid );
	int DeleteMKOutOfRange( int motid, double srcleng );

	CMorphKey* AddMorphKey( int srcmotid, double srcframe, int* existptr );
	int OrderMorphKey( int srcmotid, double settime, CMorphKey* putmp, int samedel );
	int GetBefNextMK( int srcmotid, double srcframe, CMQOObject* srctarget, CMorphKey** ppbef, CMorphKey** ppnext, int* existptr );
	int SetMorphBlendFU( int srcmotid, double srcframe, CMorphKey* srcmk, CMQOObject* srctarget, float* dstrate );

	int MakeFirstMK( int srcmotid );

	int CalcCurMorphKey( int srcmotid, double srcframe, CMorphKey* dstmkptr );
	int DestroyMorphKey( int srcmotid );

private:
	void InitParams();

	int GetName( char* dstchar, int dstleng, char* srcchar, int pos, int srcleng );
	int GetInt( int* dstint, char* srcchar, int pos, int srcleng, int* stepnum );
	int GetFloat( float* dstfloat, char* srcchar, int pos, int srcleng, int* stepnum );

	int MakeLatheBuf();
	int MakeMirrorBuf();

	int FindConnectFace( int issetface );
	int MakeMirrorPointAndFace( int axis, int doconnect );

	int CheckMirrorDis( D3DXVECTOR3* pbuf, CMQOFace* fbuf, int lno, int pnum );

	int CheckFaceSameChildIndex( CMQOFace* srcface, int chkno, CMQOFace** ppfindface );
	int FindFaceSameParentIndex( CMQOFace* srcface, int chkno, CMQOFace** ppfindface, int* findnum, int maxnum );
	int IsSameMikoBone( CMQOFace* face1, CMQOFace* face2 );
	int IsSameFaceIndex( CMQOFace* face1, CMQOFace* face2 );
	int IsSameFacePos( CMQOFace* face1, CMQOFace* face2, VEC3F* pointptr );

	int CheckMaterialSameName( int srcmatno, map<int, CMQOMaterial*> &srcmaterial, int* nameflag );
	int SetXInfluenceArray( CInfBone* ibptr, int vnum, int boneserino, DWORD* vertices, float* weights, int infnum, DWORD* setnumptr );

	int CalcFrameMK( CMQOObject* srctarget, double srcframe, CMorphKey* befptr, CMorphKey* nextptr, int existflag, CMorphKey* dstmkptr );
	int CalcMorph( CMorphKey* curkey, map<int, CMQOMaterial*>& srcmaterial );
public:
	int m_objectno;
	char m_name[256];
	std::string m_dispname;
	char m_engname[256];

	int m_patch;
	int m_segment;

	int m_visible;
	int m_locking;

	int m_shading;
	float m_facet;
	D3DXVECTOR4 m_color;
	int m_color_type;
	int m_mirror;
	int m_mirror_axis;
	int m_issetmirror_dis;
	float m_mirror_dis;

	int m_lathe;
	int m_lathe_axis;
	int m_lathe_seg;
	int m_vertex;
	//BVertex;
	int m_face;

	D3DXVECTOR3* m_pointbuf;
	CMQOFace* m_facebuf;

	int m_hascolor;
	D3DXVECTOR4* m_colorbuf;

	int m_vertex2;
	int m_face2;
	D3DXVECTOR3* m_pointbuf2;
	CMQOFace* m_facebuf2;
	D3DXVECTOR4* m_colorbuf2;

	int m_connectnum;
	CMQOFace* m_connectface;

	CPolyMesh3* m_pm3;
	CExtLine* m_extline;
	D3DXMATRIX m_multmat;

	CDispObj* m_dispobj;
	CDispObj* m_displine;

	int m_dispflag;

	map<int,CMQOObject*> m_morphobj;
	map<int, CMorphKey*> m_morphkey;

//	vector<D3DXVECTOR3> m_morphpoint;
	map<int, CMQOMaterial*> m_morphmaterial;

	int m_findinfbonecnt;

};

#endif