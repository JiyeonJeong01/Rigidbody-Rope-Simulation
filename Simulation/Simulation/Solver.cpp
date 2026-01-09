#include "pch.h"
#include "Solver.h"

#include "Collider.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "DebugHelper.h"


Solver::Solver()
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
	Solve_Penetration(pInfo);
}

void Solver::Solve_Impulse(CONTACT_INFO* pInfo)
{
	if (Is_Seperating(pInfo))
		return;

	// 상대 속도 구하기
	/*
		Vector3D vCldrP0 = collider->getVelocityOfPoint(colPoint);
		Vector3D vCldeP0 = collidee->getVelocityOfPoint(colPoint);
		Vector3D velRel = vCldeP0.sub(vCldrP0);
	 */

	

	// 충돌 지점에서 COM 으로의 벡터 구하기
	/*
	 	Vector3D rClde(collidee->getCenterOfMass(), colPoint);
		Vector3D rCldr(collider->getCenterOfMass(), colPoint);
	 */

	// COM과 노말 외적해서 임펄스 회전 축 구하기
	/*
		Vector3D jRotAxisCldr = rCldr.crossProduct(nV);
		Vector3D jRotAxisClde = rClde.crossProduct(nV);
	 */

	// 해당 회전 축에 대한 회전 저항(관성)의 역수 구하기
	/*
		double invIClde = collidee->findInverseInertiaOfAxis(jRotAxisClde);
		double invICldr = collider->findInverseInertiaOfAxis(jRotAxisCldr);
	 */

	// 노말에 대한 속도 구하기
	/*
	 	double normVel = nV.dotProduct(velRel);
	 */

	// 노말과 COM 외적하기
	/*
	 	Vector3D rCldeXn = rClde.crossProduct(nV);
		Vector3D rCldrXn = rCldr.crossProduct(nV);
	 */

	// 노말 방향으로의 상대 속도 구하기
	/*
	 	double numerator = -(restitution + 1) * normVel;
	 */


	// 충돌에 대한 저항값 구하기
	// (1/mA + 1/mB)
	/*
	 	double denomenator = (collider->getInverseMass() + collidee->getInverseMass() + rCldrXn.getMagnitudeSquared() * invICldr + rCldeXn.getMagnitudeSquared() * invIClde);
	 */

	// j = numerator / denominator
	/*
	 	double impulseMagnitude = abs(numerator / denomenator);
		Vector3D impulse = nV.multiply(impulseMagnitude);
	 */

	// 노멀 임펄스(반발) 계산한 다음, 접촉점에서 미끄러지는 방향
	// 점선 방향 k 만들기
	/*
		Vector3D velParralel = nV.multiply(normVel);
		Vector3D k = velRel.sub(velParralel);
	 */

	// k = velRel - velParralel
	//→ 상대속도에서 노멀 성분을 제거 = “접선 성분(미끄러지는 성분)”
	//	즉, k는 접촉면을 따라 미끄러지는 방향(정확히는 벡터, 아직 단위벡터 아님)
	/*
		if (k.notZero()) {
		k = k.getUnitVector();
	 */

	// 마찰 임펄스도 “충돌점 임펄스”라서 회전 항이 분모에 들어감
	/*
	  	jRotAxisCldr = rCldr.crossProduct(k);
		jRotAxisClde = rClde.crossProduct(k);
		invIClde = collidee->findInverseInertiaOfAxis(jRotAxisClde);
		invICldr = collider->findInverseInertiaOfAxis(jRotAxisCldr);
	 */

	//numerator: “접선 방향 상대속도를 0으로 만들고 싶다”
	/*
		 numerator = k.dotProduct(velRel);
	 */


	// denominator: 접선 방향 “effective mass”
	/*
		Vector3D rCldrXk = rCldr.crossProduct(k);
		Vector3D rCldeXk = rClde.crossProduct(k);
		denomenator = (invMassA + invMassB
		    + |rA×k|^2 * invIA
		    + |rB×k|^2 * invIB);
	 */

	// 5) 마찰 임펄스 크기 계산 + 클램프
	/*
		double frictionImpMag = abs(numerator / denomenator);
		double maxFriction = impulseMagnitude * collidee->getFriction();
		if (frictionImpMag > maxFriction) frictionImpMag = maxFriction;
		frictionImpulse = k.multiply(frictionImpMag);
	 */

	// 근데 현재 코드의 문제점 2개 (중요)
	/*
		frictionImpMag = abs(numerator / denomenator);
		frictionImpulse = k * frictionImpMag;

		-> 고치라고 한다
		double vt = dot(velRel, k);
		double jt = -vt / denom;
		jt = clamp(jt, -mu * jn, +mu * jn);
		frictionImpulse = k * jt;
	 */

	// Impulse가 Zero가 아니라면
	/*
	if (impulse.notZero()) {
		collider->applyImpulseAtPosition(impulse, colPoint);
		impulse = impulse.multiply(-1);
		collidee->applyImpulseAtPosition(impulse, colPoint);

		//collision history is meant to be used by outside classes to be able to know what physics
		//stuff has taken place since their last checked
		if (collider->trackingCollHistory()) {
			collider->addToColHistory(collidee->getID(), impulseMagnitude);
		}
		if (collidee->trackingCollHistory()) {
			collidee->addToColHistory(collider->getID(), impulseMagnitude);
		}
	}
	 */

	// 마찰력이 Zero가 아니라면 
	/*
	 	if (frictionImpulse.notZero()) {
		collider->applyImpulseAtPosition(frictionImpulse, colPoint);
		frictionImpulse = frictionImpulse.multiply(-1);
		collidee->applyImpulseAtPosition(frictionImpulse, colPoint);
		double angVelAfter = collider->getAngularVelocity().getMagnitudeSquared();
	}
	 */

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

	//float fCorrected = fmaxf(0.f, pInfo->fDepth - m_fSloap);

	if (pInfo->B->Get_ColType() == STATIC)
	{
		fInvB = 0.f;
		fTotalInv = fInvA;
	}

	float fMoveA = pInfo->fDepth * (fInvA / fTotalInv);
	float fMoveB = pInfo->fDepth * (fInvB / fTotalInv);

	DebugHelper::Print_Vec3(L"Norm", pInfo->vN);
	DebugHelper::Print_Float(L"MoveA", fMoveA);
	DebugHelper::Print_Float(L"MoveB", fMoveB);

	if (fMoveA > 0.f)
		pInfo->A->Get_Transform()->Translate(pInfo->vN * fMoveA);

	if (fMoveB > 0.f)
		pInfo->B->Get_Transform()->Translate(pInfo->vN * fMoveB);

	DebugHelper::Print_Vec3(L"Pos A", pInfo->A->Get_Transform()->Get_Position());
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
	// 두 개의 상대 속도를 가져와서,  노멀 벡터와 내적한 값이 양수면 멀어지는 중이다.
	// 노멀은 A -> B 
	Vec3 vVelA = pInfo->A->Get_Rigidbody()->Get_PointVelocity(pInfo->vPoint);
	Vec3 vVelB= pInfo->B->Get_Rigidbody()->Get_PointVelocity(pInfo->vPoint);

	Vec3 vVelRelative = vVelB - vVelA;

	if( D3DXVec3Dot(&vVelRelative, &pInfo->vN) > 0)
	{
		return true;
	}
	return false;
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

