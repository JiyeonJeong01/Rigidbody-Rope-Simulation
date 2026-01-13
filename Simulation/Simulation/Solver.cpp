#include "pch.h"
#include "Solver.h"

#include "Collider.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "DebugHelper.h"
#include "VectorHelper.h"


Solver::Solver()
	: m_pGraphicDevice(nullptr), m_pStaticBody(nullptr)
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

	//DebugHelper::Print_String(L"================");
	//DebugHelper::Print_Vec3(L"vNorm", vNorm);
	//DebugHelper::Print_Vec3(L"vPoint", vPoint);

	// 충돌 지점에서의 속도 구하기
	Vec3 vVel_A = aBody->Get_PointVelocity(vPoint);
	Vec3 vVel_B = bBody->Get_PointVelocity(vPoint);

	// B가 A에 대해 움직이는 상대속도
	Vec3 vVel_Rel = vVel_B - vVel_A; 

	// 충돌 지점에서 COM 으로의 벡터 r 구하기
	Vec3 vR_A = vPoint - aBody->Get_COM();
	Vec3 vR_B = vPoint - bBody->Get_COM();

	// r과 노멀벡터 외적해서 임펄스 회전 축 구하기
	// 노멀 벡터는 충돌을 해결하기 위한 임펄스가 적용할 방향이 된다
	Vec3 vJAxis_A = VectorHelper::CrossProduct(vR_A, vNorm);
	Vec3 vJAxis_B = VectorHelper::CrossProduct(vR_B, vNorm);

	// 해당 축에 대한 회전 저항(관성)의 역수 구하기 : 해당 축을 기준으로 얼마나 회전하는지 
	const float fInvInertia_A = aBody->Find_InvInertiaOfAxis(vJAxis_A);
	const float fInvInertia_B = bBody->Find_InvInertiaOfAxis(vJAxis_B);

	// 법선 벡터에 대한 상대 속도 구하기 : 충돌이 얼마나 강한지 판단한다
	// 충돌 중이므로 언제나 fVel_Norm < 0
	float fVel_Norm = VectorHelper::DotProduct(vNorm, vVel_Rel);

	// 반발계수 e : 접근하던 속도의 e배만큼 반대 방향으로 튕겨 나오게 한다
	const float fThreshold = 9.81f / 5.1f;
	float fRestitution = (fabsf(fVel_Norm) < fThreshold) ? 0.f : aBody->Get_Restitution();

	// 충돌 후 법선 상대 속도를 특정 값으로 만들기 위해 필요한 변화량
	// fVel_Norm : 법선 방향으로 얼마나 빠르게 파고들고 있는지 
	float fNumerator = -(fRestitution + 1) * fVel_Norm;

	// 충돌에서 임펄스 1에 대한 총 속도 변화 민감도
	float fMagSq_J_A = VectorHelper::DotProduct(vJAxis_A, vJAxis_A);
	float fMagSq_J_B = VectorHelper::DotProduct(vJAxis_B, vJAxis_B);
	float fDenominator = aBody->Get_InvMass() + bBody->Get_InvMass() + fMagSq_J_A * fInvInertia_A + fMagSq_J_B * fInvInertia_B;

	// 충돌에 대한 저항값 구하기 : 법선 상대 속도를 구해야 한다 
	// impulse = numerator / denominator
	// j = (원하는 속도 변화량) / (물체의 저항)
	float fJ = fNumerator / fDenominator;

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

		float mu = sqrtf(aBody->Get_Friction() * bBody->Get_Friction());
		float fMaxFriction = fJ * mu;
		if (fImpulseT > fMaxFriction)
			fImpulseT = fMaxFriction;
		if (fImpulseT < -fMaxFriction) 
			fImpulseT = -fMaxFriction;

		vJFriction = vVel_Tangent * fImpulseT;
	}

	if (!VectorHelper::Is_Zero(vImpulse))
	{
		DebugHelper::Print_Vec3(L"Impulse", vImpulse * -1.f);

		aBody->Add_ImpulseAtPoint(vImpulse * -1.f, vPoint);
		//bBody->Add_ImpulseAtPoint(vImpulse, vPoint);
	}

	if (!VectorHelper::Is_Zero(vJFriction))
	{
		//aBody->Add_ImpulseAtPoint(vJFriction * -1.f, vPoint);
		//bBody->Add_ImpulseAtPoint(vJFriction, vPoint);
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

	if (fabsf(fMoveA) > 0.f)
		pInfo->A->Get_Transform()->Translate(pInfo->vResolveN_A * fMoveA);

	if (fabsf(fMoveB) > 0.f)
		pInfo->B->Get_Transform()->Translate(pInfo->vN_PlaneA * fMoveB);

	DebugHelper::Print_Vec3(L"Solved Pos", pInfo->A->Get_Transform()->Get_Position());
}

void Solver::Solve_Friction(CONTACT_INFO* pInfo)
{
	if (Is_Seperating(pInfo))
		return;

	Collider* A = pInfo->A;
	Collider* B = pInfo->B;
	Rigidbody* aBody = A->Get_Rigidbody();
	Rigidbody* bBody = B->Get_Rigidbody();



}

void Solver::Impulse_StaticCollider(Object* pObejct, Collider* pCollider)
{
}


float Solver::Get_InvMass(Collider* pCollider)
{
	if (pCollider->Get_Rigidbody()->Get_ColType() == STATIC)
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

