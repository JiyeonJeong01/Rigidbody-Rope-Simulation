#pragma once

#include "Component.h"

class Object
{
protected:
	Object(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Object();

public:
	virtual HRESULT			Ready_GameObject();
	virtual int					Update_GameObject(const float& fTimeDelta);
	virtual void					LateUpdate_GameObject(const float& fTimeDelta);
	virtual void					Render_GameObject();

	void								Add_Component(wstring wsKey, Component* pComponent) { m_mapComponents.insert({ wsKey, pComponent }); }

protected:
	LPDIRECT3DDEVICE9										m_pGraphicDevice;
	unordered_map< wstring, Component*>		m_mapComponents;

public:
	virtual void Release();
};

