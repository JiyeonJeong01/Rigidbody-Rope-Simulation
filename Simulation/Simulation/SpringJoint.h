#pragma once
#include "Component.h"
class Rigidbody;
struct tagSpringInfo;

class SpringJoint :  public Component
{
private:
	SpringJoint(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~SpringJoint() override;

public:
	HRESULT		Ready_Component() override;
	int				Update_Component(const float& fTimeDelta) override;
	void				LateUpdate_Component(const float& fTimeDelta) override;

	bool				Find_Rigidbody();

	void				Set_Active(const float& fActive) { m_bActive = fActive; }
	void				Set_Anchor(const Vec3& vAnchor) { m_vAnchor = vAnchor; }
	void				Set_Spring(const float& fSpring) { m_fSpring = fSpring; }
	void				Set_Damper(const float& fDamper) { m_fDamper = fDamper; }

private:
	Rigidbody*	m_pRigidbody;

	Vec3				m_vAnchor;

	float       m_fSpring;           // k
	float       m_fDamper;           // c

	float       m_fRestLength;       // 목표 길이
	float       m_fMinLength;
	float       m_fMaxLength;

	bool			m_bActive;


public:
	static SpringJoint* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	void Release() override;
};

