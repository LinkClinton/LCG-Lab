#include "WindowsIndexBuffer.hpp"
#include "WindowsGraphics.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsIndexBuffer::WindowsIndexBuffer(Graphics * graphics, int size, const ResourceInfo &info) :
	IndexBuffer(graphics, size, info)
{
	D3D11_BUFFER_DESC desc;

	assert(Utility::hasBindUsage(mResourceInfo.BindUsage, BindUsage::IndexBufferUsage) == true);

	desc.BindFlags = Utility::convertBindUsage(mResourceInfo.BindUsage);
	desc.ByteWidth = mSize;
	desc.CPUAccessFlags = Utility::convertCpuAccessFlag(mResourceInfo.CpuAccessFlag);
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(unsigned int);
	desc.Usage = Utility::convertHeapType(mResourceInfo.HeapType);
	
	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateBuffer(&desc, nullptr, &mBuffer);
}

WindowsIndexBuffer::~WindowsIndexBuffer()
{
	Utility::dispose(mBuffer);
}

void WindowsIndexBuffer::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mBuffer, 0, nullptr, data, 0, 0);
}

#endif // _WIN32