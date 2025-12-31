#pragma once
class GraphicDevice
{
	DECLARE_SINGLETON(GraphicDevice)
private :
	GraphicDevice();
	~GraphicDevice();

public:
	LPDIRECT3DDEVICE9		Get_GraphicDev() { return m_pDevice; }

public:
	HRESULT Ready_GraphicDev(HWND hWnd, GraphicDevice** ppGraphicDev);

	void		Render_Begin(D3DXCOLOR Color);
	void		Render_End();

private:
	LPDIRECT3D9			m_pSDK;
	LPDIRECT3DDEVICE9	m_pDevice;	

private:
	void	Free();
};