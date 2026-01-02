#pragma once
#include "Object.h"

class Transform;
class Sphere;

class Player : public Object
{
private :
	Player(LPDIRECT3DDEVICE9 pGraphicDevice);
	~Player();

public :
	HRESULT			Ready_GameObject();
	int					Update_GameObject(const float& fTimeDelta);
	void					LateUpdate_GameObject(const float& fTimeDelta);
	void					Render_GameObject();

private :
	void					Handle_PlayerInput(const float& fTimeDelta);

private :
	Transform*									m_pTransform;
	Sphere*										m_pMesh;

public :
	static Player* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	void Release() override;
};

