#pragma once

class InputSystem
{
	DECLARE_SINGLETON(InputSystem)
private :
	InputSystem();
	~InputSystem();

public:
	HRESULT			Ready_System();
	void					Update_System();

	bool					Get_Key(int iKey);
	bool					Get_KeyDown(int iKey);
	bool					Get_KeyUp(int iKey);

	byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) const
	{
		return m_tMouseState.rgbButtons[eMouse];
	}

	long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((long*)&m_tMouseState) + eMouseState);
	}

private :
	static constexpr int		KEY_CNT = 0xff;
	bool								m_bPrevPress[KEY_CNT];
	bool								m_bCurPress[KEY_CNT];

	LPDIRECTINPUT8			m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;
	DIMOUSESTATE			m_tMouseState;

public:
	void Release();
};

