#pragma once

class GraphicDevice;
class Object;

class MainApp
{
public:	// 持失切, 社瑚切
	explicit MainApp();
	virtual ~MainApp();

public:
	HRESULT Ready_MainApp();
	int		Update_MainApp(const float& fTimeDelta);
	void		LateUpdate_MainApp(const float& fTimeDelta);
	void		Render_MainApp();

private:
	HRESULT		Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	GraphicDevice*				m_pGraphicDevice;
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	list<Object*>					m_ObjectList;

public: 
	static MainApp* Create();
	void Release();
};

