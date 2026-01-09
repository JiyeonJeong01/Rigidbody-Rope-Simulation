#include "pch.h"
#include "PhysicsWorld.h"
#include "Object.h"
#include "Collider.h"
#include "CollisionDetector.h"
#include "Solver.h"

IMPLEMENT_SINGLETON(PhysicsWorld)

unsigned int PhysicsWorld::s_iCurCnt = 0;

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
	m_pSolver = Solver::Create();

	return S_OK;
}

int PhysicsWorld::Update_System()
{
	m_CurPair.clear();
    m_ContactInfosList.clear();

	// Generate 
	m_pCollisionDetector->Generate_ContactInfo();

	// 
	for (auto& contact : m_ContactInfosList)
	{
		m_pSolver->Solve_Contacts(&contact);
	}

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
    pCollider->Set_ColliderID(s_iCurCnt++);
}

void PhysicsWorld::Remove_Collider(Collider* pCollider)
{
	if (!pCollider)
		return;

	// TODO : KeyPair도 삭제해야 한다 

	auto it = std::remove(m_vecCollider.begin(), m_vecCollider.end(), pCollider);
	m_vecCollider.erase(it, m_vecCollider.end());
}

void PhysicsWorld::Invoke_CollisionEvent()
{
    // Enter / Stay : 이번 프레임에 충돌한 pair들만 순회
    for (const PAIR_KEY& k : m_CurPair)
    {
        Collider* pA = Find_ColliderByID(k.aKey);
        Collider* pB = Find_ColliderByID(k.bKey);
        if (!pA || !pB) continue;

        // TODO : 필요한 정보가 뭘지 다 짜고 보가ㅣ 
        Collision tA{};
        Collision tB{};

        if (m_prevPair.find(k) == m_prevPair.end())
        {
            // Enter
            pA->Get_Object()->On_CollisionEnter(tA);
            pB->Get_Object()->On_CollisionEnter(tB);
        }
        else
        {
            // Stay
            pA->Get_Object()->On_CollisionStay(tA);
            pB->Get_Object()->On_CollisionStay(tB);
        }
    }

    // Exit : 지난 프레임에는 있었는데 이번 프레임엔 없는 pair들
    for (const PAIR_KEY& k : m_prevPair)
    {
        if (m_CurPair.find(k) != m_CurPair.end())
            continue;

        Collider* pA = Find_ColliderByID(k.aKey);
        Collider* pB = Find_ColliderByID(k.bKey);
        if (!pA || !pB) continue;

        Collision tA{};
        Collision tB{};

        pA->Get_Object()->On_CollisionExit(tA);
        pB->Get_Object()->On_CollisionExit(tB);
    }

    // 프레임 마무리: 이번 pair를 다음 프레임 prev로
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

void PhysicsWorld::Release()
{
	Safe_Release(m_pCollisionDetector);
	Safe_Release(m_pSolver);
}

// TODO : !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 구조 바꾸기!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Fixed_Update로 돌려야 하는 건지? 그럼 Update에서의 transform 조정 에 대한 충돌은 어떻게 해결할 건지? 
// https://chatgpt.com/c/69608a53-b5b0-8322-8032-bf02dcce763c
// https://chatgpt.com/c/6960a496-6088-8329-8287-a792784b023a
