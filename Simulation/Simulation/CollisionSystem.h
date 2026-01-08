#pragma once

class Collider;
class SphereCollider;
class PlaneCollider;

typedef struct tagResolveInfo
{
	Vec3					vN;
	Vec3					vPoint;
	Vec3					vImpulse;
	Vec3					vRelativeVel;
	float					fDepth;
}RESOLVE_INFO;


struct tagRay;

class CollisionSystem
{
	DECLARE_SINGLETON(CollisionSystem)

private :
	CollisionSystem();
	~CollisionSystem();

public :
	void					Update_System();
	void					Check_ObjectToObject();
	void					Clear_CollisionGroup();

	static bool		Detect_ShpereCollision(SphereCollider* pCollider, SphereCollider* pCollidee, RESOLVE_INFO* tOut);
	static bool		Detect_SpherePlaneCollition(RESOLVE_INFO* tOut, SphereCollider* pCollider, PlaneCollider* pCollidee);

	bool					Detect_Ray(tagRay* pRay);
	static bool		Detect_RayPlaneCollision(tagRay* pRay, PlaneCollider* pPlane);


	void					Add_Collider(Collider* pCollider);
	void					Remove_Collider(Collider* pCollider);
	void					Remove_CheckCollider(Collider* pCollider);

	void					Remove_CheckedColliderListAll();


private :
	vector<Collider*>		m_vecCollider;

public :
	void Release();
};

// Dispatcher·Î º¯°æ
// https://chatgpt.com/c/695ba8e3-6f10-8321-9663-39ed91612f2f