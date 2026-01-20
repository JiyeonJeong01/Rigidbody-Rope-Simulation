#include "pch.h"
#include "Rope.h"

#include "Raycast.h"
#include "Transform.h"
#include "SpringJoint.h"
#include "Anchor.h"
#include "Ground.h"
#include "Player.h"
#include "Wall.h"

Rope::Rope(LPDIRECT3DDEVICE9 pGraphicDevice)
    : Object(pGraphicDevice), m_pLine(nullptr), m_pOwner(nullptr), m_pSJ(nullptr), m_bRender(false)
    , m_iPointCnt(0), m_iWaveCnt(0), m_fWaveHeight(0), m_fFollowSpeed(0), m_eState(ROPE_STATE::NONE)
{
}

Rope::~Rope()
{
}

HRESULT Rope::Ready_GameObject()
{
    if (FAILED(Object::Ready_GameObject()))
        return E_FAIL;

    D3DXCreateLine(m_pGraphicDevice, &m_pLine);
    
    m_pTransform = Transform::Create(m_pGraphicDevice, this);

    m_fWaveHeight = 200.f;
    m_iWaveCnt = 2;
    m_iPointCnt = 20;
    m_fExtendVel = 100.f;

    return __super::Resolve_Dependencies();
}

int Rope::Update_GameObject(const float& fTimeDelta)
{
    return Object::Update_GameObject(fTimeDelta);
}

void Rope::LateUpdate_GameObject(const float& fTimeDelta)
{
    Object::LateUpdate_GameObject(fTimeDelta);

    switch (m_eState)
    {
    case ROPE_STATE::EXTEND:
        Extend_Rope(fTimeDelta);
        break;
    case ROPE_STATE::GRAPPLING:
        Do_Grappling();
        break;
    case ROPE_STATE::RETURN:
        break;
    }
}

void Rope::Render_GameObject()
{
    if (m_RopePoints.size() < 2) return;

    vector<VTXCOL> vtx;
    vtx.reserve(m_RopePoints.size());
    for (auto& p : m_RopePoints)
    {
        vtx.push_back({p, D3DCOLOR_ARGB(255, 0, 0, 0)});
    }
    m_pGraphicDevice->SetFVF(FVF_COL);
    m_pGraphicDevice->SetTexture(0, nullptr);
    m_pGraphicDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    Matrix matIdentity;
    D3DXMatrixIdentity(&matIdentity);
    m_pGraphicDevice->SetTransform(D3DTS_WORLD, &matIdentity);
    m_pGraphicDevice->DrawPrimitiveUP(D3DPT_LINESTRIP,
        (UINT)m_RopePoints.size() - 1,
        vtx.data(),
        sizeof(VTXCOL));
    Object::Render_GameObject();
}

void Rope::Try_Grappling()
{
    RAYCAST_HIT hit;
    list<Vec3> m_pCandidates;
    if (Raycast::Intersect_Ray(&hit, {int(WINCX * 0.5f), int(WINCY * 0.5f)}))
    {
        m_vAnchor = hit.tTarget.vPoint;
        m_vCurExtendPos = m_pOwner->Get_Transform()->Get_Position();
        m_pCandidates.push_back(hit.tTarget.vPoint);
    }

    // 공통
    m_vTrialDir = VectorHelper::Get_Normalized(hit.vRayDir);
    
    m_eState = ROPE_STATE::EXTEND;
}

void Rope::Extend_Rope(const float& fTimeDelta)
{
    m_vCurExtendPos += m_vTrialDir * m_fExtendVel * fTimeDelta;

    DebugHelper::Print_Vec3(L"Extended", m_vCurExtendPos);

    float fVisual = m_tDynamicValue.Get_Value(fTimeDelta);
    Vec3 vTip = m_pOwner->Get_Transform()->Get_Position();          // 로프 시작 위치 
    Vec3 vDir = VectorHelper::Get_Normalized(m_vAnchor - vTip);     // 팁 -> 앵커

    m_vCurGrapplePos = MathHelper::LerpVec3(m_vCurGrapplePos, m_vCurExtendPos, fTimeDelta * 10);

    Vec3 vWorldLook = VectorHelper::Look();
    Vec3 vRight = VectorHelper::CrossProduct(vDir, vWorldLook);

    if (VectorHelper::Get_LengthSq(vRight) <= 1e-6f)
    {
        vWorldLook = VectorHelper::Up();
        vRight = VectorHelper::CrossProduct(vDir, vWorldLook);
    }

    vRight = VectorHelper::Get_Normalized(vRight);
    Vec3 vUp = VectorHelper::Get_Normalized(VectorHelper::CrossProduct(vRight, vDir));

    m_RopePoints.clear();

    if (m_iPointCnt < 2)
    {
        m_RopePoints.emplace_back(vTip);
        return;
    }

    for (int i = 0; i < m_iPointCnt; ++i)
    {
        float fDelta = i / (float)(m_iPointCnt - 1);
        Vec3 vBase = MathHelper::LerpVec3(vTip, m_vCurGrapplePos, fDelta);

        float fWave = sinf(fDelta * m_iWaveCnt * D3DX_PI) * (fDelta * (1.f - fDelta));

        Vec3 vOffset = vUp * (m_fWaveHeight * fWave * fVisual);

        m_RopePoints.emplace_back(vBase + vOffset);
    }

    const float fDist = VectorHelper::Get_Length(m_vCurExtendPos - m_vAnchor);

    Vec3 vCurDir = m_vAnchor - m_vCurExtendPos;
    float fDot = VectorHelper::DotProduct(vCurDir, m_vTrialDir);

    if (fDist < 1.f || fDot < 0.f)
    {
        static_cast<Player*>(m_pOwner)->Start_Swing(m_vAnchor);
        m_eState = ROPE_STATE::GRAPPLING;
    }

}

void Rope::Do_Grappling()
{
    m_RopePoints.clear();
    m_RopePoints.push_back(m_pOwner->Get_Transform()->Get_Position());
    m_RopePoints.push_back(m_vAnchor);
}

void Rope::Set_VisualSpringValue(const DYNAMIC_VALUE& tInfo)
{
    m_tDynamicValue = tInfo;
}

Rope* Rope::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    Rope* pRope = new Rope(pGraphicDevice);

    if (FAILED(pRope->Ready_GameObject()))
    {
        Safe_Delete(pRope);
    }

    return pRope;
}

void Rope::Release()
{
    Object::Release();
}
