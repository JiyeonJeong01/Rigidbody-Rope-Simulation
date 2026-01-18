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

HRESULT InputSystem::Ready_System()
{
	if (FAILED(DirectInput8Create(g_hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;


	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수 
	m_pMouse->Acquire();

	return S_OK;

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

	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

void InputSystem::Release()
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
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
