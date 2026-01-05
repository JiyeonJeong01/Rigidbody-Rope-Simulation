#include "pch.h"
#include "PlaneCollider.h"
#include "Object.h"
#include "CollisionSystem.h"

PlaneCollider::PlaneCollider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Collider(pGraphicDev, pOwner)
	, m_bInfinite(false)
{
}

PlaneCollider::~PlaneCollider()
{
}

HRESULT PlaneCollider::Ready_Component()
{



	return S_OK;
}

int PlaneCollider::Update_Component(const float& fTimeDelta)
{
	return Collider::Update_Component(fTimeDelta);
}

void PlaneCollider::LateUpdate_Component(const float& fTimeDelta)
{
	Collider::LateUpdate_Component(fTimeDelta);
}


PlaneCollider* PlaneCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
{
	PlaneCollider* pCol = new PlaneCollider(pGraphicDev, pOwner);
	if (FAILED(pCol->Ready_Component()))
	{
		Safe_Delete(pCol);
	}

	CollisionSystem::GetInstance()->Add_Collider(pCol);
	pOwner->Add_Component(L"PlaneCollider", pCol);

	return pCol;
}

void PlaneCollider::Release()
{
	Component::Release();
}
