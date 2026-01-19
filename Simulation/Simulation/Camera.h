#pragma once
#include "Object.h"

class Transform;

class Camera : public Object
{
protected:
    Camera(LPDIRECT3DDEVICE9 pGraphicDev,
                    const Vec3* pEye, const Vec3* pAt, const Vec3* pUp,
                    float fFov, float fAspect, float fNear, float fFar);
    ~Camera() override;

public:
    HRESULT		    Ready_GameObject() override;
    int		        Update_GameObject(const float& fTimeDelta) override;
    void		    LateUpdate_GameObject(const float& fTimeDelta) override;

    void            Translate(const Vec3& vDir, const float& fSpeed);
    void            Rotate(AXIS eAxis, const float& fAngle);

    void            Set_Position(const Vec3& vPos);
    const Vec3&     Get_Pos() const;

protected:
    virtual void    Compute_ViewMatrix();

protected:
    Matrix		    m_matView, m_matProj;
    Vec3		    m_vEye, m_vAt, m_vUp;
    float		    m_fFov, m_fAspect, m_fNear, m_fFar;

public:
    static Camera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        const Vec3* pEye, const Vec3* pAt, const Vec3* pUp,
        float fFov, float fAspect, float fNear, float fFar);

public:
    void    Release() override;
};
