#include "pch.h"
#include "Player.h"

#include "Camera.h"
#include "InputSystem.h"
#include "Raycast.h"
#include "Sphere.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "SphereCollider.h"
#include "DebugHelper.h"
#include "SpringJoint.h"

Player::Player(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Object(pGraphicDevice), m_pCamera(nullptr)
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

	m_pRigidbody->Set_ColType(DYNAMIC);
	m_pRigidbody->Set_Mass(1.f);
	m_pRigidbody->Set_Drag(0.1f);
	m_pRigidbody->Set_AngularDrag(0.1f);
	m_pRigidbody->Set_GeometryType(SPHERE);
	m_pRigidbody->Set_Gravity(true);
	m_pRigidbody->Set_Restitution(0.3f);

	m_pSpringJoint = SpringJoint::Create(m_pGraphicDevice, this);
	m_pSpringJoint->Set_Damper(5.f);
	m_pSpringJoint->Set_Spring(50.f);

	Vec3 vEye = { 0.f, 5.f, -10.f };
	Vec3 vAt = { 0.f, 0.f, 0.f };
	Vec3 vUp = { 0.f, 1.f, 0.f };

	m_pCamera = Camera::Create(m_pGraphicDevice,
		&vEye, &vAt, &vUp,
		D3DXToRadian(60.f), ((float)WINCX / WINCY), 0.1f, 1000.f);

	return S_OK;
}

int Player::Update_GameObject(const float& fTimeDelta)
{

	Handle_PlayerInput(fTimeDelta);
	Vec3 vPlayer = m_pTransform->Get_Position();
	//DebugHelper::Print_Vec3(L"Pos Before Update", vPlayer);
	Object::Update_GameObject(fTimeDelta);
	vPlayer = m_pTransform->Get_Position();
	//DebugHelper::Print_Vec3(L"Pos After Update", vPlayer);

	m_pCamera->Update_GameObject(fTimeDelta);


	//DebugHelper::Print_Vec3(L"Update - Player", vPlayer);

	return 0;
}

void Player::LateUpdate_GameObject(const float& fTimeDelta)
{
	Object::LateUpdate_GameObject(fTimeDelta);
}

void Player::Render_GameObject()
{
	Vec3 vPlayer = m_pTransform->Get_Position();
	m_pCamera->Set_Position({ vPlayer.x, vPlayer.y + 2.f, vPlayer.z - 12.f });
	m_pCamera->LateUpdate_GameObject(0.016f);

	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();

	DebugHelper::Print_Vec3(L"Pos After Solve Loop", vPlayer);
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
	if (InputSystem::GetInstance()->Get_KeyDown('Q'))
	{
		m_pSpringJoint->Set_Active(true);
		Vec3 vPos = m_pTransform->Get_Position();
		m_pSpringJoint->Set_Anchor({ vPos.x -10.f, vPos.y + 20.f, vPos.z + 15.f });
	}

	if (InputSystem::GetInstance()->Get_KeyUp('Q'))
	{
		m_pSpringJoint->Set_Active(false);
	}
	if (InputSystem::GetInstance()->Get_KeyDown('E'))
	{

		m_pSpringJoint->Set_Active(true);
		Vec3 vPos = m_pTransform->Get_Position();
		m_pSpringJoint->Set_Anchor({ vPos.x +10.f, vPos.y + 20.f, vPos.z + 15.f });
	}

	if (InputSystem::GetInstance()->Get_KeyUp('E'))
	{
		m_pSpringJoint->Set_Active(false);
	}

	const float fSpeed = 50.f;

	// 상하
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pRigidbody->Add_Force({ 0.f, 0.f, fSpeed}, FORCE_MODE::FORCE);
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pRigidbody->Add_Force({ 0.f, 0.f, -fSpeed }, FORCE_MODE::FORCE);
	}

	// 좌우
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pRigidbody->Add_Force({ -fSpeed, 0.f, 0.f }, FORCE_MODE::FORCE);
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_pRigidbody->Add_Force({ fSpeed, 0.f, 0.f }, FORCE_MODE::FORCE);
	}

	// TEST
	if (InputSystem::GetInstance()->Get_KeyDown(VK_SPACE))
	{
		m_pRigidbody->Add_Force({ 0.f, fSpeed, 0.f }, FORCE_MODE::IMPULSE);
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_pRigidbody->Add_Torque({ 0.f, fSpeed, 0.f }, FORCE_MODE::FORCE);
	}

	// Raycast 테스트
	if (GetAsyncKeyState('T') & 0x8000)
	{
		POINT	ptMouse{};
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		RAYCAST_HIT hit;
		static int iskdfjlsdkf = 0;
		if (Raycast::Intersect_Ray(&hit, ptMouse))
			printf("%d\n", iskdfjlsdkf++);
	}

#pragma region DEBUGGING CAM
	//// 상하
	//if (GetAsyncKeyState('W') & 0x8000)
	//{
	//	m_pRigidbody->Add_Force({ 0.f, 0.f, fSpeed }, FORCE_MODE::FORCE);
	//}
	//else if (GetAsyncKeyState('S') & 0x8000)
	//{
	//	m_pRigidbody->Add_Force({ 0.f, 0.f, -fSpeed }, FORCE_MODE::FORCE);
	//}
	//// 좌우
	//if (GetAsyncKeyState('A') & 0x8000)
	//{
	//	m_pRigidbody->Add_Force({ -fSpeed, 0.f, 0.f }, FORCE_MODE::FORCE);
	//}
	//else if (GetAsyncKeyState('D') & 0x8000)
	//{
	//	m_pRigidbody->Add_Force({ fSpeed, 0.f, 0.f }, FORCE_MODE::FORCE);
	//}

	static bool bLock = true;
	if (GetAsyncKeyState(VK_OEM_5) & 0x8000)
	{
		bLock = !bLock;
	}
	long lMouseMove;
	if (!bLock)
	{
		if (lMouseMove = InputSystem::GetInstance()->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_X))
		{
			m_pCamera->Rotate(AXIS::AXIS_Y, D3DXToRadian(lMouseMove / 0.5f));
		}
		if (lMouseMove = InputSystem::GetInstance()->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_Y))
		{
			m_pCamera->Rotate(AXIS::AXIS_X, D3DXToRadian(lMouseMove / 0.5f));
		}
	}
#pragma endregion



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
	m_pCamera->Release();

	Object::Release();
}
