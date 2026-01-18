#pragma once
#include "Mesh.h"

class Object;
class VIBuffer;

class Plane : public Mesh
{
private:
	Plane(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner);
	~Plane() override;

public:
	HRESULT			Ready_Mesh(unsigned long dwColor, float fWidth, float fHeight);

private:
	float					m_fWidth, m_fHeight;

public:
	static Plane* Create(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner,
		                unsigned long dwColor, float fWidth, float fHeight);
};

