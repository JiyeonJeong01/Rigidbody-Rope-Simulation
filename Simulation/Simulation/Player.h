#pragma once
#include "Object.h"

class Transform;
class Rigidbody;
class SphereCollider;
class Sphere;

class Player : public Object
{
private :
	Player(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Player() override;

public :
	HRESULT			Ready_GameObject();
	int					Update_GameObject(const float& fTimeDelta);
	void					LateUpdate_GameObject(const float& fTimeDelta);
	void					Render_GameObject();

	void					On_CollisionEnter(const Collision& tCollision) override;
	void					On_CollisionStay(const Collision& tCollision) override;
	void					On_CollisionExit(const Collision& tCollision) override;

private :
	void					Handle_PlayerInput(const float& fTimeDelta);

private :
	Transform*									m_pTransform;
	Rigidbody*									m_pRigidbody;
	SphereCollider*							m_pCollider;
	Sphere*										m_pMesh;

public :
	static Player* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release() override;
};

