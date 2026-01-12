#pragma once

#include "PhysicsWorld.h"
class Object;
class Rigidbody;
class Solver
{
	Solver();
	~Solver();

	struct ImpulseInfo
	{
		Vec3 vVelocity;
		Vec3 vCOM;
		float fRestitution;
		float fInvMass;
	};

public :
	HRESULT		Ready_System();
	void Solve_Contacts(CONTACT_INFO* pInfo);

private :
	void		Solve_Impulse(CONTACT_INFO* pInfo);
	void		Solve_Penetration(CONTACT_INFO* pInfo);
	void		Solve_Friction(CONTACT_INFO* pInfo);

	void		Impulse_StaticCollider(Object* pObejct, Collider* pCollider);

	float		Get_InvMass(Collider* pCollider);
	bool		Is_Seperating(CONTACT_INFO* pInfo);

private :
	LPDIRECT3DDEVICE9		m_pGraphicDevice;
	const float m_fSloap = 0.001f;
	Rigidbody* m_pStaticBody;

public :
	static Solver* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release();
};

