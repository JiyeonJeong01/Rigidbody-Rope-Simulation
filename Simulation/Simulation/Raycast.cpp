#include "pch.h"
#include "Raycast.h"
#include "VectorHelper.h"
#include "CollisionDetector.h"

IMPLEMENT_SINGLETON(Raycast)

LPDIRECT3DDEVICE9 Raycast::m_pGraphicDevice = NULL;

HRESULT Raycast::Ready_Raycast(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	m_pGraphicDevice = pGraphicDevice;

	D3DXCreateLine(m_pGraphicDevice, &m_pLine);

	return S_OK;
}

bool Raycast::Intersect_Ray(RAYCAST_HIT* tOut, const RAY& tRay)
{
	return false;
}

bool Raycast::Intersect_Ray(Vec3 vOrigin, Vec3 vDirection)
{
	return false;
}

bool Raycast::Intersect_Ray(RAYCAST_HIT* tOut, Vec3 vOrigin, Vec3 vDirection)
{
	return false;
}

bool Raycast::Intersect_Ray(RAYCAST_HIT* tOut, const POINT& tScreen)
{
	D3DVIEWPORT9 vp;
	m_pGraphicDevice->GetViewport(&vp);

	D3DXMATRIX proj, view, invView, world;
	m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &proj);
	m_pGraphicDevice->GetTransform(D3DTS_VIEW, &view);
	D3DXMatrixIdentity(&world);

	// near/far in world from mouse
	D3DXVECTOR3 farPt((float)tScreen.x, (float)tScreen.y, 1.0f);

	D3DXVECTOR3  farWorld;
	D3DXVec3Unproject(&farWorld, &farPt, &vp, &proj, &view, &world);

	// camera world position
	D3DXMatrixInverse(&invView, nullptr, &view);
	D3DXVECTOR3 camPos(invView._41, invView._42, invView._43);

	D3DXVECTOR3 dir = farWorld - camPos;
	D3DXVec3Normalize(&dir, &dir);

	RAY ray;
	ray.vOrigin = camPos;        
	ray.vDiretion = dir;
	ray.fMaxDist = 1000.f;
	ray.dwDebugColor = D3DCOLOR_XRGB(255, 0, 0);

	Raycast::GetInstance()->Add_DebugRay(ray);

	return false;
	//return CollisionDetector::GetInstance()->Detect_Ray(&ray);
}

void Raycast::Render_Ray()
{
	Matrix matProj, matView;
	m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	Matrix matViewProj = matView * matProj;

	m_pLine->SetWidth(2.f);
	for (auto& line : m_debugRayList)
	{
		Vec3 p[2];
		p[0] = line.vOrigin;
		p[1] = p[0] + line.vDiretion * line.fMaxDist;

		m_pLine->Begin();
		m_pLine->DrawTransform(p, 2, &matViewProj, D3DCOLOR_XRGB(0, 255, 0));
		m_pLine->End();
	}

	m_debugRayList.clear();
}

void Raycast::Add_DebugRay(const RAY& tRay)
{
	m_debugRayList.push_back(tRay);
}

void Raycast::Release()
{
	delete this;
}
