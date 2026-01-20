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
	, m_pMesh(nullptr)
{
}

Wall::~Wall()
{
}

HRESULT Wall::Ready_GameObject(unsigned long dwColor, float fSizeX, float fSizeY)
{
    if (FAILED(Object::Ready_GameObject()))
        return E_FAIL;

	m_pMesh = Plane::Create(m_pGraphicDevice, this, dwColor, fSizeX, fSizeY);
	m_pMesh->Set_FillMode(D3DFILL_WIREFRAME);

	m_pTransform = Transform::Create(m_pGraphicDevice, this);
	m_pTransform->Rotate(AXIS_Y, 90.f);

	m_pCollider = PlaneCollider::Create(m_pGraphicDevice, this);
	m_pCollider->Set_Dimension({ fSizeX, fSizeY });
	m_pCollider->Set_IsInfinite(false);

	BODY body;
	body.eBodyType = STATIC;
	m_pRigidbody = Rigidbody::Create(m_pGraphicDevice, this, body);

	__super::Resolve_Dependencies();

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
    Object::Render_GameObject();

	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();
}

void Wall::Active_Highlight(bool bColor)
{
    m_pMesh->Set_Highlight(bColor);
}

void Wall::On_CollisionEnter(const COLLISION& tCollision)
{
	Object::On_CollisionEnter(tCollision);
	//m_pMesh->Set_Highlight(true);
}

void Wall::On_CollisionStay(const COLLISION& tCollision)
{
	Object::On_CollisionStay(tCollision);
}

void Wall::On_CollisionExit(const COLLISION& tCollision)
{
	Object::On_CollisionExit(tCollision);
	//m_pMesh->Set_Highlight(false);
}

void Wall::Set_Position(const Vec3& vPosition)
{
	Get_Transform()->Set_Position(vPosition);
}

Wall* Wall::Create(LPDIRECT3DDEVICE9 pGraphicDevice, unsigned long dwColor, float fSizeX, float fSizeY)
{
	Wall* pWall = new Wall(pGraphicDevice);

	if (FAILED(pWall->Ready_GameObject(dwColor, fSizeX, fSizeY)))
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
