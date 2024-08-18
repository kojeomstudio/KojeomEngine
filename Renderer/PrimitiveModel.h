#pragma once

#include "DXHeaderCollector.h"
#include "ModelBase.h"

struct SimpleVertex
{
    XMFLOAT3 Position;
    XMFLOAT4 Color;
};


struct ConstantBuffer
{
    XMMATRIX WorldMatrix;
    XMMATRIX ViewMatrix;
    XMMATRIX ProjectionMatrix;
};

class PrimitiveModel : public ModelBase
{
public:
	virtual HRESULT Init(const ID3D11Device* InD3dDevice, const ID3D11DeviceContext* InImmediateContext) override;
	virtual void Render() override;
	virtual void Shutdown() override;

public:
	ID3D11Buffer* GetVertexBuffer() const { return pVertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() const { return pIndexBuffer; }
	ID3D11Buffer* GetConstantBuffer() const { return pConstantBuffer; }

protected:
	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pIndexBuffer = nullptr;
	ID3D11Buffer* pConstantBuffer = nullptr;
};