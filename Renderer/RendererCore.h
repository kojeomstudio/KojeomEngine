#pragma once

#include <windows.h>

#include "DXHeaderCollector.h"

#include "PrimitiveModel.h"
#include "ShaderModel.h"

using namespace DirectX;

class RendererCore
{
public:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, WNDPROC InWindowProc);
	HRESULT InitDevice();
	void Shutdown();
	void Render();

private:
	HRESULT InitDeviceAndContext();
	HRESULT InitGIFactory(const UINT& InWidth, const UINT& InHeight);
	HRESULT InitTargetViewAndViewport(const UINT& InWidth, const UINT& InHeight);
	void CleanupDevice();

private:
	HINSTANCE               WindowHandleInstance = nullptr;
	HWND                    HandleWindow = nullptr;
	D3D_DRIVER_TYPE         DriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device* pD3dDevice = nullptr;
	ID3D11Device1* pD3dDevice1 = nullptr;
	ID3D11DeviceContext* pImmediateContext = nullptr;
	ID3D11DeviceContext1* pImmediateContext1 = nullptr;

	IDXGISwapChain* pSwapChain = nullptr;
	IDXGISwapChain1* pSwapChain1 = nullptr;

	ID3D11RenderTargetView* pRenderTargetView = nullptr;

	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

private:
	PrimitiveModel PrimitiveModelInst;
	ShaderModel ShaderModelInst;
};

