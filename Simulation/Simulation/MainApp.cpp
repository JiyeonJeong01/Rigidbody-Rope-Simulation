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

    Ready_Ground();
    Ready_Wall();

    PhysicsWorld::GetInstance()->Ready_System(m_pGraphicDev);
    InputSystem::GetInstance()->Ready_System();
    Raycast::GetInstance()->Ready_Raycast(m_pGraphicDev);

    m_pPlayer = Player::Create(m_pGraphicDev);
    m_pPlayer->Get_Transform()->Set_Position(-2.f, 10.f, 0.f);
    Management::GetInstance()->Add_Object(m_pPlayer);

    //Object* pEnemy = Enemy::Create(m_pGraphicDev);
    //m_ObjectList.push_back(pEnemy);
    //Management::GetInstance()->Add_Object(pEnemy);

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

    Raycast::GetInstance()->Render_Ray();

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

    return S_OK;
}

HRESULT MainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{

    return S_OK;
}

HRESULT MainApp::Ready_Ground()
{
    int iCntX = 10;
    int iCntZ = 10;
    float fSizeX = 50;
    float fSizeZ = 50;
    float fStartX = iCntX * 0.5f * fSizeX;
    float fStartZ = 0;// iCntZ * 0.5f * fSizeZ;
    fStartX *= -1.f;
    fStartZ *= -1.f;

    for (int i = 0; i < iCntX; ++i)
    {
	    for (int j = 0; j < iCntZ; ++j)
	    {
            Object* pGround = Ground::Create(m_pGraphicDev, D3DCOLOR_ARGB(255, MathHelper::Random_Int(0, 255), MathHelper::Random_Int(0, 255), MathHelper::Random_Int(0, 255)), fSizeX, fSizeZ);
            pGround->Get_Transform()->Set_Position(fStartX + i * fSizeX, -5.f, fStartZ + j * fSizeZ);
            Management::GetInstance()->Add_Object(pGround);
	    }
    }

    return S_OK;
}

HRESULT MainApp::Ready_Wall()
{
    int iCntX = 1;
    int iCntZ = 10;
    float fSizeX = 50;
    float fSizeY = 50;
    float fStartX = iCntX * 0.5f * fSizeX;
    float fStartZ = 0;// iCntZ * 0.5f * fSizeY;
    fStartX *= -1.f;
    fStartZ *= -1.f;

    for (int i = 0; i < iCntX; ++i)
    {
        for (int j = 0; j < iCntZ; ++j)
        {
            Object* pWall = Wall::Create(m_pGraphicDev, D3DCOLOR_ARGB(255, MathHelper::Random_Int(0, 180), MathHelper::Random_Int(0, 180), MathHelper::Random_Int(0, 180)), fSizeX, fSizeY);
            pWall->Get_Transform()->Set_Position(fStartX + i * fSizeX, fSizeY * 0.5f, fStartZ + j * fSizeY);
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
