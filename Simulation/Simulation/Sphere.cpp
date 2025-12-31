#include "pch.h"
#include "Sphere.h"
#include "VIBuffer.h"

Sphere::Sphere(LPDIRECT3DDEVICE9 pGraphicDevice)
: m_pGraphicDevice(pGraphicDevice)
, m_pMesh(nullptr), m_pVIBuffer(nullptr)
, m_dwColor(0), m_fRadius(0.f)
, m_bHilight(false), m_dwHilghtColor(D3DCOLOR_ARGB(255, 255, 0, 0))
{
}

Sphere::~Sphere()
{
	Safe_Delete(m_pVIBuffer);
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

	m_pVIBuffer = VIBuffer::Create(m_pGraphicDevice, tInfo);

	struct SphereVertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
	};

	SphereVertex* pSrcVertices = reinterpret_cast<SphereVertex*>(pMeshVertices);
	VTXCOL*			pDstVertices = nullptr;

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

void Sphere::Render_Mesh()
{
	m_pGraphicDevice->SetRenderState(D3DRS_FILLMODE, m_fillMode);

	if (m_bHilight)
	{
		m_pGraphicDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwHilghtColor);

		m_pGraphicDevice->SetTexture(0, nullptr);

		m_pGraphicDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_pGraphicDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

		m_pGraphicDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_pGraphicDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}

	m_pVIBuffer->Render_Buffer();
}

void Sphere::Set_FillMode(FILL_MODE eMode)
{
	switch(eMode)
	{
	case SOLID:
		m_fillMode = D3DFILL_SOLID;
		break;
	case WIRE :
		m_fillMode = D3DFILL_WIREFRAME;
		break;
	}
}

Sphere* Sphere::Create(LPDIRECT3DDEVICE9 pGraphicDevice, unsigned long dwColor, float fRadius, int iSlice)
{
	Sphere* pSphere = new Sphere(pGraphicDevice);
	if (FAILED(pSphere->Ready_Mesh(dwColor, fRadius, iSlice)))
	{
		Safe_Delete(pSphere);
	}

	return pSphere;
}

void Sphere::Release()
{
	delete this;
}
