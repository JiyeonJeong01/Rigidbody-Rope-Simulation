#pragma once
#include "Object.h"

class Transform;
class Rigidbody;
class PlaneCollider;
class Plane;

class Wall : public Object
{
private:
	Wall(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Wall() override;

public:
	HRESULT			Ready_GameObject(unsigned long dwColor, float fSizeX, float fSizeY);
	int				Update_GameObject(const float& fTimeDelta);
	void			LateUpdate_GameObject(const float& fTimeDelta);
	void			Render_GameObject();

    void            Active_Highlight(bool bColor);

	void			On_CollisionEnter(const COLLISION& tCollision) override;
	void			On_CollisionStay(const COLLISION& tCollision) override;
	void			On_CollisionExit(const COLLISION& tCollision) override;

	void			Set_Position(const Vec3& vPosition);

private:
	PlaneCollider*	m_pCollider;
	Plane*			m_pMesh;
	Rigidbody*		m_pRigidbody;

public:
	static Wall* Create(LPDIRECT3DDEVICE9 pGraphicDevice, unsigned long dwColor, float fSizeX, float fSizeY);
	void Release() override;
};

