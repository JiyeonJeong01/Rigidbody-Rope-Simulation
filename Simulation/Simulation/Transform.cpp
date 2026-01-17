#include "pch.h"
#include "Transform.h"
#include "Object.h"
#include "MathHelper.h"
#include "VectorHelper.h"

Transform::Transform(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Component(pGraphicDev, pOwner)
		, m_vScale(1.f, 1.f, 1.f), m_vQuaternion(0.f, 0.f, 0.f, 1.f)
{
	ZeroMemory(m_vRotation, sizeof(m_vRotation));
	
	D3DXMatrixIdentity(&m_matWorld);
}

Transform::~Transform()
{
}

HRESULT Transform::Ready_Component()
{
	D3DXMatrixIdentity(&m_matWorld);

	for (int i = 0; i < AXIS_END; ++i)
	{
		memcpy(&m_vRotation[i], &m_matWorld.m[i][0], sizeof(Vec3));
	}

	m_vPosition = { 0.f, 0.f, 0.f };

	return S_OK;
}

int Transform::Update_Component(const float& fTimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);

	// Clear prev info
	for (int i = 0; i < AXIS_END; ++i)
	{
		memcpy(&m_vRotation[i], &m_matWorld.m[i][0], sizeof(Vec3));
	}

	// 스케일
	for (int i = 0; i < AXIS_END; ++i)
	{
		m_vRotation[i] *= *((float*)&m_vScale + i);
	}

	// 회전 계산
	D3DXQUATERNION qRot (m_vQuaternion.x, m_vQuaternion.y, m_vQuaternion.z, m_vQuaternion.w);

	Matrix	matRot;
	D3DXMatrixRotationQuaternion(&matRot, &qRot);

	m_vRotation[AXIS_X] = Vec3(matRot._11, matRot._12, matRot._13);
	m_vRotation[AXIS_Y] = Vec3(matRot._21, matRot._22, matRot._23);
	m_vRotation[AXIS_Z] = Vec3(matRot._31, matRot._32, matRot._33);

	// 크기, 회전, 위치를 월드 행렬에 복사
	for (int i = 0; i < AXIS_END; ++i)
	{
		memcpy(&m_matWorld.m[i][0], &m_vRotation[i], sizeof(Vec3));
	}

	memcpy(&m_matWorld.m[3][0], & m_vPosition, sizeof(Vec3));

	return 0;
}

void Transform::LateUpdate_Component(const float& fTimeDelta)
{
}

void Transform::Rotate(AXIS eAxis, const float& fDegree)
{
	Vec3 vAxis(0.f, 0.f, 0.f);
	if (eAxis == AXIS_X) vAxis = VectorHelper::Right();
	else if (eAxis == AXIS_Y) vAxis = VectorHelper::Up();
	else if (eAxis == AXIS_Z) vAxis = VectorHelper::Look();

	Rotate(vAxis, fDegree);
}

void Transform::Rotate(const Vec3& vAxis, const float& fDegree)
{
	float fRadian = D3DXToRadian(fDegree);

	D3DXQUATERNION qCur(m_vQuaternion.x, m_vQuaternion.y, m_vQuaternion.z, m_vQuaternion.w);
	D3DXQUATERNION qDelta;
	D3DXQuaternionRotationAxis(&qDelta, &vAxis, fRadian);

	qCur = qCur * qDelta;

	D3DXQuaternionNormalize(&qCur, &qCur);

	m_vQuaternion = Vec4(qCur.x, qCur.y, qCur.z, qCur.w);
}

void Transform::Set_Rotation(float fX, float fY, float fZ)
{
	m_vEuler = Vec3(fX, fY, fZ);
	MathHelper::Euler_ToQuaternion(m_vEuler, m_vQuaternion);
}

void Transform::Set_Rotation(const Vec3& vAngle)
{
	m_vEuler = vAngle;
	MathHelper::Euler_ToQuaternion(m_vEuler, m_vQuaternion);
}

Vec3 Transform::Get_RotationAxis(AXIS eAxis)
{
	D3DXQUATERNION qRot(m_vQuaternion.x, m_vQuaternion.y, m_vQuaternion.z, m_vQuaternion.w);
	Matrix	matRot;
	D3DXMatrixRotationQuaternion(&matRot, &qRot);

	if(eAxis == AXIS_X) 
		return Vec3(matRot._11, matRot._12, matRot._13);
	if (eAxis == AXIS_Y)
		return Vec3(matRot._21, matRot._22, matRot._23);
	if (eAxis ==AXIS_Z)
		return Vec3(matRot._31, matRot._32, matRot._33);
	return Vec3();
}

Matrix Transform::Get_RotationMat()
{
	Matrix R;
	D3DXQUATERNION q(m_vQuaternion.x, m_vQuaternion.y, m_vQuaternion.z, m_vQuaternion.w);

	D3DXQuaternionNormalize(&q, &q);

	D3DXMatrixRotationQuaternion(&R, &q);
	return R;
}


Transform* Transform::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
{
	Transform* pTransform = new Transform(pGraphicDev, pOwner);

	if (FAILED(pTransform->Ready_Component()))
	{
		delete pTransform;
		pTransform = nullptr;
	}
	pOwner->Add_Component(L"Transform", pTransform);
	pOwner->Set_Transform(pTransform);

	return pTransform;
}

void Transform::Release()
{
	Component::Release();
}

