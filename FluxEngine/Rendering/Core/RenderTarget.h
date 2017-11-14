#pragma once
class Texture;
class Graphics;

enum class ClearFlags;

struct RenderTargetDesc
{
	bool DepthBuffer = true;
	bool ColorBuffer = true;

	int Width = -1;
	int Height = -1;
	int MultiSample = 1;

	DXGI_FORMAT DepthFormat = DXGI_FORMAT_R24G8_TYPELESS;
	DXGI_FORMAT ColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	void* pColorResource = nullptr;
	void* pDepthResource = nullptr;
};

class RenderTarget
{
public:
	RenderTarget(Graphics* pGraphics);
	~RenderTarget();

	DELETE_COPY(RenderTarget)

	bool Create(const RenderTargetDesc& RenderTargetDesc);
	void Clear(const ClearFlags clearFlags, const Color& color, const float depth, unsigned char stencil);

	Texture* GetDepthTexture() const { return m_pDepthTexture.get(); }
	Texture* GetRenderTexture() const { return m_pRenderTexture.get(); }

private:
	bool ValidateDesc(const RenderTargetDesc& desc) const;

	Graphics* m_pGraphics;

	unique_ptr<Texture> m_pRenderTexture;
	unique_ptr<Texture> m_pDepthTexture;
};
