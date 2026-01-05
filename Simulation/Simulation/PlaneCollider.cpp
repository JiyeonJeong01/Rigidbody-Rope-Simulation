#include "pch.h"
#include "PlaneCollider.h"
#include "Object.h"
#include "CollisionSystem.h"
#include "Transform.h"
#include "VectorHelper.h"

PlaneCollider::PlaneCollider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Collider(pGraphicDev, pOwner)
	, m_bInfinite(true)
{
}

PlaneCollider::~PlaneCollider()
{
}

HRESULT PlaneCollider::Ready_Component()
{
	Find_EquationOfPlane();

	return S_OK;
}

int PlaneCollider::Update_Component(const float& fTimeDelta)
{
	if (m_eColType == DYNAMIC || m_eColType == KINEMATIC)
	{
		Find_EquationOfPlane();
	}
	Find_EquationOfPlane();
	return Collider::Update_Component(fTimeDelta);
}

void PlaneCollider::LateUpdate_Component(const float& fTimeDelta)
{
	Collider::LateUpdate_Component(fTimeDelta);
}

void PlaneCollider::Find_EquationOfPlane()
{
	Transform* pTransform = m_pOwner->Get_Transform();
	if (!pTransform) return;

	// 평면 위 점
	m_vPoint = pTransform->Get_Position();

	// 법선 (월드 기준)
	pTransform->Get_Info(AXIS_Z, &m_vNorm);
	D3DXVec3Normalize(&m_vNorm, &m_vNorm);

	// ax + by + cz + d = 0
	m_fD = -D3DXVec3Dot(&m_vNorm, &m_vPoint);
}

bool PlaneCollider::Is_OnPlane(const Vec3& vPoint) const
{
	return fabsf(Calculate_DistToPlane(vPoint)) < 1e-2f;
}

float PlaneCollider::Calculate_DistToPlane(const Vec3& vPoint) const
{
	// dot(n, (p - p0)) = dot(n, p) + d
	return D3DXVec3Dot(&m_vNorm, &vPoint) + m_fD;
}

Vec3 PlaneCollider::Calculate_DirToPlane(const Vec3& vPoint) const
{
	Vec3 vProjectedPoint = vPoint - m_vNorm * Calculate_DistToPlane(vPoint);
	Vec3 vDiff = vProjectedPoint - vPoint;
	return VectorHelper::Get_Normalized(vDiff);
}

PlaneCollider* PlaneCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
{
	PlaneCollider* pCol = new PlaneCollider(pGraphicDev, pOwner);
	if (FAILED(pCol->Ready_Component()))
	{
		Safe_Delete(pCol);
	}

	CollisionSystem::GetInstance()->Add_Collider(pCol);
	pOwner->Add_Component(L"PlaneCollider", pCol);

	return pCol;
}

void PlaneCollider::Release()
{
	Component::Release();
}
