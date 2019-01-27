#include "WindowsFactory.hpp"
#include "WindowsFramework.hpp"


#ifdef _WIN32

WindowsFactory::WindowsFactory(Graphics * graphics) :
	Factory(graphics)
{

}

auto WindowsFactory::createVertexBuffer(int size, int stride, HeapType heapType) -> VertexBuffer *
{
	return new WindowsVertexBuffer(mGraphics, size, stride, heapType);
}

auto WindowsFactory::createIndexBuffer(int size, HeapType heapType) -> IndexBuffer *
{
	return new WindowsIndexBuffer(mGraphics, size, heapType);
}

auto WindowsFactory::createConstantBuffer(int size, HeapType heapType) -> ConstantBuffer *
{
	return new WindowsConstantBuffer(mGraphics, size, heapType);
}

auto WindowsFactory::createTexture2D(int width, int height, PixelFormat pixelFormat, BindUsage bindUsage, HeapType heapType) -> Texture2D *
{
	return new WindowsTexture2D(mGraphics, width, height, pixelFormat, bindUsage, heapType);
}

auto WindowsFactory::createTexture3D(int width, int height, int depth, PixelFormat pixelFormat, BindUsage bindUsage, HeapType heapType) -> Texture3D *
{
	return new WindowsTexture3D(mGraphics, width, height, depth, pixelFormat, bindUsage, heapType);
}

auto WindowsFactory::createInputLayout() -> InputLayout *
{
	return new WindowsInputLayout();
}

auto WindowsFactory::createRenderTarget(SwapChain * swapChain) -> RenderTarget * 
{
	return new WindowsRenderTarget(mGraphics, swapChain);
}

auto WindowsFactory::createRenderTarget(Texture2D * texture2D, PixelFormat pixelFormat) -> RenderTarget *
{
	return new WindowsRenderTarget(mGraphics, texture2D, pixelFormat);
}

auto WindowsFactory::createDepthStencil(Texture2D * texture2D, PixelFormat pixelFormat) -> DepthStencil *
{
	return new WindowsDepthStencil(mGraphics, texture2D, pixelFormat);
}

auto WindowsFactory::createResourceUsage(Texture2D * texture2D, PixelFormat pixelFormat) -> ResourceUsage * 
{
	return new WindowsResourceUsage(mGraphics, texture2D, pixelFormat);
}

auto WindowsFactory::createResourceUsage(Texture3D * texture3D, PixelFormat pixelFormat) -> ResourceUsage * 
{
	return new WindowsResourceUsage(mGraphics, texture3D, pixelFormat);
}

auto WindowsFactory::createUnorderedAccessUsage(Texture2D * texture2D, PixelFormat pixelFormat) -> UnorderedAccessUsage * 
{
	return new WindowsUnorderedAccessUsage(mGraphics, texture2D, pixelFormat);
}

auto WindowsFactory::createUnorderedAccessUsage(Texture3D* texture3D, PixelFormat pixelFormat) -> UnorderedAccessUsage*
{
	return new WindowsUnorderedAccessUsage(mGraphics, texture3D, pixelFormat);
}

auto WindowsFactory::createVertexShader(const std::vector<codebyte>& codeBytes, bool isCompiled) -> VertexShader *
{
	return new WindowsVertexShader(mGraphics, codeBytes, isCompiled);
}

auto WindowsFactory::createPixelShader(const std::vector<codebyte>& codeBytes, bool isCompiled) -> PixelShader *
{
	return new WindowsPixelShader(mGraphics, codeBytes, isCompiled);
}

auto WindowsFactory::createRasterizerState() -> RasterizerState *
{
	return new WindowsRasterizerState();
}

auto WindowsFactory::createDepthStencilState() -> DepthStencilState * 
{
	return new WindowsDepthStencilState();
}

auto WindowsFactory::createInput(Framework* framework) -> Input * 
{
	return new WindowsInput(framework);
}

void WindowsFactory::destoryVertexbuffer(VertexBuffer *& vertexBuffer)
{
	if (vertexBuffer == nullptr) return;
	
	delete (WindowsVertexBuffer*)vertexBuffer;

	vertexBuffer = nullptr;
}

void WindowsFactory::destoryIndexBuffer(IndexBuffer *& indexBuffer)
{
	if (indexBuffer == nullptr) return;

	delete (WindowsIndexBuffer*)indexBuffer;

	indexBuffer = nullptr;
}

void WindowsFactory::destoryConstantBuffer(ConstantBuffer *& constantBuffer)
{
	if (constantBuffer == nullptr) return;

	delete (WindowsConstantBuffer*)constantBuffer;

	constantBuffer = nullptr;
}

void WindowsFactory::destoryTexture2D(Texture2D *& texture2D)
{
	if (texture2D == nullptr) return;

	delete (WindowsTexture2D*)texture2D;

	texture2D = nullptr;
}

void WindowsFactory::destoryTexture3D(Texture3D *& texture3D)
{
	if (texture3D == nullptr) return;

	delete (WindowsTexture3D*)texture3D;

	texture3D = nullptr;
}

void WindowsFactory::destoryInputLayout(InputLayout *& inputLayout)
{
	if (inputLayout == nullptr) return;

	delete (WindowsInputLayout*)inputLayout;

	inputLayout = nullptr;
}

void WindowsFactory::destoryRenderTarget(RenderTarget *& renderTarget)
{
	if (renderTarget == nullptr) return;

	delete (WindowsRenderTarget*)renderTarget;

	renderTarget = nullptr;
}

void WindowsFactory::destoryDepthStencil(DepthStencil *& depthStencil)
{
	if (depthStencil == nullptr) return;

	delete (WindowsDepthStencil*)depthStencil;

	depthStencil = nullptr;
}

void WindowsFactory::destoryResourceUsage(ResourceUsage *& resourceUsage)
{
	if (resourceUsage == nullptr) return;

	delete (WindowsResourceUsage*)resourceUsage;

	resourceUsage = nullptr;
}

void WindowsFactory::destoryUnorderedAccessUsage(UnorderedAccessUsage *& unorderedAccessUsage)
{
	if (unorderedAccessUsage == nullptr) return;

	delete (WindowsUnorderedAccessUsage*)unorderedAccessUsage;

	unorderedAccessUsage = nullptr;
}

void WindowsFactory::destoryVertexShader(VertexShader *& vertexShader)
{
	if (vertexShader == nullptr) return;

	delete (WindowsVertexShader*)vertexShader;

	vertexShader = nullptr;
}

void WindowsFactory::destoryPixelShader(PixelShader *& pixelShader)
{
	if (pixelShader == nullptr) return;

	delete (WindowsPixelShader*)pixelShader;

	pixelShader = nullptr;
}

void WindowsFactory::destoryRasterizerState(RasterizerState *& rasterizerState)
{
	if (rasterizerState == nullptr) return;

	delete (WindowsRasterizerState*)rasterizerState;

	rasterizerState = nullptr;
}

void WindowsFactory::destoryDepthStencilState(DepthStencilState *& depthStencilState)
{
	if (depthStencilState == nullptr) return;

	delete (WindowsDepthStencilState*)depthStencilState;

	depthStencilState = nullptr;
}

void WindowsFactory::destoryInput(Input *&input) {
	if (input == nullptr) return;

	delete (WindowsInput*)input;

	input = nullptr;
}

#endif // _WIN32
