#pragma once

#include "PhysicsWorld.h"
class Object;
class Rigidbody;
class Transform;

class Solver
{
	Solver();
	~Solver();

public :
	HRESULT		Ready_System();
	void		Solve_Contacts(CONTACT_DESC* pInfo);

private :
	void		Solve_ImpulseAndFriction(CONTACT_DESC* pInfo);
	void		Solve_Penetration(CONTACT_DESC* pInfo);

	bool		Is_Seperating(CONTACT_DESC* pInfo);

	Vec3		Calc_PointVelocity(BODY_DESC* b, const Vec3& vPoint);
	float		Calc_InvInertiaOfAxis(Transform* pTransform, BODY_DESC* b, const Vec3& vAxis);

	void		Add_ImpulseAtPoint(Transform* pTransform, BODY_DESC* b, const Vec3& impulse, const Vec3& point);

private :
	LPDIRECT3DDEVICE9		m_pGraphicDevice;

	const float m_fSloap = 0.001f;

public :
	static Solver* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release();
};

