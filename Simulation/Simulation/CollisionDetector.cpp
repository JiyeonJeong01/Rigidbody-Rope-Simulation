#include "pch.h"
#include "CollisionDetector.h"

#include "Object.h"

#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "BoxCollider.h"

#include "Transform.h"
#include "Rigidbody.h"
#include "VectorHelper.h"
#include "Raycast.h"

#include "MathHelper.h"

bool CollisionDetector::s_bTEST = false;


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

			GEOMETRY_TYPE eCldr = pCollider->Get_GeometryType();
			GEOMETRY_TYPE eClde = pCollidee->Get_GeometryType();

			bool bOnCollision(false);
			CONTACT_INFO tContact;

			// TODO : 구조 바꿔야 한다 -> 함수 테이블이나 디스패쳐 등으로...  
			if (eCldr == BOX && eClde == BOX)
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
			else if (eCldr == BOX && eClde == PLANE)
			{
				bOnCollision = Detect_BoxPlaneCollision(&tContact, static_cast<BoxCollider*>(pCollider), static_cast<PlaneCollider*>(pCollidee));
			}
			else if (eCldr == PLANE && eClde == BOX)
			{
				bOnCollision = Detect_BoxPlaneCollision(&tContact, static_cast<BoxCollider*>(pCollidee), static_cast<PlaneCollider*>(pCollider));
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

	Vec3 vCldrCom = PhysicsWorld::GetInstance()->Try_GetBody(pCollider->Get_Rigidbody()->Get_BodyID())->vCOM;
	Vec3 vCldeCom = PhysicsWorld::GetInstance()->Try_GetBody(pCollidee->Get_Rigidbody()->Get_BodyID())->vCOM;

	pOut->vResolveN_A = VectorHelper::Get_Normalized(vCldrCom - vCldeCom);
	pOut->vPenetrateN_A = pOut->vResolveN_A * -1.f;
	pOut->vPoint = vCldrPos - pOut->vResolveN_A * fCldrRadius;
	pOut->vN_PlaneA = pOut->vPoint - vCldrCom;
	pOut->vN_PlaneB = pOut->vPoint - vCldeCom;
	pOut->fDepth = fRadiusSum - fDist;

	return true;
}

bool CollisionDetector::Detect_BoxPlaneCollision(CONTACT_INFO* pOut, BoxCollider* pBox, PlaneCollider* pPlane)
{
    if (!pOut || !pBox || !pPlane)
        return false;

    BODY* bB = PhysicsWorld::GetInstance()->Try_GetBody(pBox->Get_Rigidbody()->Get_BodyID());
    if (!bB) return false;

    Vec3 vPlaneN = VectorHelper::Get_Normalized(pPlane->Get_NormVector());

    // ==================== !!!!! COM이 가운데에 ㄷ없음
    const Vec3 vBoxCom = bB->vCOM;

    if (vBoxCom.y <= -4.f)
    {
        DebugHelper::DEBUG_POINT = 0;
    }

    const float fDist = pPlane->Calculate_SignedDistToPlane(vBoxCom);
    const float fAbsDist = fabsf(fDist);

    Vec3 vRight = pBox->Get_WorldAxis(AXIS_X);   
    Vec3 vUp = pBox->Get_WorldAxis(AXIS_Y);   
    Vec3 vLook = pBox->Get_WorldAxis(AXIS_Z);   

    vRight = VectorHelper::Get_Normalized(vRight);
    vUp = VectorHelper::Get_Normalized(vUp);
    vLook = VectorHelper::Get_Normalized(vLook);

    Vec3 vHalfDim = pBox->Get_Dimension() * 0.5f;

    const float fBoxRad =
        fabsf(VectorHelper::DotProduct(vPlaneN, vRight)) * vHalfDim.x +
        fabsf(VectorHelper::DotProduct(vPlaneN, vUp)) * vHalfDim.y +
        fabsf(VectorHelper::DotProduct(vPlaneN, vLook)) * vHalfDim.z;

    const float fDiff = fAbsDist - fBoxRad;
    const float fEpsilon = 1e-5f;
    if (fDiff > fEpsilon)
        return false;

    Vec3 nSep = (fDist >= 0.f) ? vPlaneN : -vPlaneN;

    // 간단한 버전 
    Vec3 vPlanePoint = vBoxCom - vPlaneN * fDist;

    // 복잡한 버전
    //Vec3 dirToPlane = -nSep; // box에서 plane쪽
    //float sx = (VectorHelper::DotProduct(dirToPlane, vRight) >= 0.f) ? 1.f : -1.f;
    //float sy = (VectorHelper::DotProduct(dirToPlane, vUp) >= 0.f) ? 1.f : -1.f;
    //float sz = (VectorHelper::DotProduct(dirToPlane, vLook) >= 0.f) ? 1.f : -1.f;

    //Vec3 support = vBoxCom
    //    + vRight * (sx * vHalfDim.x)
    //    + vUp * (sy * vHalfDim.y)
    //    + vLook * (sz * vHalfDim.z);

    //float ds = pPlane->Calculate_SignedDistToPlane(support);
    //Vec3 vPlanePoint = support - vPlaneN * ds;

    if (!pPlane->Get_IsInfinite())
    {
        if (!pPlane->Is_Contacting(vPlanePoint))
            return false;
    }

    pOut->A = pBox;
    pOut->B = pPlane;

    pOut->vResolveN_A = nSep;
    pOut->vPenetrateN_A = -nSep;
    pOut->vN_PlaneA = -nSep;
    pOut->vN_PlaneB = nSep;

    pOut->vPoint = vPlanePoint;
    pOut->fDepth = fBoxRad - fAbsDist;

    return true;
}

bool CollisionDetector::Detect_SpherePlaneCollition(CONTACT_INFO* pOut, SphereCollider* pSphere, PlaneCollider* pPlane)
{
	BODY* bS = PhysicsWorld::GetInstance()->Try_GetBody(pSphere->Get_Rigidbody()->Get_BodyID());

	if (!bS) return false;

	Vec3 vSphereCom = bS->vCOM;

	float fDistSphereToPlane = pPlane->Calculate_SignedDistToPlane(vSphereCom);
	float fAbsDist = fabsf(fDistSphereToPlane);

	float fSphereRadius = pSphere->Get_Radius() * pSphere->Get_Scale();

	float fDiff = fAbsDist - fSphereRadius;
	const float fEpsilon = 1e-5f;

	if (fDiff > fEpsilon)
		return false;

	Vec3 vPlaneN = VectorHelper::Get_Normalized(pPlane->Get_NormVector());
	Vec3 nSep = (fDistSphereToPlane >= 0.f) ? vPlaneN : -vPlaneN;

	Vec3 vPlanePoint = vSphereCom - vPlaneN * fDistSphereToPlane;

	if (!pPlane->Get_IsInfinite())
	{
		if (!pPlane->Is_Contacting(vPlanePoint))
			return false;
	}

	pOut->A = pSphere;
	pOut->B = pPlane;

	// vResolveN_A = A를 B에게서 떼어내는 방향(separation normal)
	pOut->vResolveN_A = nSep;
	pOut->vPenetrateN_A = -nSep;
	pOut->vN_PlaneA = -nSep;
	pOut->vN_PlaneB = nSep;

	pOut->vPoint = vPlanePoint;
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

bool CollisionDetector::Detect_RayPlaneCollision(tagRay* pRay, PlaneCollider* pPlane)
{
	pPlane->Get_Object()->On_CollisionExit(COLLISION{});

	const Vec3 vOrigin = pRay->vOrigin;
	const Vec3 vDir = pRay->vDiretion;
	const Vec3 vNorm = pPlane->Get_NormVector();
	const float fPlaneD = pPlane->Get_D(); // n·x + d = 0

	const float fDenom = D3DXVec3Dot(&vNorm, &vDir);
	if (fabsf(fDenom) < 1e-6f)
		return false; // 평행

	const float t = -(D3DXVec3Dot(&vNorm, &vOrigin) + fPlaneD) / fDenom;

	// vOriging 뒤 쪽
	if (t < 0.f)
		return false;

	if (t > pRay->fMaxDist)
		return false;

	const Vec3 vHit = vOrigin + vDir * t;

	if (!pPlane->Is_Contacting(vHit))
		return false;

	if (pRay->pHit)
		pRay->pHit->vPoint = vHit;
			pPlane->Get_Object()->On_CollisionEnter(COLLISION{});

	return true;
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
