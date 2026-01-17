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
	return Camera::Ready_GameObject();
}

int FollowCamera::Update_GameObject(const float& fTimeDelta)
{
 	Vec3 vTargetPos = m_pTarget->Get_Transform()->Get_Position();
	vTargetPos.y -= 5;
	vTargetPos.z -= 10;


	return Camera::Update_GameObject(fTimeDelta);
}

void FollowCamera::LateUpdate_GameObject(const float& fTimeDelta)
{
	Camera::LateUpdate_GameObject(fTimeDelta);
}

void FollowCamera::Compute_ViewMatrix()
{
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
