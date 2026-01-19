#include "pch.h"
#include "Player.h"
#include "Management.h"
#include "FollowCamera.h"
#include "InputSystem.h"
#include "Raycast.h"
#include "Sphere.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "SphereCollider.h"
#include "DebugHelper.h"
#include "Anchor.h"
#include "SpringJoint.h"
#include "Rope.h"

Player::Player(LPDIRECT3DDEVICE9 pGraphicDevice)
    : Object(pGraphicDevice), m_pCamera(nullptr)
      , m_pRope(nullptr), m_pRigidbody(nullptr), m_pCollider(nullptr), m_pMesh(nullptr), m_pSpringJoint(nullptr)
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

    // Rigidbody
	BODY body;
	body.fAngularDrag = 0.1f;
	body.fDrag = 1.f;
	body.fRestitution = 0.2f;
	body.fFriction = 10.f;
	body.fMass = 10.f;
	body.fInvMass = 1.f / body.fMass;
    body.tRotationLock = { false, false, false };
	m_pRigidbody = Rigidbody::Create(m_pGraphicDevice, this, body);
	m_pRigidbody->Set_GeometryType(SPHERE);

	m_pSpringJoint = SpringJoint::Create(m_pGraphicDevice, this);
	m_pSpringJoint->Set_Damper(m_fDamper);
	m_pSpringJoint->Set_Spring(m_fSpring);
	m_pSpringJoint->Set_RestLength(m_fRest);
    m_pSpringJoint->Set_Active(false);

	m_pAnchor = Anchor::Create(m_pGraphicDevice);
    m_pAnchor->Set_Active(false);

	Vec3 vEye = { 0.f, 5.f, -10.f };
	Vec3 vAt = { 0.f, 0.f, 0.f };
	Vec3 vUp = { 0.f, 1.f, 0.f };

	m_pCamera = FollowCamera::Create(m_pGraphicDevice,
		                                &vEye, &vAt, &vUp,
		                                D3DXToRadian(60.f), ((float)WINCX / WINCY), 0.1f, 1000.f, this);

    m_pRope = Rope::Create(m_pGraphicDevice);
    m_pRope->Set_Owner(this);
    m_pRope->Set_Active(false);

    
    Management::GetInstance()->Add_Object(m_pAnchor);
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

    // 상하 이동
    if (InputSystem::GetInstance()->Get_Key('W'))
    {
        m_pRigidbody->Set_LinearVelocity({ 0.f, 0.f, fSpeed });
    }
    else if (InputSystem::GetInstance()->Get_Key('S'))
    {
        m_pRigidbody->Set_LinearVelocity({ 0.f, 0.f, -fSpeed });
    }

    Vec3 vCurVel = m_pRigidbody->Get_BodyInfo().vLinearVel;

    // 좌우 이동
    if (InputSystem::GetInstance()->Get_Key('A'))
    {
        m_pRigidbody->Set_LinearVelocity({ -fSpeed, vCurVel.y, vCurVel.z });
    }
    else if (InputSystem::GetInstance()->Get_Key('D'))
    {
        m_pRigidbody->Set_LinearVelocity({ fSpeed, vCurVel.y, vCurVel.z });
    }


    // 점프
    if (InputSystem::GetInstance()->Get_KeyDown(VK_SPACE))
    {
        m_pRigidbody->Add_LinearImpulse({ vCurVel.x, fSpeed, vCurVel.z });
    }

    // 회전 테스트
    if (InputSystem::GetInstance()->Get_KeyDown('T'))
    {
        m_pRigidbody->Add_Torque({ vCurVel.x, fSpeed * 10.f, vCurVel.z });
    }
}

void Player::Render_GameObject()
{
	m_pCamera->LateUpdate_GameObject(0.016f);

	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();

    Object::Render_GameObject();
}

void Player::On_CollisionEnter(const COLLISION& tCollision)
{
	Object::On_CollisionEnter(tCollision);
}

void Player::On_CollisionStay(const COLLISION& tCollision)
{
	Object::On_CollisionStay(tCollision);

	m_pMesh->Set_Highlight(true);
}

void Player::On_CollisionExit(const COLLISION& tCollision)
{
	Object::On_CollisionExit(tCollision);
	m_pMesh->Set_Highlight(false);
}

void Player::Handle_PlayerInput(const float& fTimeDelta)
{

	// Raycast 테스트
	if (InputSystem::GetInstance()->Get_KeyDown('Q'))
	{
		// if (m_bSwing) return;
        m_pRope->Set_Active(true);
	    m_pRope->Try_Grappling();
	}

	if (InputSystem::GetInstance()->Get_KeyUp('Q'))
	{
		if (m_bSwing)
		{
            m_bSwing = false;

		    m_pSpringJoint->Set_Active(m_bSwing);
            m_pRope->Set_Active(m_bSwing);
            m_pAnchor->Set_Active(m_bSwing);
		}
	}

#pragma region DEBUGGING CAM
	static bool bLock = true;
	long lMouseMove;
	if (lMouseMove = InputSystem::GetInstance()->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_X))
	{
        float fDegree = lMouseMove / 20.f;
		//m_pCamera->Yaw(fDegree);
        //m_pTransform->Rotate(AXIS_Y, fDegree);

        Vec3 vRight, vForward;
        // m_pTransform->Get_Info(AXIS_X, &vRight);
        // DebugHelper::Print_Vec3(L"Right", vRight);

	    m_pTransform->Get_Info(AXIS_Z, &vForward);
	}
	if (lMouseMove = InputSystem::GetInstance()->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_Y))
	{
        float fDegree = lMouseMove / 20.f;

		//m_pCamera->Pitch(fDegree);
        //m_pTransform->Rotate(AXIS_X, fDegree);
	}
#pragma endregion
}

void Player::Render_Swing()
{
	//Matrix matProj, matView, matWorld;
	//m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
	//m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	//D3DXMatrixIdentity(&matWorld);

	//Matrix matPV = matView * matProj;

	//D3DVIEWPORT9 vp{};
	//m_pGraphicDevice->GetViewport(&vp);

	//LPD3DXLINE pLine;
	//D3DXCreateLine(m_pGraphicDevice, &pLine);
	//pLine->SetWidth(3.f);
	//pLine->Begin();

	//// 그리기 
	//Vec3 p[2]{};
	//p[0] = m_pTransform->Get_Position();
	//p[1] = m_vAnchor;

	//Vec3 vScreenPos0, vScreenPos1;

	//D3DXVec3Project(&vScreenPos0, &p[0], &vp, &matProj, &matView, &matWorld);
	//D3DXVec3Project(&vScreenPos1, &p[1], &vp, &matProj, &matView, &matWorld);

	//pLine->DrawTransform(p, 2, &matPV, D3DCOLOR_XRGB(0, 255, 0));
	//pLine->End();

}

void Player::Start_Swing(Vec3 vAnchor)
{
    m_vAnchor = vAnchor;

    m_bSwing = true;

    m_pAnchor->Get_Transform()->Set_Position(m_vAnchor);
    m_pRope->Set_Active(m_bSwing);
    m_pRope->Set_VisualSpringValue({ m_fSpring, m_fDamper, 0.f, 0.2f, 0.f });
    m_pRope->Set_Anchor(m_vAnchor);

    m_pSpringJoint->Set_Active(m_bSwing);
    m_pSpringJoint->Set_Anchor(m_vAnchor);
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
	m_pMesh->Release();

	Object::Release();
}
