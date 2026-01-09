#pragma once

class VIBuffer;
class Object;

class Mesh
{
protected:
	Mesh(LPDIRECT3DDEVICE9 pGraphicDevice, Object* pOwner);
	virtual ~Mesh();

public:
	virtual void						Render_Mesh();

	virtual void						Set_FillMode(D3DFILLMODE eMode);
	virtual void						Set_Hilight(bool bOn) { m_bHilight = bOn; }

protected:
	LPDIRECT3DDEVICE9		m_pGraphicDevice;
	Object*							m_pOwner;
	VIBuffer*							m_pVIBuffer;

	unsigned long					m_dwColor;
	unsigned long					m_dwHilghtColor;

	D3DFILLMODE					m_fillMode;

	bool									m_bHilight;

public:
	virtual void						Release();
};