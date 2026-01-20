#pragma once
#include "PhysicsWorld.h"

class Collider;
class SphereCollider;
class PlaneCollider;
class BoxCollider;

using DetectFunc = std::function<bool(CONTACT_INFO*, Collider*, Collider*)>;

class CollisionDetector
{
private :
	CollisionDetector();
	~CollisionDetector();

public :
	HRESULT	Ready_System();
	void	Generate_ContactInfo();
	void	NarrowPhase_ObjectToObject();

	bool	Detect_ShpereCollision(CONTACT_INFO* pOut, SphereCollider* pCollider, SphereCollider* pCollidee);
    bool	Detect_SpherePlaneCollision(CONTACT_INFO* pOut, SphereCollider* pCollider, PlaneCollider* pCollidee);

    bool	Detect_BoxPlaneCollision(CONTACT_INFO* pOut, BoxCollider* pBox, PlaneCollider* pPlane);

	bool	Detect_Ray(struct tagRaycastHit* pRayHit, struct tagRay* pRay);
    bool	Detect_RayPlaneCollision(struct tagRaycastHit* pRayHit, tagRay* pRay, PlaneCollider* pPlane);

private:
    array<array<DetectFunc, GEOMETRY_TYPE::G_END>, GEOMETRY_TYPE::G_END> m_DetectTable = { nullptr };

public :
	static CollisionDetector* Create();
	void Release();

private :
	// TEST
	static bool s_bTEST;
};
