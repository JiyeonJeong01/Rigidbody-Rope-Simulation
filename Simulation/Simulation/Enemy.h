#pragma once
#include "Object.h"

class Transform;
class Rigidbody;
class Sphere;
class SphereCollider;

class Enemy : public Object
{
private:
	Enemy(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Enemy() override;

public:
	HRESULT			Ready_GameObject();
	int				Update_GameObject(const float& fTimeDelta);
	void			LateUpdate_GameObject(const float& fTimeDelta);
	void			Render_GameObject();

	void			On_CollisionEnter(const COLLISION_DESC& tCollision) override;
	void			On_CollisionStay(const COLLISION_DESC& tCollision) override;
	void			On_CollisionExit(const COLLISION_DESC& tCollision) override;

private:
	Rigidbody*		m_pRigidbody;
	Sphere*			m_pMesh;
	SphereCollider* m_pCollider;

public:
	static Enemy* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release() override;
};

