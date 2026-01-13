#include "pch.h"
#include "MainApp.h"
#include "GraphicDevice.h"
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

    Ready_Environment();

    PhysicsWorld::GetInstance()->Ready_System(m_pGraphicDev);
    InputSystem::GetInstance()->Ready_System();
    Raycast::GetInstance()->Ready_Raycast(m_pGraphicDev);

    Object* pPlayer = Player::Create(m_pGraphicDev);
    pPlayer->Get_Transform()->Set_Position(0.f, 10.f, 0.f);
    m_ObjectList.push_back(pPlayer);

    //Object* pEnemy = Enemy::Create(m_pGraphicDev);
    //m_ObjectList.push_back(pEnemy);

    return S_OK;
}

int MainApp::Update_MainApp(const float& fTimeDelta)
{
    for (auto* pObj : m_ObjectList)
        pObj->Update_GameObject(fTimeDelta);

    return 0;
}

void MainApp::Fixed_Update(const float& fTimeDelta)
{

}

void MainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
    for (auto* pObj : m_ObjectList)
        pObj->LateUpdate_GameObject(fTimeDelta);

    PhysicsWorld::GetInstance()->Update_System();
    InputSystem::GetInstance()->Update_System();
}

void MainApp::Render_MainApp()
{

    m_pGraphicDevice->Render_Begin(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));

    for (auto* pObj : m_ObjectList)
        pObj->Render_GameObject();

    Raycast::GetInstance()->Render_Ray();

    m_pGraphicDevice->Render_End();

    DebugHelper::Print_String(L"=================================");

}

HRESULT MainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
    if (FAILED(GraphicDevice::GetInstance()->Ready_GraphicDev(g_hWnd, &m_pGraphicDevice)))
        return E_FAIL;

    (*ppGraphicDev) = m_pGraphicDevice->Get_GraphicDev();
    (*ppGraphicDev)->AddRef();
    (*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

    Matrix     matView, matProj;

    Vec3   vEye = { 0.f, -3.f, -2.f };
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

HRESULT MainApp::Ready_Environment()
{
    int iCntX = 10;
    int iCntZ = 10;
    float fSizeX = 20;
    float fSizeZ = 20;
    float fStartX = iCntX * 0.5f * fSizeX;
    float fStartZ = iCntZ * 0.5f * fSizeZ;
    fStartX *= -1.f;
    fStartZ *= -1.f;

    for (unsigned int i = 0; i < iCntX; ++i)
    {
	    for (unsigned int j = 0; j < iCntZ; ++j)
	    {
            Object* pGround = Ground::Create(m_pGraphicDev, D3DCOLOR_ARGB(255, MathHelper::Random_Int(0, 255), MathHelper::Random_Int(0, 255), MathHelper::Random_Int(0, 255)), fSizeX, fSizeZ);
            pGround->Get_Transform()->Set_Position(fStartX + i * fSizeX, -5.f, fStartZ + j * fSizeZ);
            m_ObjectList.push_back(pGround);
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
    for (auto* pObj : m_ObjectList)
        Safe_Release(pObj);

    // Singleton
    PhysicsWorld::DestroyInstance();
    InputSystem::DestroyInstance();
    GraphicDevice::DestroyInstance();

    delete this;
}