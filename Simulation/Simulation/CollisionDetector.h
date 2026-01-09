#pragma once
#include "PhysicsWorld.h"

class Collider;
class SphereCollider;
class PlaneCollider;

struct tagRay;

class CollisionDetector
{
private :
	CollisionDetector();
	~CollisionDetector();

public :
	HRESULT			Ready_System();
	void					Generate_ContactInfo();
	void					NarrowPhase_ObjectToObject();
	void					Clear_CollisionGroup();

	static bool		Detect_ShpereCollision(CONTACT_INFO* pOut, SphereCollider* pCollider, SphereCollider* pCollidee);
	static bool		Detect_SpherePlaneCollition(CONTACT_INFO* pOut, SphereCollider* pCollider, PlaneCollider* pCollidee);

	bool					Detect_Ray(tagRay* pRay);
	static bool		Detect_RayPlaneCollision(tagRay* pRay, PlaneCollider* pPlane);

public :
	static CollisionDetector* Create();
	void Release();
};

// Dispatcher·Î º¯°æ
// https://chatgpt.com/c/695ba8e3-6f10-8321-9663-39ed91612f2f