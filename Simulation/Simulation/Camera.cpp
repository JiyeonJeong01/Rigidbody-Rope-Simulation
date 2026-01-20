#include "pch.h"
#include "Camera.h"
#include "Transform.h"

Matrix Camera::matView = {};
Matrix Camera::matProj = {};
Matrix Camera::matInvView = {};

Camera::Camera(LPDIRECT3DDEVICE9 pGraphicDev, const Vec3* pEye, const Vec3* pAt, const Vec3* pUp,
    float fFov, float fAspect, float fNear, float fFar)
    : Object(pGraphicDev),
    m_fFov(fFov), m_fAspect(fAspect), m_fNear(fNear), m_fFar(fFar),
    m_vEye(*pEye), m_vAt(*pAt), m_vUp(*pUp)
{
    D3DXMatrixIdentity(&m_matView);
    D3DXMatrixIdentity(&m_matProj);
}

Camera::~Camera()
{
}

HRESULT Camera::Ready_GameObject()
{
    if (FAILED(Object::Ready_GameObject()))
        return E_FAIL;

    m_pTransform = Transform::Create(m_pGraphicDevice, this);

    // 뷰 변환
    D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
    m_pGraphicDevice->SetTransform(D3DTS_VIEW, &m_matView);

    // 투영 변환
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
    m_pGraphicDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);

    return S_OK;
}

int Camera::Update_GameObject(const float& fTimeDelta)
{
    Object::Update_GameObject(fTimeDelta);

    m_pTransform->Update_Component(fTimeDelta);

    return 0;
}

void Camera::LateUpdate_GameObject(const float& fTimeDelta)
{
    Object::LateUpdate_GameObject(fTimeDelta);

    // 뷰 행렬 재설정
    Compute_ViewMatrix();

    D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
    m_pGraphicDevice->SetTransform(D3DTS_VIEW, &m_matView);

    Camera::matView = m_matView;
    Camera::matProj = m_matProj;

    D3DXMatrixInverse(&matInvView, 0, &matView);
}

void Camera::Translate(const Vec3& vDir, const float& fSpeed)
{
    m_pTransform->Translate(vDir * fSpeed);
}

void Camera::Rotate(AXIS eAxis, const float& fAngle)
{
    m_pTransform->Rotate(eAxis, fAngle);
}

void Camera::Set_Position(const Vec3& vPos)
{
    m_pTransform->Set_Position(vPos);
}

const Vec3& Camera::Get_Pos() const
{
    return m_pTransform->Get_Position();
}


void Camera::Compute_ViewMatrix()
{
    // Eye
    m_vEye = m_pTransform->Get_Position();

    // At
    Vec3 vLook;
    m_pTransform->Get_Info(AXIS_Z, &vLook);
    D3DXVec3Normalize(&vLook, &vLook);
    m_vAt = m_vEye + vLook;
}

Camera* Camera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const Vec3* pEye, const Vec3* pAt, const Vec3* pUp, float fFov,
										float fAspect, float fNear, float fFar)
{
    Camera* pInstance = new Camera(pGraphicDev, pEye, pAt, pUp, fFov, fAspect, fNear, fFar);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void Camera::Release()
{
    Object::Release();
}
