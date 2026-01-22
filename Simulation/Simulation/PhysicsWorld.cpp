#include "pch.h"
#include "PhysicsWorld.h"

#include "CollisionDetector.h"
#include "Solver.h"
#include "UniformGrid.h"
#include "PhysicsUtil.h"

#include "Object.h"
#include "Collider.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(PhysicsWorld)

unsigned int PhysicsWorld::s_iCurColCnt = 0;
unsigned int PhysicsWorld::s_iCurCBodyCnt = 0;

PhysicsWorld::PhysicsWorld()
    : m_pGraphicDevice(nullptr), m_pCollisionDetector(nullptr), m_pGrid(nullptr), m_pSolver(nullptr)
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

    m_pGrid = UniformGrid::Create(pGraphicDevice,
        {g_fStartX, -5.f, g_fStartZ},
        {g_fStartX + g_iCntX * g_fSize, g_fSize * 2.f, g_fStartZ + g_fSize * g_iCntZ},
                    g_fSize * 0.5f);

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

    // Detect Collision and Generate Contact Info
    for (auto* p : m_AllColliders)
        if (p) p->Update_AABB();

    vector<COLLIDER_PAIR> pairs;
    m_pCollisionDetector->Generate_BroadPhase_Pairs(m_AllColliders, pairs);
    m_pCollisionDetector->Process_NarrowPhase(pairs);

    // Solve
    for (int i = 0; i < 1; ++i)
        for (auto& contact : m_ContactInfosList)
            m_pSolver->Solve_Contacts(&contact);

    // Apply Lock
    for (BODY_DESC& b : m_vecBodies)
    {
        PhysicsUtil::ApplyPositionLock(b);
        PhysicsUtil::ApplyRotationLock(b);
    }

    Invoke_CollisionEvent();
	return 0;
}

void PhysicsWorld::Add_ContactInfo(CONTACT_DESC tinfo)
{
	m_ContactInfosList.push_back(tinfo);
}

void PhysicsWorld::Add_ContactPair(PAIR_KEY key)
{
	m_CurPair.insert(key);
}

void PhysicsWorld::Add_Collider(Collider* pCollider)
{
	m_AllColliders.push_back(pCollider);
    pCollider->Set_ColliderID(s_iCurColCnt++);
}

void PhysicsWorld::Remove_Collider(Collider* pCollider)
{
	if (!pCollider)
		return;

	auto it = std::remove(m_AllColliders.begin(), m_AllColliders.end(), pCollider);
	m_AllColliders.erase(it, m_AllColliders.end());
}

uint_fast32_t PhysicsWorld::Create_Body(BODY_DESC body)
{
    if (body.eBodyType == STATIC)
        PhysicsUtil::Set_StaticBody(body);
    else if (body.eBodyType == KINEMATIC)
        PhysicsUtil::Set_KinematicBody(body);
    else if (body.eBodyType == DYNAMIC)
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
    if (BODY_DESC* b = Try_GetBody(iID))
    {
        b->bActive = false;
        b->vForceAccum = { 0,0,0 };
        b->vTorqueAccum = { 0,0,0 };
        b->vLinearVel = { 0,0,0 };
        b->vAngularVel = { 0,0,0 };
    }
    m_freeIds.push_back(iID);
}

BODY_DESC* PhysicsWorld::Try_GetBody(uint_fast32_t iID)
{
    if (iID >= m_vecBodies.size())
        return nullptr;

    BODY_DESC& b = m_vecBodies[iID];
    if (!b.bActive)
        return nullptr;

    return &b;
}

bool PhysicsWorld::Detect_Ray(tagRaycastHit* tOut, tagRay* pRay)
{
    return m_pCollisionDetector->Detect_Ray(tOut, pRay);
}

void PhysicsWorld::Invoke_CollisionEvent()
{
    // Enter / Stay
    for (const PAIR_KEY& k : m_CurPair)
    {
        Collider* pA = Find_ColliderByID(k.aKey);
        Collider* pB = Find_ColliderByID(k.bKey);
        if (!pA || !pB) continue;

        COLLISION_DESC tA{};
        COLLISION_DESC tB{};

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

        COLLISION_DESC tA{};
        COLLISION_DESC tB{};

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
    for (Collider* pCol : m_AllColliders)
    {
        if (pCol->Get_ColliderID() == id)
            return pCol;
    }
    return nullptr;
}

void PhysicsWorld::Query_StaticOverlap(const AABB_DESC& tAABB, _Out_ vector<Collider*>* outOverlaps)
{
    m_pGrid->Query_StaticOverlap(tAABB, outOverlaps);
}

void PhysicsWorld::Find_StaticCollider()
{
    for (auto* p : m_AllColliders)
        if (p)
            p->Update_AABB();

    m_pGrid->Find_StaticCollider(m_AllColliders);
}

void PhysicsWorld::Accumulate_Forces()
{
    for (BODY_DESC& b : m_vecBodies)
    {
        if (!b.bActive || b.eBodyType != DYNAMIC)
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
    for (BODY_DESC& b : m_vecBodies)
    {
        if (!b.bActive || b.eBodyType != DYNAMIC)
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

         PhysicsUtil::ApplyPositionLock(b);
         PhysicsUtil::ApplyRotationLock(b);

        // 초기화 : accum 값은 해당 프레임에만 적용된다. 
        b.vForceAccum = VectorHelper::Zero();
        b.vTorqueAccum = VectorHelper::Zero();
    }
}

void PhysicsWorld::Apply_Damping(float fTimeDelta)
{

    for (BODY_DESC& b : m_vecBodies)
    {
        if (!b.bActive || b.eBodyType != DYNAMIC)
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
    for (BODY_DESC& b : m_vecBodies)
    {
        if (!b.bActive || b.eBodyType != DYNAMIC)
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
