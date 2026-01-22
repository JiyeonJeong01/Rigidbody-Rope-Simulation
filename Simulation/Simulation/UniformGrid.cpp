#include "pch.h"
#include "UniformGrid.h"

#include "Collider.h"

UniformGrid::UniformGrid(LPDIRECT3DDEVICE9 pGraphic, const Vec3& vWorldMin, const Vec3& vWorldMax, float fCellSize)
    : m_pGraphicDevice(pGraphic)
    , m_vWorldMin(vWorldMin), m_vWorldMax(vWorldMax), m_fCellSize(fCellSize)
{
    m_iGridDimX = max(1, MathHelper::FloorToInt((vWorldMax.x - vWorldMin.x) / fCellSize) + 1);
    m_iGridDimY = max(1, MathHelper::FloorToInt((vWorldMax.y - vWorldMin.y) / fCellSize) + 1);
    m_iGridDimZ = max(1, MathHelper::FloorToInt((vWorldMax.z - vWorldMin.z) / fCellSize) + 1);

    m_StaticColliders.resize((size_t)m_iGridDimX * m_iGridDimY * m_iGridDimZ);
}

UniformGrid::~UniformGrid()
{
}

HRESULT UniformGrid::Ready_System()
{
    return S_OK;
}

void UniformGrid::Update_System()
{
}

void UniformGrid::Find_StaticCollider(const vector<Collider*>& AllColliders)
{

    for (auto* pCol : AllColliders)
    {
        if (pCol->Get_ColType() != C_STATIC)
            continue;

        GRID_COORD coordMin, coordMax;
        Calc_RangeInCell(pCol->Get_AABB(), &coordMin, &coordMax);

        /* Static Collider가 걸쳐진 모든 셀에 자신 등록하기 */
        for (int iZ = coordMin.iZ; iZ <= coordMax.iZ; ++iZ)
            for (int iY = coordMin.iY; iY <= coordMax.iY; ++iY)
                for (int iX = coordMin.iX; iX <= coordMax.iX; ++iX)
                {
                    m_StaticColliders[ToIndex(iX, iY, iZ)].push_back(pCol);
                }
    }
}

void UniformGrid::Calc_RangeInCell(const AABB_DESC& tAABB, GRID_COORD* pOutMin, GRID_COORD* pOutMax) const
{
    auto ToCell = [&](float fPos, float fMin, int iDim) -> int
    {
        int iGrid = static_cast<int>(floorf(fPos - fMin) / m_fCellSize);
        if (iGrid < 0)
            iGrid = 0;
        if (iGrid >= iDim)
            iGrid = iDim - 1;
        return iGrid; /* 0 ~ Dim - 1 사이의 값 반환 */
    };


    pOutMin->iX = ToCell(tAABB.vMin.x, m_vWorldMin.x, m_iGridDimX);
    pOutMin->iY = ToCell(tAABB.vMin.y, m_vWorldMin.y, m_iGridDimY);
    pOutMin->iZ = ToCell(tAABB.vMin.z, m_vWorldMin.z, m_iGridDimZ);

    pOutMax->iX = ToCell(tAABB.vMax.x, m_vWorldMin.x, m_iGridDimX);
    pOutMax->iY = ToCell(tAABB.vMax.y, m_vWorldMin.y, m_iGridDimY);
    pOutMax->iZ = ToCell(tAABB.vMax.z, m_vWorldMin.z, m_iGridDimZ);
}

void UniformGrid::Query_StaticOverlap(const AABB_DESC& tAABB, _Out_ vector<Collider*>* outOverlaps) const
{
    GRID_COORD coordMin, coordMax;
    Calc_RangeInCell(tAABB, &coordMin, &coordMax);

    /* Static Collider가 걸쳐진 모든 셀에 해당 포인터 등록하기 */
    for (int iZ = coordMin.iZ; iZ <= coordMax.iZ; ++iZ)
        for (int iY = coordMin.iY; iY <= coordMax.iY; ++iY)
            for (int iX = coordMin.iX; iX <= coordMax.iX; ++iX)
            {
                /* outOvelaps += curCellCols */
                const auto& cols = m_StaticColliders[ToIndex(iX, iY, iZ)];
                outOverlaps->insert(outOverlaps->end(), cols.begin(), cols.end());
            }
}

size_t UniformGrid::ToIndex(int iX, int iY, int iZ) const
{
                                            /* 3차원 -> 1차원 row-major */
    return iX                               /* x + 1 : 한 칸 증가 */
        + iY * m_iGridDimX                  /* y + 1 : xDim 한 줄 증가 */
        + iZ * m_iGridDimX * m_iGridDimY;   /* z + 1 : x * y 한 면 증가 */

}

UniformGrid* UniformGrid::Create(LPDIRECT3DDEVICE9 pGraphicDevice, const Vec3& vWorldMin, const Vec3& vWorldMax, float fCellSize)
{
    UniformGrid* pInstance = new UniformGrid(pGraphicDevice, vWorldMin, vWorldMax, fCellSize);
    if (FAILED(pInstance->Ready_System()))
    {
        Safe_Release(pInstance);
        pInstance = nullptr;
    }

    return pInstance;
}

void UniformGrid::Release()
{
}
