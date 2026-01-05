#include "pch.h"
#include "Sphere.h"
#include "VIBuffer.h"
#include "Object.h"

Sphere::Sphere(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner)
	: Mesh(pGraphicDevice, pOwner), m_fRadius(1.f)
{
}

Sphere::~Sphere()
{
}

HRESULT Sphere::Ready_Mesh(unsigned long dwColor, float fRadius, int iSlice)
{
	m_dwColor = dwColor;
	m_fRadius = fRadius;
	m_fillMode = D3DFILL_WIREFRAME;

	// Create Mesh
	LPD3DXMESH pMesh = NULL;
	D3DXCreateSphere(m_pGraphicDevice, fRadius, iSlice, iSlice, &pMesh, nullptr);

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

	struct SphereVertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
	};

	SphereVertex* pSrcVertices = reinterpret_cast<SphereVertex*>(pMeshVertices);
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

Sphere* Sphere::Create(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner, unsigned long dwColor, float fRadius, int iSlice)
{
	Sphere* pSphere = new Sphere(pGraphicDevice, pOwner);
	if (FAILED(pSphere->Ready_Mesh(dwColor, fRadius, iSlice)))
	{
		Safe_Delete(pSphere);
	}

	return pSphere;
}