#include "WindowsConstantBuffer.hpp"
#include "WindowsGraphics.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsConstantBuffer::WindowsConstantBuffer(Graphics * graphics, int size, const ResourceInfo &info) :
	ConstantBuffer(graphics, size, info)
{
	D3D11_BUFFER_DESC desc;

	assert(Utility::hasBindUsage(mResourceInfo.BindUsage, BindUsage::ConstantBufferUsage) == true);

	desc.BindFlags = Utility::convertBindUsage(mResourceInfo.BindUsage);
	desc.ByteWidth = mSize;
	desc.CPUAccessFlags = Utility::convertCpuAccessFlag(mResourceInfo.CpuAccessFlag);
	desc.MiscFlags = 0;
	desc.StructureByteStride = mSize;
	desc.Usage = Utility::convertHeapType(mResourceInfo.HeapType);

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateBuffer(&desc, nullptr, &mBuffer);
}

WindowsConstantBuffer::~WindowsConstantBuffer()
{
	Utility::dispose(mBuffer);
}

void WindowsConstantBuffer::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mBuffer, 0, nullptr, data, 0, 0);
}

#endif // _WIN32