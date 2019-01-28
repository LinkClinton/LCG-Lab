#include "WindowsVertexBuffer.hpp"
#include "WindowsGraphics.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsVertexBuffer::WindowsVertexBuffer(Graphics * graphics, int size, int stride, const ResourceInfo &info) :
	VertexBuffer(graphics, size, stride, info)
{
	D3D11_BUFFER_DESC desc;

	assert(Utility::hasBindUsage(mResourceInfo.BindUsage, BindUsage::VertexBufferUsage) == true);

	desc.BindFlags = Utility::convertBindUsage(mResourceInfo.BindUsage);
	desc.ByteWidth = mSize;
	desc.CPUAccessFlags = Utility::convertCpuAccessFlag(mResourceInfo.CpuAccessFlag);
	desc.MiscFlags = 0;
	desc.StructureByteStride = mStride;
	desc.Usage = Utility::convertHeapType(mResourceInfo.HeapType);

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateBuffer(&desc, nullptr, &mBuffer);
}

WindowsVertexBuffer::~WindowsVertexBuffer()
{
	Utility::dispose(mBuffer);
}

void WindowsVertexBuffer::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mBuffer, 0, nullptr, data, 0, 0);
}

#endif // _WIN32