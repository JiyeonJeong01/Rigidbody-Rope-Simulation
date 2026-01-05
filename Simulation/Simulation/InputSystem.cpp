#include "pch.h"
#include "InputSystem.h"

IMPLEMENT_SINGLETON(InputSystem)

InputSystem::InputSystem()
{
	fill(m_bPrevPress, m_bPrevPress + KEY_CNT, false);
	fill(m_bCurPress, m_bCurPress + KEY_CNT, false);
}

InputSystem::~InputSystem()
{
	Release();
}

void InputSystem::Ready_System()
{
}

void InputSystem::Update_System()
{
	for (int i = 0; i < KEY_CNT; ++i)
	{
			m_bPrevPress[i] = m_bCurPress[i];
	}

	fill(m_bCurPress, m_bCurPress + 0xfe, false);

	for (int i = 0; i < KEY_CNT; ++i)
	{
		if ((GetAsyncKeyState(i) & 0x8000) != 0)
			m_bCurPress[i] = true;
	}
}

void InputSystem::Release()
{
}

bool InputSystem::Get_Key(int iKey)
{
	if (iKey < 0 || iKey >= KEY_CNT) return false;
	return m_bCurPress[iKey];
}

bool InputSystem::Get_KeyDown(int iKey)
{
	if (iKey < 0 || iKey >= KEY_CNT) return false;
	return (m_bCurPress[iKey] && !m_bPrevPress[iKey]);
}

bool InputSystem::Get_KeyUp(int iKey)
{
	if (iKey < 0 || iKey >= KEY_CNT) return false;
	return (!m_bCurPress[iKey] && m_bPrevPress[iKey]);
}
