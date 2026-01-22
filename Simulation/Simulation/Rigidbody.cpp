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
	, m_eGeometryType(), m_pTransform(nullptr)
{
}

Rigidbody::~Rigidbody()
{
}

HRESULT Rigidbody::Ready_Component(BODY_DESC* b)
{
    if (FAILED(Component::Ready_Component()))
        return E_FAIL;

	m_pTransform = static_cast<Transform*>(m_pOwner->Find_Component(L"Transform"));

	if (!m_pTransform)
		return E_FAIL;

    m_eGeometryType = b->eGeoType;

	Calc_Dimension(b);
	Calc_Inertia(b);
	Calc_COM(b);
	b->pTransform = m_pTransform;

	m_eCachedBodyType = b->eBodyType; // 매번 접근하는 과정 생략을 위해 캐싱

	return S_OK;
}

int Rigidbody::Update_Component(const float& fTimeDelta)
{
    Component::Update_Component(fTimeDelta);

	if (m_eCachedBodyType != DYNAMIC)
		return 0;

	BODY_DESC* b = Try_GetMyBody();
	b->vCOM = m_pTransform->Get_Position();

	return 0;
}

void Rigidbody::LateUpdate_Component(const float& fTimeDelta)
{
    Component::LateUpdate_Component(fTimeDelta);

	if (m_eCachedBodyType != DYNAMIC)
		return ;
}

void Rigidbody::Fixed_Update(const float& fTimeDelta)
{
    Component::Fixed_Update(fTimeDelta);

	if (m_eCachedBodyType != DYNAMIC)
		return ;
}

void Rigidbody::Calc_Dimension(BODY_DESC* b)
{
	UINT iVtxCnt = 0;
	VIBuffer* pVIBuffer = static_cast<VIBuffer*>(m_pOwner->Find_Component(L"Buffer"));

	iVtxCnt = pVIBuffer->Get_VIBufferInfo().dwVtxCnt;
     
	VTXCOL* pVertices = nullptr;
	pVIBuffer->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

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

	pVIBuffer->Get_VertexBuffer()->Unlock();
}

void Rigidbody::Calc_Inertia(BODY_DESC* b)
{
	// 물리 공식을 따른 Rigidbody 형상에 따른 Inertia Tensor 계산하기 
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
	case BOX:
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

void Rigidbody::Calc_COM(BODY_DESC* b)
{
	b->vCOM = m_pTransform->Get_Position();
}

float Rigidbody::Calc_InvInertiaOfAxis(const Vec3& vAxis)
{
	if (VectorHelper::Is_Zero(vAxis) || m_eCachedBodyType != DYNAMIC)
		return 0.f; 

	BODY_DESC* b = Try_GetMyBody();
	if (!b) return 0.f;

	Vec3 vBaseAxis = VectorHelper::Get_Normalized(vAxis);

	Matrix matR = m_pTransform->Get_RotationMat();
	Matrix matRT = *D3DXMatrixTranspose(&matRT, &matR);

	// DX row-vector 규약 기준 관성 역텐서 변환
	// World(Inv_Inertia) = trans(R) * Local(Inv_Inerta) * R
	Matrix matInvInerta = matRT * b->matInvInertiaTensor * matR;

	Vec3 vInvAxis = VectorHelper::TransformNormal(&vBaseAxis, &matInvInerta);

	return VectorHelper::DotProduct(vBaseAxis, vInvAxis);
}

void Rigidbody::Translate(const Vec3 vDeltaPos)
{
	// STATIC BODY는 절대 움직이지 않는다
	if (m_eCachedBodyType == STATIC)
		return;

	BODY_DESC* b = Try_GetMyBody();
	if (!b)
		return;

	b->vCOM += vDeltaPos;
	m_pTransform->Translate(vDeltaPos);
}

void Rigidbody::Add_LinearImpulse(Vec3 vImpulse)
{
	if (m_eCachedBodyType != DYNAMIC)
		return;

	BODY_DESC* b = Try_GetMyBody();
	if (!b) return;

	b->vLinearVel += vImpulse * b->fInvMass;
}

void Rigidbody::Add_Force(Vec3 vForce)
{
	if (m_eCachedBodyType != DYNAMIC)
		return;

	BODY_DESC* b = Try_GetMyBody();
	if (!b) return;

	b->vForceAccum += vForce;
}

void Rigidbody::Add_Torque(Vec3 vTorque)
{
	if (m_eCachedBodyType != DYNAMIC)
		return;

	BODY_DESC* b = Try_GetMyBody();
	if (!b) return;

	b->vTorqueAccum += vTorque;
}

void Rigidbody::Set_LinearVelocity(const Vec3& vVel)
{
    if (m_eCachedBodyType != DYNAMIC)
        return;

    BODY_DESC* b = Try_GetMyBody();
    if (!b) return;

    b->vLinearVel = vVel;
}

void Rigidbody::Set_AngularVelocity(const Vec3& vVel)
{
    if (m_eCachedBodyType != DYNAMIC)
        return;

    BODY_DESC* b = Try_GetMyBody();
    if (!b) return;

    b->vAngularVel = vVel;
}

const BODY_DESC& Rigidbody::Get_BodyInfo()
{
    BODY_DESC* b = Try_GetMyBody();
    if (!b) return BODY_DESC{};
    else return *b;
}

Vec3 Rigidbody::Get_PointVelocity(const Vec3& vPoint)
{
	if (m_eCachedBodyType == STATIC)
		return VectorHelper::Zero();

	const BODY_DESC* b = Try_GetMyBody();

	Vec3 vComToPoint = vPoint - b->vCOM;
	Vec3 vRot = VectorHelper::CrossProduct(b->vAngularVel, vComToPoint);
	return b->vLinearVel + vRot;
}

Vec3 Rigidbody::Get_COM()
{
	BODY_DESC* b = Try_GetMyBody();
	b->vCOM = m_pTransform->Get_Position();
	return b->vCOM;
}

Rigidbody* Rigidbody::Create(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner, BODY_DESC tBody)
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
