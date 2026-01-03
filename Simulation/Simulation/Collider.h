#pragma once
#include "Component.h"

class Rigidbody;

typedef struct tagCollision
{
	friend class Collider;
	friend class Rigidbody;
	Object* pCounterObject;			// 충돌 당한 오브젝트
	Collider* pCounterCollider;		// 충돌 당한 오브젝트의 콜라이더
	Rigidbody* pCounterRigidbody;

	Vec3					vN;
	Vec3					vPoint;
	Vec3					vImpulse;
	Vec3					vRelativeVel;
	float					fDepth;
} Collision;

class Collider :  public Component
{
protected:
	Collider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~Collider();

public :
	virtual GEOMETRY_TYPE		Get_GeometryType()							PURE;
	virtual  bool							Check_Collision(Collider* pCollider) PURE;

	bool											Is_Contacted(Collider* pCollider);
	void											Add_ContactCollider(Collider* pCollider);
	void											Remove_ContactCollider(Collider* pCollider);
	int											Get_ContactCount() const					{ return (int)m_usetContactCols.size(); }

protected:
	Rigidbody*								Find_Rigidbody();

public :
	// Getter/Setter
	unordered_set<Collider*>&	Get_ContactCollider()						{ return m_usetContactCols; }

	bool											Get_OnCol() const								{ return m_bOnCol; }
	void											Set_OnCol(const bool& bOnCol)		{ m_bOnCol = bOnCol; }

	const Collision&						Get_Collision() const							{ return m_tCollision; }
	void											Set_CollisionInfo(const Collision& tCol) { m_tCollision = tCol; }

	COL_STATE								Get_ColState() const							{ return m_eColState; }
	void											Set_ColState(COL_STATE eColState) { m_eColState = eColState; }

	const Vec3&							Get_Offset() const								{ return m_vOffset; }
	void											Set_Offset(const Vec3& vOffset)		{ m_vOffset = vOffset;}

	float											Get_Scale() const								{ return m_fScale; }
	void											Set_Scale(const float& fScale)			{ m_fScale = fScale; }

	COLLIDER_TYPE						Get_ColType() const							{ return m_eColType; }
	Rigidbody*								Get_Rigidbody();

protected :
	Collision									m_tCollision;
	COL_STATE								m_eColState;
	COLLIDER_TYPE						m_eColType;
	bool											m_bOnCol;
	Vec3											m_vOffset;
	float											m_fScale = 1.f;

	unordered_set<Collider*>		m_usetContactCols;

	Rigidbody*								m_pRigidbody;

public:
	void Release() override;
};

