#include "pch.h"
#include "Solver.h"

#include "Collider.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "DebugHelper.h"
#include "VectorHelper.h"


Solver::Solver(): m_pStaticBody(nullptr)
{
}

Solver::~Solver()
{
}

HRESULT Solver::Ready_System()
{
	m_pStaticBody;

	return S_OK;
}

void Solver::Solve_Contacts(CONTACT_INFO* pInfo)
{
	Solve_Impulse(pInfo);
	Solve_Penetration(pInfo);
}

void Solver::Solve_Impulse(CONTACT_INFO* pInfo)
{
	if (Is_Seperating(pInfo))
		return;

	Collider* A = pInfo->A;
	Collider* B = pInfo->B;
	Rigidbody* aBody = A->Get_Rigidbody();
	Rigidbody* bBody = B->Get_Rigidbody();
	Vec3 vPoint = pInfo->vPoint;					// 충돌 지점 
	Vec3 vNorm = pInfo->vPenetrateN_A;	// A가 B로 침범하는 방향

	// 충돌 지점에서의 속도 구하기
	const Vec3 vVel_A = aBody->Get_PointVelocity(vPoint);
	const Vec3 vVel_B = bBody->Get_PointVelocity(vPoint);

	// B가 A에 대해 움직이는 상대속도
	Vec3 vVel_Rel = vVel_B - vVel_A; 

	// 충돌 지점에서 COM 으로의 벡터 r 구하기
	Vec3 vR_A = vPoint - aBody->Get_COM();
	Vec3 vR_B = vPoint - bBody->Get_COM();

	// COM과 노말 외적해서 임펄스 회전 축 구하기
	Vec3 vJAxis_A = VectorHelper::CrossProduct(vR_A, vNorm);
	Vec3 vJAxis_B = VectorHelper::CrossProduct(vR_B, vNorm);

	// 해당 축에 대한 회전 저항(관성)의 역수 구하기 : 해당 축을 기준으로 얼마나 회전하는지 
	const float fInvInertia_A = aBody->Find_InvInertiaOfAxis(vJAxis_A);
	const float fInvInertia_B = bBody->Find_InvInertiaOfAxis(vJAxis_B);

	// 법선 벡터에 대한 상대 속도 구하기 : 상대속도를 법선 방향으로 투영한 스칼라 값
	float fVel_Norm = VectorHelper::DotProduct(vNorm, vVel_Rel);

	// 법선 방향으로의 상대 속도 구하기
	// 반발계수 e : 접근하던 속도의 e배만큼 반대 방향으로 튕겨 나오게 한다
	const float fThreshold = 9.81f / 2.25f;
	float fRestitution = (fabsf(fVel_Norm) < fThreshold) ? 0.f : aBody->Get_Restitution();
	float fNumerator = -(fRestitution + 1) * fVel_Norm;

	// 충돌에 대한 저항값 구하기
	// 임펄스 1이 속도를 얼마나 바꾸는지, 같은 임펄스를 줬을 때 얼마나 저항하는지
	// 이 충돌에서 임펄스 1을 줬을 때, 선형 + 회전까지 포함해서 노멀 상대속도가 얼마나 변하는가
	float fMagSq_J_A = VectorHelper::DotProduct(vJAxis_A, vJAxis_A);
	float fMagSq_J_B = VectorHelper::DotProduct(vJAxis_B, vJAxis_B);
	float fDenominator = aBody->Get_InvMass() + bBody->Get_InvMass() + fMagSq_J_A * fInvInertia_A + fMagSq_J_B * fInvInertia_B;

	// impulse = numerator / denominator
	float fJ = fNumerator / fDenominator;

	//DebugHelper::Print_Float(L"Norm Velocity", fVel_Norm);
	//DebugHelper::Print_Float(L"Numerator", fNumerator);
	//DebugHelper::Print_Float(L"Denominator", fDenominator);
	DebugHelper::Print_Float(L"Impulse", fJ);

	if (fJ < 0.f)
		fJ = 0.f;
	
	Vec3 vImpulse = vNorm * fJ;

	// 아래로 떨어지는 속도: 노멀 성분 (바닥을 파고듦)
	Vec3 vVel_Parralel = vNorm * fVel_Norm;

	// 옆으로 미끄러지는 속도 : 접선 성분(마찰이 막아야 함)
	// 상대속도에서 노멀 성분을 제거 = “접선 성분(미끄러지는 성분)”
	Vec3 vVel_Tangent = vVel_Rel - vVel_Parralel;
	Vec3 vJFriction = VectorHelper::Zero();

	if (!VectorHelper::Is_Zero(vVel_Tangent))
	{
		vVel_Tangent = VectorHelper::Get_Normalized(vVel_Tangent);

		const Vec3 vTangentAxis_A = VectorHelper::CrossProduct(vR_A, vVel_Tangent);
		const Vec3 vTangentAxis_B = VectorHelper::CrossProduct(vR_B, vVel_Tangent);

		float fInvInertiaT_A = aBody->Find_InvInertiaOfAxis(vTangentAxis_A);
		float fInvInertiaT_B = bBody->Find_InvInertiaOfAxis(vTangentAxis_B);

		float fMagSq_TAxis_A = VectorHelper::DotProduct(vTangentAxis_A, vTangentAxis_A);
		float fMagSq_TAxis_B = VectorHelper::DotProduct(vTangentAxis_B, vTangentAxis_B);

		float fDenominatorT = aBody->Get_InvMass() + bBody->Get_InvMass() +
												fMagSq_TAxis_A * fInvInertiaT_A +
												fMagSq_TAxis_B * fInvInertiaT_B;

		// 여기서 vVel_Tangent는 단위 t
		float vt = VectorHelper::DotProduct(vVel_Rel, vVel_Tangent); 
		float fImpulseT = -vt / fDenominatorT;

		float fMaxFriction = fJ * bBody->Get_Friction();
		if (fImpulseT > fMaxFriction)
			fImpulseT = fMaxFriction;
		if (fImpulseT < -fMaxFriction) 
			fImpulseT = -fMaxFriction;

		vJFriction = vVel_Tangent * fImpulseT;
	}

	if (!VectorHelper::Is_Zero(vImpulse))
	{
		vImpulse *= 0.2f;
		aBody->Add_ImpulseAtPoint(vImpulse * -1.f, vPoint);
		bBody->Add_ImpulseAtPoint(vImpulse, vPoint);
	}

	if (!VectorHelper::Is_Zero(vJFriction))
	{
		vJFriction *= 0.2f;
		aBody->Add_ImpulseAtPoint(vJFriction * -1.f, vPoint);
		bBody->Add_ImpulseAtPoint(vJFriction, vPoint);

		DebugHelper::Print_Vec3(L"Friction_A", vJFriction * -1.f);
		DebugHelper::Print_Vec3(L"Friction_A", vJFriction);
	}
}

void Solver::Solve_Penetration(CONTACT_INFO* pInfo)
{
	if (pInfo == nullptr || pInfo->A == nullptr || pInfo->B == nullptr)
		return;

	if (pInfo->fDepth <= 0.f)
		return;

	float fInvA = Get_InvMass(pInfo->A);
	float fInvB = Get_InvMass(pInfo->B);

	float fTotalInv = fInvA + fInvB;
	if (fTotalInv < 0.f)
		return;

	if (pInfo->B->Get_ColType() == STATIC)
	{
		fInvB = 0.f;
		fTotalInv = fInvA;
	}

	float fMoveA = pInfo->fDepth * (fInvA / fTotalInv);
	float fMoveB = pInfo->fDepth * (fInvB / fTotalInv);

	//DebugHelper::Print_Vec3(L"Norm", pInfo->vN);
	//DebugHelper::Print_Float(L"MoveA", fMoveA);
	//DebugHelper::Print_Float(L"MoveB", fMoveB);

	if (fMoveA > 0.f)
		pInfo->A->Get_Transform()->Translate(pInfo->vN_PlaneB * fMoveA);

	if (fMoveB > 0.f)
		pInfo->B->Get_Transform()->Translate(pInfo->vN_PlaneA * fMoveB);

	//DebugHelper::Print_Vec3(L"Pos A", pInfo->A->Get_Transform()->Get_Position());
}

float Solver::Get_InvMass(Collider* pCollider)
{
	if (pCollider->Get_ColType() == STATIC)
		return 0.f;

	Rigidbody* rb = pCollider->Get_Rigidbody();
	if (!rb)
		return 0.f;

	float fM = rb->Get_Mass();
	if (fM <= 0.f)
		return 0.f;

	return 1.f / fM;
}

bool Solver::Is_Seperating(CONTACT_INFO* pInfo)
{
	// vResolveN_A : "A를 B에게서 멀어지게 하는 방향" (A separation direction)
	// vRel = vB - vA : A 기준에서 본 B의 상대속도
	Vec3 vA, vB;
	if (pInfo->A->Get_ColType() == STATIC)
		vA = VectorHelper::Zero();
	else
		vA = pInfo->A->Get_Rigidbody()->Get_PointVelocity(pInfo->vPoint);

	if (pInfo->B->Get_ColType() == STATIC)
		vB = VectorHelper::Zero();
	else 
		vB = pInfo->B->Get_Rigidbody()->Get_PointVelocity(pInfo->vPoint);

	const Vec3 vRel = vB - vA;

	// dot(vRel, vResolveN_A) < 0  => B가 -resolve 방향으로 움직임 => A와 B가 서로 멀어지는 중 (separating)
	const float vn = VectorHelper::DotProduct(vRel, pInfo->vResolveN_A);
	const float eps = 1e-4f;

	return vn < -eps;
}

Solver* Solver::Create()
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

