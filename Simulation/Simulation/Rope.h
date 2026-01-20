#pragma once
#include "Object.h"

class Anchor;

class Rope : public Object
{
public :
    enum class ROPE_STATE { NONE, SUCC_EXTEND, FAIL_EXTEND, GRAPPLING, RETURN };

private:
    Rope(LPDIRECT3DDEVICE9 pGraphicDevice);
    ~Rope() override;

public:
    HRESULT	Ready_GameObject();
    int		Update_GameObject(const float& fTimeDelta);
    void	LateUpdate_GameObject(const float& fTimeDelta);
    void	Render_GameObject();

    void    Try_Grappling();
    void    Finish_Grappling();

    void    Set_Owner(Object* pObj) { m_pOwner = pObj; }
    void    Set_RopeAmplitueInfo(const AMPLITUDE_VALUE& tInfo);
    void    Set_Anchor(const Vec3& vAnchor) { m_vAnchor = vAnchor; }

private :
    void    Extend_Rope(const float& fTimeDelta);
    void    Do_Grappling(); // 성공 시, 업데이트
    void    Return_Rope(const float& fTimeDelta);

    void    Calc_RopeShape(const Vec3& vCurTip, const float& fTimeDelta);
    bool    Calc_RopeComplete(const Vec3& vCurTip);

    bool    Check_ValidAnchor(const Vec3& vPoint);

private:
    LPD3DXLINE      m_pLine;
    Object*         m_pOwner;
    AMPLITUDE_VALUE   m_tDynamicValue;

    Vec3        m_vAnchor;
    int         m_iPointCnt, m_iWaveCnt;
    float       m_fWaveHeight, m_fFollowSpeed;

    vector<Vec3>  m_RopePoints;

    Vec3                m_vTrialDir;
    ROPE_STATE          m_eState;
    float               m_fExtendVel;
    Vec3                m_vCurDynamicPos;
    bool                m_bAnchorFailed{};
    Anchor*             m_pAnchor{};
public:
    static Rope* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
    void Release() override;
};
