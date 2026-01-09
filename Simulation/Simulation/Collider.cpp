#include "pch.h"
#include "Collider.h"
#include "Object.h"
#include "Rigidbody.h"

Collider::Collider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Component(pGraphicDev, pOwner), m_pRigidbody(nullptr), m_eColType(STATIC)
	, m_eColState(), m_bOnCol(false)
{
	ZeroMemory(&m_tCollision, sizeof(Collision));
	ZeroMemory(&m_vOffset, sizeof(Vec3));
}

Collider::~Collider()
{
}

bool Collider::Is_Contacted(Collider* pCollider)
{
	return (m_usetContactCols.find(pCollider) != m_usetContactCols.end());
}

void Collider::Add_ContactCollider(Collider* pCollider)
{
	if (!Is_Contacted(pCollider))
		m_usetContactCols.insert(pCollider);
}

void Collider::Remove_ContactCollider(Collider* pCollider)
{
	if (Is_Contacted(pCollider))
		m_usetContactCols.erase(pCollider);
}

void Collider::Add_CheckedCollider(uint_fast16_t iId)
{
	if (Is_CheckedCollider(iId))
		return;

	m_CheckedCollidersList.push_back(iId);
}

bool Collider::Is_CheckedCollider(uint_fast16_t iId)
{
	for (auto id : m_CheckedCollidersList)
		if (id == iId)
			return true;

	return false;
}

Rigidbody* Collider::Find_Rigidbody()
{
	if (m_pOwner == nullptr)
		return nullptr;

	Component* pRigidbody = dynamic_cast<Rigidbody *>( m_pOwner->Find_Component(L"Rigidbody"));
	if (pRigidbody == nullptr)
		return nullptr;

	return m_pRigidbody =  static_cast<Rigidbody*>(pRigidbody);
}

Rigidbody* Collider::Get_Rigidbody()
{
	if (m_pRigidbody == nullptr)
	{
		Find_Rigidbody();

		if (m_pRigidbody == nullptr && m_eColType == DYNAMIC)
		{
			m_eColType = STATIC;
		}
		if (m_pRigidbody)
		{
			m_eColType = DYNAMIC;
		}
	}

	return m_pRigidbody;
}

void Collider::Release()
{
	// TODO : CCollisionManager한테 지워달라고 하기
	m_usetContactCols.clear();
	Component::Release();
}
