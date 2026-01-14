#include "pch.h"
#include "Collider.h"
#include "Object.h"
#include "Rigidbody.h"

Collider::Collider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Component(pGraphicDev, pOwner), m_pRigidbody(nullptr), m_eColType(STATIC)
	, m_eColState(), m_bOnCol(false)
{
	ZeroMemory(&m_tCollision, sizeof(COLLISION));
	ZeroMemory(&m_vOffset, sizeof(Vec3));
}

Collider::~Collider()
{
}

HRESULT Collider::Resolve_Dependency()
{
	Component::Resolve_Dependency();

	Component* pRigidbody = dynamic_cast<Rigidbody*>(m_pOwner->Find_Component(L"Rigidbody"));
	if (pRigidbody == nullptr)
		return E_FAIL;

	m_pRigidbody = static_cast<Rigidbody*>(pRigidbody);


	return S_OK;
}

Rigidbody* Collider::Get_Rigidbody()
{
	if (m_pRigidbody == nullptr)
	{
		if (m_pRigidbody == nullptr && m_eColType == DYNAMIC)
		{
			m_eColType = STATIC;
		}
	}

	return m_pRigidbody;
}

void Collider::Release()
{
	Component::Release();
}
