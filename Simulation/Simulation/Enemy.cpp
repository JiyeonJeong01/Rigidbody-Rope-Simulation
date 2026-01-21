#include "pch.h"
#include "Enemy.h"

#include "Sphere.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "SphereCollider.h"

Enemy::Enemy(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Object(pGraphicDevice)
	, m_pRigidbody(nullptr), m_pMesh(nullptr)
{
}

Enemy::~Enemy()
{
}

HRESULT Enemy::Ready_GameObject()
{
    if (FAILED(Object::Ready_GameObject()))
        return E_FAIL;

	m_pMesh = Sphere::Create(m_pGraphicDevice, this, D3DCOLOR_ARGB(255, 0, 255, 255), 1.f, 10);

	m_pTransform = Transform::Create(m_pGraphicDevice, this);
	m_pTransform->Set_Position(3.f, 0.f, 0.f);

	m_pCollider = SphereCollider::Create(m_pGraphicDevice, this);

	BODY body;
	body.eBodyType = DYNAMIC;
	body.fAngularDrag = 0.f;
	body.fDrag = 0.5f;
    body.fRestitution = 1.f;
    body.fMass = 0.1f;
    body.eGeoType = SPHERE;
	m_pRigidbody = Rigidbody::Create(m_pGraphicDevice, this, body);

	__super::Resolve_Dependencies();

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
    Object::Render_GameObject();

	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();
}

void Enemy::On_CollisionEnter(const COLLISION& tCollision)
{
	Object::On_CollisionEnter(tCollision);
}

void Enemy::On_CollisionStay(const COLLISION& tCollision)
{
	Object::On_CollisionStay(tCollision);
}

void Enemy::On_CollisionExit(const COLLISION& tCollision)
{
	Object::On_CollisionExit(tCollision);
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
