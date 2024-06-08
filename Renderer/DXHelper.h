#pragma once

#include <d3d11_1.h>
#include <directxcolors.h>
#include <d3dcompiler.h>

// definitions. 

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
