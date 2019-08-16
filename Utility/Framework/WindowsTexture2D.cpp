#include "WindowsTexture2D.hpp"
#include "WindowsGraphics.hpp"

#ifdef _WIN32

#include <DirectXTex.h>

WindowsTexture2D::WindowsTexture2D(Graphics * graphics, int width, int height, PixelFormat pixelFormat, const ResourceInfo &info) :
	Texture2D(graphics, width, height, pixelFormat, info)
{
	D3D11_TEXTURE2D_DESC desc;

	desc.ArraySize = 1;
	desc.BindFlags = Utility::convertBindUsage(mResourceInfo.BindUsage);
	desc.CPUAccessFlags = Utility::convertCpuAccessFlag(mResourceInfo.CpuAccessFlag);
	desc.Format = Utility::convertPixelFormat(mPixelFormat);
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = Utility::convertHeapType(mResourceInfo.HeapType);
	desc.Width = mWidth;

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateTexture2D(&desc, nullptr,
		&mTexture2D);
}

WindowsTexture2D::~WindowsTexture2D()
{
	Utility::dispose(mTexture2D);
}

void WindowsTexture2D::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mTexture2D, 0, nullptr, data, mRowPitch, 0);
}

auto WindowsTexture2D::map() -> MappedData {
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->Map(mTexture2D, 0,
		Utility::convertCpuAccesssFlagToMapType(mResourceInfo.CpuAccessFlag), 0, &mappedResource);

	return MappedData(mappedResource.pData, mappedResource.RowPitch, mappedResource.DepthPitch);
}

void WindowsTexture2D::unmap() {
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->Unmap(mTexture2D, 0);
}

void WindowsTexture2D::save(const std::wstring& fileName) {
	auto graphics = static_cast<WindowsGraphics*>(mGraphics);

	DirectX::ScratchImage image;
	HRESULT result = S_OK;

	result = DirectX::CaptureTexture(graphics->mDevice, graphics->mDeviceContext, mTexture2D, image);
	result = DirectX::SaveToWICFile(*image.GetImage(0,0,0), DirectX::WIC_FLAGS::WIC_FLAGS_NONE,
		GetWICCodec(DirectX::WICCodecs::WIC_CODEC_PNG), fileName.data());

	assert(result == S_OK);

	image.Release();
}

#endif // _WIN32
