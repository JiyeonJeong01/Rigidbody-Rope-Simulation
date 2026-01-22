#include "pch.h"
#include "CollisionDetector.h"

#include "Object.h"

#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "BoxCollider.h"
#include "Ground.h"

#include "Transform.h"
#include "Rigidbody.h"
#include "VectorHelper.h"
#include "Raycast.h"

#include "MathHelper.h"
#include "Raycast.h"
#include "Wall.h"

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
    Register_DetectTable();

	return S_OK;
}

void CollisionDetector::Process_NarrowPhase(const vector<COLLIDER_PAIR>& pairs)
{
    for (auto& pair : pairs)
    {
        Collider* pCollider = pair.A;
        Collider* pCollidee = pair.B;

        if (pCollider == nullptr || pCollidee == nullptr)
            continue;

        GEOMETRY_TYPE eCldr = pCollider->Get_GeometryType();
        GEOMETRY_TYPE eClde = pCollidee->Get_GeometryType();

        bool bOnCollision(false);
        CONTACT_DESC tContact;

        auto& fn = m_DetectTable[eCldr][eClde];
        if (!fn) continue;
        bOnCollision = fn(&tContact, pCollider, pCollidee);

        if (bOnCollision)
        {
            Fill_CollisionInfo(&tContact);
            PhysicsWorld::GetInstance()->Add_ContactInfo(tContact);
            PhysicsWorld::GetInstance()->Add_ContactPair(PAIR_KEY(pCollider->Get_ColliderID(), pCollidee->Get_ColliderID()));
        }
    }
}

void CollisionDetector::Generate_BroadPhase_Pairs(const vector<Collider*>& allColliders, vector<COLLIDER_PAIR>& outPair)
{
    vector<Collider*> staticCols;

    const size_t iTotalCnt = static_cast<int>(allColliders.size());
    for (size_t i = 0; i < iTotalCnt; ++i)
    {
        Collider* pCldr = allColliders[i];

        if (!pCldr || pCldr->Get_ColType() != COL_TYPE::C_DYNAMIC)
            continue;

        const AABB_DESC tCldrAABB = pCldr->Get_AABB();

        /* Dynamic <-> Static */
        staticCols.clear();
        PhysicsWorld::GetInstance()->Query_StaticOverlap(tCldrAABB, &staticCols);

        if (staticCols.empty())
            continue;

        for (auto* pStatic : staticCols)
        {
            if (!pStatic) continue;

            if(Check_AABB_Overlap(tCldrAABB, pStatic->Get_AABB()))
            {
                outPair.emplace_back(pCldr, pStatic);
            }
        }

        /* Dynamic <-> Dynamic */
        for (size_t j = i + 1; j < iTotalCnt; ++j)
        {
            Collider* pClde = allColliders[j];

            if (!pClde || pClde->Get_ColType() != COL_TYPE::C_DYNAMIC)
                continue;

            if (Check_AABB_Overlap(tCldrAABB, pClde->Get_AABB()))
            {
                outPair.emplace_back(pCldr, pClde);
            }
        }
    }
}

bool CollisionDetector::Detect_ShpereCollision(CONTACT_DESC* pOut, SphereCollider* pCollider, SphereCollider* pCollidee)
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

bool CollisionDetector::Detect_BoxPlaneCollision(CONTACT_DESC* pOut, BoxCollider* pBox, PlaneCollider* pPlane)
{
    if (!pOut || !pBox || !pPlane)
        return false;

    BODY_DESC* bB = PhysicsWorld::GetInstance()->Try_GetBody(pBox->Get_Rigidbody()->Get_BodyID());
    if (!bB) return false;

    Vec3 vPlaneN = VectorHelper::Get_Normalized(pPlane->Get_NormVector());

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

    Vec3 vPlanePoint = vBoxCom - vPlaneN * fDist;

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

bool CollisionDetector::Detect_SpherePlaneCollision(CONTACT_DESC* pOut, SphereCollider* pSphere, PlaneCollider* pPlane)
{
	BODY_DESC* bS = PhysicsWorld::GetInstance()->Try_GetBody(pSphere->Get_Rigidbody()->Get_BodyID());

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

bool CollisionDetector::Detect_Ray(RAYCAST_HIT* pRayHit, tagRay* pRay)
{
	const auto& vecCollider = PhysicsWorld::GetInstance()->Get_Colliders();
	size_t iTotalColCnt = vecCollider.size();

	for (int i = 0; i < iTotalColCnt; ++i)
	{
		if (vecCollider[i]->Get_GeometryType() == PLANE)
		{
			Detect_RayPlaneCollision(pRayHit, pRay, static_cast<PlaneCollider*>(vecCollider[i]));
		}
	}

    // 감지 실패 
    if (pRayHit->tTargetList.size() == 0)
        return false;

    pRayHit->vRayOrigin = pRay->vOrigin;
    pRayHit->vRayDir = pRay->vDiretion;

    float fClosest = numeric_limits<float>::max();
    HIT_TARGET tClosest{};

    for(auto t : pRayHit->tTargetList)
    {
        float fDist = VectorHelper::Get_Length(pRayHit->vRayOrigin - t.vPoint);

        if (fDist < fClosest)
        {
            fClosest = fDist;
            tClosest = t;
        }
    }

    pRayHit->tTarget = tClosest;

    return true;
}

bool CollisionDetector::Detect_RayPlaneCollision(RAYCAST_HIT* pRayHit, tagRay* pRay, PlaneCollider* pPlane)
{
	pPlane->Get_Object()->On_CollisionExit(COLLISION_DESC{});

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

    pRayHit->tTargetList.push_back({ pPlane->Get_Object(), vHit });

	return true;
}

void CollisionDetector::Register_DetectTable()
{
    /* 함수 테이블 등록 */

    /* SPHERE -> */
    m_DetectTable[SPHERE][SPHERE] = [this](CONTACT_DESC* pInfo, Collider* c1, Collider* c2)
        -> bool { return Detect_ShpereCollision(pInfo, static_cast<SphereCollider*>(c1), static_cast<SphereCollider*>(c2)); };
    m_DetectTable[SPHERE][BOX] = [this](CONTACT_DESC* pInfo, Collider* c1, Collider* c2)
        -> bool { return /* NOT IMPLEMENTED */ false; };
    m_DetectTable[SPHERE][PLANE] = [this](CONTACT_DESC* pInfo, Collider* c1, Collider* c2)
        -> bool { return Detect_SpherePlaneCollision(pInfo, static_cast<SphereCollider*>(c1), static_cast<PlaneCollider*>(c2)); };

    /* BOX -> */
    m_DetectTable[BOX][SPHERE] = [this](CONTACT_DESC* pInfo, Collider* c1, Collider* c2)
        -> bool {  return /* NOT IMPLEMENTED */ false; };
    m_DetectTable[BOX][BOX] = [this](CONTACT_DESC* pInfo, Collider* c1, Collider* c2)
        -> bool {  return /* NOT IMPLEMENTED */ false; };
    m_DetectTable[BOX][PLANE] = [this](CONTACT_DESC* pInfo, Collider* c1, Collider* c2)
        -> bool {  return Detect_BoxPlaneCollision(pInfo, static_cast<BoxCollider*>(c1), static_cast<PlaneCollider*>(c2)); };

    /* PLANE -> */
    m_DetectTable[PLANE][SPHERE] = [this](CONTACT_DESC* pInfo, Collider* c1, Collider* c2)
        -> bool {  return Detect_SpherePlaneCollision(pInfo, static_cast<SphereCollider*>(c2), static_cast<PlaneCollider*>(c1));  };
    m_DetectTable[PLANE][BOX] = [this](CONTACT_DESC* pInfo, Collider* c1, Collider* c2)
        -> bool {  return Detect_BoxPlaneCollision(pInfo, static_cast<BoxCollider*>(c2), static_cast<PlaneCollider*>(c1)); };
    m_DetectTable[PLANE][PLANE] = [this](CONTACT_DESC* pInfo, Collider* c1, Collider* c2)
        -> bool {  return /* NOT IMPLEMENTED */ false;  };

}

void CollisionDetector::Fill_CollisionInfo(CONTACT_DESC* pInfo)
{
    /* Fill A's Collision Info */
    pInfo->tCollisionA.pCounterObject = pInfo->B->Get_Object();
    pInfo->tCollisionA.pCounterCollider = pInfo->B;
    pInfo->tCollisionA.vPoint = pInfo->vPoint;

    /* Fill B's Collision Info */
    pInfo->tCollisionB.pCounterObject = pInfo->A->Get_Object();
    pInfo->tCollisionB.pCounterCollider = pInfo->A;
    pInfo->tCollisionB.vPoint = pInfo->vPoint;
}


bool CollisionDetector::Check_AABB_Overlap(const AABB_DESC& a, const AABB_DESC& b)
{
    if (a.vMax.x < b.vMin.x || a.vMin.x > b.vMax.x)
        return false;
    if (a.vMax.y < b.vMin.y || a.vMin.y > b.vMax.y)
        return false;
    if (a.vMax.z < b.vMin.z || a.vMin.z > b.vMax.z)
        return false;
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
