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
#include "Anchor.h"
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
	m_pCollider = SphereCollider::Create(m_pGraphicDevice, this);

	BODY body;
	body.fAngularDrag = 0.1f;
	body.fDrag = 0.1f;
	body.fRestitution = 0.f;
	body.fFriction = 2.f;
	body.fMass = 10.f;
	body.fInvMass = 1.f / body.fMass;
	m_pRigidbody = Rigidbody::Create(m_pGraphicDevice, this, body);
	m_pRigidbody->Set_GeometryType(SPHERE);

	m_pSpringJoint = SpringJoint::Create(m_pGraphicDevice, this);
	m_pSpringJoint->Set_Damper(1.f);
	m_pSpringJoint->Set_Spring(10.f);
	m_pSpringJoint->Set_RestLength(0.5f);

	m_pAnchor = Anchor::Create(m_pGraphicDevice);

	Vec3 vEye = { 0.f, 5.f, -10.f };
	Vec3 vAt = { 0.f, 0.f, 0.f };
	Vec3 vUp = { 0.f, 1.f, 0.f };

	m_pCamera = Camera::Create(m_pGraphicDevice,
		&vEye, &vAt, &vUp,
		D3DXToRadian(60.f), ((float)WINCX / WINCY), 0.1f, 1000.f);

	__super::Resolve_Dependencies();

	return S_OK;
}

int Player::Update_GameObject(const float& fTimeDelta)
{

	Handle_PlayerInput(fTimeDelta);
	Vec3 vPlayer = m_pTransform->Get_Position();
	Object::Update_GameObject(fTimeDelta);
	vPlayer = m_pTransform->Get_Position();

	m_pCamera->Update_GameObject(fTimeDelta);

	if (m_bSwing)
	{
		m_pAnchor->Update_GameObject(fTimeDelta);
	}
	return 0;
}

void Player::LateUpdate_GameObject(const float& fTimeDelta)
{
	Object::LateUpdate_GameObject(fTimeDelta);

	if (m_bSwing)
		m_pAnchor->LateUpdate_GameObject(fTimeDelta);
}

void Player::Fixed_Update(const float& fTimeDElta)
{
	Object::Fixed_Update(fTimeDElta);
}

void Player::Render_GameObject()
{
	Vec3 vPlayer = m_pTransform->Get_Position();
	m_pCamera->Set_Position({ vPlayer.x, vPlayer.y + 2.f, vPlayer.z - 15.f });
	m_pCamera->LateUpdate_GameObject(0.016f);

	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();

	if (m_bSwing)
	{
		Render_Swing();
		m_pAnchor->Render_GameObject();
	}

}

void Player::On_CollisionEnter(const COLLISION& tCollision)
{
	Object::On_CollisionEnter(tCollision);
}

void Player::On_CollisionStay(const COLLISION& tCollision)
{
	Object::On_CollisionStay(tCollision);

	m_pMesh->Set_Hilight(true);
}

void Player::On_CollisionExit(const COLLISION& tCollision)
{
	Object::On_CollisionExit(tCollision);
	m_pMesh->Set_Hilight(false);
}

void Player::Handle_PlayerInput(const float& fTimeDelta)
{
	const float fSpeed = 80.f;

	// 상하 이동
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pRigidbody->Add_Force({ 0.f, 0.f, fSpeed});
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pRigidbody->Add_Force({ 0.f, 0.f, -fSpeed });
	}

	// 좌우 이동
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pRigidbody->Add_Force({ -fSpeed, 0.f, 0.f });
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_pRigidbody->Add_Force({ fSpeed, 0.f, 0.f });
	}

	// 점프
	if (InputSystem::GetInstance()->Get_KeyDown(VK_SPACE))
	{
		m_pRigidbody->Add_LinearImpulse({ 0.f, fSpeed, 0.f });
	}

	// Raycast 테스트
	if (InputSystem::GetInstance()->Get_KeyDown(VK_LBUTTON))
	{
		if (m_bSwing)
			return;

		RAYCAST_HIT hit;
		static int iskdfjlsdkf = 0;
		if (Raycast::Intersect_Ray(&hit, m_pTransform->Get_Position()))
		{
			DebugHelper::Print_Vec3(L"success", hit.vPoint);
			m_vAnchor = hit.vPoint;

			m_pAnchor->Get_Transform()->Set_Position(m_vAnchor);
			m_bSwing = true;

			m_pSpringJoint->Set_Active(true);
			m_pSpringJoint->Set_Anchor(m_vAnchor);
		}

	}

	if (InputSystem::GetInstance()->Get_KeyUp(VK_LBUTTON))
	{
		if (m_bSwing)
			m_bSwing = false;

		m_pSpringJoint->Set_Active(false);
		m_bSwing = false;
	}

#pragma region DEBUGGING CAM
	static bool bLock = true;
	if (InputSystem::GetInstance()->Get_KeyDown('M'))
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

void Player::Render_Swing()
{
	Matrix matProj, matView, matWorld;
	m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matWorld);

	Matrix matPV = matView * matProj;

	D3DVIEWPORT9 vp{};
	m_pGraphicDevice->GetViewport(&vp);

	LPD3DXLINE pLine;
	D3DXCreateLine(m_pGraphicDevice, &pLine);
	pLine->SetWidth(3.f);
	pLine->Begin();

	// 그리기 
	Vec3 p[2]{};
	p[0] = m_pTransform->Get_Position();
	p[1] = m_vAnchor;

	Vec3 vScreenPos0, vScreenPos1;

	D3DXVec3Project(&vScreenPos0, &p[0], &vp, &matProj, &matView, &matWorld);
	D3DXVec3Project(&vScreenPos1, &p[1], &vp, &matProj, &matView, &matWorld);

	pLine->DrawTransform(p, 2, &matPV, D3DCOLOR_XRGB(0, 255, 0));
	pLine->End();

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
