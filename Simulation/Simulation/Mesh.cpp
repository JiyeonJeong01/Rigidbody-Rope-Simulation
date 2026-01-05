#include "pch.h"
#include "Mesh.h"
#include "VIBuffer.h"
#include "Object.h"

Mesh::Mesh(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner)
	: m_pGraphicDevice(pGraphicDevice), m_pOwner(pOwner)
	, m_pVIBuffer(nullptr), m_dwColor(0)
	, m_bHilight(false), m_dwHilghtColor(D3DCOLOR_ARGB(255, 255, 0, 0))
{
}

Mesh::~Mesh()
{
}

void Mesh::Render_Mesh()
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


void Mesh::Set_FillMode(FILL_MODE eMode)
{
	switch (eMode)
	{
	case SOLID:
		m_fillMode = D3DFILL_SOLID;
		break;
	case WIRE:
		m_fillMode = D3DFILL_WIREFRAME;
		break;
	}
}

void Mesh::Release()
{
	delete this;
}
