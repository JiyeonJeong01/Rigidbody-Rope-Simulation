#pragma once
class InputSystem
{
	DECLARE_SINGLETON(InputSystem)
private :
	InputSystem();
	~InputSystem();

public:
	void					Ready_System();
	void					Update_System();

	bool					Get_Key(int iKey);
	bool					Get_KeyDown(int iKey);
	bool					Get_KeyUp(int iKey);

private :
	static constexpr int		KEY_CNT = 0xff;
	bool								m_bPrevPress[KEY_CNT];
	bool								m_bCurPress[KEY_CNT];

public:
	void Release();
};

