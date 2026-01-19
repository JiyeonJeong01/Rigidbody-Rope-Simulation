#pragma once
#include "Camera.h"
class FollowCamera : public Camera
{
private:
    FollowCamera(LPDIRECT3DDEVICE9 pGraphicDev,
        const Vec3* pEye, const Vec3* pAt, const Vec3* pUp,
        float fFov, float fAspect, float fNear, float fFar, Object* pTarget);
    ~FollowCamera() override;

public:
    HRESULT		    Ready_GameObject() override;
    int		        Update_GameObject(const float& fTimeDelta) override;
    void		    LateUpdate_GameObject(const float& fTimeDelta) override;
    void            Yaw(const float& fDegree);
    void            Pitch(const float& fDegree);

private:
    void            Compute_ViewMatrix() override;

	Object*         m_pTarget{};
    Vec3            m_vOffset{};
    float           m_fFixedY{};

public:
    static FollowCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        const Vec3* pEye, const Vec3* pAt, const Vec3* pUp,
        float fFov, float fAspect, float fNear, float fFar, Object* pTarget);

public:
    void    Release() override;
};
