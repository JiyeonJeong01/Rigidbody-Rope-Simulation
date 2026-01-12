#include "pch.h"
#include "SpringJoint.h"

#include "DebugHelper.h"
#include "Object.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "VectorHelper.h"

SpringJoint::SpringJoint(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Component(pGraphicDev, pOwner)
	  , m_pRigidbody(nullptr), m_fSpring(0), m_fDamper(0), m_fRestLength(0), m_fMinLength(0), m_fMaxLength(0),
	  m_bActive(false)
{
}

SpringJoint::~SpringJoint()
{
}

HRESULT SpringJoint::Ready_Component()
{
	return Component::Ready_Component();
}

int SpringJoint::Update_Component(const float& fTimeDelta)
{
	if (!m_bActive)
		return 0;

	if (m_pRigidbody == nullptr && !Find_Rigidbody())
		return 0;

	// ===== 현재 상태 =====
	const Vec3 vPos = Get_Transform()->Get_Position();
	const Vec3 vVel = m_pRigidbody->Get_LinearVelocity(m_pRigidbody->Get_COM());

	Vec3 vDir = vPos - m_vAnchor;
	float fDist = VectorHelper::Get_Length(vDir);

	if (fDist <= 0.001f)
		return 0;

	Vec3 vN = vDir / fDist;   // 로프 방향 단위 벡터

	// ===== 로프는 늘어났을 때만 작동 =====
	float fX = fDist - m_fRestLength;
	if (fX <= 0.f)
		return 0;

	// ===== 로프 방향 속도 =====
	float fV = VectorHelper::DotProduct(vVel, vN);

	// ===== Spring + Damper =====
	// F = -k x - c v
	float fForceMag = (-m_fSpring * fX) - (m_fDamper * fV);
	Vec3 vForce = fForceMag * vN;

	// ===== Rigidbody에 힘 적용 =====
	m_pRigidbody->Add_Force(vForce, FORCE_MODE::FORCE);
	// DebugHelper::Print_Vec3(L"Spring", vForce);

	return Component::Update_Component(fTimeDelta);
}

void SpringJoint::LateUpdate_Component(const float& fTimeDelta)
{
	Component::LateUpdate_Component(fTimeDelta);
}

bool SpringJoint::Find_Rigidbody()
{
	if (m_pOwner == nullptr)
		return false;

	Component* pRigidbody = dynamic_cast<Rigidbody*>(m_pOwner->Find_Component(L"Rigidbody"));
	if (pRigidbody == nullptr)
		return false;

	m_pRigidbody = static_cast<Rigidbody*>(pRigidbody);
	return true;
}

SpringJoint* SpringJoint::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
{
	SpringJoint* pSpringJoint = new SpringJoint(pGraphicDev, pOwner);

	if (FAILED(pSpringJoint->Ready_Component()))
	{
		delete pSpringJoint;
		pSpringJoint = nullptr;
	}
	pOwner->Add_Component(L"SpringJoint", pSpringJoint);

	return pSpringJoint;
}

void SpringJoint::Release()
{
	Component::Release();
}
