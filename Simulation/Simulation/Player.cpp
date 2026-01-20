#include "pch.h"
#include "Player.h"
#include "Management.h"
#include "FollowCamera.h"
#include "InputSystem.h"
#include "Box.h"
#include "Sphere.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "SphereCollider.h"
#include "DebugHelper.h"
#include "Anchor.h"
#include "BoxCollider.h"
#include "Ground.h"
#include "SpringJoint.h"
#include "Rope.h"

Player::Player(LPDIRECT3DDEVICE9 pGraphicDevice)
    : Object(pGraphicDevice), m_pCamera(nullptr)
      , m_pRope(nullptr), m_pRigidbody(nullptr), m_pCollider(nullptr), m_pMainMesh(nullptr), m_pSpringJoint(nullptr)
{
}

Player::~Player()
{
}

HRESULT Player::Ready_GameObject()
{
    D3DXCreateLine(m_pGraphicDevice, &m_pLine);

	m_pMainMesh = Sphere::Create(m_pGraphicDevice, this, D3DCOLOR_ARGB(255, 0, 255, 0), 1.f, 10);
	m_pAssistMesh = Box::Create(m_pGraphicDevice, this, D3DCOLOR_ARGB(255, 0, 255, 0),{3.f, 0.3f, 0.3f});

	m_pTransform = Transform::Create(m_pGraphicDevice, this);
	m_pCollider = SphereCollider::Create(m_pGraphicDevice, this);
	//m_pCollider = BoxCollider::Create(m_pGraphicDevice, this);

    // Rigidbody
	BODY body;
	body.fAngularDrag = 0.1f;
	body.fDrag = 1.f;
	body.fRestitution = 0.5f;
	body.fFriction = 10.f;
	body.fMass = 10.f;
	body.fInvMass = 1.f / body.fMass;
    body.eGeoType = SPHERE;

    // Rigidbody Lock 기능
    body.tRotationLock = { true, false, true };
    // body.tPositionLock = { false, true, false };

	m_pRigidbody = Rigidbody::Create(m_pGraphicDevice, this, body);

    // SpringJoint 
	m_pSpringJoint = SpringJoint::Create(m_pGraphicDevice, this);
	m_pSpringJoint->Set_Damper(m_fDamper);
	m_pSpringJoint->Set_Spring(m_fSpring);
	m_pSpringJoint->Set_RestLength(m_fRest);
    m_pSpringJoint->Set_Active(false);

	Vec3 vEye = { 0.f, 5.f, -10.f };
	Vec3 vAt = { 0.f, 0.f, 0.f };
	Vec3 vUp = { 0.f, 1.f, 0.f };

	m_pCamera = FollowCamera::Create(m_pGraphicDevice,
		                                &vEye, &vAt, &vUp,
		                                D3DXToRadian(60.f), ((float)WINCX / WINCY), 0.1f, 1000.f, this);

    m_pRope = Rope::Create(m_pGraphicDevice);
    m_pRope->Set_Owner(this);

    Management::GetInstance()->Add_Object(m_pCamera);
    Management::GetInstance()->Add_Object(m_pRope);

	__super::Resolve_Dependencies();

	return S_OK;
}

int Player::Update_GameObject(const float& fTimeDelta)
{
	Handle_PlayerInput(fTimeDelta);
	Vec3 vPlayer = m_pTransform->Get_Position();

	return Object::Update_GameObject(fTimeDelta);
}

void Player::LateUpdate_GameObject(const float& fTimeDelta)
{
    Object::LateUpdate_GameObject(fTimeDelta);
}

void Player::FixedUpdate_GameObject(const float& fTimeDElta)
{
	Object::FixedUpdate_GameObject(fTimeDElta);

    const float fSpeed = 10.f;

    float ix(0.f), iz(0.f);

    if (InputSystem::GetInstance()->Get_Key('A')) ix -= 1.f;
    if (InputSystem::GetInstance()->Get_Key('D')) ix += 1.f;
    if (InputSystem::GetInstance()->Get_Key('W')) iz += 1.f;
    if (InputSystem::GetInstance()->Get_Key('S')) iz -= 1.f;

    const bool bHasInput = (ix != 0.f || iz != 0.f);
    Vec3 vCurVel = m_pRigidbody->Get_BodyInfo().vLinearVel;

    if (bHasInput && m_bGround)
    {
        Vec3 vCamPos = m_pCamera->Get_Pos();
        Vec3 vPlyrPos = m_pTransform->Get_Position();

        Vec3 vToPlyr = VectorHelper::Get_Normalized(vPlyrPos - vCamPos);

        Vec3 vLook = vToPlyr;
        vLook.y = 0;
        vLook = VectorHelper::Get_Normalized(vLook);

        Vec3 vRight = VectorHelper::CrossProduct(Vec3(0.f, 1.f, 0.f), vLook);
        vRight.y = 0.f;
        vRight = VectorHelper::Get_Normalized(vRight);

        Vec3 vMoveDir = vRight * ix + vLook * iz;
        if (!VectorHelper::Is_Zero(vMoveDir))
            vMoveDir = VectorHelper::Get_Normalized(vMoveDir);

        Vec3 vNewVel = vMoveDir * fSpeed;
        vNewVel.y = vCurVel.y;
        m_pRigidbody->Set_LinearVelocity(vNewVel);
    }

    // 점프
    if (InputSystem::GetInstance()->Get_KeyDown(VK_SPACE))
    {
        m_pRigidbody->Add_LinearImpulse({ vCurVel.x, fSpeed, vCurVel.z });
    }

    // 회전 테스트

    if (InputSystem::GetInstance()->Get_Key('T'))
    {
        m_pRigidbody->Add_Torque({ fSpeed * 10.f, vCurVel.y, vCurVel.z });
    }
    if (InputSystem::GetInstance()->Get_Key('Y'))
    {
        m_pRigidbody->Add_Torque({ vCurVel.x, fSpeed * 10.f, vCurVel.z });
    }
    if (InputSystem::GetInstance()->Get_Key('U'))
    {
        m_pRigidbody->Add_Torque({ vCurVel.x, vCurVel.y, fSpeed * 10.f });
    }

}

void Player::Render_GameObject()
{
	m_pCamera->LateUpdate_GameObject(0.016f);

	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMainMesh->Render_Mesh();
    m_pAssistMesh->Render_Mesh();

    Draw_Crosshair();

    Object::Render_GameObject();
}

void Player::On_CollisionEnter(const COLLISION& tCollision)
{
	Object::On_CollisionEnter(tCollision);
}

void Player::On_CollisionStay(const COLLISION& tCollision)
{
	Object::On_CollisionStay(tCollision);

	m_pMainMesh->Set_Highlight(true);
	m_pAssistMesh->Set_Highlight(true);
    m_bGround = true;
}

void Player::On_CollisionExit(const COLLISION& tCollision)
{
	Object::On_CollisionExit(tCollision);
	m_pMainMesh->Set_Highlight(false);
	m_pAssistMesh->Set_Highlight(false);
    m_bGround = false;
}

void Player::Handle_PlayerInput(const float& fTimeDelta)
{

	// Raycast 테스트
	if (InputSystem::GetInstance()->Get_KeyDown('Q'))
	{
		if (m_bSwing) return;
        m_bSwing = true;
        m_pRope->Set_RopeAmplitueInfo({ m_fSpring, m_fDamper, 0.f, 0.2f, 0.f });
	    m_pRope->Try_Grappling();
	}

	if (InputSystem::GetInstance()->Get_KeyUp('Q'))
	{
		if (m_bSwing)
		{
            m_bSwing = false;

		    m_pSpringJoint->Set_Active(m_bSwing);
            m_pRope->Finish_Grappling();
		}
	}

	long lMouseMove;
	if (lMouseMove = InputSystem::GetInstance()->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_X))
	{
        float fDegree = lMouseMove / 20.f;
        m_fYawDegree += fDegree;
		m_pCamera->Yaw(fDegree);
	}
	if (lMouseMove = InputSystem::GetInstance()->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_Y))
	{
        float fDegree = lMouseMove / 20.f;
        m_fPitchDegree += fDegree;
		m_pCamera->Pitch(fDegree);
	}
}

void Player::Start_Swing(Vec3 vAnchor)
{
    m_vAnchor = vAnchor;

    m_bSwing = true;

    m_pRope->Set_Active(m_bSwing);
    m_pRope->Set_Anchor(m_vAnchor);

    m_pSpringJoint->Set_Active(m_bSwing);
    m_pSpringJoint->Set_Anchor(m_vAnchor);
}

void Player::Draw_Crosshair()
{
    if (!m_pLine) return;

    const float fCX = WINCX * 0.5f;
    const float fCY = WINCY * 0.5f;
    const float fLen = 10;


    D3DXVECTOR2 h[2] = { {fCX - fLen, fCY}, {fCX + fLen, fCY} };
    D3DXVECTOR2 v[2] = { {fCX, fCY - fLen}, {fCX, fCY + fLen} };

    m_pLine->SetWidth(2.0f);
    m_pLine->Begin();
    m_pLine->Draw(h, 2, D3DCOLOR_ARGB(255, 255, 255, 255));
    m_pLine->Draw(v, 2, D3DCOLOR_ARGB(255, 255, 255, 255));
    m_pLine->End();
}

void Player::End_Swing()
{
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
    Safe_Release(m_pMainMesh);
    Safe_Release(m_pAssistMesh);
    Safe_Release(m_pLine);

	Object::Release();
}
