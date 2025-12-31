#pragma once

typedef struct tagVIBufferInfo
{
	unsigned long					dwVtxSize;	// 버텍스 구조체 크기
	unsigned long					dwVtxCnt;		// 버텍스 개수
	unsigned long					dwTriCnt;		// 출력하고자하는 삼각형 개수
	unsigned long					dwFVF;		// 버텍스 포맷을 지정하는 상수 값

	D3DFORMAT								IdxFmt;
	unsigned long								dwIdxSize;

	tagVIBufferInfo()
	: dwVtxSize(0), dwVtxCnt(0), dwTriCnt(0), dwFVF(0), IdxFmt(D3DFMT_UNKNOWN), dwIdxSize(0)
	{ }
}VIBUFFER_INFO;

class VIBuffer
{
protected:
	VIBuffer(LPDIRECT3DDEVICE9 pGraphicDev, VIBUFFER_INFO tInfo);
	~VIBuffer();

public:
	HRESULT		Ready_Component();
	void				Render_Buffer();

	LPDIRECT3DVERTEXBUFFER9		Get_VertexBuffer() { return m_pVB; }
	LPDIRECT3DINDEXBUFFER9			Get_IndexBuffer() { return m_pIB; }

protected:
	VIBUFFER_INFO								m_tInfo;

	LPDIRECT3DDEVICE9						m_pGraphicDevice;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9			m_pIB;

public :
	static VIBuffer* Create(LPDIRECT3DDEVICE9 pGraphicDev, VIBUFFER_INFO tInfo);
	void Release();
};

