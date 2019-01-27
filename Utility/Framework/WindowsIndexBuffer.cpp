#include "WindowsIndexBuffer.hpp"
#include "WindowsGraphics.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsIndexBuffer::WindowsIndexBuffer(Graphics * graphics, int size, HeapType heapType) :
	IndexBuffer(graphics, size, heapType)
{
	D3D11_BUFFER_DESC desc;

	desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = mSize;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(unsigned int);
	desc.Usage = Utility::ConvertHeapType(heapType);
	
	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateBuffer(&desc, nullptr, &mBuffer);
}

WindowsIndexBuffer::~WindowsIndexBuffer()
{
	Utility::Dispose(mBuffer);
}

void WindowsIndexBuffer::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mBuffer, 0, nullptr, data, 0, 0);
}

#endif // _WIN32