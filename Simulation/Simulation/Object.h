#pragma once

#include "Collider.h"
#include "Component.h"
class Transform;

class Object
{
protected:
	Object(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual ~Object();

public:
	virtual HRESULT			Ready_GameObject();
	virtual int					Update_GameObject(const float& fTimeDelta);
	virtual void					LateUpdate_GameObject(const float& fTimeDelta);
	virtual void					Render_GameObject();

	Transform*					Get_Transform() const								{ return m_pTransform; }
	void								Set_Transform(Transform* pTransform)	{ m_pTransform = pTransform;}

	void								Add_Component(wstring wsKey, Component* pComponent) { m_mapComponents.insert({ wsKey, pComponent }); }
	Component*				Find_Component(wstring wsKey);

	virtual void					On_CollisionEnter(const Collision& tCollision) {};
	virtual void					On_CollisionStay(const Collision& tCollision) {};
	virtual void					On_CollisionExit(const Collision& tCollision) {};

protected:
	LPDIRECT3DDEVICE9										m_pGraphicDevice;
	unordered_map< wstring, Component*>		m_mapComponents;

	Transform*														m_pTransform;

public:
	virtual void Release();
};

