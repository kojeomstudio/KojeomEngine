#pragma once

#include "DXHeaderCollector.h"

class ModelBase
{
public:
	virtual HRESULT Init(const ID3D11Device* InD3dDevice, const ID3D11DeviceContext* InImmediateContext)
	{
		pD3dDevice = const_cast<ID3D11Device*>(InD3dDevice);
		pImmediateContext = const_cast<ID3D11DeviceContext*>(InImmediateContext);

		return S_OK;
	};
	virtual void Render() {};
	virtual void Shutdown() {};

protected:
	ID3D11Device* pD3dDevice = nullptr;
	ID3D11DeviceContext* pImmediateContext = nullptr;
};

