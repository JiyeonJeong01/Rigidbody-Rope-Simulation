#include "pch.h"
#include "PhysicsWorld.h"
#include "Object.h"
#include "Collider.h"
#include "CollisionDetector.h"
#include "Rigidbody.h"
#include "Solver.h"
#include "Transform.h"
#include "PhysicsUtil.h"
IMPLEMENT_SINGLETON(PhysicsWorld)

unsigned int PhysicsWorld::s_iCurColCnt = 0;
unsigned int PhysicsWorld::s_iCurCBodyCnt = 0;

PhysicsWorld::PhysicsWorld()
{
}

PhysicsWorld::~PhysicsWorld()
{
	Release();
}

HRESULT PhysicsWorld::Ready_System(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	m_pGraphicDevice = pGraphicDevice;

	m_pCollisionDetector = CollisionDetector::Create();
	m_pSolver = Solver::Create(pGraphicDevice);

	return S_OK;
}

int PhysicsWorld::Update_System(float fTimeDelta)
{
	m_CurPair.clear();
    m_ContactInfosList.clear();

    // Apply Forces
    Accumulate_Forces();
    Integrate_Forces(fTimeDelta);
	
    // Damping
    Apply_Damping(fTimeDelta);

    // inte
    Integrate_Velocities(fTimeDelta);

    // Generate 
    m_pCollisionDetector->Generate_ContactInfo();

    // Solve
    for (int i = 0; i < 1; ++i)
        for (auto& contact : m_ContactInfosList)
            m_pSolver->Solve_Contacts(&contact);

    // Apply Lock
    //for (BODY& b : m_vecBodies)
    //{
    //    PhysicsUtil::ApplyPositionLock(b);
    //    PhysicsUtil::ApplyRotationLock(b);
    //}

    Invoke_CollisionEvent();
	return 0;
}

void PhysicsWorld::Add_ContactInfo(CONTACT_INFO tinfo)
{
	m_ContactInfosList.push_back(tinfo);
}

void PhysicsWorld::Add_ContactPair(PAIR_KEY key)
{
	m_CurPair.insert(key);
}

void PhysicsWorld::Add_Collider(Collider* pCollider)
{
	m_vecCollider.push_back(pCollider);
    pCollider->Set_ColliderID(s_iCurColCnt++);
}

void PhysicsWorld::Remove_Collider(Collider* pCollider)
{
	if (!pCollider)
		return;

	// TODO : KeyPair도 삭제해야 한다 

	auto it = std::remove(m_vecCollider.begin(), m_vecCollider.end(), pCollider);
	m_vecCollider.erase(it, m_vecCollider.end());
}

uint_fast32_t PhysicsWorld::Create_Body(BODY body)
{
    if (body.eType == STATIC)
        PhysicsUtil::Set_StaticBody(body);
    else if (body.eType == KINEMATIC)
        PhysicsUtil::Set_KinematicBody(body);
    else if (body.eType == DYNAMIC)
        PhysicsUtil::Set_DynamicBody(body);

    if (!m_freeIds.empty())
    {
        uint32_t id = m_freeIds.back();
        m_freeIds.pop_back();
        m_vecBodies[id] = body;
        return id;
    }

    m_vecBodies.push_back(std::move(body));
    return static_cast<uint32_t>(m_vecBodies.size() - 1);
}

void PhysicsWorld::Remove_Body(uint_fast32_t iID)
{
    if (BODY* b = Try_GetBody(iID))
    {
        b->bActive = false;
        b->vForceAccum = { 0,0,0 };
        b->vTorqueAccum = { 0,0,0 };
        b->vLinearVel = { 0,0,0 };
        b->vAngularVel = { 0,0,0 };
    }
    m_freeIds.push_back(iID);
}

BODY* PhysicsWorld::Try_GetBody(uint_fast32_t iID)
{
    if (iID >= m_vecBodies.size())
        return nullptr;

    BODY& b = m_vecBodies[iID];
    if (!b.bActive)
        return nullptr;

    return &b;
}

void PhysicsWorld::Invoke_CollisionEvent()
{
    // Enter / Stay
    for (const PAIR_KEY& k : m_CurPair)
    {
        Collider* pA = Find_ColliderByID(k.aKey);
        Collider* pB = Find_ColliderByID(k.bKey);
        if (!pA || !pB) continue;

        // TODO : 필요한 정보가 뭘지 다 짜고 보가ㅣ 
        COLLISION tA{};
        COLLISION tB{};

        if (m_prevPair.find(k) == m_prevPair.end())
        {
            // Enter
            pA->Get_Object()->On_CollisionEnter(tA);
            pB->Get_Object()->On_CollisionEnter(tB);

            pA->Set_OnCol(true);
            pB->Set_OnCol(true);
        }
        else
        {
            // Stay
            pA->Get_Object()->On_CollisionStay(tA);
            pB->Get_Object()->On_CollisionStay(tB);
        }
    }

    // Exit
    for (const PAIR_KEY& k : m_prevPair)
    {
        if (m_CurPair.find(k) != m_CurPair.end())
            continue;

        Collider* pA = Find_ColliderByID(k.aKey);
        Collider* pB = Find_ColliderByID(k.bKey);
        if (!pA || !pB) continue;

        COLLISION tA{};
        COLLISION tB{};

        pA->Get_Object()->On_CollisionExit(tA);
        pB->Get_Object()->On_CollisionExit(tB);

        pA->Set_OnCol(false);
        pB->Set_OnCol(false);
    }

    // swap 
    m_prevPair.swap(m_CurPair);
    m_CurPair.clear();
}

Collider* PhysicsWorld::Find_ColliderByID(uint32_t id)
{
    for (Collider* pCol : m_vecCollider)
    {
        if (pCol->Get_ColliderID() == id)
            return pCol;
    }
    return nullptr;
}

bool PhysicsWorld::Detect_Ray(tagRay* pRay)
{
    return m_pCollisionDetector->Detect_Ray(pRay);
}

void PhysicsWorld::Accumulate_Forces()
{
    for (BODY& b : m_vecBodies)
    {
        if (!b.bActive || b.eType != DYNAMIC)
            continue;

        if (b.bGravity)
        {
            float mass = (b.fInvMass > 0.f) ? (1.f / b.fInvMass) : b.fMass;
            b.vForceAccum += m_vGravity * mass;
        }
    }
}

void PhysicsWorld::Integrate_Forces(float fTimeDelta)
{
    for (BODY& b : m_vecBodies)
    {
        if (!b.bActive || b.eType != DYNAMIC)
            continue;

        // Linear
        if (!VectorHelper::Is_Zero(b.vForceAccum))
        {
            b.vLinearVel += b.vForceAccum * b.fInvMass * fTimeDelta;
        }

        // Angular
        if (!VectorHelper::Is_Zero(b.vTorqueAccum))
        {
            // IinvWorld = R * IinvLocal * R^T
            Matrix matR = b.pTransform->Get_RotationMat();
            Matrix matRT = *D3DXMatrixTranspose(&matRT, &matR);

            // DX row-vector 규약 기준 관성 역텐서 변환
            // World(Inv_Inertia) = trans(R) * Local(Inv_Inerta) * R
            Matrix matInvInerta = matRT * b.matInvInertiaTensor * matR;

            Vec3 deltaW = *D3DXVec3TransformNormal(&deltaW, &b.vTorqueAccum, &matInvInerta);

            b.vAngularVel += deltaW * fTimeDelta;
        }

        // PhysicsUtil::ApplyPositionLock(b);
        // PhysicsUtil::ApplyRotationLock(b);

        // 초기화 : accum 값은 해당 프레임에만 적용된다. 
        b.vForceAccum = VectorHelper::Zero();
        b.vTorqueAccum = VectorHelper::Zero();
    }
}

void PhysicsWorld::Apply_Damping(float fTimeDelta)
{

    for (BODY& b : m_vecBodies)
    {
        if (!b.bActive || b.eType != DYNAMIC)
            continue;

        // Linear damping
        if (b.fDrag > 0.f)
        {
            b.vLinearVel -= b.vLinearVel * b.fDrag * fTimeDelta;

            if (D3DXVec3LengthSq(&b.vLinearVel) < fDampEps * fDampEps)
                b.vLinearVel = VectorHelper::Zero();
        }

        // Angular damping
        if (b.fAngularDrag > 0.f)
        {
            b.vAngularVel -= b.vAngularVel * b.fAngularDrag * fTimeDelta;

            if (D3DXVec3LengthSq(&b.vAngularVel) < fDampEps * fDampEps)
                b.vAngularVel = VectorHelper::Zero();
        }
    }
}

void PhysicsWorld::Integrate_Velocities(float fTimeDelta)
{
    for (BODY& b : m_vecBodies)
    {
        if (!b.bActive || b.eType != DYNAMIC)
            continue;

        // Linear integration
        Vec3 deltaPos = b.vLinearVel * fTimeDelta;
        b.vCOM += deltaPos;

        if (b.pTransform)
            b.pTransform->Translate(deltaPos);

        // Angular integration
        if (!VectorHelper::Is_Zero(b.vAngularVel) && b.pTransform)
        {
            Vec3 axis = VectorHelper::Get_Normalized(b.vAngularVel);
            float angle = D3DXVec3Length(&b.vAngularVel) * fTimeDelta;

            b.pTransform->Rotate(axis, angle);
        }
    }
}

void PhysicsWorld::Release()
{
	Safe_Release(m_pCollisionDetector);
	Safe_Release(m_pSolver);
}
