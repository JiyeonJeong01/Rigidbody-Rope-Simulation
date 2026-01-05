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

private :
	void								Find_EquationOfPlane();

public:
	GEOMETRY_TYPE		Get_GeometryType() override			{ return PLANE; }
	bool								Is_OnPlane(const Vec3& vPoint) const;
	float								Calculate_DistToPlane(const Vec3& vPoint) const;

	bool								Get_IsInfinite() const							{ return m_bInfinite; }	
	void								Set_IsInfinite(bool bInfinite)				{ m_bInfinite = bInfinite; }

	const Vec3&				Get_NormVector() const						{ return m_vNorm; }

private:
	bool								m_bInfinite;
	Vec3								m_vNorm;
	Vec3								m_vPoint;
	float								m_fD;

public:
	static PlaneCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	void Release() override;
};

