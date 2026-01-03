#pragma once

// 화면 해상도
#define	WINCX		800
#define	WINCY		600

#define MODE_WIN 1

#define GRAVITY -9.81f;

typedef		D3DXVECTOR2					Vec2;
typedef		D3DXVECTOR3					Vec3;
typedef		D3DXVECTOR4					Vec4;

typedef		D3DXMATRIX						Matrix;

enum COL_STATE { ENTER, STAY, SEPERATE, EXIT, C_END };
enum COLLIDER_TYPE { STATIC, DYNAMIC, KINEMATIC, CT_END };
enum AXIS { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };
enum FILL_MODE { SOLID, WIRE, RM_END };
enum GEOMETRY_TYPE { SPHERE, BOX, G_END };
enum FORCE_MODE { FORCE, IMPULSE, ACCELER, VEL_CHANGE, F_END };