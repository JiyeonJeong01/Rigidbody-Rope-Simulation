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
	// ============ 멤버 함수 ============
	HRESULT				Ready_Component(BODY* b); // Find~ 함수들 실행하기
	int					Update_Component(const float& fTimeDelta)override;					// m_fLinearVel, COM, m_fAngularVel, m_vLook 설정
	void				LateUpdate_Component(const float& fTimeDelta) override;
	void				Step();

	void				Find_Dimension(BODY* b); // m_vDimension과 m_vDimensionCenter를 구한다
	void				Find_Inertia(BODY* b);	
	void				Find_COM(BODY* b);
	float				Find_InvInertiaOfAxis(const Vec3& vAxis);

	void				Translate(const Vec3 vDeltaPos);

	void				Integrate_Transform(const float& fTimeDelta);		// COM과 Transform의 위치를 vTrans만큼 이동

	Vec3				Acclerate_Gyro(const float& fTimeDelta);

	void				Add_ImpulseAtPoint(const Vec3& impulse, const Vec3& point); // m_fLinearVel, m_fAngularVel 반영
	void				Add_LinearImpulse(Vec3 vImpulse);
	void				Add_Force(Vec3 vForce, FORCE_MODE eForce);
	void				Add_Torque(Vec3 vTorque, FORCE_MODE eForce);

//private :
//	void				Apply_Drag(const float& fTimeDelta);
//	void				Apply_AngularDrag(const float& fTimeDelta);
//	void				Apply_Gravity(const float& fTimeDelta);

public:
	uint_fast16_t		Get_BodyID() const { return m_iId; }
	void				Set_BodyID(uint_fast16_t iId) { m_iId = iId; }
	BODY*				Try_GetMyBody() { return PhysicsWorld::GetInstance()->Try_GetBody(m_iId);}

	Vec3				Get_PointVelocity(const Vec3& vPoint);
	Vec3				Get_COM();

	void				Set_GeometryType(GEOMETRY_TYPE eGeometry)	{ m_eGeometryType = eGeometry; }
	GEOMETRY_TYPE		Get_GeometryType() const										 { return m_eGeometryType; }

private:
	// ============ 멤버 변수 ============

	BODY							m_tBody;

	COLLIDER_TYPE					m_eColType;
	GEOMETRY_TYPE					m_eGeometryType;

	Transform*						m_pTransform;
	VIBuffer*						m_pVIBuffer;

	const float						fEpsilon = 0.01f;
	uint_fast16_t					m_iId;

public:
	static Rigidbody* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner, BODY tBody);
	void Release() override;
};

