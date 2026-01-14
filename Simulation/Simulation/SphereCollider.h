#pragma once
#include "Collider.h"
class SphereCollider : public Collider
{
private :
	SphereCollider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~SphereCollider() override;

public:
	HRESULT						Ready_Component() override;
	int							Update_Component(const float& fTimeDelta) override;
	void						LateUpdate_Component(const float& fTimeDelta) override;

public :
	GEOMETRY_TYPE		Get_GeometryType() override		{ return SPHERE; }

	float								Get_Radius() const							{ return m_fRadius; }
	void								Set_Radius(const float& fRadius)	{ m_fRadius = fRadius; }

private :
	float								m_fRadius;

public:
	static SphereCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	void Release() override;
};

