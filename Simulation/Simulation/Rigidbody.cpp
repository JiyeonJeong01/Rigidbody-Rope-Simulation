#include "pch.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "VIBuffer.h"
#include "VectorHelper.h"

Rigidbody::Rigidbody(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_fMass(0), m_fMassI(0), m_fFriction(0), m_fRestritution(0)
	  , m_iRotFreezeMask(0)
	  , m_eGeometryType(), m_pTransform(nullptr), m_pVIBuffer(nullptr)
{
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::Ready_Component()
{
	Find_Dimension();
	Find_Inertia();
	Find_ColliderRadius();

	/*
	* TODO : 회전 어떻게 적용할 건지 고민!
	* 현재 VRAM의 버텍스 버퍼를 한 번 생성한 뒤 수정하지 않는 구조인데, 회전을 어떻게 적용하는가에 따라
	* 원본 버텍스를 별도로 저장해야 할 수 있다. 
	*/
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
	switch (m_eGeometryType)
	{
	case SPHERE:
		{
		float fInertia = (2.f / 5.f) * m_fMass * powf(m_vDimension.x * 0.5f, 2.f);
		m_matInertiaTensor.m[0][0] = fInertia;
		m_matInertiaTensor.m[1][1] = fInertia;
		m_matInertiaTensor.m[2][2] = fInertia;
		D3DXMatrixInverse(&m_matInertiaTensorI, 0, &m_matInertiaTensor);
		}
		break;
	case BOX:
		{
		float k = (1.f / 12.f) * m_fMass;
		m_matInertiaTensor.m[0][0] = k * (m_vDimension.y * m_vDimension.y + m_vDimension.z * m_vDimension.z);
		m_matInertiaTensor.m[1][1] = k * (m_vDimension.x * m_vDimension.x + m_vDimension.z * m_vDimension.z);
		m_matInertiaTensor.m[2][2] = k * (m_vDimension.x * m_vDimension.x + m_vDimension.y * m_vDimension.y);

		D3DXMatrixInverse(&m_matInertiaTensorI, 0, &m_matInertiaTensor);
		}
		break;

	}


}

void Rigidbody::Find_ColliderRadius()
{
	// TODO : 콜라이더 구현 이후 
}

void Rigidbody::Acclerate_Linearly(Vec3 vVel)
{
	if (m_bFixed | m_bKinematic)
		return;

	m_vLinearVel += vVel;
}

void Rigidbody::Apply_Transform(Vec3 vTrans, const float& fTimeDelta)
{
	// 선 속도 적용
	Vec3 vMoveDelta = m_vLinearVel * fTimeDelta;
	m_vCOM += vMoveDelta;

	// 각 속도 적용
	if (!VectorHelper::Is_ZeroVector(m_vAngularVel))
	{
		m_vAngularVel = Acclerate_Gyro(fTimeDelta);
		Vec3 vRotAxis = VectorHelper::Get_Normalized(m_vAngularVel);
		float fRotMagnitude = D3DXVec3Length(&m_vAngularVel);

		m_pTransform->Rotate(vRotAxis, fRotMagnitude);
	}
	else
	{
		m_pTransform->Translate(vMoveDelta);
	}
}

const Vec3& Rigidbody::Acclerate_Gyro(const float& fTimeDelta)
{
	return Vec3();
}

void Rigidbody::Update_Component()
{
}

void Rigidbody::Apply_ImpulseAtPosition(Vec3 vImpluse, Vec3 vPos)
{
}
