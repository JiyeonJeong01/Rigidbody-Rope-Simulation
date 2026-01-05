#include "pch.h"
#include "CollisionSystem.h"

#include "Object.h"

#include "SphereCollider.h"
#include "PlaneCollider.h"

#include "Transform.h"
#include "Rigidbody.h"
#include "VectorHelper.h"

IMPLEMENT_SINGLETON(CollisionSystem)

CollisionSystem::CollisionSystem()
{
}

CollisionSystem::~CollisionSystem()
{
	Release();
}

void CollisionSystem::Update_System()
{
	Remove_CheckedColliderListAll();

	size_t iTotalColCnt = m_vecCollider.size();

	for (int i = 0; i < iTotalColCnt; ++i)
	{
		Collider* pCollider = m_vecCollider[i];
		if (pCollider == nullptr || pCollider->Get_Object() == nullptr || pCollider->Get_Transform() == nullptr)
			continue;

		for (int j = i + 1; j < iTotalColCnt; ++j)
		{
			Collider* pCollidee = m_vecCollider[j];

			if (pCollidee == nullptr || pCollidee->Get_Object() == nullptr || pCollidee->Get_Transform() == nullptr)
				continue;

			pCollidee->Add_CheckedCollider(pCollider->Get_ColliderID());

			GEOMETRY_TYPE eCldr = pCollider->Get_GeometryType();
			GEOMETRY_TYPE eClde = pCollidee->Get_GeometryType();

			bool bOnCollision(false);
			RESOLVE_INFO tResolve;

			if (eCldr == CUBE && eClde == CUBE)
			{
				
			}
			else if (eCldr == SPHERE && eClde == SPHERE)
			{
				bOnCollision = Detect_ShpereCollision(static_cast<SphereCollider*>(pCollider), static_cast<SphereCollider*>(pCollidee), &tResolve);
			}
			else if (eCldr == SPHERE && eClde == PLANE)
			{
				bOnCollision = Detect_SpherePlaneCollition(&tResolve, static_cast<SphereCollider*>(pCollider), static_cast<PlaneCollider*>(pCollidee));
			}

			Collision tCldr, tClde;
			if (bOnCollision)
			{
				if (pCollider->Is_Contacted(pCollidee))
				{
					pCollider->Get_Object()->On_CollisionStay(tCldr);
					pCollidee->Get_Object()->On_CollisionStay(tClde);
				}
				else
				{
					pCollider->Add_ContactCollider(pCollidee);
					pCollidee->Add_ContactCollider(pCollider);
					pCollider->Get_Object()->On_CollisionEnter(tCldr);
					pCollidee->Get_Object()->On_CollisionEnter(tClde);
				}
			}
			else
			{				
				if (pCollider->Is_Contacted(pCollidee))
				{
					pCollider->Remove_ContactCollider(pCollidee);
					pCollidee->Remove_ContactCollider(pCollider);
					pCollider->Get_Object()->On_CollisionExit(tCldr);
					pCollidee->Get_Object()->On_CollisionExit(tClde);
				}
			}
		}
	}

	for (auto* c : m_vecCollider)
		if (c) c->Set_OnCol(c->Get_ContactCount() > 0);
}

void CollisionSystem::Clear_CollisionGroup()
{
	m_vecCollider.clear();
}

bool CollisionSystem::Detect_ShpereCollision(SphereCollider* pCollider, SphereCollider* pCollidee, RESOLVE_INFO* tOut)
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

	tOut->fDepth = fRadiusSum - fDist;
	if (fDist < 1e-6f)
		tOut->vN = Vec3(0.f, 1.f, 0.f); // 임의 축 또는 이전 프레임 normal
	else
		tOut->vN = vDiff / fDist;

	tOut->vPoint = vCldrPos + tOut->vN * (fCldrRadius - tOut->fDepth * 0.5f);

	Vec3 vCldrVel, vCldeVel;

	if (pCollider->Get_ColType() != STATIC)
		vCldrVel = pCollider->Get_Rigidbody()->Get_PointVelocity(tOut->vPoint);
	else
		vCldrVel = VectorHelper::Zero();

	if (pCollidee->Get_ColType() != STATIC)
		vCldeVel = pCollidee->Get_Rigidbody()->Get_PointVelocity(tOut->vPoint);
	else
		vCldeVel = VectorHelper::Zero();

	tOut->vRelativeVel = vCldeVel - vCldrVel;

	return true;
}

bool CollisionSystem::Detect_SpherePlaneCollition(RESOLVE_INFO* tOut, SphereCollider* pSphere, PlaneCollider* pPlane)
{
	Vec3 vSpherePos = pSphere->Get_Transform()->Get_Position() + pSphere->Get_Offset();
	Vec3 vPlanePos = pPlane->Get_Transform()->Get_Position() + pPlane->Get_Offset();

	// 1. 평면위에 투영된 원의 중심 구하기
	float fDistSphereToPlane = pPlane->Calculate_DistToPlane(vSpherePos);
	float fSphereRadius = pSphere->Get_Radius() * pSphere->Get_Scale();

	return fabsf(fDistSphereToPlane) < fSphereRadius;
}

void CollisionSystem::Add_Collider(Collider* pCollider)
{
	m_vecCollider.push_back(pCollider);
}

void CollisionSystem::Remove_Collider(Collider* pCollider)
{
	if (!pCollider)
		return;

	Remove_CheckCollider(pCollider);

	auto it = std::remove(m_vecCollider.begin(), m_vecCollider.end(), pCollider);
	m_vecCollider.erase(it, m_vecCollider.end());
}

void CollisionSystem::Remove_CheckCollider(Collider* pCollider)
{
	for (auto* p : m_vecCollider)
	{
		if (p && p != pCollider)
			p->Remove_ContactCollider(pCollider);
	}
}

void CollisionSystem::Remove_CheckedColliderListAll()
{
	for (auto* c : m_vecCollider)
		if (c) c->Clear_CheckedCollider();
}

void CollisionSystem::Release()
{
	Clear_CollisionGroup();
}
