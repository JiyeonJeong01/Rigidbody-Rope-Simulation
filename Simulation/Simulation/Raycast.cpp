#include "pch.h"
#include "Raycast.h"
#include "VectorHelper.h"
#include "CollisionSystem.h"

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

	// 뷰포트 공간 -> 투영 공간
	D3DVIEWPORT9	ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDevice->GetViewport(&ViewPort);

	//Vec3 vOriginPos{ 0.f, 0.f, 0.f };

	//Vec3 vScreenPos
	//{
	//	(float)tScreen.x / (ViewPort.Width * 0.5f) - 1.f,
	//	(float)tScreen.y / -(ViewPort.Height * 0.5f) + 1.f,
	//	1.f
	//};

	//// 투영 공간 -> 뷰 공간
	//Vec3 vViewPos;
	//Matrix matProj, matProjInv;
	//m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	//D3DXMatrixInverse(&matProjInv, 0, &matProj);
	//D3DXVec3TransformCoord(&vViewPos, &vScreenPos, &matProjInv);
	//D3DXVec3TransformCoord(&vOriginPos, &vOriginPos, &matProjInv);

	//// 뷰 공간 -> 월드 공간
	//Vec3 vWorldPos;
	//Matrix matView, matViewInv;
	//m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
	//D3DXMatrixInverse(&matViewInv, 0, &matView);
	//D3DXVec3TransformCoord(&vWorldPos, &vViewPos, &matViewInv);
	//D3DXVec3TransformCoord(&vOriginPos, &vOriginPos, &matViewInv);

	// ====================================

	// 뷰 포트 -> 투영
	Vec3	vMousePos;

	vMousePos.x = tScreen.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = tScreen.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.1f;

	// 투영 -> 뷰 스페이스
	D3DXMATRIX		matProj;
	m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);


	// 뷰 스페이스 -> 월드
	D3DXMATRIX		matView;
	m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	Vec3	vOriginPos{ 0.f, 5.f, -10.f };			// 뷰 스페이스
	Vec3	vRayDir = vMousePos - vOriginPos;		// 뷰 스페이스

	D3DXVec3TransformCoord(&vOriginPos, &vOriginPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	Vec3 vDir = VectorHelper::Get_Normalized(vMousePos - vOriginPos);
	RAY ray{ vOriginPos, vDir};
	ray.dwDebugColor = D3DCOLOR_XRGB(255, 0, 0);

	Raycast::GetInstance()->Add_DebugRay(ray);
	
	return CollisionSystem::GetInstance()->Detect_Ray(&ray);
}

void Raycast::Render_Ray()
{
	Matrix matProj, matView;
	m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	Matrix matViewProj = matView * matProj;

	Vec3 test[2];
	test[0] = { -100.f, 10.f, 0.f };
	test[1] = { 100.f, 0.f, 0.f };
	m_pLine->Begin();
	m_pLine->DrawTransform(test, 2, &matViewProj, D3DCOLOR_XRGB(0, 255, 0));
	m_pLine->End();

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
