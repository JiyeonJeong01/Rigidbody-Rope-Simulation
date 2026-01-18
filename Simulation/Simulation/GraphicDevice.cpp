#include "pch.h"
#include "GraphicDevice.h"

IMPLEMENT_SINGLETON(GraphicDevice)

GraphicDevice::GraphicDevice() : m_pSDK(nullptr), m_pDevice(nullptr)
{
}

GraphicDevice::~GraphicDevice()
{
    Free();
}

HRESULT GraphicDevice::Ready_GraphicDev(HWND hWnd, GraphicDevice** ppGraphicDev)
{
    // DX9 장치 초기화
    m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
    if (nullptr == m_pSDK)
        return E_FAIL;

    D3DCAPS9       DeviceCaps;
    ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

    if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
        return E_FAIL;

    unsigned long  dwFlag(0);

    if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        dwFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING;

    else
        dwFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    D3DPRESENT_PARAMETERS       d3dpp;
    ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

    d3dpp.BackBufferWidth = WINCX;
    d3dpp.BackBufferHeight = WINCY;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferCount = 1;

    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;

    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;

    d3dpp.Windowed = MODE_WIN;

    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        dwFlag,
        &d3dpp,
        &m_pDevice)))
        return E_FAIL;

    *ppGraphicDev = this;

    return S_OK;
}

void GraphicDevice::Render_Begin(D3DXCOLOR Color)
{
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, Color, 1.f,  0);
    m_pDevice->BeginScene();
}

void GraphicDevice::Render_End()
{
    m_pDevice->EndScene();
    m_pDevice->Present(NULL, NULL, NULL, NULL);
}

void GraphicDevice::Free()
{
    m_pDevice->Release();
    m_pDevice = NULL;

    m_pSDK->Release();
    m_pSDK = NULL;
}
