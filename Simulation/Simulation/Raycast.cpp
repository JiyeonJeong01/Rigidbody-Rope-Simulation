#include "pch.h"
#include "Raycast.h"
#include "VectorHelper.h"
#include "CollisionDetector.h"

IMPLEMENT_SINGLETON(Raycast)

LPDIRECT3DDEVICE9 Raycast::m_pGraphicDevice = NULL;
D3DVIEWPORT9 Raycast::m_Viewport;

HRESULT Raycast::Ready_Raycast(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	m_pGraphicDevice = pGraphicDevice;

	D3DXCreateLine(m_pGraphicDevice, &m_pLine);
    m_pGraphicDevice->GetViewport(&m_Viewport);

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
	ray.vOrigin = Vec3(o.x, o.y, o.z);
	ray.vDiretion = Vec3(dir.x, dir.y, dir.z);
	ray.fMaxDist = 500.f;
	ray.dwDebugColor = D3DCOLOR_XRGB(255, 0, 0);

	return PhysicsWorld::GetInstance()->Detect_Ray(tOut, &ray);
}

bool Raycast::Intersect_Ray(RAYCAST_HIT* tOut, POINT pt)
{
    if (!tOut)
        return false;

    D3DXMATRIX matView{}, matProj{}, matWorld{};
    m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
    m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &matProj);
    D3DXMatrixIdentity(&matWorld);

    D3DVIEWPORT9 vp{};
    m_pGraphicDevice->GetViewport(&vp);

    const LONG minX = (LONG)vp.X;
    const LONG minY = (LONG)vp.Y;
    const LONG maxX = (LONG)(vp.X + vp.Width - 1);
    const LONG maxY = (LONG)(vp.Y + vp.Height - 1);

    if (pt.x < minX) pt.x = minX;
    if (pt.y < minY) pt.y = minY;
    if (pt.x > maxX) pt.x = maxX;
    if (pt.y > maxY) pt.y = maxY;

    Vec3 vNearS((float)pt.x, (float)pt.y, 0.0f);
    Vec3 vFarS((float)pt.x, (float)pt.y, 1.0f);

    D3DXVec3Unproject(&vNearS, &vNearS, &vp, &matProj, &matView, &matWorld);
    D3DXVec3Unproject(&vFarS, &vFarS, &vp, &matProj, &matView, &matWorld);

    Vec3 vNear(vNearS.x, vNearS.y, vNearS.z);
    Vec3 vFar(vFarS.x, vFarS.y, vFarS.z);

    Matrix matInvView{};
    D3DXMatrixInverse(&matInvView, nullptr, &matView);

    Vec3 vCamPos(matInvView._41, matInvView._42, matInvView._43);
    Vec3 vCamLook(matInvView._31, matInvView._32, matInvView._33);
    vCamLook = VectorHelper::Get_Normalized(vCamLook);

    Vec3 vRayDir = VectorHelper::Get_Normalized(vFar - vNear);

    RAY ray{};
    ray.vOrigin = vNear;
    ray.vDiretion = vRayDir;
    ray.fMaxDist = 1000.f;
    ray.dwDebugColor = D3DCOLOR_XRGB(255, 0, 0);

    tOut->vRayDir = ray.vDiretion;

    return PhysicsWorld::GetInstance()->Detect_Ray(tOut, &ray);
}

void Raycast::Release()
{
	delete this;
}
