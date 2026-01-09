#include "pch.h"
#include "Enemy.h"

#include "Sphere.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "SphereCollider.h"

Enemy::Enemy(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Object(pGraphicDevice)
	, m_pTransform(nullptr), m_pRigidbody(nullptr), m_pMesh(nullptr)
{
}

Enemy::~Enemy()
{
}

HRESULT Enemy::Ready_GameObject()
{
	m_pMesh = Sphere::Create(m_pGraphicDevice, this, D3DCOLOR_ARGB(255, 0, 0, 255), 1.f, 10);

	m_pTransform = Transform::Create(m_pGraphicDevice, this);
	m_pTransform->Set_Position(2.f, 0.f, 1.f);

	// m_pRigidbody = Rigidbody::Create(m_pGraphicDevice, this);

	m_pCollider = SphereCollider::Create(m_pGraphicDevice, this);


	//m_pRigidbody->Set_Mass(10.f);
	//m_pRigidbody->Set_Drag(0.5f);
	//m_pRigidbody->Set_AngularDrag(5.f);
	//m_pRigidbody->Set_GeometryType(SPHERE);

	return S_OK;
}

int Enemy::Update_GameObject(const float& fTimeDelta)
{
	Object::Update_GameObject(fTimeDelta);

	return 0;
}

void Enemy::LateUpdate_GameObject(const float& fTimeDelta)
{
	Object::LateUpdate_GameObject(fTimeDelta);

}

void Enemy::Render_GameObject()
{
	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();
}

Enemy* Enemy::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	Enemy* pEnemy = new Enemy(pGraphicDevice);

	if (FAILED(pEnemy->Ready_GameObject()))
	{
		Safe_Delete(pEnemy);
	}

	return pEnemy;
}

void Enemy::Release()
{
	m_pMesh->Release();

	Object::Release();
}
