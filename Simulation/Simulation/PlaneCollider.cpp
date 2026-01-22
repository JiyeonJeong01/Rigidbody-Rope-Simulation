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

void PlaneCollider::Update_AABB()
{
    const Vec3 vCenter = Get_Transform()->Get_Position();

    // 월드 공간에서의 노멀 벡터
    Vec3 n = m_vNorm;
    n = VectorHelper::Get_Normalized(n);

    // 무한 평면
    if (m_bInfinite)
    {
        const float INF_EXT = 1e6f;   // 필요하면 더 키우거나 월드 크기로
        const float THICK = 1e-2f;  // 평면 두께(얇은 AABB)

        Vec3 vExt = { INF_EXT, INF_EXT, INF_EXT };

        // 평면은 얇으니까 노멀 방향으로만 얇게 만들고 싶으면 아래처럼 할 수도 있음
        m_tAABB.vMin = vCenter - vExt;
        m_tAABB.vMax = vCenter + vExt;
        return;
    }

    // 평면의 회전을 고려하여 u, v, n이 서로 직교하는 로컬 좌표계 만들기
    Vec3 vRef = Vec3(0, 1, 0);
    if (fabsf(VectorHelper::DotProduct(vRef, n)) > 0.99f)
        vRef = Vec3(1, 0, 0);

    Vec3 vU = VectorHelper::Get_Normalized(VectorHelper::CrossProduct(vRef, n));
    Vec3 vV = VectorHelper::Get_Normalized(VectorHelper::CrossProduct(n, vU));

    const float fHalfW = m_vDimension.x * 0.5f;
    const float fHalfH = m_vDimension.y * 0.5f;

    // 4개 코너 계산
    const Vec3 c0 = vCenter + vU * fHalfW + vV * fHalfH;
    const Vec3 c1 = vCenter + vU * fHalfW - vV * fHalfH;
    const Vec3 c2 = vCenter - vU * fHalfW + vV * fHalfH;
    const Vec3 c3 = vCenter - vU * fHalfW - vV * fHalfH;

    // min/max 뽑기
    Vec3 vMin = c0;
    Vec3 vMax = c0;

    auto Expand = [&](const Vec3& p)
        {
            vMin.x = (p.x < vMin.x) ? p.x : vMin.x;
            vMin.y = (p.y < vMin.y) ? p.y : vMin.y;
            vMin.z = (p.z < vMin.z) ? p.z : vMin.z;

            vMax.x = (p.x > vMax.x) ? p.x : vMax.x;
            vMax.y = (p.y > vMax.y) ? p.y : vMax.y;
            vMax.z = (p.z > vMax.z) ? p.z : vMax.z;
        };

    Expand(c1);
    Expand(c2);
    Expand(c3);

    // 평면은 두께가 0이라 AABB가 너무 얇으면 브로드페이즈에서 누락될 수 있음
    const float thickEps = 1e-2f;

    vMin.x -= thickEps; vMin.y -= thickEps; vMin.z -= thickEps;
    vMax.x += thickEps; vMax.y += thickEps; vMax.z += thickEps;

    m_tAABB.vMin = vMin;
    m_tAABB.vMax = vMax;
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
