#pragma once
#include "Collider.h"

class BoxCollider : public Collider
{
private:
    BoxCollider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
    ~BoxCollider() override;

public:
    HRESULT			Ready_Component() override;
    int				Update_Component(const float& fTimeDelta) override;
    void			LateUpdate_Component(const float& fTimeDelta) override;
    HRESULT			Resolve_Dependency() override;

public:
    bool			Is_Contacting(const Vec3& vPoint);

    GEOMETRY_TYPE	Get_GeometryType() override { return BOX; }

    const Vec3&     Get_Dimension() const { return m_vDimension; }
    void			Set_Dimension(const Vec3& vDim);

    const Vec3&     Get_WorldAxis(AXIS eAxis);

private:
    Vec3			m_vDimension;

public:
    static BoxCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
    void Release() override;
};
