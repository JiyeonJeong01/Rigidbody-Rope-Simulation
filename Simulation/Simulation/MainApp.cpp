#include "pch.h"
#include "MainApp.h"
#include "GraphicDevice.h"
#include "CollisionSystem.h"
#include "InputSystem.h"

#include "Player.h"
#include "Enemy.h"
#include "Ground.h"

MainApp::MainApp()
    : m_pGraphicDevice(nullptr), m_pGraphicDev(nullptr)
{
}

MainApp::~MainApp()
{
}

HRESULT MainApp::Ready_MainApp()
{
    if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
        return E_FAIL;

    if (FAILED(Ready_Scene(m_pGraphicDev)))
        return E_FAIL;

    InputSystem::GetInstance()->Ready_System();

    m_pPlayer = Player::Create(m_pGraphicDev);
    //m_pEnemy = Enemy::Create(m_pGraphicDev);
    m_pGround = Ground::Create(m_pGraphicDev);

    return S_OK;
}

int MainApp::Update_MainApp(const float& fTimeDelta)
{
    CollisionSystem::GetInstance()->Update_System();
    InputSystem::GetInstance()->Update_System();

    m_pPlayer->Update_GameObject(fTimeDelta);
   // m_pEnemy->Update_GameObject(fTimeDelta);
    m_pGround->Update_GameObject(fTimeDelta);

    return 0;
}

void MainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
    m_pPlayer->LateUpdate_GameObject(fTimeDelta);
    //m_pEnemy->LateUpdate_GameObject(fTimeDelta);
    m_pGround->LateUpdate_GameObject(fTimeDelta);
}

void MainApp::Render_MainApp()
{
    m_pGraphicDevice->Render_Begin(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));

    m_pPlayer->Render_GameObject();
   //m_pEnemy->Render_GameObject();
    m_pGround->Render_GameObject();

    m_pGraphicDevice->Render_End();
}

HRESULT MainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
    if (FAILED(GraphicDevice::GetInstance()->Ready_GraphicDev(g_hWnd, &m_pGraphicDevice)))
        return E_FAIL;

    (*ppGraphicDev) = m_pGraphicDevice->Get_GraphicDev();
    (*ppGraphicDev)->AddRef();
    (*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

    Matrix     matView, matProj;

    Vec3   vEye = { 0.f, 5.f, -10.f };
    Vec3   vAt = { 0.f, 0.f, 0.f };
    Vec3   vUp = { 0.f, 1.f, 0.f };

    D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
    D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.f), (float)WINCX / WINCY, 0.1f, 1000.f);

    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);
    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

    return S_OK;
}

HRESULT MainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{

    return S_OK;
}

MainApp* MainApp::Create()
{
    MainApp* pMainApp = new MainApp;
    if (FAILED( pMainApp->Ready_MainApp()))
    {
        delete pMainApp;
        pMainApp = nullptr;
    }

    return pMainApp;
}

void MainApp::Release()
{
    // Objects
    Safe_Release(m_pPlayer);
    //Safe_Release(m_pEnemy);
    Safe_Release(m_pGround);

    // Singleton
    CollisionSystem::DestroyInstance();
    InputSystem::DestroyInstance();
    GraphicDevice::DestroyInstance();

    delete this;
}