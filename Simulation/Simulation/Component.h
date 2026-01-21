#pragma once

class Object;
class Transform;

class Component abstract
{
protected:
	Component(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	virtual ~Component();

public :
	virtual HRESULT			Ready_Component();
	virtual int				Update_Component(const float& fTimeDelta);
	virtual void			LateUpdate_Component(const float& fTimeDelta);
	virtual void			Fixed_Update(const float& fTimeDelta);
	virtual void			Render_Component();
	virtual HRESULT 		Resolve_Dependency() { return S_OK; }

	Transform*				Get_Transform();
	Object*					Get_Object() const { return m_pOwner; }

    bool                    Get_Active() const { return m_bActive; }
    void                    Set_Active(bool bActive) { m_bActive = bActive; }

protected :
	LPDIRECT3DDEVICE9		m_pGraphicDevice;
	Object*					m_pOwner;
    bool                    m_bActive;
public:
	virtual void Release();
};

