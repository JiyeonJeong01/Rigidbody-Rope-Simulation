#pragma once
#include "d3dx9.h"

typedef struct tagVertexColor
{
	D3DXVECTOR3	vPosition;
	unsigned long		dwColor;

}VTXCOL;

const unsigned long	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

typedef struct tagIndex16
{
	unsigned short  _0;
	unsigned short  _1;
	unsigned short  _2;

}INDEX16;

typedef struct tagIndex32
{
	unsigned long 	_0;
	unsigned long	 _1;
	unsigned long	 _2;

}INDEX32;


typedef struct tagRectFloat {
	float left;
	float top;
	float right;
	float bottom;
}RECT_F;