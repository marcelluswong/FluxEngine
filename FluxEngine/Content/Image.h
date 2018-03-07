#pragma once

#include "Resource.h"

class Image : public Resource
{
public:
	Image(Context* pContext);
	virtual ~Image();

	virtual bool Load(InputStream& inputStream) override;
	virtual bool Save(OutputStream& outputStream) override;

	bool SetSize(const int x, const int y, const int components);
	bool SetData(const unsigned int* pPixels);
	bool SetPixel(const int x, const int y, const Color& color);

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int GetBPP() const { return 4; }
	int ActualBPP() const { return m_BytesPerPixel; }
	unsigned char* GetData() { return m_Pixels.data(); }
	
	SDL_Surface* GetSDLSurface();

private:
	int m_Width = 0;
	int m_Height = 0;
	int m_BytesPerPixel = 0;
	int m_Components = 0;
	int m_Depth;
	std::vector<unsigned char> m_Pixels;
};