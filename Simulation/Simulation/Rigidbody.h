#pragma once
#include "Component.h"
#include "PhysicsWorld.h"
#include "Physics_Struct.h"

class Object;
class Transform;
class VIBuffer;

class Rigidbody : public Component
{
private:
	Rigidbody(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~Rigidbody() override;

public :
	HRESULT			Ready_Component(BODY* b); // Find~ 함수들 실행하기
	int				Update_Component(const float& fTimeDelta)override;					// m_fLinearVel, COM, m_fAngularVel, m_vLook 설정
	void			LateUpdate_Component(const float& fTimeDelta) override;
	void			Fixed_Update(const float& fTimeDelta);

	void			Translate(const Vec3 vDeltaPos);

	void			Add_LinearImpulse(Vec3 vImpulse);
	void			Add_Force(Vec3 vForce);
	void			Add_Torque(Vec3 vTorque);

	void			Calc_Dimension(BODY* b); // m_vDimension과 m_vDimensionCenter를 구한다
	void			Calc_Inertia(BODY* b);	
	void			Calc_COM(BODY* b);
	float			Calc_InvInertiaOfAxis(const Vec3& vAxis);


public:
	uint_fast16_t	Get_BodyID() const { return m_iId; }
	void			Set_BodyID(uint_fast16_t iId) { m_iId = iId; }
	BODY*			Try_GetMyBody() { return PhysicsWorld::GetInstance()->Try_GetBody(m_iId);}

	Vec3			Get_PointVelocity(const Vec3& vPoint);
	Vec3			Get_COM();

	void			Set_GeometryType(GEOMETRY_TYPE eGeometry)	{ m_eGeometryType = eGeometry; }
	GEOMETRY_TYPE	Get_GeometryType() const					{ return m_eGeometryType; }

private:
	// ============ 멤버 변수 ============
	BODY			m_tBody;

	BODY_TYPE		m_eCachedBodyType;
	GEOMETRY_TYPE	m_eGeometryType;

	Transform*		m_pTransform;

	const float		fEpsilon = 0.01f;
	uint_fast16_t	m_iId;

public:
	static Rigidbody* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner, BODY tBody);
	void Release() override;
};

