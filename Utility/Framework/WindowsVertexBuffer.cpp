#include "WindowsVertexBuffer.hpp"
#include "WindowsGraphics.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsVertexBuffer::WindowsVertexBuffer(Graphics * graphics, int size, int stride) :
	VertexBuffer(graphics, size, stride)
{
	D3D11_BUFFER_DESC desc;

	desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = mSize;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = mStride;
	desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

	static_cast<WindowsGraphics*>(mGraphics)->mDevice->CreateBuffer(&desc, nullptr, &mBuffer);
}

WindowsVertexBuffer::~WindowsVertexBuffer()
{
	Utility::Dispose(mBuffer);
}

void WindowsVertexBuffer::update(void * data)
{
	static_cast<WindowsGraphics*>(mGraphics)->mDeviceContext->UpdateSubresource(mBuffer, 0, nullptr, data, 0, 0);
}

#endif // _WIN32