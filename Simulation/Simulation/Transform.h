#pragma once
class Transform
{
private:
	Transform(LPDIRECT3DDEVICE9 pGraphicDev);
	~Transform();

public:
	HRESULT		Ready_Component();
	int				Update_Component(const float& fTimeDelta);
	void				LateUpdate_Component(const float& fTimeDelta);

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
	const Vec3& Get_Rotation() { return m_vEuler; }

	void Set_Position(float fX, float fY, float fZ) { m_vPosition = Vec3(fX, fY, fZ); }
	void Set_Position(const Vec3& vPosition) { m_vPosition = vPosition; }

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

	LPDIRECT3DDEVICE9 m_pGraphicDevice;

public:
	static Transform* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void Release();
};

