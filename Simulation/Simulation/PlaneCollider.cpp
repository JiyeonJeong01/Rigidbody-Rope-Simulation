#include "pch.h"
#include "PlaneCollider.h"
#include "Object.h"
#include "CollisionDetector.h"
#include "Transform.h"
#include "VectorHelper.h"

PlaneCollider::PlaneCollider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Collider(pGraphicDev, pOwner)
	, m_bInfinite(true)
{
	m_vDimension = {1.f, 1.f};
}

PlaneCollider::~PlaneCollider()
{
}

HRESULT PlaneCollider::Ready_Component()
{
	Calc_EquationOfPlane();

	return S_OK;
}

int PlaneCollider::Update_Component(const float& fTimeDelta)
{
	Calc_EquationOfPlane();
	return Collider::Update_Component(fTimeDelta);
}

void PlaneCollider::LateUpdate_Component(const float& fTimeDelta)
{
	Collider::LateUpdate_Component(fTimeDelta);
}

HRESULT PlaneCollider::Resolve_Dependency()
{
	if (FAILED(Collider::Resolve_Dependency()))
		return E_FAIL;

	if (m_eBodyType == STATIC)
		Calc_EquationOfPlane();

    return S_OK;
} 

void PlaneCollider::Calc_EquationOfPlane()
{
	Transform* pTransform = m_pOwner->Get_Transform();
	if (!pTransform) return;

	// 평면 위 점
	m_vPoint = pTransform->Get_Position();

	// 법선 벡터
	m_vNorm = pTransform->Get_RotationAxis(AXIS_Z);
	D3DXVec3Normalize(&m_vNorm, &m_vNorm);

	// ax + by + cz + d = 0
	m_fD = -D3DXVec3Dot(&m_vNorm, &m_vPoint);
}

bool PlaneCollider::Is_Contacting(const Vec3& vPoint)
{
	const Vec3 vCenter = Get_Transform()->Get_Position();

	// 월드 공간에서의 노멀 벡터
	Vec3 n = m_vNorm;
	n = VectorHelper::Get_Normalized(n);

	const Vec3 vR = vPoint - vCenter;

	const float fPlaneEps = 1e-2f;
	const float fDist = VectorHelper::DotProduct(n, vR);

	// fDist == 0, 평면 위에 존재한다
	if (fabsf(fDist) > fPlaneEps)
		return false;

	// 무한 평면이라면 vPoint가 범위 안에 있는지 확인하지 않고 return 한다.
	if (m_bInfinite)
		return true;

	// 평면의 회전을 고려하여 u, v, n이 서로 직교하는 로컬 좌표계 만들기 
	// 평행하는 벡터 제외하기 : NaN 방지
	Vec3 vRef = Vec3(0, 1, 0);
	if (fabsf(VectorHelper::DotProduct(vRef, n)) > 0.99f)	// 거의 평행하는 축
		vRef = Vec3(1, 0, 0);

	Vec3 vU = VectorHelper::Get_Normalized(VectorHelper::CrossProduct(vRef, n));
	Vec3 vV = VectorHelper::Get_Normalized(VectorHelper::CrossProduct(n, vU));

	const float fHalfW = m_vDimension.x * 0.5f;
	const float fHalfH = m_vDimension.y * 0.5f;

	// 평면의 로컬 좌표계에서 vPoint가 범위 내에 있는지 확인한다.
	const float fU = VectorHelper::DotProduct(vU, vR);
	const float fV = VectorHelper::DotProduct(vV, vR);

	const float edgeEps = 1e-3f;
	const bool inU = fabsf(fU) <= (fHalfW + edgeEps);
	const bool inV = fabsf(fV) <= (fHalfH + edgeEps);

	return inU && inV;
}

float PlaneCollider::Calculate_SignedDistToPlane(const Vec3& vPoint) const
{
	// dot(n, (p - p0)) = dot(n, p) + d
	// 공간의 점과 평면 간의 최소 거리
	float fDist = D3DXVec3Dot(&m_vNorm, &vPoint) + m_fD;
	return fDist;
}

void PlaneCollider::Set_Dimension(const Vec2& vDim)
{
	m_vDimension = vDim;
	m_bInfinite = false;
}

PlaneCollider* PlaneCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
{
	PlaneCollider* pCol = new PlaneCollider(pGraphicDev, pOwner);
	if (FAILED(pCol->Ready_Component()))
	{
		Safe_Delete(pCol);
	}

	PhysicsWorld::GetInstance()->Add_Collider(pCol);
	pOwner->Add_Component(L"PlaneCollider", pCol);

	return pCol;
}

void PlaneCollider::Release()
{
	Component::Release();
}
