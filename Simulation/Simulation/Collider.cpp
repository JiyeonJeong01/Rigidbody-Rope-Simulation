#include "pch.h"
#include "Collider.h"
#include "Object.h"
#include "Rigidbody.h"

Collider::Collider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Component(pGraphicDev, pOwner), m_pRigidbody(nullptr), m_bOnCol(false), m_vOffset{}, m_fScale{1.f}, m_eColType(C_STATIC)
{
}

Collider::~Collider()
{
}

void Collider::Update_AABB()
{

}

HRESULT Collider::Resolve_Dependency()
{
	Component::Resolve_Dependency();

	Component* pRigidbody = dynamic_cast<Rigidbody*>(m_pOwner->Find_Component(L"Rigidbody"));
	if (pRigidbody == nullptr)
		return E_FAIL;

	m_pRigidbody = static_cast<Rigidbody*>(pRigidbody);
	if (nullptr == m_pRigidbody)
		m_eBodyType = STATIC;

	return S_OK;
}

Rigidbody* Collider::Get_Rigidbody()
{
	if (m_pRigidbody == nullptr)
	{
		m_pRigidbody = dynamic_cast<Rigidbody*>(m_pOwner->Find_Component(L"Rigidbody"));
	}

	return m_pRigidbody;
}

void Collider::Release()
{
	Component::Release();
}
