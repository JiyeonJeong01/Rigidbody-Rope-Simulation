#include "pch.h"
#include "Player.h"

#include "Sphere.h"
#include "Transform.h"

Player::Player(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Object(pGraphicDevice)
	, m_pTransform(nullptr), m_pMesh(nullptr)
{
}

Player::~Player()
{
}

HRESULT Player::Ready_GameObject()
{
	m_pTransform = Transform::Create(m_pGraphicDevice, this);
	m_pMesh = Sphere::Create(m_pGraphicDevice, this, D3DCOLOR_ARGB(255, 0, 255, 0), 1.f, 10);

	return S_OK;
}

int Player::Update_GameObject(const float& fTimeDelta)
{
	Handle_PlayerInput(fTimeDelta);


	// components 
	m_pTransform->Update_Component(fTimeDelta);


	return 0;
}

void Player::LateUpdate_GameObject(const float& fTimeDelta)
{
	// components 
	m_pTransform->LateUpdate_Component(fTimeDelta);

}

void Player::Render_GameObject()
{
	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();
}

void Player::Handle_PlayerInput(const float& fTimeDelta)
{
	// »óÇÏ
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pTransform->Translate({ 0.f, 0.f, 0.01f });
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pTransform->Translate({ 0.f, 0.f, -0.01f });
	}

	// ÁÂ¿ì
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		
	}

	// TEST
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_pTransform->Rotate(AXIS_X, 5.f);
	}
	else if (GetAsyncKeyState('Y') & 0x8000)
	{
		m_pTransform->Rotate(AXIS_Y, 5.f);
	}
	else if (GetAsyncKeyState('Z') & 0x8000)
	{
		m_pTransform->Rotate(AXIS_Z, 5.f);
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x0001)
	{
		m_pMesh->Set_Hilight(true);
	}
}

Player* Player::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	Player* pPlayer = new Player(pGraphicDevice);
	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Delete(pPlayer);
	}

	return pPlayer;
}

void Player::Release()
{
	m_pMesh->Release();

	Object::Release();
}
