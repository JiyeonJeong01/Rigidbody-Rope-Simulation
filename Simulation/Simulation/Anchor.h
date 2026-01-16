#pragma once
#include "Object.h"

class Sphere;

class Anchor : public Object
{
private:
	Anchor(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Anchor() override;

public:
	HRESULT	Ready_GameObject() override;
	void	Render_GameObject() override;


private:
	Transform* m_pTransform;
	Sphere* m_pMesh;

public:
	static Anchor* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release() override;
};

