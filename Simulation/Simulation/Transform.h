#pragma once
#include "Component.h"

class Object;

class Transform : public  Component
{
private:
	Transform(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~Transform();

public:
	HRESULT		Ready_Component() override;
	int				Update_Component(const float& fTimeDelta) override;
	void				LateUpdate_Component(const float& fTimeDelta) override;

public:
	void Translate(const Vec3& vDeltaPos)
	{
		m_vPosition += vDeltaPos;
	}
	void Rotate(AXIS eAxis, const float& fAngle);
	void Rotate(const Vec3& vAxis, const float& fAngle);

	// getter/setter
	void Set_Scale(const Vec3& vScale) { m_vScale = vScale; }
	const Vec3& Get_Scale() { return m_vScale; }

	void Set_Rotation(float fX, float fY, float fZ);
	void Set_Rotation(const Vec3& vAngle);
	const Vec3& Get_RotationEuler() { return m_vEuler; }
	const Vec4& Get_RotationQuat() { return m_vQuaternion; }
	Matrix Get_RotationMat();

	void Set_Position(float fX, float fY, float fZ) { m_vPosition = Vec3(fX, fY, fZ); }
	void Set_Position(const Vec3& vPosition) { m_vPosition = vPosition; }
	const Vec3& Get_Position() { return m_vPosition; }

	Matrix* Get_WorldMatrix()
	{
		return &m_matWorld;
	}

	void Get_Info(AXIS eAxis, Vec3* pAxis) { memcpy(pAxis, &m_matWorld.m[eAxis][0], sizeof(Vec3)); }

private :
	void Euler_ToQuaternion();

private:
	Vec3		m_vScale;
	Vec3		m_vEuler;
	Vec4		m_vQuaternion;
	Vec3		m_vPosition;

	Vec3			m_vRotation[AXIS_END];
	Matrix		m_matWorld;

public:
	static Transform* Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	void Release() override;
};

