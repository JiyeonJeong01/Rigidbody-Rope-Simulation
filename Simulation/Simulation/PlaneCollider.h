#pragma once
#include "Collider.h"

class PlaneCollider : public Collider
{
private:
	PlaneCollider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~PlaneCollider() override;

public:
	HRESULT			Ready_Component() override;
	int				Update_Component(const float& fTimeDelta) override;
	void			LateUpdate_Component(const float& fTimeDelta) override;
	HRESULT			Resolve_Dependency() override;

private :
	void			Calc_EquationOfPlane();

public:
	bool			Is_Contacting(const Vec3& vPoint);

	float			Calculate_SignedDistToPlane(const Vec3& vPoint) const;

	GEOMETRY_TYPE	Get_GeometryType() override			{ return PLANE; }

	bool			Get_IsInfinite() const				{ return m_bInfinite; }	
	void			Set_IsInfinite(bool bInfinite)		{ m_bInfinite = bInfinite; }

	const Vec3&		Get_NormVector() const				{ return m_vNorm; }
	const float&	Get_D() const						{ return m_fD; }

	const Vec2&		Get_Dimension() const				{ return m_vDimension; }
	void			Set_Dimension(const Vec2& vDim); 

private:
	bool			m_bInfinite;
	Vec3			m_vNorm;
	Vec3			m_vPoint;
	float			m_fD;

	Vec2			m_vDimension;
public:
	static PlaneCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	void Release() override;
};
