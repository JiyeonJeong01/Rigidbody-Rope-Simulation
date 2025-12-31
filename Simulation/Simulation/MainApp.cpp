#include "pch.h"
#include "MainApp.h"
#include "GraphicDevice.h"

MainApp::MainApp()
    : m_pGraphicDevice(nullptr), m_pGraphicDev(nullptr)
{
}

MainApp::~MainApp()
{
    Free();
}

HRESULT MainApp::Ready_MainApp()
{
    if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
        return E_FAIL;

    if (FAILED(Ready_Scene(m_pGraphicDev)))
        return E_FAIL;

    return S_OK;
}

int MainApp::Update_MainApp(const float& fTimeDelta)
{
    return 0;
}

void MainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
}

void MainApp::Render_MainApp()
{
    m_pGraphicDevice->Render_Begin(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

    m_pGraphicDevice->Render_End();
}

HRESULT MainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
    if (FAILED(GraphicDevice::GetInstance()->Ready_GraphicDev(g_hWnd, &m_pGraphicDevice)))
        return E_FAIL;

    (*ppGraphicDev) = m_pGraphicDevice->Get_GraphicDev();
    (*ppGraphicDev)->AddRef();

    (*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);


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

void MainApp::Free()
{
    GraphicDevice::DestroyInstance();
}

