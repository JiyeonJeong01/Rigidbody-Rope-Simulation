#include "pch.h"
#include "MainApp.h"
#include "GraphicDevice.h"
#include "Management.h"
#include "PhysicsWorld.h"
#include "InputSystem.h"

#include "Player.h"
#include "Enemy.h"
#include "Ground.h"
#include "Raycast.h"
#include "Wall.h"

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

    Ready_GroundAndCeiling();
    Ready_Wall();

    PhysicsWorld::GetInstance()->Ready_System(m_pGraphicDev);
    InputSystem::GetInstance()->Ready_System();
    Raycast::GetInstance()->Ready_Raycast(m_pGraphicDev);

    m_pPlayer = Player::Create(m_pGraphicDev);
    m_pPlayer->Get_Transform()->Set_Position(-40.f, 10.f, 0.f);
    Management::GetInstance()->Add_Object(m_pPlayer);

    const Vec3 basePos = { -40.f, 10.f, 60.f };

    for (int i = 0; i < 10; ++i)
    {
        Object* pEnemy = Enemy::Create(m_pGraphicDev);

        float offsetX = (i % 5) * 5.f;      
        float offsetY = (i / 5) * 10.f;     
        float offsetZ = (i % 3) * 7.f;      

        pEnemy->Get_Transform()->Set_Position(
            basePos.x + offsetX,
            basePos.y + offsetY,
            basePos.z + offsetZ
        );

        Management::GetInstance()->Add_Object(pEnemy);
    }

    PhysicsWorld::GetInstance()->Find_StaticCollider();

    return S_OK;
}

int MainApp::Update_MainApp(const float& fTimeDelta)
{
    Management::GetInstance()->Update_Management(fTimeDelta);

    return 0;
}

void MainApp::Fixed_Update(const float& fTimeDelta)
{
    Management::GetInstance()->FixedUpdate_Management(fTimeDelta);

    PhysicsWorld::GetInstance()->Update_System(fTimeDelta);
}

void MainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
    InputSystem::GetInstance()->Update_System();
    Management::GetInstance()->LateUpdate_Management(fTimeDelta);
}

void MainApp::Render_MainApp()
{
    m_pGraphicDevice->Render_Begin(D3DXCOLOR(0.8f, 0.8f, 0.5f, 1.f));

    Management::GetInstance()->Render_Management();

    m_pGraphicDevice->Render_End();

    // FPS, 플레이어 위치 상단 바에 출력하기 
    ++iFPSCounter;
    if ((unsigned)(llElapsedTime + 1000) < GetTickCount64())
    {
        iFPS = iFPSCounter;
        iFPSCounter = 0;
        llElapsedTime = GetTickCount64();
        if (m_pPlayer)
            m_vPlayerPos = m_pPlayer->Get_Transform()->Get_Position();
    }

    swprintf_s(szFPS, L"FPS : %d, Player : %.2f, %.2f, %.2f", iFPS, m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z);

    SetWindowText(g_hWnd, szFPS);

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

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    ShowCursor(FALSE);

    return S_OK;
}

HRESULT MainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{

    return S_OK;
}

HRESULT MainApp::Ready_GroundAndCeiling()
{
    for (int i = 0; i < g_iCntX; ++i)
    {
	    for (int j = 0; j < g_iCntZ; ++j)
	    {
            Object* pGround = Ground::Create(m_pGraphicDev, D3DCOLOR_ARGB(255, MathHelper::Random_Int(0, 255), MathHelper::Random_Int(0, 255), MathHelper::Random_Int(0, 255)), g_fSize, g_fSize);
            pGround->Get_Transform()->Set_Position(g_fStartX + i * g_fSize, -5.f, g_fStartZ + j * g_fSize);
            Management::GetInstance()->Add_Object(pGround);
	    }
    }

    for (int i = 0; i < g_iCntX; ++i)
    {
	    for (int j = 0; j < g_iCntZ; ++j)
	    {
            Object* pGround = Ground::Create(m_pGraphicDev, D3DCOLOR_ARGB(255, MathHelper::Random_Int(0, 255), MathHelper::Random_Int(0, 255), MathHelper::Random_Int(0, 255)), g_fSize, g_fSize);
            pGround->Get_Transform()->Set_Position(g_fStartX + i * g_fSize, 80.f, g_fStartZ + j * g_fSize);
            Management::GetInstance()->Add_Object(pGround);
	    }
    }

    return S_OK;
}

HRESULT MainApp::Ready_Wall()
{

    for (int i = 0; i < g_iCntX + 1; ++i)
    {
        for (int j = 0; j < g_iCntZ; ++j)
        {
            Object* pWall = Wall::Create(m_pGraphicDev, D3DCOLOR_ARGB(255, MathHelper::Random_Int(100, 255), MathHelper::Random_Int(100, 255), MathHelper::Random_Int(100, 255)), g_fSize, g_fSize);
            pWall->Get_Transform()->Set_Position(g_fStartX + i * g_fSize, g_fSize * 0.1f, g_fStartZ + j * g_fSize);

            if (i == 1)
            {
                pWall->Get_Transform()->Rotate(AXIS_Z, 30);
            }
            if (i == 2)
            {
                pWall->Get_Transform()->Rotate(AXIS_Z, 110);
            }

            Management::GetInstance()->Add_Object(pWall);
        }
    }

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
    Management::GetInstance()->Release_Management();

    // Singleton
    PhysicsWorld::DestroyInstance();
    InputSystem::DestroyInstance();
    GraphicDevice::DestroyInstance();

    delete this;
}
