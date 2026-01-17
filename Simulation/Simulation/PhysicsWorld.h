#pragma once
#include "Physics_Struct.h"

class CollisionDetector;
class Solver;

class Rigidbody;

class PhysicsWorld
{
	DECLARE_SINGLETON(PhysicsWorld)
private:
	PhysicsWorld();
	~PhysicsWorld();

public :
	HRESULT			Ready_System(LPDIRECT3DDEVICE9 pGraphicDevice);
	int				Update_System(float fTimeDelta);

	list<CONTACT_INFO>		m_ContactInfosList;

	void					Add_ContactInfo(CONTACT_INFO tinfo);
	void					Add_ContactPair(PAIR_KEY key);

	void					Add_Collider(Collider* pCollider);
	void					Remove_Collider(Collider* pCollider);

	uint_fast32_t			Create_Body(BODY tBody);
	void					Remove_Body(uint_fast32_t iID);
	BODY*					Try_GetBody(uint_fast32_t iID);

	void					Invoke_CollisionEvent();

	const vector<Collider*>&		Get_Colliders() { return m_vecCollider; }
	Collider*						Find_ColliderByID(uint32_t id);

	bool					Detect_Ray(struct tagRay* pRay);

private :
	void					Accumulate_Forces();
	void					Integrate_Forces(float fTimeDelta);
	void					Apply_Damping(float fTimeDelta);
	void					Integrate_Velocities(float fTimeDelta);

private :
	LPDIRECT3DDEVICE9			m_pGraphicDevice;

	unordered_set<PAIR_KEY, PAIR_KEY_HASH>		m_prevPair;
	unordered_set<PAIR_KEY, PAIR_KEY_HASH>		m_CurPair;

	CollisionDetector*				m_pCollisionDetector;
	Solver*								m_pSolver;

	static unsigned int s_iCurColCnt;
	static unsigned int s_iCurCBodyCnt;

private:
	vector<Collider*>		m_vecCollider;
	vector<BODY>			m_vecBodies;
	std::vector<uint32_t> m_freeIds;
	const float m_fGravity = -9.81f;
	const Vec3 m_vGravity = { 0.f, m_fGravity, 0.f };
	const float fDampEps = 0.0001f;


public :
	void Release();
};

