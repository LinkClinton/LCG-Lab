#include "WindowsResourceUsage.hpp"
#include "WindowsGraphics.hpp"
#include "WindowsTexture2D.hpp"
#include "WindowsTexture3D.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsResourceUsage::WindowsResourceUsage(Graphics* graphics, Texture2D * texture2D, PixelFormat pixelFormat)
	: ResourceUsage(graphics, texture2D, pixelFormat)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

	viewDesc.Format = Utility::ConvertPixelFormat(mPixelFormat);
	viewDesc.Texture2D.MipLevels = 1;
	viewDesc.Texture2D.MostDetailedMip = 0;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateShaderResourceView(
		static_cast<WindowsTexture2D*>(texture2D)->mTexture2D, &viewDesc,
		&mShaderResourceView);
}

WindowsResourceUsage::WindowsResourceUsage(Graphics * graphics, Texture3D * texture3D, PixelFormat pixelFormat)
	: ResourceUsage(graphics, texture3D, pixelFormat)
{

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

	viewDesc.Format = Utility::ConvertPixelFormat(mPixelFormat);
	viewDesc.Texture3D.MipLevels = 1;
	viewDesc.Texture3D.MostDetailedMip = 0;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE3D;
	
	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateShaderResourceView(
		static_cast<WindowsTexture3D*>(texture3D)->mTexture3D, &viewDesc,
		&mShaderResourceView);
}

WindowsResourceUsage::~WindowsResourceUsage()
{
	Utility::Dispose(mShaderResourceView);
}

#endif // _WIN32
