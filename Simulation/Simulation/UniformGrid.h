#pragma once

#include "Physics_Struct.h"

class UniformGrid
{
private:
    UniformGrid(LPDIRECT3DDEVICE9 pGraphic, const Vec3& vWorldMin, const Vec3& vWorldMax, float fCellSize);
    ~UniformGrid();

public :
    HRESULT Ready_System();
    void    Update_System();

    void    Find_StaticCollider(const vector<Collider*>& AllColliders);
    void    Calc_RangeInCell(const AABB_DESC& tAABB, _Out_ GRID_COORD *pOutMin, _Out_ GRID_COORD* pOutMax) const;
    void    Query_StaticOverlap(const AABB_DESC& tAABB, _Out_ vector<Collider*>* outOverlaps) const;

    const vector<vector<Collider*>>& Get_StaticColliders() { return m_StaticColliders; }

private:
    size_t ToIndex(int iX, int iY, int iZ) const;

private:
    LPDIRECT3DDEVICE9       m_pGraphicDevice;
    Vec3                    m_vWorldMin{}, m_vWorldMax;
    int                     m_iGridDimX{}, m_iGridDimY{}, m_iGridDimZ{};
    float                   m_fCellSize{};
    vector<vector<Collider*>> m_StaticColliders;

public:
    static UniformGrid* Create(LPDIRECT3DDEVICE9 pGraphicDevice, const Vec3& vWorldMin, const Vec3& vWorldMax, float fCellSize);
    void Release();
};



