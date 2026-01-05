#include "pch.h"
#include "Player.h"

#include "Sphere.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "SphereCollider.h"

Player::Player(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Object(pGraphicDevice)
	, m_pTransform(nullptr), m_pRigidbody(nullptr), m_pMesh(nullptr)
{
}

Player::~Player()
{
}

HRESULT Player::Ready_GameObject()
{
	m_pMesh = Sphere::Create(m_pGraphicDevice, this, D3DCOLOR_ARGB(255, 0, 255, 0), 1.f, 10);

	m_pTransform = Transform::Create(m_pGraphicDevice, this);
	m_pRigidbody = Rigidbody::Create(m_pGraphicDevice, this);
	m_pCollider = SphereCollider::Create(m_pGraphicDevice, this);

	m_pRigidbody->Set_Mass(10.f);
	m_pRigidbody->Set_Drag(0.5f);
	m_pRigidbody->Set_AngularDrag(5.f);
	m_pRigidbody->Set_GeometryType(SPHERE);
	m_pRigidbody->Set_Gravity(true);

	return S_OK;
}

int Player::Update_GameObject(const float& fTimeDelta)
{
	Handle_PlayerInput(fTimeDelta);

	Object::Update_GameObject(fTimeDelta);

	return 0;
}

void Player::LateUpdate_GameObject(const float& fTimeDelta)
{
	Object::LateUpdate_GameObject(fTimeDelta);

}

void Player::Render_GameObject()
{
	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();
}

void Player::On_CollisionEnter(const Collision& tCollision)
{
	Object::On_CollisionEnter(tCollision);
}

void Player::On_CollisionStay(const Collision& tCollision)
{
	Object::On_CollisionStay(tCollision);

	m_pMesh->Set_Hilight(true);
}

void Player::On_CollisionExit(const Collision& tCollision)
{
	Object::On_CollisionExit(tCollision);
	m_pMesh->Set_Hilight(false);
}

void Player::Handle_PlayerInput(const float& fTimeDelta)
{
	const float fSpeed = 10.f;

	// »óÇÏ
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pRigidbody->Add_Force({ 0.f, 0.f, fSpeed}, FORCE_MODE::FORCE);
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pRigidbody->Add_Force({ 0.f, 0.f, -fSpeed }, FORCE_MODE::FORCE);
	}

	// ÁÂ¿ì
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pRigidbody->Add_Force({ -fSpeed, 0.f, 0.f }, FORCE_MODE::FORCE);
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_pRigidbody->Add_Force({ fSpeed, 0.f, 0.f }, FORCE_MODE::FORCE);
	}

	// TEST
	if (GetAsyncKeyState(VK_SPACE) & 0x0001)
	{
		// m_pMesh->Set_Hilight(true);
		m_pRigidbody->Add_Force({ 0.f, fSpeed, 0.f }, FORCE_MODE::IMPULSE);
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_pRigidbody->Add_Torque({ 0.f, fSpeed, 0.f }, FORCE_MODE::FORCE);
	}
	//else if (GetAsyncKeyState('Y') & 0x8000)
	//{
	//	m_pTransform->Rotate(AXIS_Y, 5.f);
	//}
	//else if (GetAsyncKeyState('Z') & 0x8000)
	//{
	//	m_pTransform->Rotate(AXIS_Z, 5.f);
	//}
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
