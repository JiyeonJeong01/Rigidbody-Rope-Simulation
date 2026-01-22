#pragma once
#include "PhysicsWorld.h"

class Collider;
class SphereCollider;
class PlaneCollider;
class BoxCollider;

using DetectFunc = std::function<bool(CONTACT_DESC*, Collider*, Collider*)>;

class CollisionDetector
{
private :
	CollisionDetector();
	~CollisionDetector();

public :
	HRESULT	Ready_System();
    /* Broad Phase */
    void    Generate_BroadPhase_Pairs(const vector<Collider*>& allColliders, _Out_ vector< COLLIDER_PAIR>& outPair);

    /* Narrow Phase */
	void	Process_NarrowPhase(const vector<COLLIDER_PAIR>& pairs);
	bool	Detect_ShpereCollision(CONTACT_DESC* pOut, SphereCollider* pCollider, SphereCollider* pCollidee);
    bool	Detect_SpherePlaneCollision(CONTACT_DESC* pOut, SphereCollider* pCollider, PlaneCollider* pCollidee);

    bool	Detect_BoxPlaneCollision(CONTACT_DESC* pOut, BoxCollider* pBox, PlaneCollider* pPlane);

	bool	Detect_Ray(struct tagRaycastHit* pRayHit, struct tagRay* pRay);
    bool	Detect_RayPlaneCollision(struct tagRaycastHit* pRayHit, tagRay* pRay, PlaneCollider* pPlane);

private :
    void    Register_DetectTable();
    void    Fill_CollisionInfo(CONTACT_DESC* pInfo);
    bool    Check_AABB_Overlap(const AABB_DESC& a, const AABB_DESC& b);

private:
    array<array<DetectFunc, GEOMETRY_TYPE::G_END>, GEOMETRY_TYPE::G_END> m_DetectTable = { nullptr };

public :
	static CollisionDetector* Create();
	void Release();

private :
	// TEST
	static bool s_bTEST;
};

