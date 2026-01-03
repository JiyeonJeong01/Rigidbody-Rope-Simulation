#pragma once
#include "Component.h"

typedef struct tagCollision
{
	friend class Collider;
	Object* pCounterObject;			// 충돌 당한 오브젝트
	Collider* pCounterCollider;		// 충돌 당한 오브젝트의 콜라이더
	COL_STATE       eColState;	
} Collision;

class Collider :  public Component
{
protected:
	Collider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	virtual ~Collider();

public :
	virtual GEOMETRY_TYPE		Get_GeometryType()							PURE;
	virtual  bool							Check_Collision(Collider* pCollider) PURE;

	bool											Is_Overlapped(Collider* pCollider);
	void											Add_OverlapCollider(Collider* pCollider);
	void											Remove_OverlapCollider(Collider* pCollider);

public :
	// Getter/Setter
	unordered_set<Collider*>&	Get_OverlapCollider()						{ return m_usetOverlapCol; }

	bool											Get_OnCol() const								{ return m_bOnCol; }
	void											Set_OnCol(const bool& bOnCol)		{ m_bOnCol = bOnCol; }

	const Collision&						Get_Collision() const							{ return m_tCollision; }
	void											Set_Collisiont(const Collision& tCol) { m_tCollision = tCol; }

	COL_STATE								Get_ColState() const							{ return m_eColState; }
	void											Set_ColState(COL_STATE eColState) { m_eColState = eColState; }

	const Vec3&							Get_Offset() const								{ return m_vOffset; }
	void											Set_Offset(const Vec3& vOffset)		{ m_vOffset = vOffset;}

	float											Get_Scale() const								{ return m_fScale; }
	void											Set_Scale(const float& fScale)			{ m_fScale = fScale; }

protected :
	Collision									m_tCollision;
	COL_STATE								m_eColState;
	bool											m_bOnCol;
	Vec3											m_vOffset;
	float											m_fScale = 1.f;

	unordered_set<Collider*>		m_usetOverlapCol;

public:
	void Release() override;
};

