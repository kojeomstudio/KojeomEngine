#include "RendererCore.h"

HRESULT RendererCore::InitWindow(HINSTANCE hInstance, int nCmdShow, WNDPROC InWindowProc)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = InWindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, L"IDI_WINLOGO");
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"KojeomStudio";
    wcex.hIconSm = LoadIcon(wcex.hInstance, L"");

    if (!RegisterClassEx(&wcex))
    {
        return E_FAIL;
    }

    WindowHandleInstance = hInstance;

    RECT ClientWindowRect = { 0, 0, 800, 600 };
    AdjustWindowRect(&ClientWindowRect, WS_OVERLAPPEDWINDOW, FALSE);

    HandleWindow = CreateWindow(L"KojeomStudio", L"Direct3D 11 Tutorials",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, ClientWindowRect.right - ClientWindowRect.left, ClientWindowRect.bottom - ClientWindowRect.top, nullptr, nullptr, hInstance,
        nullptr);

    if (!HandleWindow)
    {
        return E_FAIL;
    }

    ShowWindow(HandleWindow, nCmdShow);

    return S_OK;
}

HRESULT RendererCore::InitDevice()
{
    HRESULT HandleResult = S_OK;

    // calc window rect.
    RECT ClientWindowRect;
    GetClientRect(HandleWindow, &ClientWindowRect);
    UINT width = ClientWindowRect.right - ClientWindowRect.left;
    UINT height = ClientWindowRect.bottom - ClientWindowRect.top;

    HandleResult = InitDeviceAndContext();
    if (FAILED(HandleResult))
    {
        return HandleResult;
    }

    HandleResult = InitGIFactory(width, height);
    if (FAILED(HandleResult))
    {
        return HandleResult;
    }

    HandleResult = InitTargetViewAndViewport(width, height);
    if (FAILED(HandleResult))
    {
        return HandleResult;
    }
    
    HandleResult = ShaderModelInst.Init(pD3dDevice, pImmediateContext);
    HandleResult = PrimitiveModelInst.Init(pD3dDevice, pImmediateContext);

    // Initialize the world matrix
    WorldMatrix = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    ViewMatrix = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

    return S_OK;
}

void RendererCore::Shutdown()
{
    CleanupDevice();
    PrimitiveModelInst.Shutdown();
    ShaderModelInst.Shutdown();
}

void RendererCore::Render()
{
    // Update our time
    static float time = 0.0f;
    if (DriverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        time += (float)XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0)
        {
            timeStart = timeCur;
        }

        time = (timeCur - timeStart) / 1000.0f;
    }

    //
    // Animate the cube
    //
    WorldMatrix = XMMatrixRotationY(time);

    //
    // Clear the back buffer
    //
    pImmediateContext->ClearRenderTargetView(pRenderTargetView, Colors::MidnightBlue);

    //
    PrimitiveModelInst.Render();
    ShaderModelInst.Render();

    //
    // Renders a triangle
    //
    ID3D11Buffer* pConstantBuffer = PrimitiveModelInst.GetConstantBuffer();

    pImmediateContext->VSSetShader(ShaderModelInst.GetVertexShader(), nullptr, 0);
    pImmediateContext->VSSetConstantBuffers(0, 1, &(pConstantBuffer));
    pImmediateContext->PSSetShader(ShaderModelInst.GetPixelShader(), nullptr, 0);
    pImmediateContext->DrawIndexed(36, 0, 0);        // 36 vertices needed for 12 triangles in a triangle list

    //
    // Update variables
    //
    ConstantBuffer cb;
    cb.WorldMatrix = XMMatrixTranspose(WorldMatrix);
    cb.ViewMatrix = XMMatrixTranspose(ViewMatrix);
    cb.ProjectionMatrix = XMMatrixTranspose(ProjectionMatrix);
    pImmediateContext->UpdateSubresource(PrimitiveModelInst.GetConstantBuffer(), 0, nullptr, &cb, 0, 0);

    //
    // Present our back buffer to our front buffer
    //
    pSwapChain->Present(0, 0);
}

HRESULT RendererCore::InitDeviceAndContext()
{
    HRESULT HandleResult;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        DriverType = driverTypes[driverTypeIndex];
        HandleResult = D3D11CreateDevice(nullptr, DriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &pD3dDevice, &FeatureLevel, &pImmediateContext);

        if (HandleResult == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            HandleResult = D3D11CreateDevice(nullptr, DriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &pD3dDevice, &FeatureLevel, &pImmediateContext);
        }

        if (SUCCEEDED(HandleResult))
        {
            break;
        }
    }

    return HandleResult;
}

HRESULT RendererCore::InitGIFactory(const UINT& InWidth, const UINT& InHeight)
{
    HRESULT HandleResult = S_OK;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        HandleResult = pD3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(HandleResult))
        {
            IDXGIAdapter* adapter = nullptr;
            HandleResult = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(HandleResult))
            {
                HandleResult = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(HandleResult))
    {
        return HandleResult;
    }

    // Create swap chain
    IDXGIFactory2* dxgiFactory_2 = nullptr;
    HandleResult = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory_2));
    if (dxgiFactory_2)
    {
        // DirectX 11.1 or later
        HandleResult = pD3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&pD3dDevice1));
        if (SUCCEEDED(HandleResult))
        {
            (void)pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&pImmediateContext1));
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = InWidth;
        sd.Height = InHeight;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        HandleResult = dxgiFactory_2->CreateSwapChainForHwnd(pD3dDevice, HandleWindow, &sd, nullptr, nullptr, &pSwapChain1);
        if (SUCCEEDED(HandleResult))
        {
            HandleResult = pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&pSwapChain));
        }

        dxgiFactory_2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = InWidth;
        sd.BufferDesc.Height = InHeight;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = HandleWindow;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        HandleResult = dxgiFactory->CreateSwapChain(pD3dDevice, &sd, &pSwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation(HandleWindow, DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    return HandleResult;
}

HRESULT RendererCore::InitTargetViewAndViewport(const UINT& InWidth, const UINT& InHeight)
{
    HRESULT HandleResult = S_OK;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    HandleResult = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(HandleResult))
    {
        return HandleResult;
    }

    HandleResult = pD3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(HandleResult))
    {
        return HandleResult;
    }

    pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)InWidth;
    vp.Height = (FLOAT)InHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pImmediateContext->RSSetViewports(1, &vp);

    return HandleResult;
}

void RendererCore::CleanupDevice()
{
    if (pImmediateContext) pImmediateContext->ClearState();
    if (pRenderTargetView) pRenderTargetView->Release();
    if (pSwapChain1) pSwapChain1->Release();
    if (pSwapChain) pSwapChain->Release();
    if (pImmediateContext1) pImmediateContext1->Release();
    if (pImmediateContext) pImmediateContext->Release();
    if (pD3dDevice1) pD3dDevice1->Release();
    if (pD3dDevice) pD3dDevice->Release();
}
