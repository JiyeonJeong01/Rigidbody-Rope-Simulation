#pragma once

#include "PhysicsWorld.h"
class Rigidbody;
class Solver
{
	Solver();
	~Solver();

public :
	HRESULT		Ready_System();
	void Solve_Contacts(CONTACT_INFO* pInfo);

private :
	void		Solve_Impulse(CONTACT_INFO* pInfo);
	void		Solve_Penetration(CONTACT_INFO* pInfo);

	float		Get_InvMass(Collider* pCollider);
	bool		Is_Seperating(CONTACT_INFO* pInfo);

private :
	const float m_fSloap = 0.001f;
	Rigidbody* m_pStaticBody;

public :
	static Solver* Create();
	void Release();
};

