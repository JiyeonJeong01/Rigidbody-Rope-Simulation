#pragma once
class Object;

typedef struct tagHitTarget
{
    Object* pObject;
    Vec3    vPoint;
}HIT_TARGET;

typedef struct tagRaycastHit
{
    HIT_TARGET  tTarget;
    list<HIT_TARGET> tTargetList;
	float fDist;		// 거리
    Vec3 vRayDir;       // Ray의 방향
    Vec3 vRayOrigin;    // Ray의 원점
}RAYCAST_HIT;

typedef struct tagRay
{
	Vec3 vOrigin{};
	Vec3 vDiretion{};
	float fMaxDist{};
	unsigned long dwDebugColor{};
	tagRay() : vOrigin(0, 0, 0), vDiretion(0, 0, 0), fMaxDist(0), dwDebugColor(0) {}
	tagRay(Vec3 vO, Vec3 vD, float fDist, unsigned long dwColor) : vOrigin(vO), vDiretion(vD), fMaxDist(100.f), dwDebugColor(0) {}
}RAY;

class Raycast
{
	DECLARE_SINGLETON(Raycast)
	Raycast() {};
	~Raycast() {};

public :
	HRESULT		Ready_Raycast(LPDIRECT3DDEVICE9 pGraphicDevice);
	static bool Intersect_Ray(RAYCAST_HIT* tOut, Vec3 vOrigin);     // vOrigin -> Mouse 방향으로 Ray 생성
	static bool Intersect_Ray(RAYCAST_HIT* tOut, POINT pt);         // 화면의 위치

private:
	static LPDIRECT3DDEVICE9		m_pGraphicDevice;
    static D3DVIEWPORT9             m_Viewport;

	LPD3DXLINE						m_pLine;

public :
	void Release();
};

