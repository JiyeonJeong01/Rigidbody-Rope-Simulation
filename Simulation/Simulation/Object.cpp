#include "pch.h"
#include "Object.h"

Object::Object(LPDIRECT3DDEVICE9 pGraphicDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pTransform(nullptr), m_bActive(true)
{
}

Object::~Object()
{
}

HRESULT Object::Ready_GameObject()
{
	return S_OK;
}

int Object::Update_GameObject(const float& fTimeDelta)
{
    if (!m_bActive) return 0;

	for (auto& component : m_mapComponents)
	{
		component.second->Update_Component(fTimeDelta);
	}

    for (auto* pChild : m_pChildren)
        pChild->Update_GameObject(fTimeDelta);

	return 0;
}

void Object::LateUpdate_GameObject(const float& fTimeDelta)
{
    if (!m_bActive) return;

	for (auto& component : m_mapComponents)
	{
		component.second->LateUpdate_Component(fTimeDelta);
	}

    for (auto* pChild : m_pChildren)
        pChild->LateUpdate_GameObject(fTimeDelta);
}

void Object::FixedUpdate_GameObject(const float& fTimeDelta)
{
    if (!m_bActive) return;

	for (auto& component : m_mapComponents)
	{
		component.second->Fixed_Update(fTimeDelta);
	}

    for (auto* pChild : m_pChildren)
        pChild->FixedUpdate_GameObject(fTimeDelta);
}

void Object::Render_GameObject()
{
    if (!m_bActive) return;

	for (auto& component : m_mapComponents)
	{
		component.second->Render_Component();
	}

    for (auto* pChild : m_pChildren)
        pChild->Render_GameObject();
}

Component* Object::Find_Component(wstring wsKey)
{
	for (auto& p : m_mapComponents)
	{
		if (p.first.compare(wsKey) == 0)
		{
			return p.second;
		}
	}

	return nullptr;
}

HRESULT Object::Resolve_Dependencies()
{
	for (auto& comp : m_mapComponents)
	{
		if (FAILED(comp.second->Resolve_Dependency()))
			return E_FAIL;
	}

	return S_OK;
}

void Object::Release()
{
	for_each(m_mapComponents.begin(), m_mapComponents.end(),
		[&](auto& p)->void
	{
			if (p.second != nullptr)
			{
				p.second->Release();
				p.second = nullptr;
			}
	});
     
	delete this;
}
