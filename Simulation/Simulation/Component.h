#pragma once

class Object;
class Transform;

class Component
{
protected:
	Component(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	virtual ~Component();

public :
	virtual HRESULT			Ready_Component();
	virtual int				Update_Component(const float& fTimeDelta);
	virtual void			LateUpdate_Component(const float& fTimeDelta);
	virtual void			Render_Component();
	virtual HRESULT 		Resolve_Dependency() { return S_OK; }

	Transform*				Get_Transform();
	Object*					Get_Object() const { return m_pOwner; }

protected :
	LPDIRECT3DDEVICE9		m_pGraphicDevice;
	Object*					m_pOwner;

public:
	virtual void Release();
};

