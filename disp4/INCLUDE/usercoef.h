#ifndef USERCOEFH
#define USERCOEFH

#ifndef COEFH
#include <d3dx9.h>
#endif

#define PI          3.14159265358979323846f
#define	PAI			3.14159265359
#define	PAIDIV2		1.570796326795
#define	PAIDIV4		0.7853981633974
#define	PAI2DEG		57.29577951308
#define	DEG2PAI		0.01745329251994
#define D2R			0.01745329251994

#define	DEGXZINITVAL	0.0f
#define DEGYINITVAL		0.0f
#define	EYEYINITVAL	750.0f
#define	CAMDISTINITVAL	2900.0f

#define MAXBONENUM	102

enum {
	INDX,
	INDY,
	INDZ,
	INDMAX
};

typedef struct tag_vec3f
{
	float x;
	float y;
	float z;
} VEC3F;


typedef struct tag_dvec3
{
	double x;
	double y;
	double z;
} DVEC3;


enum {
	SKEL_TOPOFJOINT,
	SKEL_TORSO,
	SKEL_LEFT_HIP,
	SKEL_LEFT_KNEE,
	SKEL_LEFT_FOOT,
	SKEL_RIGHT_HIP,
	SKEL_RIGHT_KNEE,
	SKEL_RIGHT_FOOT,
	SKEL_NECK,
	SKEL_HEAD,
	SKEL_LEFT_SHOULDER,
	SKEL_LEFT_ELBOW,
	SKEL_LEFT_HAND,
	SKEL_RIGHT_SHOULDER,
	SKEL_RIGHT_ELBOW,
	SKEL_RIGHT_HAND,
	SKEL_MAX
};


static char strskel[ SKEL_MAX ][30] = {
	"TOPOFJOINT",
	"TORSO",
	"LEFT_HIP",
	"LEFT_KNEE",
	"LEFT_FOOT",
	"RIGHT_HIP",
	"RIGHT_KNEE",
	"RIGHT_FOOT",
	"NECK",
	"HEAD",
	"LEFT_SHOULDER",
	"LEFT_ELBOW",
	"LEFT_HAND",
	"RIGHT_SHOULDER",
	"RIGHT_ELBOW",
	"RIGHT_HAND"
};

static char strconvskel[ SKEL_MAX ][30] = {
	"TOP_OF_JOINT",
	"SKEL_TORSO",
	"SKEL_LEFT_HIP",
	"SKEL_LEFT_KNEE",
	"SKEL_LEFT_FOOT",
	"SKEL_RIGHT_HIP",
	"SKEL_RIGHT_KNEE",
	"SKEL_RIGHT_FOOT",
	"SKEL_NECK",
	"SKEL_HEAD",
	"SKEL_LEFT_SHOULDER",
	"SKEL_LEFT_ELBOW",
	"SKEL_LEFT_HAND",
	"SKEL_RIGHT_SHOULDER",
	"SKEL_RIGHT_ELBOW",
	"SKEL_RIGHT_HAND"
};

typedef struct tag_tpos
{// -1から+1の値
	float ftop;
	float fleft;
	float fbottom;
	float fright;
}TPOS;

enum {
	CAPMODE_ALL,
	CAPMODE_ONE,
	CAPMODE_MAX
};
enum {
	POSMODE_TOPOFJOINT,
	POSMODE_GLOBAL,
	POSMODE_ZERO,
	POSMODE_MAX
};


typedef struct tag_rpselem
{
	int framecnt;
	int skelno;
	float confidence;
	D3DXVECTOR3 pos;
	int twistflag;
}RPSELEM;

typedef struct tag_tselem
{
	int skelno;
	char jointname[256];
	int jointno;
	int twistflag;
}TSELEM;


enum {
	ROTAXIS_X,
	ROTAXIS_Y,
	ROTAXIS_Z,
	ROTAXIS_MAX
};

typedef struct tag_color4uc
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
} COLOR4UC;

typedef struct tag_color3uc
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} COLOR3UC;


typedef struct tag_color4f
{
	float r;
	float g;
	float b;
	float a;
} COLOR4F;

typedef struct RDBColor3f
{
public:
	float r;
	float g;
	float b;
} RDBColor3f;

enum tag_calcmode
{
	CALCMODE_NONE,//未設定
	CALCMODE_NOSKIN0,//スキニングなし
	CALCMODE_ONESKIN0,//距離と内積から計算
	CALCMODE_ONESKIN1,//距離から計算
	CALCMODE_DIRECT0,//値の直接指定
	CALCMODE_SYM,//対称設定
	CALCMODE_MAX
};

enum {
	BONETYPE_RDB2,
	BONETYPE_MIKO,
	BONETYPE_MAX
};

enum {
	SYMAXIS_NONE,
	SYMAXIS_X,
	SYMAXIS_Y,
	SYMAXIS_Z,
	SYMAXIS_MAX
};


#endif

