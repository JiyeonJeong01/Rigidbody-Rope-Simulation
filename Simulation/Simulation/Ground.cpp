#include "pch.h"
#include "Ground.h"

#include "Plane.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "PlaneCollider.h"

Ground::Ground(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Object(pGraphicDevice)
	, m_pMesh(nullptr)
{
}

Ground::~Ground()
{
}

HRESULT Ground::Ready_GameObject(unsigned long dwColor, float fSizeX, float fSizeY)
{
    if (FAILED(Object::Ready_GameObject()))
        return E_FAIL;

	m_pMesh = Plane::Create(m_pGraphicDevice, this, dwColor, fSizeX, fSizeY);
	m_pMesh->Set_FillMode(D3DFILL_SOLID);

	m_pTransform = Transform::Create(m_pGraphicDevice, this);
	m_pTransform->Rotate(AXIS_X, 270.f);

	m_pCollider = PlaneCollider::Create(m_pGraphicDevice, this);
	m_pCollider->Set_Dimension({ fSizeX, fSizeY });
	m_pCollider->Set_IsInfinite(false);

	BODY body;
	body.eType = STATIC;
	m_pRigidbody = Rigidbody::Create(m_pGraphicDevice, this, body);

	__super::Resolve_Dependencies();

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

	Vec3 vGround = m_pTransform->Get_Position();
}

void Ground::Render_GameObject()
{
    Object::Render_GameObject();

	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();
}

void Ground::Active_Highlight(bool bColor)
{
    m_pMesh->Set_Highlight(bColor);
}

void Ground::On_CollisionEnter(const COLLISION& tCollision)
{
	Object::On_CollisionEnter(tCollision);
}

void Ground::On_CollisionStay(const COLLISION& tCollision)
{
	Object::On_CollisionStay(tCollision);
}

void Ground::On_CollisionExit(const COLLISION& tCollision)
{
	Object::On_CollisionExit(tCollision);
}

Ground* Ground::Create(LPDIRECT3DDEVICE9 pGraphicDevice, unsigned long dwColor, float fSizeX, float fSizeY)
{
	Ground* pGround = new Ground(pGraphicDevice);

	if (FAILED(pGround->Ready_GameObject(dwColor, fSizeX, fSizeY)))
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
