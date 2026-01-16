#pragma once
#include "Object.h"

class Anchor;
class Camera;
class Transform;
class Rigidbody;
class SphereCollider;
class Sphere;
class SpringJoint;

class Player : public Object
{
private :
	Player(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Player() override;

public :
	HRESULT				Ready_GameObject();
	int					Update_GameObject(const float& fTimeDelta);
	void				LateUpdate_GameObject(const float& fTimeDelta);
	void				Render_GameObject();

	void				On_CollisionEnter(const COLLISION& tCollision) override;
	void				On_CollisionStay(const COLLISION& tCollision) override;
	void				On_CollisionExit(const COLLISION& tCollision) override;

private :
	void				Handle_PlayerInput(const float& fTimeDelta);

private :
	Camera*				m_pCamera;

	Transform*			m_pTransform;
	Rigidbody*			m_pRigidbody;
	SphereCollider*		m_pCollider;
	Sphere*				m_pMesh;
	SpringJoint*		m_pSpringJoint;

	bool				m_bSwing{};
	Vec3				m_vAnchor;
	Anchor* m_pAnchor{};

public :
	static Player* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release() override;
};

