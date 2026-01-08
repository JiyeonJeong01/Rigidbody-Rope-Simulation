#include "pch.h"
#include "Wall.h"
#include "pch.h"
#include "Wall.h"

#include "Plane.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "PlaneCollider.h"

Wall::Wall(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Object(pGraphicDevice)
	, m_pTransform(nullptr), m_pMesh(nullptr)
{
}

Wall::~Wall()
{
}

HRESULT Wall::Ready_GameObject()
{
	m_pMesh = Plane::Create(m_pGraphicDevice, this, D3DCOLOR_ARGB(255, 0, 0, 255), 10.f, 10.f);

	m_pTransform = Transform::Create(m_pGraphicDevice, this);

	m_pCollider = PlaneCollider::Create(m_pGraphicDevice, this);
	m_pCollider->Set_Dimension({ 10.f, 10.f });

	return S_OK;
}

int Wall::Update_GameObject(const float& fTimeDelta)
{
	Object::Update_GameObject(fTimeDelta);

	return 0;
}

void Wall::LateUpdate_GameObject(const float& fTimeDelta)
{
	Object::LateUpdate_GameObject(fTimeDelta);
}

void Wall::Render_GameObject()
{
	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();
}

void Wall::On_CollisionEnter(const Collision& tCollision)
{
	Object::On_CollisionEnter(tCollision);
}

void Wall::On_CollisionStay(const Collision& tCollision)
{
	Object::On_CollisionStay(tCollision);

	m_pMesh->Set_Hilight(true);
}

void Wall::On_CollisionExit(const Collision& tCollision)
{
	Object::On_CollisionExit(tCollision);
	m_pMesh->Set_Hilight(false);
}

void Wall::Set_Position(const Vec3& vPosition)
{
	Get_Transform()->Set_Position(vPosition);
}


Wall* Wall::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	Wall* pWall = new Wall(pGraphicDevice);

	if (FAILED(pWall->Ready_GameObject()))
	{
		Safe_Delete(pWall);
	}

	return pWall;
}

void Wall::Release()
{
	m_pMesh->Release();

	Object::Release();
}
