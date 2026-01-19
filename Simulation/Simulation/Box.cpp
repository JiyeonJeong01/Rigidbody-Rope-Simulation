#include "pch.h"
#include "Box.h"
#include "VIBuffer.h"
#include "Object.h"

Box::Box(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner)
    : Mesh(pGraphicDevice, pOwner), m_vDimension{1.f, 1.f, 1.f}
{
}

Box::~Box()
{
}

HRESULT Box::Ready_Mesh(unsigned long dwColor, const Vec3& vDimension)
{
    m_dwColor = dwColor;
    m_fillMode = D3DFILL_WIREFRAME;

    LPD3DXMESH pMesh = NULL;
    D3DXCreateBox(m_pGraphicDevice, vDimension.x, vDimension.y, vDimension.z, &pMesh, nullptr);

    void* pMeshVertices = nullptr;
    void* pMeshIndices = nullptr;

    pMesh->LockVertexBuffer(D3DLOCK_READONLY, &pMeshVertices);
    pMesh->LockIndexBuffer(D3DLOCK_READONLY, &pMeshIndices);

    UINT	iVtxCnt = pMesh->GetNumVertices();
    UINT	iIdxCnt = pMesh->GetNumFaces() * 3;

    VIBUFFER_INFO tInfo;
    tInfo.dwVtxSize = sizeof(VTXCOL);
    tInfo.dwVtxCnt = iVtxCnt;
    tInfo.dwTriCnt = pMesh->GetNumFaces();
    tInfo.dwFVF = FVF_COL;
    tInfo.IdxFmt = D3DFMT_INDEX16;
    tInfo.dwIdxSize = sizeof(INDEX16);

    m_pVIBuffer = VIBuffer::Create(m_pGraphicDevice, m_pOwner, tInfo);

    struct BoxVertex
    {
        D3DXVECTOR3 position;
        D3DXVECTOR3 normal;
    };

    BoxVertex* pSrcVertices = reinterpret_cast<BoxVertex*>(pMeshVertices);
    VTXCOL* pDstVertices = nullptr;

    m_pVIBuffer->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pDstVertices), 0);
    for (UINT i = 0; i < iVtxCnt; ++i)
    {
        VTXCOL vtx;
        vtx.vPosition = pSrcVertices[i].position;
        vtx.dwColor = m_dwColor;
        pDstVertices[i] = vtx;
    }

    WORD* pSrcIndices = reinterpret_cast<WORD*>(pMeshIndices);
    WORD* pDstIndices = nullptr;

    m_pVIBuffer->Get_IndexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pDstIndices), 0);
    for (UINT i = 0; i < iIdxCnt; ++i)
    {
        pDstIndices[i] = pSrcIndices[i];
    }

    m_pVIBuffer->Get_VertexBuffer()->Unlock();
    m_pVIBuffer->Get_IndexBuffer()->Unlock();
    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();

    return S_OK;
}

Box* Box::Create(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner, unsigned long dwColor, const Vec3& vDimension)
{
    Box* pBox = new Box(pGraphicDevice, pOwner);
    if (FAILED(pBox->Ready_Mesh(dwColor, vDimension)))
    {
        Safe_Delete(pBox);
    }

    return pBox;
}
