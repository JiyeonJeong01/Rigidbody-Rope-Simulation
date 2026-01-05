#pragma once
#include "Object.h"

class Transform;
class PlaneCollider;
class Plane;

class Ground : public Object
{
private:
	Ground(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Ground() override;

public:
	HRESULT			Ready_GameObject();
	int					Update_GameObject(const float& fTimeDelta);
	void					LateUpdate_GameObject(const float& fTimeDelta);
	void					Render_GameObject();

	void					On_CollisionEnter(const Collision& tCollision) override;
	void					On_CollisionStay(const Collision& tCollision) override;
	void					On_CollisionExit(const Collision& tCollision) override;

private:
	Transform*			m_pTransform;
	PlaneCollider*	m_pCollider;
	Plane*					m_pMesh;

public:
	static Ground* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release() override;
};

