#pragma once
#include "Object.h"
#include "Transform.h"

class Transform;
class PlaneCollider;
class Plane;
class Rigidbody;

class Ground : public Object
{
private:
	Ground(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Ground() override;

public:
	HRESULT			Ready_GameObject(unsigned long dwColor, float fSizeX, float fSizeY);
	int				Update_GameObject(const float& fTimeDelta);
	void			LateUpdate_GameObject(const float& fTimeDelta);
	void			Render_GameObject();

    void            Active_Highlight(bool bColor);

	void			On_CollisionEnter(const COLLISION& tCollision) override;
	void			On_CollisionStay(const COLLISION& tCollision) override;
	void			On_CollisionExit(const COLLISION& tCollision) override;

private:
	PlaneCollider*	m_pCollider;
	Plane*			m_pMesh;
	Rigidbody*		m_pRigidbody;

public:
	static Ground* Create(LPDIRECT3DDEVICE9 pGraphicDevice, unsigned long dwColor, float fSizeX, float fSizeY);
	void Release() override;
};
