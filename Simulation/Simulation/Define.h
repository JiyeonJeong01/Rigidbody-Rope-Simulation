#pragma once

// 화면 해상도
#define	WINCX		1080
#define	WINCY		720

#define MODE_WIN 1

#define GRAVITY -9.81f;

typedef		D3DXVECTOR2					Vec2;
typedef		D3DXVECTOR3					Vec3;
typedef		D3DXVECTOR4					Vec4;

typedef		D3DXMATRIX						Matrix;

enum COL_STATE { ENTER, STAY, SEPERATE, EXIT, C_END };
enum COL_TYPE { C_STATIC, C_DYNAMIC };
enum BODY_TYPE { STATIC, DYNAMIC, KINEMATIC, CT_END };
enum AXIS { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };
enum FILL_MODE { SOLID, WIRE, RM_END };
enum GEOMETRY_TYPE { SPHERE, BOX, PLANE, G_END };
enum FORCE_MODE { FORCE, IMPULSE, ACCELER, VEL_CHANGE, F_END };

enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

static int g_iCntX = 20;
static int g_iCntZ = 20;
static float g_fSize = 80;
static float g_fStartX = g_iCntX * 0.5f * g_fSize * -1.f;
static float g_fStartZ = 0;
