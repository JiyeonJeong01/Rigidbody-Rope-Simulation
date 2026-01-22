#pragma once
#include "Component.h"
#include "Physics_Struct.h"

class Rigidbody;

class Collider :  public Component
{
protected:
	Collider(LPDIRECT3DDEVICE9 pGraphicDev, Object* pOwner);
	~Collider() override;

public :
	virtual GEOMETRY_TYPE		Get_GeometryType()							PURE;
    virtual void                Update_AABB();

	uint_fast16_t				Get_ColliderID() const						{ return m_iId;}	
	void						Set_ColliderID(uint_fast16_t iId)			{ m_iId = iId; }
	HRESULT						Resolve_Dependency() override;

public :
	// Getter/Setter

	bool						Get_OnCol() const							{ return m_bOnCol; }
	void						Set_OnCol(const bool& bOnCol)				{ m_bOnCol = bOnCol; }

	const Vec3&					Get_Offset() const							{ return m_vOffset; }
	void						Set_Offset(const Vec3& vOffset)				{ m_vOffset = vOffset;}

	float						Get_Scale() const							{ return m_fScale; }
	void						Set_Scale(const float& fScale)				{ m_fScale = fScale; }

	Rigidbody*					Get_Rigidbody();
    const AABB_DESC&            Get_AABB()  const                           { return m_tAABB;}
    COL_TYPE                    Get_ColType() const                         { return m_eColType; }
    void                        Set_ColType(COL_TYPE eType)                 { m_eColType = eType; }

protected :
    virtual void                Calc_AABB() {};

protected :
    AABB_DESC                   m_tAABB;
    COL_TYPE                    m_eColType;
	uint_fast16_t				m_iId;
	Rigidbody*					m_pRigidbody;
	BODY_TYPE					m_eBodyType;
	Vec3						m_vOffset;
	float						m_fScale;
	bool						m_bOnCol;

public:
	void Release() override;
};

