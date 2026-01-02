#include "pch.h"
#include "Object.h"

Object::Object(LPDIRECT3DDEVICE9 pGraphicDevice)
	: m_pGraphicDevice(pGraphicDevice)
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
	for (auto& component : m_mapComponents)
	{
		component.second->Update_Component(fTimeDelta);
	}

	return 0;
}

void Object::LateUpdate_GameObject(const float& fTimeDelta)
{
	for (auto& component : m_mapComponents)
	{
		component.second->LateUpdate_Component(fTimeDelta);
	}
}

void Object::Render_GameObject()
{
	for (auto& component : m_mapComponents)
	{
		component.second->Render_Component();
	}
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
