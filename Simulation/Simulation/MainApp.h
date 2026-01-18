#pragma once

class GraphicDevice;
class Object;

class MainApp
{
public:
	explicit MainApp();
	virtual ~MainApp();

public:
	HRESULT		Ready_MainApp();
	int			Update_MainApp(const float& fTimeDelta);
	void		Fixed_Update(const float& fTimeDelta);
	void		LateUpdate_MainApp(const float& fTimeDelta);
	void		Render_MainApp();

private:
	HRESULT		Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT		Ready_Ground();
	HRESULT		Ready_Wall();

private:
	GraphicDevice*		m_pGraphicDevice;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	list<Object*>		m_ObjectList;

	int					iFPS{};
	int					iFPSCounter{};
	long long			llElapsedTime = 0;
	TCHAR				szFPS[128];

	Object*				m_pPlayer{};
	Vec3				m_vPlayerPos{};

public: 
	static MainApp* Create();
	void Release();
};

