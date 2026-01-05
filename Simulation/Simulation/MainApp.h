#pragma once

class GraphicDevice;
class Player;
class Enemy;
class Ground;

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

	Player*							m_pPlayer;
	Enemy*							m_pEnemy;
	Ground*							m_pGround;

public: 
	static MainApp* Create();
	void Release();
};

