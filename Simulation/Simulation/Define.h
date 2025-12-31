#pragma once

// 화면 해상도
#define	WINCX		800
#define	WINCY		600

#define MODE_WIN 1

typedef		D3DXVECTOR2					Vec2;
typedef		D3DXVECTOR3					Vec3;
typedef		D3DXVECTOR4					Vec4;

typedef		D3DXMATRIX						Matrix;

enum AXIS { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };
enum FILL_MODE { SOLID, WIRE, RM_END };