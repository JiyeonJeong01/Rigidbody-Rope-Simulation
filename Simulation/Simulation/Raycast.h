#pragma once
class Object;

typedef struct tagRaycastHit
{
	Object* pObject;
	Vec3 vPoint;			// The impact point
	float fDist;				// Ray'r origint to impactPoint
}RAYCAST_HIT;

typedef struct tagRay
{
	RAYCAST_HIT* pHit;

	Vec3 vOrigin;
	Vec3 vDiretion;
	float fMaxDist;
	unsigned long dwDebugColor;
	tagRay() {};
	tagRay(Vec3 vO, Vec3 vD, float fDist, unsigned long dwColor) : vOrigin(vO), vDiretion(vD), fMaxDist(100.f), dwDebugColor(0) {}
}RAY;

class Raycast
{
	DECLARE_SINGLETON(Raycast)
	Raycast() {};
	~Raycast() {};

public :
	HRESULT		Ready_Raycast(LPDIRECT3DDEVICE9 pGraphicDevice);
	static bool Intersect_Ray(RAYCAST_HIT* tOut, Vec3 vOrigin);

	void Render_Ray();

private:
	void				Add_DebugRay(const RAY& tRay);

private:
	static LPDIRECT3DDEVICE9		m_pGraphicDevice;

	LPD3DXLINE						m_pLine;
	list<RAY>						m_debugRayList;

public :
	void Release();
};

