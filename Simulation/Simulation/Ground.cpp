#include "pch.h"
#include "Ground.h"

#include "Plane.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "PlaneCollider.h"

Ground::Ground(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Object(pGraphicDevice)
	, m_pTransform(nullptr), m_pMesh(nullptr)
{
}

Ground::~Ground()
{
}

HRESULT Ground::Ready_GameObject()
{
	m_pMesh = Plane::Create(m_pGraphicDevice, this, D3DCOLOR_ARGB(255, 0, 0, 255), 10.f, 10.f);

	m_pTransform = Transform::Create(m_pGraphicDevice, this);
	m_pTransform->Rotate(AXIS_X, 90.f);
	m_pTransform->Set_Position({ 0.f, -2.f, 0.f });

	m_pCollider = PlaneCollider::Create(m_pGraphicDevice, this);

	return S_OK;
}

int Ground::Update_GameObject(const float& fTimeDelta)
{
	Object::Update_GameObject(fTimeDelta);

	return 0;
}

void Ground::LateUpdate_GameObject(const float& fTimeDelta)
{
	Object::LateUpdate_GameObject(fTimeDelta);
}

void Ground::Render_GameObject()
{
	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();
}

void Ground::On_CollisionEnter(const Collision& tCollision)
{
	Object::On_CollisionEnter(tCollision);
}

void Ground::On_CollisionStay(const Collision& tCollision)
{
	Object::On_CollisionStay(tCollision);

	m_pMesh->Set_Hilight(true);
}

void Ground::On_CollisionExit(const Collision& tCollision)
{
	Object::On_CollisionExit(tCollision);
	m_pMesh->Set_Hilight(false);
}

Ground* Ground::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	Ground* pGround = new Ground(pGraphicDevice);

	if (FAILED(pGround->Ready_GameObject()))
	{
		Safe_Delete(pGround);
	}

	return pGround;
}

void Ground::Release()
{
	m_pMesh->Release();

	Object::Release();
}
