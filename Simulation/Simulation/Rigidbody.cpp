#include "pch.h"
#include "Rigidbody.h"

#include "DebugHelper.h"
#include "Object.h"
#include "PhysicsWorld.h"
#include "Transform.h"
#include "VIBuffer.h"
#include "VectorHelper.h"

Rigidbody::Rigidbody(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner)
	: Component(pGraphicDev, pOwner)
	, m_eGeometryType(), m_pTransform(nullptr), m_pVIBuffer(nullptr)
{
}

Rigidbody::~Rigidbody()
{
}

HRESULT Rigidbody::Ready_Component(BODY* b)
{
	m_pTransform = static_cast<Transform*>(m_pOwner->Find_Component(L"Transform"));
	m_pVIBuffer = static_cast<VIBuffer*>(m_pOwner->Find_Component(L"Buffer"));

	if (!m_pTransform || !m_pVIBuffer)
		return E_FAIL;

	Find_Dimension(b);
	Find_Inertia(b);
	Find_COM(b);
	b->pTransform = m_pTransform;

	// TODO : 콜라이더와 동기화하는 코드도 필요해보인다. 

	return S_OK;
}

int Rigidbody::Update_Component(const float& fTimeDelta)
{
	BODY* b = Try_GetMyBody();
	if (!b || b->eType != DYNAMIC)
		return 0.f;

	b->vCOM = m_pTransform->Get_Position();

	// vCOM = m_pTransform->Get_Position();

	//if (m_eColType != DYNAMIC)
	//	return 0;

	//if (bGravity)
	//	Apply_Gravity(fTimeDelta);

	//if (fDrag > 0.f)
	//	Apply_Drag(fTimeDelta);

	//if (fAngularDrag > 0.f)
	//	Apply_AngularDrag(fTimeDelta);

	// Integrate_Transform(fTimeDelta);

	return 0;
}

void Rigidbody::LateUpdate_Component(const float& fTimeDelta)
{
}

void Rigidbody::Step()
{

}

void Rigidbody::Find_Dimension(BODY* b)
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

	b->vDimension = Vec3(fMaxX - fMinX, fMaxY - fMinY, fMaxZ - fMinZ);
	b->vDimensionCenter = Vec3(fMaxX + fMinX, fMaxY + fMinY, fMaxZ + fMinZ) * 0.5f;

	m_pVIBuffer->Get_VertexBuffer()->Unlock();
}

void Rigidbody::Find_Inertia(BODY* b)
{
	if (b->eType == STATIC)
	{
		b->fInvMass = 0.f;
		b->vLinearVel = VectorHelper::Zero();
		b->vAngularVel = VectorHelper::Zero();

		D3DXMatrixIdentity(&b->matInertiaTensor);
		ZeroMemory(&b->matInvInertiaTensor, sizeof(Matrix));
		return;
	}

	// Rigidbody 모양에 따른 Inertia Tensor 계산하기 
	D3DXMatrixIdentity(&b->matInertiaTensor);

	switch (m_eGeometryType)
	{
	case SPHERE:
		{
		Vec3 vDim = b->vDimension;
		float fInertia = (2.f / 5.f) * b->fMass * powf(vDim.x * 0.5f, 2.f);

		b->matInertiaTensor.m[0][0] = fInertia;
		b->matInertiaTensor.m[1][1] = fInertia;
		b->matInertiaTensor.m[2][2] = fInertia;

		}
		break;
	case CUBE:
		{
		float k = (1.f / 12.f) * b->fMass;

		Vec3 vDim = b->vDimension;
		b->matInertiaTensor.m[0][0] = k * (vDim.y * vDim.y + vDim.z * vDim.z);
		b->matInertiaTensor.m[1][1] = k * (vDim.x * vDim.x + vDim.z * vDim.z);
		b->matInertiaTensor.m[2][2] = k * (vDim.x * vDim.x + vDim.y * vDim.y);

		}
		break;
	}

	D3DXMatrixInverse(&b->matInvInertiaTensor, 0, &b->matInertiaTensor);
}

void Rigidbody::Find_COM(BODY* b)
{
	b->vCOM = m_pTransform->Get_Position();
}

float Rigidbody::Find_InvInertiaOfAxis(const Vec3& vAxis)
{
	if (VectorHelper::Is_Zero(vAxis))
		return 0.f; 

	BODY* b = Try_GetMyBody();
	if (!b || b->eType != DYNAMIC)
		return 0.f;

	Vec3 vBaseAxis = VectorHelper::Get_Normalized(vAxis);

	Matrix matR = m_pTransform->Get_RotationMat();
	Matrix matRT = *D3DXMatrixTranspose(&matRT, &matR);
	Matrix matInertiaInv = matR * b->matInvInertiaTensor * matRT; // R * Inverse(I_Local) * Transpose(R) * v

	Vec3 vInvAxis = VectorHelper::TransformNormal(&vBaseAxis, &matInertiaInv);

	return VectorHelper::DotProduct(vBaseAxis, vInvAxis);
}

void Rigidbody::Translate(const Vec3 vDeltaPos)
{
	BODY* b = Try_GetMyBody();
	if (!b || b->eType == STATIC)
		return;

	b->vCOM += vDeltaPos;
	m_pTransform->Translate(vDeltaPos);
}

void Rigidbody::Integrate_Transform(const float& fTimeDelta)
{
	BODY* b = Try_GetMyBody();
	if (!b || b->eType != DYNAMIC)
		return;

	// 선 속도 적용
	Vec3 vMoveDelta = b->vLinearVel * fTimeDelta;
	m_pTransform->Translate(vMoveDelta);

	// 각 속도 적용
	if (!VectorHelper::Is_Zero(b->vAngularVel))
	{
		// TODO : 보정 로직 추가하기 
		// vAngularVel = Acclerate_Gyro(fTimeDelta);
		Vec3 vRotAxis = VectorHelper::Get_Normalized(b->vAngularVel);
		float fRotMagnitude = D3DXVec3Length(&b->vAngularVel) * fTimeDelta;

		m_pTransform->Rotate(vRotAxis, fRotMagnitude);
	}

	// m_pTransform->Get_Info(AXIS_Z, &b->m_vLook);
}

Vec3 Rigidbody::Acclerate_Gyro(const float& fTimeDelta)
{
	//Matrix matRot = m_pTransform->Get_RotationMat();
	//Matrix matRotInv = *D3DXMatrixInverse(&matRotInv, 0, &matRot);

	//Vec3 vWB = *D3DXVec3TransformNormal(&vWB, &vAngularVel, &matRotInv);
	//Vec3 vW2B = vWB;

	//int nItr = 1;

	//for (int i = 0; i < nItr; ++i)
	//{
	//	Vec3 vTemp = *D3DXVec3TransformNormal(&vTemp, &vW2B, &matInertiaTensor);
	//	D3DXVec3Cross(&vTemp, &vt)

	//}

	return Vec3();
}

void Rigidbody::Add_ImpulseAtPoint(const Vec3& impulse, const Vec3& point)
{
	BODY* b = Try_GetMyBody();
	if (!b || b->eType != DYNAMIC)
		return;

	// Linear impulse
	b->vLinearVel += impulse * b->fInvMass;

	// Angular impulse
	Vec3 r = point - b->vCOM;
	if (VectorHelper::Is_NearlyZero(r))
		return;

	Vec3 angularImpulse = VectorHelper::CrossProduct(r, impulse);

	Matrix R = m_pTransform->Get_RotationMat();
	Matrix RT = *D3DXMatrixTranspose(&RT, &R);
	Matrix IinvWorld = R * b->matInvInertiaTensor * RT;

	Vec3 deltaW = *D3DXVec3TransformNormal(&deltaW, &angularImpulse, &IinvWorld);

	b->vAngularVel += deltaW;
}


void Rigidbody::Add_LinearImpulse(Vec3 vImpulse)
{
	BODY* b = Try_GetMyBody();
	if (!b || b->eType != DYNAMIC)
		return;

	b->vLinearVel += vImpulse * b->fInvMass;
}

void Rigidbody::Add_Force(Vec3 vForce, FORCE_MODE eForce)
{
	BODY* b = Try_GetMyBody();
	if (!b || b->eType != DYNAMIC)
		return;

	b->vForceAccum += vForce;
}

void Rigidbody::Add_Torque(Vec3 vTorque, FORCE_MODE eForce)
{
	BODY* b = Try_GetMyBody();
	if (!b || b->eType != DYNAMIC)
		return;

	b->vTorqueAccum += vTorque;
}

//void Rigidbody::Apply_Drag(const float& fTimeDelta)
//{
//	if (m_eColType != DYNAMIC)
//		return;
//
//	vLinearVel -= vLinearVel * fDrag * fTimeDelta;
//
//	if (D3DXVec3LengthSq(&vLinearVel) < fEpsilon * fEpsilon)
//		vLinearVel = VectorHelper::Zero();
//}
//
//void Rigidbody::Apply_AngularDrag(const float& fTimeDelta)
//{
//	if (m_eColType != DYNAMIC)
//		return;
//
//	vAngularVel -= vAngularVel * fAngularDrag * fTimeDelta;
//
//	if (D3DXVec3LengthSq(&vAngularVel) < fEpsilon * fEpsilon)
//		vAngularVel = VectorHelper::Zero();
//}
//
//void Rigidbody::Apply_Gravity(const float& fTimeDelta)
//{
//	if (m_eColType != DYNAMIC)
//		return;
//
//	Vec3 vGravity{ 0.f, -9.81f, 0.f };
//	vLinearVel += vGravity * fTimeDelta;
//}

Vec3 Rigidbody::Get_PointVelocity(const Vec3& vPoint)
{
	if (m_eColType == STATIC)
		return VectorHelper::Zero();

	const BODY* b = Try_GetMyBody();

	Vec3 vComToPoint = vPoint - b->vCOM;
	Vec3 vRot = VectorHelper::CrossProduct(b->vAngularVel, vComToPoint);
	return b->vLinearVel + vRot;
}

Vec3 Rigidbody::Get_COM()
{
	// TODO : ================ 하 진짜 짱나네 이거 스케일이랑 오프셋도 고려 해
	BODY* b = Try_GetMyBody();
	b->vCOM = m_pTransform->Get_Position();
	return b->vCOM;
}

Rigidbody* Rigidbody::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner, BODY tBody)
{
	Rigidbody* pBody = new Rigidbody(pGraphicDev, pOwner);
	if (FAILED(pBody->Ready_Component(&tBody)))
	{
		Safe_Delete(pBody);
	}

	pBody->Set_BodyID(PhysicsWorld::GetInstance()->Create_Body(std::move(tBody)));

	pOwner->Add_Component(L"Rigidbody", pBody);

	return pBody;
}

void Rigidbody::Release()
{
	Component::Release();
}
