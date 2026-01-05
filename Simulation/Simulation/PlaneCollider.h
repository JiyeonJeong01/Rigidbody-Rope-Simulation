#pragma once
#include "Collider.h"
class PlaneCollider : public Collider
{
private:
	PlaneCollider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~PlaneCollider() override;

public:
	HRESULT						Ready_Component() override;
	int								Update_Component(const float& fTimeDelta) override;
	void								LateUpdate_Component(const float& fTimeDelta) override;

public:
	GEOMETRY_TYPE		Get_GeometryType() override			{ return PLANE; }

	bool								Get_IsInfinite() const							{ return m_bInfinite; }	
	void								Set_IsInfinite(bool bInfinite)				{ m_bInfinite = bInfinite; }

private:
	bool								m_bInfinite;


public:
	static PlaneCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	void Release() override;
};

