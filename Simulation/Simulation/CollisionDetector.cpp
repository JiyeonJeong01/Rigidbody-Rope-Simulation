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
			else if (eCldr == PLANE && eClde == SPHERE)
			{
				bOnCollision = Detect_SpherePlaneCollition(&tContact, static_cast<SphereCollider*>(pCollidee), static_cast<PlaneCollider*>(pCollider));
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

	pOut->vResolveN_A = VectorHelper::Get_Normalized(pCollider->Get_Rigidbody()->Get_COM() - pCollidee->Get_Rigidbody()->Get_COM());
	pOut->vPenetrateN_A = pOut->vResolveN_A * -1.f;
	pOut->vPoint = vCldrPos - pOut->vResolveN_A * fCldrRadius;
	pOut->vN_PlaneA = pCollider->Calculate_COMToPoint(pOut->vPoint);
	pOut->vN_PlaneB = pCollidee->Calculate_COMToPoint(pOut->vPoint);
	pOut->fDepth = fRadiusSum - fDist;

	return true;
}

bool CollisionDetector::Detect_SpherePlaneCollition(CONTACT_INFO* pOut, SphereCollider* pSphere, PlaneCollider* pPlane)
{
	Vec3 vSphereCom = pSphere->Get_Rigidbody()->Get_COM();

	// 평면 위에 투영된 원의 중심과 원의 중심 간의 거리 구하기
	float fDistSphereToPlane = pPlane->Calculate_SignedDistToPlane(vSphereCom);
	float fAbsDist = fabsf(fDistSphereToPlane);
	float fSphereRadius = pSphere->Get_Radius() * pSphere->Get_Scale();

	if (fAbsDist > fSphereRadius)
	{
		return false;
	}

	// TODO : 여기 수정 
	Vec3 vDir = pPlane->Calculate_ResolveDirFromPlane(vSphereCom);
	Vec3 vPoint = vSphereCom;
	vPoint.y = pPlane->Get_Rigidbody()->Get_COM().y;

	if (!pPlane->Get_IsInfinite())
	{
		if (!pPlane->Is_OnPlane(vPoint))
			return false;
	}

	pOut->A = pSphere;
	pOut->B = pPlane;

	// vResolveN_A : A를 B에게서 멀어지게 하는 법선 벡터 
	pOut->vResolveN_A = VectorHelper::Get_Normalized(vDir);
	pOut->vPenetrateN_A = VectorHelper::Get_Normalized(pPlane->Calculate_PenetrationDirToPlane(vSphereCom));
	pOut->vN_PlaneA = fDistSphereToPlane >= 0 ? pPlane->Get_NormVector() : pPlane->Get_NormVector() * -1.f;		// 일단은 평면이 오브젝트를 밀어내는 방향으로 정의
	pOut->vN_PlaneB = fDistSphereToPlane >= 0 ? pPlane->Get_NormVector() : pPlane->Get_NormVector() * -1.f;		// 일단은 평면이 오브젝트를 밀어내는 방향으로 정의
	pOut->vPoint = vPoint;
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
