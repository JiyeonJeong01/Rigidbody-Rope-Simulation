#include "pch.h"
#include "CollisionDetector.h"

#include "Object.h"

#include "SphereCollider.h"
#include "PlaneCollider.h"

#include "Transform.h"
#include "Rigidbody.h"
#include "VectorHelper.h"
#include "Raycast.h"

#include "MathHelper.h"

CollisionDetector::CollisionDetector()
{
}

CollisionDetector::~CollisionDetector()
{
	Release();
}

HRESULT CollisionDetector::Ready_System()
{
	return S_OK;
}

void CollisionDetector::Generate_ContactInfo()
{
	NarrowPhase_ObjectToObject();
}

void CollisionDetector::NarrowPhase_ObjectToObject()
{
	// TODO !! 레이어 마스크 처리 


	const auto& vecCollider = PhysicsWorld::GetInstance()->Get_Colliders();

	size_t iTotalColCnt = vecCollider.size();

	for (int i = 0; i < iTotalColCnt; ++i)
	{
		Collider* pCollider = vecCollider[i];
		if (pCollider == nullptr || pCollider->Get_Object() == nullptr || pCollider->Get_Transform() == nullptr)
			continue;

		for (int j = i + 1; j < iTotalColCnt; ++j)
		{
			Collider* pCollidee = vecCollider[j];

			if (pCollidee == nullptr || pCollidee->Get_Object() == nullptr || pCollidee->Get_Transform() == nullptr)
				continue;

			pCollidee->Add_CheckedCollider(pCollider->Get_ColliderID());

			GEOMETRY_TYPE eCldr = pCollider->Get_GeometryType();
			GEOMETRY_TYPE eClde = pCollidee->Get_GeometryType();

			bool bOnCollision(false);
			CONTACT_INFO tContact;

			if (eCldr == CUBE && eClde == CUBE)
			{

			}
			else if (eCldr == SPHERE && eClde == SPHERE)
			{
				bOnCollision = Detect_ShpereCollision(&tContact, static_cast<SphereCollider*>(pCollider), static_cast<SphereCollider*>(pCollidee));
			}
			else if (eCldr == SPHERE && eClde == PLANE)
			{
				bOnCollision = Detect_SpherePlaneCollition(&tContact, static_cast<SphereCollider*>(pCollider), static_cast<PlaneCollider*>(pCollidee));
			}


			if (bOnCollision)
			{
				PhysicsWorld::GetInstance()->Add_ContactInfo(tContact);
				PhysicsWorld::GetInstance()->Add_ContactPair(PAIR_KEY(pCollider->Get_ColliderID(), pCollidee->Get_ColliderID()));
			}
		}
	}
}

bool CollisionDetector::Detect_ShpereCollision(CONTACT_INFO* pOut, SphereCollider* pCollider, SphereCollider* pCollidee)
{
	Vec3 vCldrPos = pCollider->Get_Transform()->Get_Position() + pCollider->Get_Offset();
	Vec3 vCldePos = pCollidee->Get_Transform()->Get_Position() + pCollidee->Get_Offset();

	Vec3 vDiff = vCldePos - vCldrPos;

	float fDist = D3DXVec3Length(&vDiff);
	float fCldrRadius = pCollider->Get_Radius() * pCollider->Get_Scale();
	float fCldeRadius = pCollidee->Get_Radius() * pCollidee->Get_Scale();
	float fRadiusSum = fCldrRadius + fCldeRadius;

	if (fRadiusSum < fDist)
		return false;
	pOut->A = pCollider;
	pOut->B = pCollidee;

	pOut->fDepth = fRadiusSum - fDist;
	if (fDist < 1e-6f)
		pOut->vN = Vec3(0.f, 1.f, 0.f); // 임의 축 또는 이전 프레임 normal
	else
		pOut->vN = vDiff / fDist;

	pOut->vPoint = vCldrPos + pOut->vN * (fCldrRadius - pOut->fDepth * 0.5f);

	// TODO : 솔버로 옮길 거 
	//Vec3 vCldrVel, vCldeVel;

	//if (pCollider->Get_ColType() != STATIC)
	//	vCldrVel = pCollider->Get_Rigidbody()->Get_PointVelocity(pOut->vPoint);
	//else
	//	vCldrVel = VectorHelper::Zero();

	//if (pCollidee->Get_ColType() != STATIC)
	//	vCldeVel = pCollidee->Get_Rigidbody()->Get_PointVelocity(pOut->vPoint);
	//else
	//	vCldeVel = VectorHelper::Zero();

	return true;
}

bool CollisionDetector::Detect_SpherePlaneCollition(CONTACT_INFO* pOut, SphereCollider* pSphere, PlaneCollider* pPlane)
{
	Vec3 vSpherePos = pSphere->Get_Transform()->Get_Position() + pSphere->Get_Offset();

	// 평면 위에 투영된 원의 중심과 원의 중심 간의 거리 구하기
	float fDistSphereToPlane = pPlane->Calculate_DistToPlane(vSpherePos);
	float fAbsDist = fabsf(fDistSphereToPlane);
	float fSphereRadius = pSphere->Get_Radius() * pSphere->Get_Scale();

	if (fAbsDist > fSphereRadius)
		return false;

	pOut->A = pSphere;
	pOut->B = pPlane;

	// NOTE !! 노멀 벡터를 뭘 써야할지 명확히 해야 한다.
	// 1. 표면의 법선 벡터를 쓸지? 아니면 충돌한 방향을 정규화할지? 
	//pOut->vN = pPlane->Calculate_DirToPlane(vSpherePos);
	pOut->vN = pPlane->Get_NormVector();
	pOut->vN = fDistSphereToPlane >= 0 ? pOut->vN : -pOut->vN;
	pOut->vPoint = vSpherePos - pOut->vN * fSphereRadius;
	pOut->fDepth = fSphereRadius - fAbsDist;

	return true;
}

bool CollisionDetector::Detect_Ray(tagRay* pRay)
{
	const auto& vecCollider = PhysicsWorld::GetInstance()->Get_Colliders();
	size_t iTotalColCnt = vecCollider.size();

	for (int i = 0; i < iTotalColCnt; ++i)
	{
		if (vecCollider[i]->Get_GeometryType() == PLANE)
		{
			if (Detect_RayPlaneCollision(pRay, static_cast<PlaneCollider*>(vecCollider[i])))
				return true;
		}
	}

	return false;
}

bool CollisionDetector::Detect_RayPlaneCollision(struct tagRay* pRay, PlaneCollider* pPlane)
{
	Vec3 vRayOrigin = pRay->vOrigin;
	Vec3 vRayDirection = pRay->vDiretion;

	Vec3 vPlaneNorm = pPlane->Get_NormVector();
	float fD = pPlane->Get_D();

	if (D3DXVec3Dot(&vPlaneNorm, &vRayDirection) == 0)
		return false;

	float t = D3DXVec3Dot(&vPlaneNorm, &vRayOrigin) * -1.f / D3DXVec3Dot(&vPlaneNorm, &vRayDirection);

	if (t < 0)
		return false;

	const Vec3 vHitPoint = vRayOrigin + vRayDirection * t;

	return pPlane->Is_OnPlane(vHitPoint);
}

CollisionDetector* CollisionDetector::Create()
{
	CollisionDetector* pInstance = new CollisionDetector();
	if (FAILED(pInstance->Ready_System()))
	{
		Safe_Release(pInstance);
		pInstance = nullptr;
	}

	return pInstance;
}

void CollisionDetector::Release()
{

}
