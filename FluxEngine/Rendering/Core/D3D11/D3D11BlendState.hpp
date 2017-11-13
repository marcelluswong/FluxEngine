#include "D3D11GraphicsImpl.h"

void* BlendState::GetOrCreate(Graphics* pGraphics)
{
	unsigned int stateHash =
		(unsigned char)m_BlendMode << 0
		| (unsigned char)m_AlphaToCoverage << 8
		| (unsigned char)m_ColorWriteMask << 16;

	auto state = m_BlendStates.find(stateHash);
	if (state != m_BlendStates.end())
		return state->second;

	AUTOPROFILE_DESC(BlendState_Create, ToHex(stateHash));

	m_BlendStates[stateHash] = nullptr;
	void*& pBlendState = m_BlendStates[stateHash];

	D3D11_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = m_AlphaToCoverage;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0] = D3D11RenderTargetBlendDesc(m_BlendMode, (unsigned char)m_ColorWriteMask);

	HR(pGraphics->GetImpl()->GetDevice()->CreateBlendState(&desc, (ID3D11BlendState**)&pBlendState));

	return pBlendState;
}