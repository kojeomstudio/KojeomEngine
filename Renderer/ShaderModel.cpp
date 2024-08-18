#include "ShaderModel.h"

const wchar_t* ShaderFileName = L"TutorialShader.fxh";

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT ShaderModel::Init(const ID3D11Device* InD3dDevice, const ID3D11DeviceContext* InImmediateContext)
{
	ModelBase::Init(InD3dDevice, InImmediateContext);

    HRESULT HandleResult = S_OK;

    // Compile the vertex shader
    {
        ID3DBlob* pVSBlob = nullptr;
        HandleResult = CompileShaderFromFile(ShaderFileName, "VS", "vs_4_0", &pVSBlob);
        if (FAILED(HandleResult))
        {
            MessageBox(nullptr,
                L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return HandleResult;
        }

        // Create the vertex shader
        HandleResult = pD3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShader);
        if (FAILED(HandleResult))
        {
            pVSBlob->Release();
            return HandleResult;
        }

        // Define the input layout
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE(layout);

        // Create the input layout
        HandleResult = pD3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(), &pVertexLayout);
        pVSBlob->Release();

        // Set the input layout
        pImmediateContext->IASetInputLayout(pVertexLayout);
    }

    // Compile the pixel shader
    {
        ID3DBlob* pPSBlob = nullptr;
        HandleResult = CompileShaderFromFile(ShaderFileName, "PS", "ps_4_0", &pPSBlob);
        if (FAILED(HandleResult))
        {
            MessageBox(nullptr,
                L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return HandleResult;
        }

        // Create the pixel shader
        HandleResult = pD3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShader);
        pPSBlob->Release();
    }

    return HandleResult;
}

void ShaderModel::Render()
{
}

void ShaderModel::Shutdown()
{
    if (pVertexLayout)
    {
        pVertexLayout->Release();
    }
    if (pVertexShader)
    {
        pVertexShader->Release();
    }
    if (pPixelShader)
    {
        pPixelShader->Release();
    }
}
