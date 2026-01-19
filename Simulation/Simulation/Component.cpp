#include "pch.h"
#include "Component.h"
#include "Object.h"

Component::Component(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: m_pGraphicDevice(pGraphicDev), m_pOwner(pOwner), m_bActive(true)
{
}

Component::~Component()
{
}

HRESULT Component::Ready_Component()
{
	return S_OK;
}

int Component::Update_Component(const float& fTimeDelta)
{
	return 0;
}

void Component::LateUpdate_Component(const float& fTimeDelta)
{
}

void Component::Fixed_Update(const float& fTimeDelta)
{
}

void Component::Render_Component()
{
}

Transform* Component::Get_Transform()
{
	return m_pOwner->Get_Transform();
}


void Component::Release()
{
	delete this;
}
