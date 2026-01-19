#pragma once
#include "Mesh.h"

class Object;
class VIBuffer;

class Box : public Mesh
{
private:
    Box(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner);
    ~Box() override;

public:
    HRESULT		Ready_Mesh(unsigned long dwColor, const Vec3& vDimension);

private:
    Vec3    m_vDimension;

public:
    static Box* Create(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner, unsigned long dwColor, const Vec3& vDimension);
};
