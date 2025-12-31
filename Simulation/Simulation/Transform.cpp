#include "pch.h"
#include "Transform.h"

Transform::Transform(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDevice(pGraphicDev), m_vScale(1.f, 1.f, 1.f), m_vRadian(0.f, 0.f, 0.f)
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
	m_vRadian = Vec3((D3DXToRadian(m_vDegree.x)), (D3DXToRadian(m_vDegree.y)), (D3DXToRadian(m_vDegree.z)));

	Matrix matRot[AXIS_END];
	D3DXMatrixRotationX(&matRot[AXIS_X], m_vRadian.x);
	D3DXMatrixRotationY(&matRot[AXIS_Y], m_vRadian.y);
	D3DXMatrixRotationZ(&matRot[AXIS_Z], m_vRadian.z);

	for (int i = 0; i < AXIS_END; ++i)
	{
		for (int j = 0; j < AXIS_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vRotation[i], &m_vRotation[i], &matRot[j]);
		}
	}

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

Transform* Transform::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Transform* pTransform = new Transform(pGraphicDev);

	if (FAILED(pTransform->Ready_Component()))
	{
		delete pTransform;
		pTransform = nullptr;
	}

	return pTransform;
}

void Transform::Release()
{

}
