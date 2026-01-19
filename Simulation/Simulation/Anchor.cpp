#include "pch.h"
#include "Anchor.h"

#include "Transform.h"
#include "Sphere.h"

Anchor::Anchor(LPDIRECT3DDEVICE9 pGraphicDevice)
	: Object(pGraphicDevice)
{
}

Anchor::~Anchor()
{
}

HRESULT Anchor::Ready_GameObject()
{
    if (FAILED(Object::Ready_GameObject()))
        return E_FAIL;

	m_pMesh = Sphere::Create(m_pGraphicDevice, this, D3DCOLOR_ARGB(255, 255, 255, 255), 0.5f, 10);
	m_pTransform = Transform::Create(m_pGraphicDevice, this);

    return S_OK;
}

void Anchor::Render_GameObject()
{
	Object::Render_GameObject();
	m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());
	m_pMesh->Render_Mesh();
}

Anchor* Anchor::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	Anchor* pAnchor = new Anchor(pGraphicDevice);

	if (FAILED(pAnchor->Ready_GameObject()))
	{
		Safe_Delete(pAnchor);
	}
	return pAnchor;
}

void Anchor::Release()
{
	m_pMesh->Release();

	Object::Release();
}
