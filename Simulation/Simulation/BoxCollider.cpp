#include "pch.h"
#include "BoxCollider.h"

#include "PhysicsWorld.h"
#include "Transform.h"
#include "Object.h"

BoxCollider::BoxCollider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
    : Collider(pGraphicDev, pOwner), m_vDimension(1., 1., 1.)
{
}

BoxCollider::~BoxCollider()
{
}

HRESULT BoxCollider::Ready_Component()
{
    return Collider::Ready_Component();
}

int BoxCollider::Update_Component(const float& fTimeDelta)
{
    return Collider::Update_Component(fTimeDelta);
}

void BoxCollider::LateUpdate_Component(const float& fTimeDelta)
{
    Collider::LateUpdate_Component(fTimeDelta);
}

HRESULT BoxCollider::Resolve_Dependency()
{
    return Collider::Resolve_Dependency();
}

void BoxCollider::Set_Dimension(const Vec3& vDim)
{
    m_vDimension = vDim;
}

const Vec3& BoxCollider::Get_WorldAxis(AXIS eAxis)
{
    Vec3 vAxis{0., 0., 0.};
    Get_Transform()->Get_Info(eAxis, &vAxis);

    return vAxis;
}

BoxCollider* BoxCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
{
    BoxCollider* pCol = new BoxCollider(pGraphicDev, pOwner);
    if (FAILED(pCol->Ready_Component()))
    {
        Safe_Delete(pCol);
    }

    PhysicsWorld::GetInstance()->Add_Collider(pCol);
    pOwner->Add_Component(L"BoxCollider", pCol);

    return pCol;
}

void BoxCollider::Release()
{
    Collider::Release();
}
