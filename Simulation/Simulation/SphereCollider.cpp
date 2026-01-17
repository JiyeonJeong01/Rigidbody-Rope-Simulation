#include "pch.h"
#include "SphereCollider.h"
#include "Object.h"
#include "CollisionDetector.h"
#include "Rigidbody.h"
#include "VectorHelper.h"

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
