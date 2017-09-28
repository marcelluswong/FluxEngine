#include "stdafx.h"

#include "EffectLoader.h"
#include "Rendering\Core\Graphics.h"

ID3DX11Effect* EffectLoader::LoadContent(const string& assetFile)
{
	HRESULT hr;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;
	
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	wstring path = wstring(assetFile.begin(), assetFile.end());
	hr = D3DX11CompileEffectFromFile(path.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		m_pGraphics->GetDevice(),
		&pEffect,
		&pErrorBlob);

	if(FAILED(hr))
	{
		if(pErrorBlob!=nullptr)
		{
			char *errors = (char*)pErrorBlob->GetBufferPointer();
 
			stringstream ss;
			for(unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				ss<<errors[i];
 
			OutputDebugStringA(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			FLUX_LOG(ERROR, ss.str());
		}
		else
		{
			stringstream ss;
			ss<<"EffectLoader: Failed to CreateEffectFromFile!\nPath: ";
			ss<<assetFile;
			FLUX_LOG_HR(ss.str(), hr);
			return nullptr;
		}
	}
	return pEffect;
}

void EffectLoader::Destroy(ID3DX11Effect* objToDestroy)
{
	SafeRelease(objToDestroy);
}
