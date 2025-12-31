#pragma once
class VIBuffer;

class Sphere
{
private :
	Sphere(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Sphere();

public :
	HRESULT			Ready_Mesh(unsigned long dwColor, float fRadius, int iSlice);
	void					Render_Mesh();

	void					Set_FillMode(FILL_MODE eMode);
	void					Set_Hilight(bool bOn) { m_bHilight = bOn; }

private :
	LPDIRECT3DDEVICE9		m_pGraphicDevice;
	LPD3DXMESH					m_pMesh;

	VIBuffer*							m_pVIBuffer;

	unsigned long					m_dwColor;
	unsigned long					m_dwHilghtColor;
	float									m_fRadius;

	D3DFILLMODE					m_fillMode;

	bool									m_bHilight;

public :
	static Sphere* Create(LPDIRECT3DDEVICE9 pGraphicDevice, unsigned long dwColor, float fRadius, int iSlice);
	void Release();
};

