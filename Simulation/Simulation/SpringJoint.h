#pragma once
#include "Component.h"
#include "Physics_Struct.h"
class Rigidbody;

class SpringJoint :  public Component
{
private:
	SpringJoint(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~SpringJoint() override;

public:
	HRESULT		Ready_Component() override;
	int			Update_Component(const float& fTimeDelta) override;
	void		LateUpdate_Component(const float& fTimeDelta) override;
	void		Fixed_Update(const float& fTimeDelta) override;

	bool		Find_Rigidbody();

	void		Set_Anchor(const Vec3& vAnchor)		{ m_vAnchor = vAnchor; }
	void		Set_Spring(const float& fSpring)	{ m_fSpring = fSpring; }
	void		Set_Damper(const float& fDamper)	{ m_fDamper = fDamper; }
	void		Set_RestLength(float fLen)			{ m_fRestLength = fLen; }

    const Vec3&     Get_Anchor()     const { return m_vAnchor; }
    float           Get_Spring()     const { return m_fSpring; }
    float           Get_Damper()     const { return m_fDamper; }
    float           Get_RestLength() const { return m_fRestLength; }


private:
	Rigidbody*	m_pRigidbody;

	Vec3		m_vAnchor;

	float       m_fSpring;           // k
	float       m_fDamper;           // c

	float       m_fRestLength;       // 목표 길이
	float       m_fMinLength;
	float       m_fMaxLength;

public:
	static SpringJoint* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	void Release() override;
};
