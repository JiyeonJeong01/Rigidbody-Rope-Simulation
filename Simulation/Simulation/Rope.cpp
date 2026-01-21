#include "pch.h"
#include "Rope.h"

#include "Raycast.h"
#include "Transform.h"
#include "Anchor.h"
#include "Management.h"
#include "Player.h"

Rope::Rope(LPDIRECT3DDEVICE9 pGraphicDevice)
    : Object(pGraphicDevice), m_pLine(nullptr), m_pOwner(nullptr)
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
    m_fExtendVel = 200.f;

    m_pAnchor = Anchor::Create(m_pGraphicDevice);
    m_pAnchor->Set_Active(false);
    Management::GetInstance()->Add_Object(m_pAnchor);

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
    case ROPE_STATE::SUCC_EXTEND : case ROPE_STATE::FAIL_EXTEND :
        Extend_Rope(fTimeDelta);
        break;
    case ROPE_STATE::GRAPPLING:
        Do_Grappling();
        break;
    case ROPE_STATE::RETURN:
        Return_Rope(fTimeDelta);
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
    bool bValidAnchor = false;

    if (Raycast::Intersect_Ray(&hit, {int(WINCX * 0.5f), int(WINCY * 0.5f)}))
    {
        // 최종 성공 여부 확인
        // 조준점은 화면 중앙이지만, 로프는 플레이어의 몸에서 발사된다.
        // 화면 중앙(카메라)와 플레이어 사이의 충돌체가 아닌지 확인해야 한다.
        if (Check_ValidAnchor(hit.tTarget.vPoint))
        {
            m_vAnchor = hit.tTarget.vPoint;
            bValidAnchor = true;
        }
        else
        {
            float fMinDist = std::numeric_limits<float>::max();
            Vec3 vMinPoint = { 0, 0, 0 };
            for (auto h : hit.tTargetList)
            {
                if (!Check_ValidAnchor(h.vPoint))
                    continue;

                float fDist = VectorHelper::Get_Length({ m_pOwner->Get_Transform()->Get_Position() - h.vPoint });
                if (fDist < fMinDist)
                {
                    fMinDist = fDist;
                    vMinPoint = h.vPoint;
                    bValidAnchor = true;
                }
            }
            if (bValidAnchor)
                m_vAnchor = vMinPoint;
        }
    }

    if (bValidAnchor)
    {
        // 레이캐스트 성공
        m_eState = ROPE_STATE::SUCC_EXTEND;
    }
    else
    {
        // 레이캐스트 실패
        m_vAnchor = m_pOwner->Get_Transform()->Get_Position() + hit.vRayDir * 100.f;
        m_eState = ROPE_STATE::FAIL_EXTEND;
    }

    // 공통
    m_vTrialDir = VectorHelper::Get_Normalized(hit.vRayDir);
    m_vCurDynamicPos = m_pOwner->Get_Transform()->Get_Position(); // 로프 확장 시작 
}

void Rope::Finish_Grappling()
{
    m_eState = ROPE_STATE::RETURN;
}

void Rope::Extend_Rope(const float& fTimeDelta)
{
    Vec3 vTip = m_pOwner->Get_Transform()->Get_Position();          // 로프 시작 위치

    // 흔들리는 로프 모양 계산하기 
    Calc_RopeShape(vTip, fTimeDelta);

    const float fDist = VectorHelper::Get_Length(m_vCurDynamicPos - m_vAnchor);

    Vec3 vCurDir = m_vAnchor - m_vCurDynamicPos;
    float fDot = VectorHelper::DotProduct(vCurDir, m_vTrialDir);

    if (fDist < 1.f || fDot < 0.f)
    {
        if (m_eState == ROPE_STATE::FAIL_EXTEND)
        {
            m_eState = ROPE_STATE::RETURN;
        }
        else
        {
            static_cast<Player*>(m_pOwner)->Start_Swing(m_vAnchor);
            m_eState = ROPE_STATE::GRAPPLING;
            // m_pAnchor->Set_Active(true);
            // m_pAnchor->Get_Transform()->Set_Position(m_vAnchor);
        }

        m_vTrialDir *= -1.f;
    }
}

void Rope::Do_Grappling()
{
    m_RopePoints.clear();
    m_RopePoints.push_back(m_pOwner->Get_Transform()->Get_Position());
    m_RopePoints.push_back(m_vAnchor);
}

void Rope::Return_Rope(const float& fTimeDelta)
{
    Vec3 vTip = m_pOwner->Get_Transform()->Get_Position();          // 로프 시작 위치 

    // 흔들리는 로프 모양 계산하기 
    Calc_RopeShape(vTip, fTimeDelta);

    if (Calc_RopeComplete(vTip))
    {
        if (m_eState == ROPE_STATE::SUCC_EXTEND)
            m_pAnchor->Set_Active(false);

        m_eState = ROPE_STATE::NONE;
        m_RopePoints.clear();
    }
}

void Rope::Calc_RopeShape(const Vec3& vCurTip, const float& fTimeDelta)
{
    m_vCurDynamicPos += m_vTrialDir * m_fExtendVel * fTimeDelta;

    float fAmplitude = m_tDynamicValue.Get_Value(fTimeDelta);
    Vec3 vDir = VectorHelper::Get_Normalized(m_vAnchor - vCurTip);     // 팁 -> 앵커

    DebugHelper::Print_Float(L"Amplitude", fAmplitude);

    Vec3 vWorldUp = VectorHelper::Up();
    Vec3 vRight = VectorHelper::CrossProduct(vDir, vWorldUp);

    if (VectorHelper::Get_LengthSq(vRight) <= 1e-6f)
    {
        vWorldUp = VectorHelper::Look();
        vRight = VectorHelper::CrossProduct(vDir, vWorldUp);
    }

    vRight = VectorHelper::Get_Normalized(vRight);

    m_RopePoints.clear();

    for (int i = 0; i < m_iPointCnt; ++i)
    {
        float fRatio = i / (float)(m_iPointCnt - 1);
        Vec3 vBase = MathHelper::LerpVec3(vCurTip, m_vCurDynamicPos, fRatio);

        /* 사인 웨이브 */
        /* fRatio : 현재 점이 전체 파형 중 몇 %인지 */
        /* m_iWaveCnt : 파형(반구)의 개수 */
        /* PI : 파형(반구) */
        float fShape = sinf(fRatio * m_iWaveCnt * D3DX_PI);
        float fStrength = fRatio * (1 - fRatio); /* 가운데에 흔들림이 크게 적용하도록 */
        float fWave = fShape + fStrength;

        Vec3 vOffset = vRight * (m_fWaveHeight * fWave * fAmplitude);

        m_RopePoints.emplace_back(vBase + vOffset);
    }
}

bool Rope::Calc_RopeComplete(const Vec3& vCurTip)
{
    const float fDist = VectorHelper::Get_Length(m_vCurDynamicPos - m_vAnchor);

    Vec3 vCurDir = vCurTip - m_vCurDynamicPos;
    float fDot = VectorHelper::DotProduct(vCurDir, m_vTrialDir);

    if (fDist < 0.1f || fDot < 0.f)
        return true;
    return false;
}

bool Rope::Check_ValidAnchor(const Vec3& vPoint)
{
    Vec3 vOwnerPos = m_pOwner->Get_Transform()->Get_Position();

    Matrix matView, matInvView;
    m_pGraphicDevice->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matInvView, 0, &matView);

    Vec3 vCamPos = { matInvView._41, matInvView._42, matInvView._43 };

    float fDistToPoint = VectorHelper::Get_Length({ vCamPos - vPoint });
    float fDistToPlayer = VectorHelper::Get_Length({ vCamPos - vOwnerPos });

    if (fDistToPoint > fDistToPlayer)
        return true;
    else
        return false;
}


void Rope::Set_RopeAmplitueInfo(const AMPLITUDE_VALUE& tInfo)
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
