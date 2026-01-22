#include "pch.h"
#include "SphereCollider.h"
#include "Object.h"
#include "Rigidbody.h"
#include "Transform.h"

SphereCollider::SphereCollider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Collider(pGraphicDev, pOwner)
	, m_fRadius(1.f)
{
}

SphereCollider::~SphereCollider()
{
}

HRESULT SphereCollider::Ready_Component()
{
	
	return S_OK;
}

int SphereCollider::Update_Component(const float& fTimeDelta)
{
	return Collider::Update_Component(fTimeDelta);
}

void SphereCollider::LateUpdate_Component(const float& fTimeDelta)
{
	Collider::LateUpdate_Component(fTimeDelta);
}

HRESULT SphereCollider::Resolve_Dependency()
{
	if (FAILED(Collider::Resolve_Dependency()))
		return E_FAIL;

    return S_OK;
}

void SphereCollider::Update_AABB()
{
    Collider::Update_AABB();

    const Vec3 vCenter = Get_Transform()->Get_Position();
    const float r = m_fRadius;                           

    m_tAABB.vMin = { vCenter.x - r, vCenter.y - r, vCenter.z - r };
    m_tAABB.vMax = { vCenter.x + r, vCenter.y + r, vCenter.z + r };
}

SphereCollider* SphereCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
{
	SphereCollider* pCol = new SphereCollider(pGraphicDev, pOwner);
	if (FAILED(pCol->Ready_Component()))
	{
		Safe_Delete(pCol);
	}
	pCol->Get_Rigidbody();
	PhysicsWorld::GetInstance()->Add_Collider(pCol);
	pOwner->Add_Component(L"SphereCollider", pCol);

	return pCol;
}

void SphereCollider::Release()
{
	Component::Release();
}
