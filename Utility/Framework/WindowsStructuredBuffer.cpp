#include "WindowsStructuredBuffer.hpp"
#include "WindowsGraphics.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsStructuredBuffer::WindowsStructuredBuffer(Graphics* graphics, int element_size, int element_count,
	const ResourceInfo& info) : StructuredBuffer(graphics, element_size, element_count, info)
{
	D3D11_BUFFER_DESC desc;

	assert(Utility::hasBindUsage(mResourceInfo.BindUsage, BindUsage::ShaderResourceUsage) == true);

	desc.BindFlags = Utility::convertBindUsage(mResourceInfo.BindUsage);
	desc.ByteWidth = mSize;
	desc.CPUAccessFlags = Utility::convertCpuAccessFlag(mResourceInfo.CpuAccessFlag);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = mElementSize;
	desc.Usage = Utility::convertHeapType(mResourceInfo.HeapType);

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateBuffer(&desc, nullptr, &mBuffer);
}

WindowsStructuredBuffer::~WindowsStructuredBuffer() {
	Utility::dispose(mBuffer);
}

void WindowsStructuredBuffer::update(void* data) {
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mBuffer, 0, nullptr, data, 0, 0);
}

#endif

