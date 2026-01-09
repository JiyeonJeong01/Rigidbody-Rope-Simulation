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
	m_eColType = STATIC;
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
	//if (m_eColType == DYNAMIC || m_eColType == KINEMATIC)
	//{
	//	Find_EquationOfPlane();
	//}
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
	m_vNorm = pTransform->Get_RotationAxis(AXIS_Z);
	D3DXVec3Normalize(&m_vNorm, &m_vNorm);

	// ax + by + cz + d = 0
	m_fD = -D3DXVec3Dot(&m_vNorm, &m_vPoint);
}

bool PlaneCollider::Is_OnPlane(const Vec3& vPoint)
{
	if (m_bInfinite)
		return fabsf(Calculate_SignedDistToPlane(vPoint)) < 1e-2f;
	
	Vec3 vDiff = Get_Transform()->Get_Position() - vPoint;
	float fDist = D3DXVec3Length(&vDiff);

	// TODO : 아 너무 힘들다 일단은 구로 변경해서 AABB 처리 했는데 RECT 로 바꿔야 함
	return (fDist ) < m_vDimension.x * 0.5f;
}

float PlaneCollider::Calculate_SignedDistToPlane(const Vec3& vPoint) const
{
	// dot(n, (p - p0)) = dot(n, p) + d
	return D3DXVec3Dot(&m_vNorm, &vPoint) + m_fD;
}

Vec3 PlaneCollider::Calculate_PenetrationDirToPlane(const Vec3& vPoint) const
{
	Vec3 vDiff = -m_vNorm * Calculate_SignedDistToPlane(vPoint);
	return VectorHelper::Get_Normalized(vDiff);
}

Vec3 PlaneCollider::Calculate_ResolveDirFromPlane(const Vec3& vPoint) const
{
	float fDist = Calculate_SignedDistToPlane(vPoint); // +면 norm쪽, -면 반대쪽

	const float eps = 1e-6f;
	if (fabsf(fDist) < eps)
		return m_vNorm; // 접촉면 위에 있으면 그냥 노멀 반환

	//  겹침 해소 방향
	return (fDist < 0.f) ? m_vNorm : -m_vNorm;
}

Vec3 PlaneCollider::Project_OnPlane(const Vec3& vPoint) const
{
	return Vec3();
}

const RECT_F& PlaneCollider::Get_Bound()
{
	if (m_eColType != STATIC)
	{
		Vec3 vPos = Get_Transform()->Get_Position();
		Vec2 vHalfDim = m_vDimension * 0.5f;
		m_tBound = {
			vPos.x -= vHalfDim.x,
			vPos.y += vHalfDim.y,
			vPos.x += vHalfDim.x,
			vPos.y -= vHalfDim.y,
		};
	}

	return m_tBound;
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
