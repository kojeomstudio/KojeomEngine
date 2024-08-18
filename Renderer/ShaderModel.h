#pragma once

#include "DXHeaderCollector.h"
#include "ModelBase.h"

class ShaderModel : public ModelBase
{
public:
	virtual HRESULT Init(const ID3D11Device* InD3dDevice, const ID3D11DeviceContext* InImmediateContext) override;
	virtual void Render() override;
	virtual void Shutdown() override;

public:
	ID3D11VertexShader* GetVertexShader() { return pVertexShader; }
	ID3D11PixelShader* GetPixelShader() { return pPixelShader; }

private:
	ID3D11VertexShader* pVertexShader = nullptr;
	ID3D11PixelShader* pPixelShader = nullptr;
	ID3D11InputLayout* pVertexLayout = nullptr;
};

