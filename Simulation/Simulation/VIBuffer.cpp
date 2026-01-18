#include "pch.h"
#include "VIBuffer.h"
#include "Object.h"

VIBuffer::VIBuffer(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner, VIBUFFER_INFO tInfo)
    : Component(pGraphicDev, pOwner)
	, m_tInfo(tInfo)
   , m_pVB(nullptr), m_pIB(nullptr)
{

}

VIBuffer::~VIBuffer()
{
    m_pVB->Release();
    m_pIB->Release();
}

HRESULT VIBuffer::Ready_Component()
{
    // 버텍스 버퍼 생성
    if (FAILED(m_pGraphicDevice->CreateVertexBuffer(m_tInfo.dwVtxCnt * m_tInfo.dwVtxSize,
																						0,
																						m_tInfo.dwFVF,
																						D3DPOOL_MANAGED,
																						&m_pVB,
																						NULL)))
						 																return E_FAIL;

    // 인덱스 버퍼 생성 
    if (FAILED(m_pGraphicDevice->CreateIndexBuffer(m_tInfo.dwIdxSize * m_tInfo.dwTriCnt,
																						0,
																						m_tInfo.IdxFmt,
																						D3DPOOL_MANAGED,
                                                                                                              &m_pIB,
                                                                                        NULL)))
                                                                                        return E_FAIL;
    return S_OK;
}

void VIBuffer::Render_Buffer()
{
    m_pGraphicDevice->SetStreamSource(0, m_pVB, 0, m_tInfo.dwVtxSize);

    m_pGraphicDevice->SetFVF(m_tInfo.dwFVF);
    m_pGraphicDevice->SetIndices(m_pIB);

    m_pGraphicDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_tInfo.dwVtxCnt, 0, m_tInfo.dwTriCnt);
}

VIBuffer* VIBuffer::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwenr, VIBUFFER_INFO tInfo)
{
    VIBuffer* pBuffer = new VIBuffer(pGraphicDev, pOwenr, tInfo);
    if (FAILED(pBuffer->Ready_Component()))
    {
        Safe_Delete(pBuffer);
    }

    pOwenr->Add_Component(L"Buffer", pBuffer);
    return pBuffer;
}

void VIBuffer::Release()
{
    Component::Release();
}
