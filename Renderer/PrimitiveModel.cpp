#include "PrimitiveModel.h"

HRESULT PrimitiveModel::Init(const ID3D11Device* InD3dDevice, const ID3D11DeviceContext* InImmediateContext)
{
    ModelBase::Init(InD3dDevice, InImmediateContext);

    HRESULT HandleResult = S_OK;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
    };

    D3D11_BUFFER_DESC BufferDesc = {};
    BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    BufferDesc.ByteWidth = sizeof(SimpleVertex) * 8;
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vertices;
    HandleResult = pD3dDevice->CreateBuffer(&BufferDesc, &InitData, &pVertexBuffer);
    if (FAILED(HandleResult))
    {
        return HandleResult;
    }

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

    // Create index buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };
    BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    BufferDesc.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    BufferDesc.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    HandleResult = pD3dDevice->CreateBuffer(&BufferDesc, &InitData, &pIndexBuffer);
    if (FAILED(HandleResult))
    {
        return HandleResult;
    }

    // Set index buffer
    pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    BufferDesc.ByteWidth = sizeof(ConstantBuffer);
    BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    BufferDesc.CPUAccessFlags = 0;
    HandleResult = pD3dDevice->CreateBuffer(&BufferDesc, nullptr, &pConstantBuffer);
    
    return HandleResult;
}

void PrimitiveModel::Render()
{

}

void PrimitiveModel::Shutdown()
{
    if (pVertexBuffer)
    {
        pVertexBuffer->Release();
    }

    if (pIndexBuffer)
    {
        pIndexBuffer->Release();
    }

    if (pConstantBuffer)
    {
        pConstantBuffer->Release();
    }
}
