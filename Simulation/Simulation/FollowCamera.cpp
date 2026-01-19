#include "pch.h"
#include "FollowCamera.h"

#include "Transform.h"

FollowCamera::FollowCamera(LPDIRECT3DDEVICE9 pGraphicDev, const Vec3* pEye, const Vec3* pAt, const Vec3* pUp,
                           float fFov, float fAspect, float fNear, float fFar, Object* pTarget)
		: Camera(pGraphicDev, pEye, pAt, pUp, fFov, fAspect, fNear, fFar), m_pTarget(pTarget)
{
}

FollowCamera::~FollowCamera()
{
}

HRESULT FollowCamera::Ready_GameObject()
{
    if (FAILED(Camera::Ready_GameObject()))
        return E_FAIL;

	m_vOffset = { 0.f, 4.f, -15.f };

	return Camera::Ready_GameObject();
}

int FollowCamera::Update_GameObject(const float& fTimeDelta)
{
    Camera::Update_GameObject(fTimeDelta);

 	Vec3 vTargetPos = m_pTarget->Get_Transform()->Get_Position();

	Vec3 vNewPos = vTargetPos + m_vOffset;

    vNewPos.y = max(vTargetPos.y, vNewPos.y);

	m_pTransform->Set_Position(vNewPos);
	m_vAt = vTargetPos;

	return 0;
}

void FollowCamera::LateUpdate_GameObject(const float& fTimeDelta)
{
	Camera::LateUpdate_GameObject(fTimeDelta);
}

void FollowCamera::Pitch(const float& fDegree)
{
    float fRad = D3DXToRadian(fDegree);

    // 플레이어(피벗) 위치
    Vec3 pivot = m_pTarget->Get_Transform()->Get_Position();
    float fC = cosf(fRad);
    float fS = sinf(fRad);

    Vec3 vTmp = m_vOffset;

    // x 축 기준 회전 : pitch
    m_vOffset.y = vTmp.y * fC + vTmp.z * fS;
    m_vOffset.z = vTmp.z * fC - vTmp.y * fS;
}

void FollowCamera::Yaw(const float& fDegree)
{
    float fRad = D3DXToRadian(fDegree);

    // 플레이어(피벗) 위치
    Vec3 pivot = m_pTarget->Get_Transform()->Get_Position();
	float fC = cosf(fRad);
	float fS = sinf(fRad);

	Vec3 vTmp = m_vOffset;

    // y 축 기준 회전 : yaw
	m_vOffset.x = vTmp.x * fC - vTmp.z * fS;
	m_vOffset.z = vTmp.x * fS + vTmp.z * fC;
}

void FollowCamera::Compute_ViewMatrix()
{
	// Eye
	m_vEye = m_pTransform->Get_Position();

	// At
	Vec3 vLook;
    m_pTransform->Get_Info(AXIS_Z, &vLook);
	D3DXVec3Normalize(&vLook, &vLook);
}

FollowCamera* FollowCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const Vec3* pEye, const Vec3* pAt, const Vec3* pUp,
	float fFov, float fAspect, float fNear, float fFar, Object* pTarget)
{
	FollowCamera* pInstance = new FollowCamera(pGraphicDev, pEye, pAt, pUp, fFov, fAspect, fNear, fFar, pTarget);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void FollowCamera::Release()
{
	Camera::Release();
}
