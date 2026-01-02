#include "pch.h"
#include "Component.h"
#include "Object.h"

Component::Component(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: m_pGraphicDevice(pGraphicDev), m_pOwner(pOwner)
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

void Component::Render_Component()
{
}


void Component::Release()
{
	delete this;
}
