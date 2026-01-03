#include "pch.h"
#include "Collider.h"
#include "Object.h"

Collider::Collider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Component(pGraphicDev, pOwner), m_eColState(), m_bOnCol(false)
{
	ZeroMemory(&m_tCollision, sizeof(Collision));
	ZeroMemory(&m_vOffset, sizeof(Vec3));
}

Collider::~Collider()
{
}

bool Collider::Is_Overlapped(Collider* pCollider)
{
	return (m_usetOverlapCol.find(pCollider) != m_usetOverlapCol.end());
}

void Collider::Add_OverlapCollider(Collider* pCollider)
{
	if (!Is_Overlapped(pCollider))
		m_usetOverlapCol.insert(pCollider);
}

void Collider::Remove_OverlapCollider(Collider* pCollider)
{
	if (Is_Overlapped(pCollider))
		m_usetOverlapCol.erase(pCollider);
}

void Collider::Release()
{
	// TODO : CCollisionManager한테 지워달라고 하기
	m_usetOverlapCol.clear();
	Component::Release();
}
