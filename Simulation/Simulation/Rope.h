#pragma once
#include "Object.h"

class SpringJoint;

using Callback = function<void()>;

class Rope : public Object
{
    enum class ROPE_STATE { NONE, EXTEND, GRAPPLING, RETURN };

private:
    Rope(LPDIRECT3DDEVICE9 pGraphicDevice);
    ~Rope() override;

public:
    HRESULT	Ready_GameObject();
    int		Update_GameObject(const float& fTimeDelta);
    void	LateUpdate_GameObject(const float& fTimeDelta);
    void	Render_GameObject();

    void    Try_Grappling();
    void    Extend_Rope(const float& fTimeDelta);
    void    Do_Grappling(); // 성공 시, 업데이트

    void    Add_Listener(Callback onSuccess) { m_OnSuccess.push_back(onSuccess); }

    void    Set_Owner(Object* pObj) { m_pOwner = pObj; }
    void    Set_Render(bool bRender) { m_bRender = bRender; }
    void    Set_VisualSpringValue(const DYNAMIC_VALUE& tInfo);
    void    Set_Anchor(const Vec3& vAnchor) { m_vAnchor = vAnchor; }

private:
    LPD3DXLINE      m_pLine;
    Object*         m_pOwner;
    SpringJoint*    m_pSJ;
    DYNAMIC_VALUE   m_tDynamicValue;

    bool        m_bRender;

    Vec3        m_vAnchor, m_vTip, m_vCurGrapplePos;
    int         m_iPointCnt, m_iWaveCnt;
    float       m_fWaveHeight, m_fFollowSpeed;

    vector<Vec3>  m_RopePoints;

    Vec3                m_vTrialDir;
    list<Callback>      m_OnSuccess;
    ROPE_STATE          m_eState;
    float               m_fExtendVel;
    Vec3                m_vCurExtendPos;

public:
    static Rope* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
    void Release() override;
};


