#include "WindowsUnorderedAccessUsage.hpp"
#include "WindowsGraphics.hpp"
#include "WindowsTexture2D.hpp"
#include "WindowsTexture3D.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsUnorderedAccessUsage::WindowsUnorderedAccessUsage(Graphics * graphics, Texture2D * texture2D, PixelFormat pixelFormat)
	: UnorderedAccessUsage(graphics, texture2D, pixelFormat)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;

	viewDesc.Format = Utility::convertPixelFormat(mPixelFormat);
	viewDesc.Texture2D.MipSlice = 0;
	viewDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateUnorderedAccessView(
		static_cast<WindowsTexture2D*>(texture2D)->mTexture2D, &viewDesc, 
		&mUnorderedAccessView);
}

WindowsUnorderedAccessUsage::WindowsUnorderedAccessUsage(Graphics * graphics, Texture3D * texture3D, PixelFormat pixelFormat)
	: UnorderedAccessUsage(graphics, texture3D, pixelFormat)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDesc;

	viewDesc.Format = Utility::convertPixelFormat(mPixelFormat);
	viewDesc.Texture3D.MipSlice = 0;
	viewDesc.Texture3D.FirstWSlice = 0;
	viewDesc.Texture3D.WSize = texture3D->getDepth();
	viewDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE3D;

	auto result = static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateUnorderedAccessView(
		static_cast<WindowsTexture3D*>(texture3D)->mTexture3D, &viewDesc,
		&mUnorderedAccessView);
}

WindowsUnorderedAccessUsage::~WindowsUnorderedAccessUsage()
{
	Utility::dispose(mUnorderedAccessView);
}

#endif // _WIN32

