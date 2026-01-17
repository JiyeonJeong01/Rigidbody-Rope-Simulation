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

bool Raycast::Intersect_Ray(RAYCAST_HIT* tOut, Vec3 vOrigin)
{
	if (tOut == nullptr)
		return false;

	POINT pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	D3DVIEWPORT9 vp{};
	m_pGraphicDevice->GetViewport(&vp);

	D3DXMATRIX matView, matProj, matWorld;
	m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matWorld);

	RECT rc{};
	GetClientRect(g_hWnd, &rc);

	POINT ptBB = pt;
	if (rc.right > 0 && rc.bottom > 0 &&
		((DWORD)rc.right != vp.Width || (DWORD)rc.bottom != vp.Height))
	{
		ptBB.x = LONG(pt.x * (vp.Width / float(rc.right)));
		ptBB.y = LONG(pt.y * (vp.Height / float(rc.bottom)));
	}

	D3DXVECTOR3 vNear((float)ptBB.x, (float)ptBB.y, 0.0f);
	D3DXVECTOR3 vFar((float)ptBB.x, (float)ptBB.y, 1.0f);

	D3DXVec3Unproject(&vNear, &vNear, &vp, &matProj, &matView, &matWorld);
	D3DXVec3Unproject(&vFar, &vFar, &vp, &matProj, &matView, &matWorld);

	D3DXVECTOR3 pixelDir = vFar - vNear;
	D3DXVec3Normalize(&pixelDir, &pixelDir);

	D3DXVECTOR3 o(vOrigin.x, vOrigin.y, vOrigin.z);
	D3DXVECTOR3 dir = pixelDir;

	RAY ray{};
	ray.pHit = tOut;
	ray.vOrigin = Vec3(o.x, o.y, o.z);
	ray.vDiretion = Vec3(dir.x, dir.y, dir.z);
	ray.fMaxDist = 500.f;
	ray.dwDebugColor = D3DCOLOR_XRGB(255, 0, 0);

	Raycast::GetInstance()->Add_DebugRay(ray);

	return PhysicsWorld::GetInstance()->Detect_Ray(&ray);
}

void Raycast::Render_Ray()
{
	// µð¹ö±ë¿ë ·»´õ

	//Matrix matProj, matView, matWorld;
	//m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
	//m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	//D3DXMatrixIdentity(&matWorld);

	//Matrix matPV = matView * matProj;

	//D3DVIEWPORT9 vp{};
	//m_pGraphicDevice->GetViewport(&vp);

	//m_pLine->SetWidth(3.f);
	//m_pLine->Begin();

	//for (auto& line : m_debugRayList)
	//{
	//	Vec3 p[2]{};
	//	p[0] = line.vOrigin;
	//	p[1] = p[0] + line.vDiretion * line.fMaxDist;

	//	Vec3 vScreenPos0, vScreenPos1;

	//	D3DXVec3Project(&vScreenPos0, &p[0], &vp, &matProj, &matView, &matWorld);
	//	D3DXVec3Project(&vScreenPos1, &p[1], &vp, &matProj, &matView, &matWorld);

	//	m_pLine->DrawTransform(p, 2, &matPV, D3DCOLOR_XRGB(0, 255, 0));
	//}
	//m_pLine->End();

	//m_debugRayList.clear();
}

void Raycast::Add_DebugRay(const RAY& tRay)
{
	m_debugRayList.push_back(tRay);
}

void Raycast::Release()
{
	delete this;
}
