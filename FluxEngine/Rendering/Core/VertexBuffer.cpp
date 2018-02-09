#include "FluxEngine.h"
#include "VertexBuffer.h"
#include "Graphics.h"

#ifdef D3D11
#include "D3D11/D3D11VertexBuffer.hpp"
#endif

VertexBuffer::VertexBuffer(Graphics* pGraphics) :
	m_pGraphics(pGraphics)
{
}

VertexBuffer::~VertexBuffer()
{
	Release();
}

void VertexBuffer::SetVertexSize(const vector<VertexElement>& elements)
{
	m_VertexStride = 0;
	for (const VertexElement& element : elements)
	{
		m_VertexStride += VertexElement::GetSizeOfType(element.Type);
	}
}

void VertexBuffer::UpdateOffsets(vector<VertexElement>& elements)
{
	m_BufferHash = 0;

	unsigned int offset = 0;
	for (VertexElement& element : elements)
	{
		m_BufferHash <<= 10;
		m_BufferHash |= element.GetHash();
		element.Offset = offset;
		offset += VertexElement::GetSizeOfType(element.Type);
	}
}

void VertexBuffer::Release()
{
	SafeRelease(m_pBuffer);
}