#pragma once
#include "Mesh.h"

class Object;
class VIBuffer;

class Sphere : public Mesh
{
private :
	Sphere(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner);
	~Sphere() override;

public :
	HRESULT		Ready_Mesh(unsigned long dwColor, float fRadius, int iSlice);

private :
	float		m_fRadius;

public :
	static Sphere* Create(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner, unsigned long dwColor, float fRadius, int iSlice);
};
