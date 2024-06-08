#pragma once

#include <windows.h>
#include "DXHelper.h"

using namespace DirectX;

class RendererCore
{
public:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, WNDPROC InWindowProc);
	HRESULT InitDevice();
	void Shutdown();
	void Render();

private:
	void CleanupDevice();

private:
	HINSTANCE               g_hInst = nullptr;
	HWND                    g_hWnd = nullptr;
	D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device* g_pd3dDevice = nullptr;
	ID3D11Device1* g_pd3dDevice1 = nullptr;
	ID3D11DeviceContext* g_pImmediateContext = nullptr;
	ID3D11DeviceContext1* g_pImmediateContext1 = nullptr;

	IDXGISwapChain* g_pSwapChain = nullptr;
	IDXGISwapChain1* g_pSwapChain1 = nullptr;

	ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

	ID3D11VertexShader* g_pVertexShader = nullptr;
	ID3D11PixelShader* g_pPixelShader = nullptr;

	ID3D11InputLayout* g_pVertexLayout = nullptr;
	ID3D11Buffer* g_pVertexBuffer = nullptr;

	ID3D11Buffer* g_pIndexBuffer = nullptr;
	ID3D11Buffer* g_pConstantBuffer = nullptr;

	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
};

