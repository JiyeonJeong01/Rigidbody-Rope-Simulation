#include "pch.h"
#include "Rigidbody.h"
#include "Object.h"
#include "Transform.h"
#include "VIBuffer.h"
#include "VectorHelper.h"

Rigidbody::Rigidbody(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Component(pGraphicDev, pOwner)
	, m_fMass(0), m_fMassI(0), m_fDrag(0), m_fRestritution(0)
	, m_iRotFreezeMask(0), m_bFixed(false), m_bKinematic(false)
	, m_eGeometryType(), m_pTransform(nullptr), m_pVIBuffer(nullptr)
{
	ZeroMemory(m_vCOM, sizeof(Vec3));
	ZeroMemory(m_vDimension, sizeof(Vec3));
	ZeroMemory(m_vDimensionCenter, sizeof(Vec3));
	ZeroMemory(m_vLinearVel, sizeof(Vec3));
	ZeroMemory(m_vLook, sizeof(Vec3));
	ZeroMemory(m_vAngularVel, sizeof(Vec3));
}

Rigidbody::~Rigidbody()
{
}

HRESULT Rigidbody::Ready_Component()
{
	m_pTransform = static_cast<Transform*>(m_pOwner->Find_Component(L"Transform"));
	m_pVIBuffer = static_cast<VIBuffer*>(m_pOwner->Find_Component(L"Buffer"));

	Find_Inertia();
	Find_Dimension();
	Find_ColliderRadius();

	/*
	* TODO : 회전 어떻게 적용할 건지 고민!
	* 현재 VRAM의 버텍스 버퍼를 한 번 생성한 뒤 수정하지 않는 구조인데, 회전을 어떻게 적용하는가에 따라
	* 원본 버텍스를 별도로 저장해야 할 수 있다. 
	*/

	return S_OK;
}

void Rigidbody::Find_Dimension()
{
	UINT iVtxCnt = 0;
	iVtxCnt = m_pVIBuffer->Get_VIBufferInfo().dwVtxCnt;

	VTXCOL* pVertices = nullptr;
	m_pVIBuffer->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	float fMaxX(-INFINITY), fMinX(INFINITY);
	float fMaxY(-INFINITY), fMinY(INFINITY);
	float fMaxZ(-INFINITY), fMinZ(INFINITY);

	for (UINT i = 0; i < iVtxCnt; ++i)
	{
		const auto& pos = pVertices[i].vPosition;

		fMinX = min(fMinX, pos.x);
		fMaxX = max(fMaxX, pos.x);

		fMinY = min(fMinY, pos.y);
		fMaxY = max(fMaxY, pos.y);

		fMinZ = min(fMinZ, pos.z);
		fMaxZ = max(fMaxZ, pos.z);
	}

	m_vDimension = Vec3(fMaxX - fMinX, fMaxY - fMinY, fMaxZ - fMinZ);
	m_vDimensionCenter = Vec3(fMaxX + fMinX, fMaxY + fMinY, fMaxZ + fMinZ) * 0.5f;
}

void Rigidbody::Find_Inertia()
{
	// Rigidbody 모양에 따른 Inertia Tensor 계산하기 
	D3DXMatrixIdentity(&m_matInertiaTensor);

	switch (m_eGeometryType)
	{
	case SPHERE:
		{
		float fInertia = (2.f / 5.f) * m_fMass * powf(m_vDimension.x * 0.5f, 2.f);

		m_matInertiaTensor.m[0][0] = fInertia;
		m_matInertiaTensor.m[1][1] = fInertia;
		m_matInertiaTensor.m[2][2] = fInertia;

		D3DXMatrixInverse(&m_matInertiaTensorInv, 0, &m_matInertiaTensor);
		}
		break;
	case BOX:
		{
		float k = (1.f / 12.f) * m_fMass;

		m_matInertiaTensor.m[0][0] = k * (m_vDimension.y * m_vDimension.y + m_vDimension.z * m_vDimension.z);
		m_matInertiaTensor.m[1][1] = k * (m_vDimension.x * m_vDimension.x + m_vDimension.z * m_vDimension.z);
		m_matInertiaTensor.m[2][2] = k * (m_vDimension.x * m_vDimension.x + m_vDimension.y * m_vDimension.y);

		D3DXMatrixInverse(&m_matInertiaTensorInv, 0, &m_matInertiaTensor);
		}
		break;

	}
}

void Rigidbody::Find_ColliderRadius()
{
	// TODO : 콜라이더 구현 이후 
}

void Rigidbody::Add_LinearImpulse(Vec3 vVel)
{
	if (m_bFixed | m_bKinematic)
		return;

	m_vLinearVel += vVel;
}

void Rigidbody::Translate(const Vec3 vDeltaPos)
{
	m_vCOM += vDeltaPos;
	m_pTransform->Translate(vDeltaPos);
}

void Rigidbody::Integrate_Transform(const float& fTimeDelta)
{
	// 선 속도 적용
	Vec3 vMoveDelta = m_vLinearVel * fTimeDelta;
	m_vCOM += vMoveDelta;

	// 각 속도 적용
	if (!VectorHelper::Is_ZeroVector(m_vAngularVel))
	{
		// TODO : 보정 로직 추가하기 
		// m_vAngularVel = Acclerate_Gyro(fTimeDelta);
		Vec3 vRotAxis = VectorHelper::Get_Normalized(m_vAngularVel) * fTimeDelta;
		float fRotMagnitude = D3DXVec3Length(&m_vAngularVel);

		m_pTransform->Rotate(vRotAxis, fRotMagnitude);
		m_pTransform->Get_Info(AXIS_Z, &m_vLook);
	}
	else
	{
		m_pTransform->Translate(vMoveDelta);
	}
}

Vec3 Rigidbody::Acclerate_Gyro(const float& fTimeDelta)
{
	//Matrix matRot = m_pTransform->Get_RotationMat();
	//Matrix matRotInv = *D3DXMatrixInverse(&matRotInv, 0, &matRot);

	//Vec3 vWB = *D3DXVec3TransformNormal(&vWB, &m_vAngularVel, &matRotInv);
	//Vec3 vW2B = vWB;

	//int nItr = 1;

	//for (int i = 0; i < nItr; ++i)
	//{
	//	Vec3 vTemp = *D3DXVec3TransformNormal(&vTemp, &vW2B, &m_matInertiaTensor);
	//	D3DXVec3Cross(&vTemp, &vt)

	//}

	return Vec3();
}

int Rigidbody::Update_Component(const float& fTimeDelta)
{
	m_vCOM = m_pTransform->Get_Position();

	Apply_Drag(fTimeDelta);
	Apply_Gravity(fTimeDelta);

	Integrate_Transform(fTimeDelta);

	return 0;
}

void Rigidbody::LateUpdate_Component(const float& fTimeDelta)
{
}

void Rigidbody::Add_ImpulseAtPoint(Vec3 vImpulse, Vec3 vPos, float fMass)
{
	if (m_bFixed || m_bKinematic)
		return;

	// ===== 1. Linear Velocity =====
	// 선 속도 += (impulse / 질량)
	m_vLinearVel += vImpulse * m_fMassI;

	// ===== 2. Angular Velocity =====
	//  r = position - centerOfMass : 질량 중심에서 힘이 작용한 위치까지의 벡터 
	Vec3 vR = vPos - m_vCOM;

	// 각 운동량 L = r × impulse
	Vec3 vAngularMomentum = -1.f * VectorHelper::CrossProduct(vR, vImpulse); // 왼손 좌표계 

	// ===== 3. Inertia Tensor =====
	// Inverse(I_World) = R * Inverse(I_Local) * Transpose(R)
	Matrix matR = m_pTransform->Get_RotationMat();
	Matrix matRT = *D3DXMatrixTranspose(&matRT, &matR);
	Matrix matInertiaInv = matR * m_matInertiaTensorInv * matRT;

	// ===== 4. 각속도 변화량, vDeltaW 구하기 =====
	Vec3 vDeltaW = *D3DXVec3TransformNormal(&vDeltaW, &vAngularMomentum, &matInertiaInv);

	m_vAngularVel += vDeltaW;
}

void Rigidbody::Add_Force(Vec3 vImpulse, FORCE_MODE eForce)
{
	const float fTimeDelta = 0.016f;
	switch (eForce)
	{
	case FORCE_MODE::FORCE :
		{
			// 질량을 고려하여 지속적으로 힘을 가한다.
			vImpulse *= fTimeDelta;
		}
		break;
	case FORCE_MODE::IMPULSE :
		{
			// 질량을 고려하여 순간적인 힘을 가한다.
		}
		break;
	}

	Add_ImpulseAtPoint(vImpulse, m_vCOM, 1.f);
}

void Rigidbody::Apply_Drag(const float& fTimeDelta)
{
	// 유체 저항 적용하기 
	if (m_fDrag > 0.f)
	{
		m_vLinearVel -= m_vLinearVel * m_fDrag * fTimeDelta;
		m_vAngularVel -= m_vAngularVel * m_fDrag * fTimeDelta;

		if (D3DXVec3LengthSq(&m_vLinearVel) < fEpsilon * fEpsilon)
			m_vLinearVel = VectorHelper::Zero();

		if (D3DXVec3LengthSq(&m_vAngularVel) < fEpsilon * fEpsilon)
			m_vAngularVel = VectorHelper::Zero();
	}
}

void Rigidbody::Apply_Gravity(const float& fTimeDelta)
{
	Vec3 vGravity{ 0.f, -9.81f, 0.f };
	m_vLinearVel += vGravity * fTimeDelta;

	float fTmpY = m_pTransform->Get_Position().y + m_vLinearVel.y * fTimeDelta;

	if (fTmpY < -0.1f)
	{
		m_vLinearVel.y = 0.f;

		Vec3 vPos = m_pTransform->Get_Position();
		vPos.y = 0.f;
		m_pTransform->Set_Position(vPos);
	}
	printf("%.2f\n", m_pTransform->Get_Position().y);
}

Rigidbody* Rigidbody::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
{
	Rigidbody* pBody = new Rigidbody(pGraphicDev, pOwner);
	if (FAILED(pBody->Ready_Component()))
	{
		Safe_Delete(pBody);
	}
	pOwner->Add_Component(L"Rigidbody", pBody);
	return pBody;
}

void Rigidbody::Release()
{
	Component::Release();
}
