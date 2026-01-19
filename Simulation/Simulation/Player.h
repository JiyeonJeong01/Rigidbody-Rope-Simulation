#pragma once
#include "Object.h"

class Anchor;
class Camera;
class FollowCamera;
class Transform;
class Rigidbody;
class SphereCollider;
class Sphere;
class Box;
class BoxCollider;
class SpringJoint;
class Rope;

class Player : public Object
{
private :
	Player(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Player() override;

public :
	HRESULT				Ready_GameObject();
	int					Update_GameObject(const float& fTimeDelta);
	void				LateUpdate_GameObject(const float& fTimeDelta);
	void				FixedUpdate_GameObject(const float& fTimeDElta);
	void				Render_GameObject();

	void				On_CollisionEnter(const COLLISION& tCollision) override;
	void				On_CollisionStay(const COLLISION& tCollision) override;
	void				On_CollisionExit(const COLLISION& tCollision) override;

private :
	void				Handle_PlayerInput(const float& fTimeDelta);
    void                Start_Swing(Vec3 vAnchor);
    void                End_Swing();

private :
    // Components
	Rigidbody*			m_pRigidbody;
	//SphereCollider*		m_pCollider;
	Sphere*		        m_pMainMesh;
    Collider*           m_pCollider;
    Box*				m_pAssistMesh;
	SpringJoint*		m_pSpringJoint;

    // Children
    Anchor* m_pAnchor{};
    FollowCamera* m_pCamera;
    Rope* m_pRope;

    // Variables
	bool				m_bSwing{};
	Vec3				m_vAnchor;
    float               m_fSpring = 10.f;
    float               m_fDamper = 5.f;
    float               m_fRest = 0.5f;

    // Camera
    float               m_fYawDegree{};
    float               m_fPitchDegree{};


public :
	static Player* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release() override;
};

