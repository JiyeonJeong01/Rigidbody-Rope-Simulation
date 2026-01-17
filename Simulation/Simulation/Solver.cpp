#include "pch.h"
#include "Solver.h"

#include "Collider.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "DebugHelper.h"
#include "VectorHelper.h"


Solver::Solver()
	: m_pGraphicDevice(nullptr)
{
}

Solver::~Solver()
{
}

HRESULT Solver::Ready_System()
{
	return S_OK;
}

void Solver::Solve_Contacts(CONTACT_INFO* pInfo)
{
	Solve_ImpulseAndFriction(pInfo);
	Solve_Penetration(pInfo);
}

void Solver::Solve_ImpulseAndFriction(CONTACT_INFO* pInfo)
{
	// 멀어지는 중이었다면 solve하지 않기 
	if (Is_Seperating(pInfo))
		return;

	Collider* A = pInfo->A;
	Collider* B = pInfo->B;
	Transform* tA = A->Get_Transform();
	Transform* tB = B->Get_Transform();
	BODY* bA = PhysicsWorld::GetInstance()->Try_GetBody(A->Get_Rigidbody()->Get_BodyID());
	BODY* bB = PhysicsWorld::GetInstance()->Try_GetBody(B->Get_Rigidbody()->Get_BodyID());


	// ==================================================== //
	// ================== Solve Impulse =================== //
	// ==================================================== //
	// 충돌 지점 
	Vec3 vPoint = pInfo->vPoint;
	// A가 B로 침범하는 방향
	Vec3 vNorm = pInfo->vPenetrateN_A;				


	// 충돌 지점에서의 속도 구하기
	Vec3 vVel_A = Calc_PointVelocity(bA, vPoint);
	Vec3 vVel_B = Calc_PointVelocity(bB, vPoint);

	// B가 A에 대해 움직이는 상대속도
	Vec3 vVel_Rel = vVel_B - vVel_A; 

	// 충돌 지점에서 COM 으로의 벡터 r 구하기
	Vec3 vR_A = vPoint - bA->vCOM;
	Vec3 vR_B = vPoint - bB->vCOM;

	// r과 노멀벡터 외적해서 임펄스 회전 축 구하기
	// 노멀 벡터는 충돌을 해결하기 위한 임펄스가 적용할 방향이 된다
	Vec3 vJAxis_A = VectorHelper::CrossProduct(vR_A, vNorm);
	Vec3 vJAxis_B = VectorHelper::CrossProduct(vR_B, vNorm);

	// 해당 축에 대한 회전 저항(관성)의 역수 구한다
	// = 해당 축을 기준으로 얼마나 회전하는지 구한다 
	const float fInvInertia_A = Calc_InvInertiaOfAxis(tA, bA, vJAxis_A);
	const float fInvInertia_B = Calc_InvInertiaOfAxis(tB, bB, vJAxis_B);

	// 법선 벡터에 대한 상대 속도 구하기 : 충돌이 얼마나 강한지 판단한다
	// 충돌 중이므로 언제나 fVel_Norm < 0
	float fVel_Norm = VectorHelper::DotProduct(vNorm, vVel_Rel);

	// 반발계수 e : 접근하던 속도의 e배만큼 반대 방향으로 튕겨 나오게 한다
	const float fThreshold = 9.81f / 5.1f;
	float fRestitution = (fabsf(fVel_Norm) < fThreshold) ? 0.f : max(bA->fRestitution, bB->fRestitution);

	// 충돌 후 법선 상대 속도를 특정 값으로 만들기 위해 필요한 변화량
	// fVel_Norm : 법선 방향으로 얼마나 빠르게 파고들고 있는지 
	float fNumerator = -(fRestitution + 1) * fVel_Norm;

	// 충돌에서 임펄스 1에 대한 총 속도 변화 민감도
	float fMagSq_J_A = VectorHelper::DotProduct(vJAxis_A, vJAxis_A);
	float fMagSq_J_B = VectorHelper::DotProduct(vJAxis_B, vJAxis_B);
	float fDenominator = bA->fInvMass + bB->fInvMass + fMagSq_J_A * fInvInertia_A + fMagSq_J_B * fInvInertia_B;

	// 충돌에 대한 저항값 구하기 : 법선 상대 속도를 구해야 한다 
	// impulse = numerator / denominator
	// j = (원하는 속도 변화량) / (물체의 저항)
	float fJ = fNumerator / fDenominator;
	if (fJ < 0.f)
		fJ = 0.f;
	
	Vec3 vImpulse = vNorm * fJ;

	// ==================================================== //
	// ================== Solve Friction ================== //
	// ==================================================== //

	// 상대 속도 = 노멀 성분 + 접선 성분 
	// 아래로 떨어지는 속도: 노멀 성분 (바닥을 파고든다)
	Vec3 vVel_Parralel = vNorm * fVel_Norm;

	// 옆으로 미끄러지는 속도 : 접선 성분(마찰이 막아야 함)
	Vec3 vVel_Tangent = vVel_Rel - vVel_Parralel;

	Vec3 vJFriction = VectorHelper::Zero();

	if (!VectorHelper::Is_Zero(vVel_Tangent))
	{
		// 미끄러지는 방향 추출
		vVel_Tangent = VectorHelper::Get_Normalized(vVel_Tangent);

		// 상대 속도를 접선에 투영 : 표면을 따라 미끄러지는 실제 속도 성분 
		float vt = VectorHelper::DotProduct(vVel_Rel, vVel_Tangent);

		// 마찰이 만들 회전 계산하기
		// 물체가 마찰로 인해 회전할 축 구하기
		const Vec3 vTangentAxis_A = VectorHelper::CrossProduct(vR_A, vVel_Tangent);
		const Vec3 vTangentAxis_B = VectorHelper::CrossProduct(vR_B, vVel_Tangent);

		// 해당 회전 축을 기준으로 회전하는 정도 구하기 
		float fInvInertiaT_A = Calc_InvInertiaOfAxis(tA, bA, vTangentAxis_A);
		float fInvInertiaT_B = Calc_InvInertiaOfAxis(tB, bB, vTangentAxis_B);

		// 레버암 길이를 반영한 회전 기여도 
		float fMagSq_TAxis_A = VectorHelper::DotProduct(vTangentAxis_A, vTangentAxis_A);
		float fMagSq_TAxis_B = VectorHelper::DotProduct(vTangentAxis_B, vTangentAxis_B);

		// invMass : 작을수록(물체가 무거울수록) 움직이기 어렵다
		// invInertia : 작을수록 회전하기 어렵다
		// => fDenominator를 분모로 사용하여, 움직임의 민감도로 작용하게 한다. 
		float fDenominatorT = bA->fInvMass + bB->fInvMass + fMagSq_TAxis_A * fInvInertiaT_A + fMagSq_TAxis_B * fInvInertiaT_B;

		float fImpulseT = -vt / fDenominatorT;

		// 마찰 임펄스가 노멀 임펄스가 허용하는 최대치를 넘지 않도록 제한한다.
		float mu = sqrtf(bA->fFriction * bB->fFriction);

		float fMaxFriction = fJ * mu;
		// 클램프 
		if (fImpulseT > fMaxFriction)
			fImpulseT = fMaxFriction;
		if (fImpulseT < -fMaxFriction) 
			fImpulseT = -fMaxFriction;

		vJFriction = vVel_Tangent * fImpulseT;
	}

	if (!VectorHelper::Is_Zero(vImpulse))
	{
		Add_ImpulseAtPoint(tA, bA, vImpulse * -1.f, vPoint);
		Add_ImpulseAtPoint(tB, bB, vImpulse, vPoint);
	}

	if (!VectorHelper::Is_NearlyZero(vJFriction))
	{
		Add_ImpulseAtPoint(tA, bA, vJFriction * -1.f, vPoint);
		Add_ImpulseAtPoint(tB, bB, vJFriction, vPoint);
	}
}

void Solver::Solve_Penetration(CONTACT_INFO* pInfo)
{
	if (pInfo == nullptr || pInfo->A == nullptr || pInfo->B == nullptr)
		return;

	if (pInfo->fDepth <= 0.f)
		return;

	Collider* A = pInfo->A;
	Collider* B = pInfo->B;

	BODY* bA = PhysicsWorld::GetInstance()->Try_GetBody(A->Get_Rigidbody()->Get_BodyID());
	BODY* bB = PhysicsWorld::GetInstance()->Try_GetBody(B->Get_Rigidbody()->Get_BodyID());
	if (!bA || !bB)
		return;

	float fInvA = bA->fInvMass;
	float fInvB = bB->fInvMass;

	if (bA->eType == STATIC) fInvA = 0.f;
	if (bB->eType == STATIC) fInvB = 0.f;

	float fTotalInv = fInvA + fInvB;
	if (fTotalInv <= 0.f)
		return;

	float fMoveA = pInfo->fDepth * (fInvA / fTotalInv);
	float fMoveB = pInfo->fDepth * (fInvB / fTotalInv);

	if (fabsf(fMoveA) > 0.f)
	{
		pInfo->A->Get_Transform()->Translate(pInfo->vResolveN_A * fMoveA);
	}

	if (fabsf(fMoveB) > 0.f)
	{
		pInfo->B->Get_Transform()->Translate(pInfo->vResolveN_A * -1.f * fMoveB);
	}
}

void Solver::Add_ImpulseAtPoint(Transform* pTransform, BODY* b, const Vec3& impulse, const Vec3& point)
{
	if (b->eType != DYNAMIC)
		return;

	// Linear impulse
	b->vLinearVel += impulse * b->fInvMass;

	// Angular impulse
	Vec3 r = point - b->vCOM;
	if (VectorHelper::Is_NearlyZero(r))
		return;

	Vec3 angularImpulse = VectorHelper::CrossProduct(r, impulse);

	Matrix R = pTransform->Get_RotationMat();
	Matrix RT = *D3DXMatrixTranspose(&RT, &R);
	Matrix IinvWorld = R * b->matInvInertiaTensor * RT;

	Vec3 deltaW = *D3DXVec3TransformNormal(&deltaW, &angularImpulse, &IinvWorld);

	b->vAngularVel += deltaW;
}

bool Solver::Is_Seperating(CONTACT_INFO* pInfo)
{
	// vResolveN_A : "A를 B에게서 멀어지게 하는 방향" (A separation direction)
	// vRel = vB - vA : A 기준에서 본 B의 상대속도

	BODY* bA = PhysicsWorld::GetInstance()->Try_GetBody(pInfo->A->Get_Rigidbody()->Get_BodyID());
	BODY* bB = PhysicsWorld::GetInstance()->Try_GetBody(pInfo->B->Get_Rigidbody()->Get_BodyID());

	Vec3 vA, vB;
	if (bA->eType == STATIC)
		vA = VectorHelper::Zero();
	else
		vA = pInfo->A->Get_Rigidbody()->Get_PointVelocity(pInfo->vPoint);

	if (bB->eType == STATIC)
		vB = VectorHelper::Zero();
	else 
		vB = pInfo->B->Get_Rigidbody()->Get_PointVelocity(pInfo->vPoint);

	const Vec3 vRel = vB - vA;

	// dot(vRel, vResolveN_A) < 0  => B가 -resolve 방향으로 움직임 => A와 B가 서로 멀어지는 중 (separating)
	const float vn = VectorHelper::DotProduct(vRel, pInfo->vResolveN_A);
	const float eps = 1e-4f;

	return vn < -eps;
}

Vec3 Solver::Calc_PointVelocity(BODY* b, const Vec3& vPoint)
{
	if (b->eType == STATIC)
		return VectorHelper::Zero();

	Vec3 vComToPoint = vPoint - b->vCOM;
	Vec3 vRot = VectorHelper::CrossProduct(b->vAngularVel, vComToPoint);
	return b->vLinearVel + vRot;
}

float Solver::Calc_InvInertiaOfAxis(Transform* pTransform, BODY* b, const Vec3& vAxis)
{
	if (VectorHelper::Is_Zero(vAxis))
		return 0.f;

	if (b->eType != DYNAMIC)
		return 0.f;

	Vec3 vBaseAxis = VectorHelper::Get_Normalized(vAxis);

	Matrix matR = pTransform->Get_RotationMat();
	Matrix matRT = *D3DXMatrixTranspose(&matRT, &matR);

	// DX row-vector 규약 기준 관성 역텐서 변환
	// World(Inv_Inertia) = trans(R) * Local(Inv_Inerta) * R
	Matrix matInvInerta = matRT * b->matInvInertiaTensor * matR;

	Vec3 vInvAxis = VectorHelper::TransformNormal(&vBaseAxis, &matInvInerta);

	return VectorHelper::DotProduct(vBaseAxis, vInvAxis);
}

Solver* Solver::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	Solver* pInstance = new Solver();
	if (FAILED(pInstance->Ready_System()))
	{
		Safe_Release(pInstance);
		pInstance = nullptr;
	}

	return pInstance;
}

void Solver::Release()
{

}

