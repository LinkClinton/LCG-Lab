#include "WindowsConstantBuffer.hpp"
#include "WindowsGraphics.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsConstantBuffer::WindowsConstantBuffer(Graphics * graphics, int size, HeapType heapType) :
	ConstantBuffer(graphics, size, heapType)
{
	D3D11_BUFFER_DESC desc;

	desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = mSize;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = mSize;
	desc.Usage = Utility::ConvertHeapType(heapType);

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateBuffer(&desc, nullptr, &mBuffer);
}

WindowsConstantBuffer::~WindowsConstantBuffer()
{
	Utility::Dispose(mBuffer);
}

void WindowsConstantBuffer::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mBuffer, 0, nullptr, data, 0, 0);
}

#endif // _WIN32