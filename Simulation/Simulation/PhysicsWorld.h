#pragma once
#include "Physics_Struct.h"

class CollisionDetector;
class Solver;

class PhysicsWorld
{
	DECLARE_SINGLETON(PhysicsWorld)
private:
	PhysicsWorld();
	~PhysicsWorld();

public :
	HRESULT		Ready_System(LPDIRECT3DDEVICE9 pGraphicDevice);
	int				Update_System();

	list<CONTACT_INFO>		m_ContactInfosList;

	void					Add_ContactInfo(CONTACT_INFO tinfo);
	void					Add_ContactPair(PAIR_KEY key);

	void					Add_Collider(Collider* pCollider);
	void					Remove_Collider(Collider* pCollider);

	void					Invoke_CollisionEvent();

	const vector<Collider*>&		Get_Colliders() { return m_vecCollider; }
	Collider*									Find_ColliderByID(uint32_t id);

private :
	LPDIRECT3DDEVICE9			m_pGraphicDevice;

	unordered_set<PAIR_KEY, PAIR_KEY_HASH>		m_prevPair;
	unordered_set<PAIR_KEY, PAIR_KEY_HASH>		m_CurPair;

	CollisionDetector*				m_pCollisionDetector;
	Solver*								m_pSolver;

	static unsigned int s_iCurCnt;

private:
	vector<Collider*>		m_vecCollider;
	const float m_fGravity = -9.81f;

public :
	void Release();
};

