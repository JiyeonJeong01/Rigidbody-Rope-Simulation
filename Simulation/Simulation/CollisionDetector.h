#pragma once
#include "PhysicsWorld.h"

class Collider;
class SphereCollider;
class PlaneCollider;
class BoxCollider;

class CollisionDetector
{
private :
	CollisionDetector();
	~CollisionDetector();

public :
	HRESULT			Ready_System();
	void			Generate_ContactInfo();
	void			NarrowPhase_ObjectToObject();
	void			Clear_CollisionGroup();

	static bool		Detect_ShpereCollision(CONTACT_INFO* pOut, SphereCollider* pCollider, SphereCollider* pCollidee);
    static bool		Detect_BoxPlaneCollision(CONTACT_INFO* pOut, BoxCollider* pBox, PlaneCollider* pPlane);
	static bool		Detect_SpherePlaneCollition(CONTACT_INFO* pOut, SphereCollider* pCollider, PlaneCollider* pCollidee);

	bool			Detect_Ray(struct tagRaycastHit* tOut, struct tagRay* pRay);
	static bool		Detect_RayPlaneCollision(struct tagRaycastHit* tOut, tagRay* pRay, PlaneCollider* pPlane);

public :
	static CollisionDetector* Create();
	void Release();

private :
	// TEST
	static bool s_bTEST;
};
