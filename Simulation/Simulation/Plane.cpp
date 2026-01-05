#include "pch.h"
#include "Plane.h"
#include "VIBuffer.h"
#include "Object.h"

Plane::Plane(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner)
	: Mesh(pGraphicDevice, pOwner), m_fWidth(1.f), m_fHeight(1.f)
{
}

Plane::~Plane()
{
}

HRESULT Plane::Ready_Mesh(unsigned long dwColor, float fWidth, float fHeight)
{
	m_dwColor = dwColor;
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_fillMode = D3DFILL_WIREFRAME;

	// Create Mesh
	VIBUFFER_INFO tInfo;
	tInfo.dwVtxSize = sizeof(VTXCOL);
	tInfo.dwVtxCnt = 4;
	tInfo.dwTriCnt = 2;
	tInfo.dwFVF = FVF_COL;
	tInfo.IdxFmt = D3DFMT_INDEX16;
	tInfo.dwIdxSize = sizeof(INDEX16);

	m_pVIBuffer = VIBuffer::Create(m_pGraphicDevice, m_pOwner, tInfo);

	VTXCOL* pDstVertices = nullptr;
	INDEX16* pDstIndices = nullptr;

	m_pVIBuffer->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pDstVertices), 0);
	m_pVIBuffer->Get_IndexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pDstIndices), 0);

	Vec2 vMin = { fWidth * -0.5f, fHeight * -0.5f };
	Vec2 vMax = vMin * -1.f;

	VTXCOL tmpVtx;

	// 0: Left-Top
	tmpVtx.vPosition = { vMin.x, vMax.y, 0.f };
	pDstVertices[1] = tmpVtx;

	// 1: Right-Top
	tmpVtx.vPosition = { vMax.x, vMax.y, 0.f };
	pDstVertices[2] = tmpVtx;

	// 2: Right-Bottom
	tmpVtx.vPosition = { vMax.x, vMin.y, 0.f };
	pDstVertices[3] = tmpVtx;

	// 3: Left-Bottom
	tmpVtx.vPosition = { vMin.x, vMin.y, 0.f };
	pDstVertices[0] = tmpVtx;

	for (int i = 0; i < 4; ++i)
		pDstVertices[i].dwColor = m_dwColor;

	// Index-Buffer
	const INDEX16 indices[6] = { 0, 1, 2, 0, 2, 3 };

	for (int i = 0; i < 6; ++i)
		pDstIndices[i] = indices[i];

	m_pVIBuffer->Get_VertexBuffer()->Unlock();
	m_pVIBuffer->Get_IndexBuffer()->Unlock();

	return S_OK;
}

void Plane::Render_Mesh()
{
	// 저장
	DWORD oldFill;
	m_pGraphicDevice->GetRenderState(D3DRS_FILLMODE, &oldFill);

	DWORD oldTFactor;
	m_pGraphicDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &oldTFactor);

	DWORD s0ColorOp, s0ColorArg1;
	m_pGraphicDevice->GetTextureStageState(0, D3DTSS_COLOROP, &s0ColorOp);
	m_pGraphicDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &s0ColorArg1);

	DWORD s1ColorOp, s1AlphaOp;
	m_pGraphicDevice->GetTextureStageState(1, D3DTSS_COLOROP, &s1ColorOp);
	m_pGraphicDevice->GetTextureStageState(1, D3DTSS_ALPHAOP, &s1AlphaOp);

	IDirect3DBaseTexture9* oldTex0 = nullptr;
	m_pGraphicDevice->GetTexture(0, &oldTex0); // AddRef됨

	// 설정
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

	// 복원
	m_pGraphicDevice->SetRenderState(D3DRS_FILLMODE, oldFill);
	m_pGraphicDevice->SetRenderState(D3DRS_TEXTUREFACTOR, oldTFactor);

	m_pGraphicDevice->SetTextureStageState(0, D3DTSS_COLOROP, s0ColorOp);
	m_pGraphicDevice->SetTextureStageState(0, D3DTSS_COLORARG1, s0ColorArg1);
	m_pGraphicDevice->SetTextureStageState(1, D3DTSS_COLOROP, s1ColorOp);
	m_pGraphicDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, s1AlphaOp);

	m_pGraphicDevice->SetTexture(0, oldTex0);
	if (oldTex0) oldTex0->Release(); // GetTexture AddRef 해제
}

Plane* Plane::Create(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner, unsigned long dwColor, float fWidth, float fHeight)
{
	Plane* pPlane = new Plane(pGraphicDevice, pOwner);
	if (FAILED(pPlane->Ready_Mesh(dwColor, fWidth, fHeight)))
	{
		Safe_Delete(pPlane);
	}

	return pPlane;
}