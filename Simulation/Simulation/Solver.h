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
	void		Solve_Contacts(CONTACT_INFO* pInfo);

private :
	void		Solve_Impulse(CONTACT_INFO* pInfo);
	void		Solve_Penetration(CONTACT_INFO* pInfo);

	bool		Is_Seperating(CONTACT_INFO* pInfo);

	Vec3		Calc_PointVelocity(BODY* b, const Vec3& vPoint);
	float		Calc_InvInertiaOfAxis(Transform* pTransform, BODY* b, const Vec3& vAxis);

	void		Add_ImpulseAtPoint(Transform* pTransform, BODY* b, const Vec3& impulse, const Vec3& point);

private :
	LPDIRECT3DDEVICE9		m_pGraphicDevice;

	const float m_fSloap = 0.001f;

public :
	static Solver* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release();
};

