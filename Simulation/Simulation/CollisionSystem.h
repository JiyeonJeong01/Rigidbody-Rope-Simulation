#pragma once

class Collider;
class SphereCollider;

typedef struct tagResolveInfo
{
	Vec3					vN;
	Vec3					vPoint;
	Vec3					vImpulse;
	Vec3					vRelativeVel;
	float					fDepth;
}RESOLVE_INFO;


class CollisionSystem
{
	DECLARE_SINGLETON(CollisionSystem)

private :
	CollisionSystem();
	~CollisionSystem();

public :
	void					Update_System();
	void					Clear_CollisionGroup();

	static bool		Detect_ShpereCollision(SphereCollider* pCollider, SphereCollider* pCollidee, RESOLVE_INFO* tOut);

	void					Add_Collider(Collider* pCollider);
	void					Remove_Collider(Collider* pCollider);
	void					Remove_CheckCollider(Collider* pCollider);

private :
	vector<Collider*>		m_vecCollider;

public :
	void Release();
};

