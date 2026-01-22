#pragma once

#include "Component.h"
#include "Physics_Struct.h"
class Transform;

class Object
{
protected:
	Object(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual ~Object();

public:
	virtual HRESULT			Ready_GameObject();
	virtual int				Update_GameObject(const float& fTimeDelta);
	virtual void			LateUpdate_GameObject(const float& fTimeDelta);
	virtual void			FixedUpdate_GameObject(const float& fTimeDelta);
	virtual void			Render_GameObject();

	Transform*				Get_Transform() const								{ return m_pTransform; }
	void					Set_Transform(Transform* pTransform)				{ m_pTransform = pTransform;}

    void                    Set_Active(bool bActive)                            { m_bActive = bActive; }
    bool                    Get_Active() const                                  { return m_bActive; }

	void					Add_Component(wstring wsKey, Component* pComponent) { m_mapComponents.insert({ wsKey, pComponent }); }
	Component*				Find_Component(wstring wsKey);

	virtual void			On_CollisionEnter(const COLLISION_DESC& tCollision) {};
	virtual void			On_CollisionStay(const COLLISION_DESC& tCollision) {};
	virtual void			On_CollisionExit(const COLLISION_DESC& tCollision) {};

	virtual HRESULT			Resolve_Dependencies();

protected:
	LPDIRECT3DDEVICE9						m_pGraphicDevice;
	unordered_map< wstring, Component*>		m_mapComponents;

	Transform*								m_pTransform;

    list<Object*>                           m_pChildren;
    bool                                    m_bActive{};

public:
	virtual void Release();
};

