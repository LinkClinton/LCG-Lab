#include "WindowsFactory.hpp"
#include "WindowsFramework.hpp"


#ifdef _WIN32

WindowsFactory::WindowsFactory(Graphics * graphics) :
	Factory(graphics)
{

}

auto WindowsFactory::createVertexBuffer(int size, int stride, const ResourceInfo &info) -> VertexBuffer *
{
	return new WindowsVertexBuffer(mGraphics, size, stride, info);
}

auto WindowsFactory::createIndexBuffer(int size, const ResourceInfo &info) -> IndexBuffer *
{
	return new WindowsIndexBuffer(mGraphics, size, info);
}

auto WindowsFactory::createStructuredBuffer(int element_size, int element_count,
	const ResourceInfo& info) -> StructuredBuffer* {
	return new WindowsStructuredBuffer(mGraphics, element_size, element_count, info);
}

auto WindowsFactory::createConstantBuffer(int size, const ResourceInfo &info) -> ConstantBuffer *
{
	return new WindowsConstantBuffer(mGraphics, size, info);
}

auto WindowsFactory::createTexture2D(int width, int height, PixelFormat pixelFormat, const ResourceInfo &info) -> Texture2D *
{
	return new WindowsTexture2D(mGraphics, width, height, pixelFormat, info);
}

auto WindowsFactory::createTexture3D(int width, int height, int depth, PixelFormat pixelFormat, const ResourceInfo &info) -> Texture3D *
{
	return new WindowsTexture3D(mGraphics, width, height, depth, pixelFormat, info);
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

void WindowsFactory::destroyVertexBuffer(VertexBuffer *& vertexBuffer)
{
	if (vertexBuffer == nullptr) return;
	
	delete (WindowsVertexBuffer*)vertexBuffer;

	vertexBuffer = nullptr;
}

void WindowsFactory::destroyIndexBuffer(IndexBuffer *& indexBuffer)
{
	if (indexBuffer == nullptr) return;

	delete (WindowsIndexBuffer*)indexBuffer;

	indexBuffer = nullptr;
}

void WindowsFactory::destroyStructuredBuffer(StructuredBuffer*& structuredBuffer) {
	if (structuredBuffer == nullptr) return;

	delete (WindowsStructuredBuffer*)structuredBuffer;

	structuredBuffer = nullptr;
}

void WindowsFactory::destroyConstantBuffer(ConstantBuffer *& constantBuffer)
{
	if (constantBuffer == nullptr) return;

	delete (WindowsConstantBuffer*)constantBuffer;

	constantBuffer = nullptr;
}

void WindowsFactory::destroyTexture2D(Texture2D *& texture2D)
{
	if (texture2D == nullptr) return;

	delete (WindowsTexture2D*)texture2D;

	texture2D = nullptr;
}

void WindowsFactory::destroyTexture3D(Texture3D *& texture3D)
{
	if (texture3D == nullptr) return;

	delete (WindowsTexture3D*)texture3D;

	texture3D = nullptr;
}

void WindowsFactory::destroyInputLayout(InputLayout *& inputLayout)
{
	if (inputLayout == nullptr) return;

	delete (WindowsInputLayout*)inputLayout;

	inputLayout = nullptr;
}

void WindowsFactory::destroyRenderTarget(RenderTarget *& renderTarget)
{
	if (renderTarget == nullptr) return;

	delete (WindowsRenderTarget*)renderTarget;

	renderTarget = nullptr;
}

void WindowsFactory::destroyDepthStencil(DepthStencil *& depthStencil)
{
	if (depthStencil == nullptr) return;

	delete (WindowsDepthStencil*)depthStencil;

	depthStencil = nullptr;
}

void WindowsFactory::destroyResourceUsage(ResourceUsage *& resourceUsage)
{
	if (resourceUsage == nullptr) return;

	delete (WindowsResourceUsage*)resourceUsage;

	resourceUsage = nullptr;
}

void WindowsFactory::destroyUnorderedAccessUsage(UnorderedAccessUsage *& unorderedAccessUsage)
{
	if (unorderedAccessUsage == nullptr) return;

	delete (WindowsUnorderedAccessUsage*)unorderedAccessUsage;

	unorderedAccessUsage = nullptr;
}

void WindowsFactory::destroyVertexShader(VertexShader *& vertexShader)
{
	if (vertexShader == nullptr) return;

	delete (WindowsVertexShader*)vertexShader;

	vertexShader = nullptr;
}

void WindowsFactory::destroyPixelShader(PixelShader *& pixelShader)
{
	if (pixelShader == nullptr) return;

	delete (WindowsPixelShader*)pixelShader;

	pixelShader = nullptr;
}

void WindowsFactory::destroyRasterizerState(RasterizerState *& rasterizerState)
{
	if (rasterizerState == nullptr) return;

	delete (WindowsRasterizerState*)rasterizerState;

	rasterizerState = nullptr;
}

void WindowsFactory::destroyDepthStencilState(DepthStencilState *& depthStencilState)
{
	if (depthStencilState == nullptr) return;

	delete (WindowsDepthStencilState*)depthStencilState;

	depthStencilState = nullptr;
}

void WindowsFactory::destroyInput(Input *&input) {
	if (input == nullptr) return;

	delete (WindowsInput*)input;

	input = nullptr;
}

#endif // _WIN32
