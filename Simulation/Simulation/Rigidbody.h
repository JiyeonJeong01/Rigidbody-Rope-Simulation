#pragma once
#include "Component.h"

class Object;
class Transform;
class VIBuffer;

class Rigidbody : public Component
{
private:
	Rigidbody(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~Rigidbody();

public :
	// ============ 멤버 함수 ============
	HRESULT Ready_Component() override; // Find~ 함수들 실행하기
	int			Update_Component(const float& fTimeDelta)override;					// m_fLinearVel, COM, m_fAngularVel, m_vLook 설정
	void			LateUpdate_Component(const float& fTimeDelta) override;

	void Find_Dimension(); // m_vDimension과 m_vDimensionCenter를 구한다
	void Find_Inertia();			//
	void Find_ColliderRadius();

	void Add_LinearImpulse(Vec3 vVel);		// m_fLinearVel += vVel
	void Translate(const Vec3 vDeltaPos);

	void Integrate_Transform(const float& fTimeDelta);		// COM과 Transform의 위치를 vTrans만큼 이동

	Vec3 Acclerate_Gyro(const float& fTimeDelta);							// 회전 저항 행렬을 고려한 각 속도 구하기

	void Add_ImpulseAtPoint(Vec3 vImpulse, Vec3 vPos, float fMassVal); // m_fLinearVel, m_fAngularVel 반영
	void Add_Force(Vec3 vImpulse, FORCE_MODE eForce);
	void Add_Torque(Vec3 vImpulse, FORCE_MODE eForce);

private :
	void Apply_Drag(const float& fTimeDelta);
	void Apply_AngularDrag(const float& fTimeDelta);
	void Apply_Gravity(const float& fTimeDelta);

public:
	void Set_LinearVelocity(const Vec3& vVel) { m_vLinearVel = vVel; }
	const Vec3& Get_LinearVelocity(const Vec3& vVel) const { return m_vLinearVel; }

	void Set_AngularVelocity(const Vec3 vVel) { m_vAngularVel = vVel; }
	const Vec3& Get_AngularVelocity(const Vec3 vVel) const { return m_vAngularVel; }

	bool Get_Fixed() { return m_bFixed; }
	void Set_Fixed(bool bFix) { m_bFixed = bFix; }

	bool Get_IsKinematic() { return m_bKinematic; }
	void Set_IsKinematic(bool bKinematic) { m_bKinematic = bKinematic; }

	void Set_GeometryType(GEOMETRY_TYPE eGeometry) { m_eGeometryType = eGeometry; }
	GEOMETRY_TYPE Get_GeometryType() const { return m_eGeometryType; }

	void Set_Mass(const float& fMass) { m_fMass = fMass; m_fMassI = 1.f/m_fMass; }
	float Get_Mass() const { return m_fMass; }

	void Set_Drag(const float& fDrag) { m_fDrag = fDrag; }
	float Get_Drag() const { return m_fDrag; }

	void Set_AngularDrag(const float& fDrag) { m_fAngularDrag = fDrag; }
	float Get_AngularDrag() const { return m_fAngularDrag; }

	void Set_Gravity(const float& bGravity) { m_bGravity = bGravity; }
	float Get_Gravity() const { return m_bGravity; }

private:
	// ============ 멤버 변수 ============
	float m_fMass, m_fMassI;				// 질량
	float m_fDrag, m_fAngularDrag;	// 저항 
	float m_fRestritution;						// 반발 계수
	Matrix m_matInertiaTensor, m_matInertiaTensorInv; // 회전 저항 행렬과 역행렬

	int m_iRotFreezeMask;			// 회전 잠금 축 마스크

	Vec3 m_vCOM;							// Center Of Mass : 강체의 위치. 질량 중심 위치
	Vec3 m_vLinearVel;					// 선 속도. COM의 이동 속도
	Vec3 m_vAngularVel;				// 각 속도. 회전 축과 회전 크기

	Vec3 m_vLook;						// 방향
	Vec3 m_vDimension;				// 바운드 박스 : 각 축에 대한 콜라이더 크기
	Vec3 m_vDimensionCenter;	// 바운드 박스의 중심
	GEOMETRY_TYPE	m_eGeometryType;

	bool				m_bFixed, m_bKinematic, m_bGravity;

	Transform* m_pTransform;
	VIBuffer* m_pVIBuffer;

	const float fEpsilon = 0.01f;

public:
	static Rigidbody* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	void Release() override;
};

