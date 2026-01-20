#include "pch.h"
#include "SpringJoint.h"

#include "DebugHelper.h"
#include "Object.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "VectorHelper.h"

SpringJoint::SpringJoint(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Component(pGraphicDev, pOwner)
	  , m_pRigidbody(nullptr), m_fSpring(0), m_fDamper(0), m_fRestLength(0), m_fMinLength(0), m_fMaxLength(0)
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
	return Component::Update_Component(fTimeDelta);
}

void SpringJoint::LateUpdate_Component(const float& fTimeDelta)
{
	Component::LateUpdate_Component(fTimeDelta);
}

void SpringJoint::Fixed_Update(const float& fTimeDelta)
{
	Component::Fixed_Update(fTimeDelta);

	if (!m_bActive || (m_pRigidbody == nullptr && !Find_Rigidbody()))
		return ;

	BODY* b = PhysicsWorld::GetInstance()->Try_GetBody(m_pRigidbody->Get_BodyID());
	if (!b) return ;

	const Vec3 vPos = Get_Transform()->Get_Position();

	const Vec3 vVel = b->vLinearVel;
	const Vec3 vDir = vPos - m_vAnchor;
	const float fDist = VectorHelper::Get_Length(vDir);

	if (fDist <= 1e-3f)
		return ;

	Vec3 vN = VectorHelper::Get_Normalized(vDir);

	// 로프는 늘어났을 때만 작동
	float fX = fDist - m_fRestLength;
	if (fX <= 0.f)
		return ;

	// 앵커 방향으로 멀어지는지/가까워지는지
	float fV = VectorHelper::DotProduct(vVel, vN);

	// Spring + Damper
	// F = -k x - c v
    // -k x : 늘어난 만큼 자연 길이로 돌아가려는 힘
    // -c v : 현재 움직임을 방해하는 힘 
	float fForceMag = (-m_fSpring * fX) - (m_fDamper * fV);
	Vec3 vForce = fForceMag * vN;

    // 로프 상승 효과 넣기 위하여 임의 조정 
    vForce.y *= 1.8f;

	// Rigidbody에 힘 적용
	 m_pRigidbody->Add_Force(vForce);
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
