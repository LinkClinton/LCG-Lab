#include "VMRenderFramework.hpp"
#include "Helper.hpp"

#include <glm/glm.hpp>

void VMRenderFramework::render(void * sender, float mDeltaTime)
{
	float rgba[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	unsigned int uavClear[4] = { 0, 0, 0, 0 };
	auto unorderedAccessUsage = mVirtualMemoryManager->getUnorderedAccessUsage();

	mGraphics->clearState();
	mGraphics->clearRenderTarget(mSwapChain->getRenderTarget(), rgba);
	mGraphics->clearUnorderedAccessUsageUint(unorderedAccessUsage[0], uavClear);
	mGraphics->clearUnorderedAccessUsageUint(unorderedAccessUsage[1], uavClear);

	mGraphics->setInputLayout(mInputLayout);
	mGraphics->setIndexBuffer(mIndexBuffer);
	mGraphics->setVertexBuffer(mVertexBuffer);

	mGraphics->setVertexShader(mVertexShader);
	mGraphics->setPixelShader(mPixelShader);

	mGraphics->setConstantBuffer(mMatrixBuffer, 0);
	mGraphics->setConstantBuffer(mVirtualMemoryManager->getMultiResolutionSizeBuffer(), 1);
	mGraphics->setConstantBuffer(mVirtualMemoryManager->getMultiResolutionBaseBuffer(), 2);
	mGraphics->setConstantBuffer(mVirtualMemoryManager->getMultiResolutionBlockBaseBuffer(), 3);

	mGraphics->setResourceUsage(mVirtualMemoryManager->getPageDirectory()->getTextureUsage(), 0);
	mGraphics->setResourceUsage(mVirtualMemoryManager->getPageTable()->getTextureUsage(), 1);
	mGraphics->setResourceUsage(mVirtualMemoryManager->getBlockTable()->getTextureUsage(), 2);

	mGraphics->setUnorderedAccessUsage(unorderedAccessUsage);

	mGraphics->setPrimitiveType(PrimitiveType::TriangleList);
	mGraphics->setRasterizerState(mRasterizerState);

	mGraphics->setViewPort(0, 0, (float)mWidth, (float)mHeight);
	mGraphics->setRenderTarget(mSwapChain->getRenderTarget());

	mGraphics->drawIndexed(36, 0, 0);

	mSwapChain->present(true);

	mVirtualMemoryManager->solveCacheMiss();
}

void VMRenderFramework::update(void * sender, float mDeltaTime)
{
	glm::vec2 offset = glm::vec2(0, 0);

	if (mInput->isKeyDown(KeyCode::Space) == true) mCamera.zoom(-1.0f, mDeltaTime);
	if (mInput->isKeyDown(KeyCode::F) == true) mCamera.zoom(1.0f, mDeltaTime);

	if (mInput->isKeyDown(KeyCode::W) == true) offset += glm::vec2(0, 1);
	if (mInput->isKeyDown(KeyCode::S) == true) offset += glm::vec2(0, -1);
	if (mInput->isKeyDown(KeyCode::A) == true) offset += glm::vec2(1, 0);
	if (mInput->isKeyDown(KeyCode::D) == true) offset += glm::vec2(-1, 0);

	if (glm::length(offset) != 0) mCamera.pan(glm::normalize(offset), getDeltaTime());

	mCamera.update(mDeltaTime);

	//update matrix
	mMatrixStructure.WorldTransform = glm::mat4(1);
	mMatrixStructure.CameraTransform = mCamera.viewMatrix();
	mMatrixStructure.ProjectTransform = mCamera.projectionMatrix();
	mMatrixStructure.EyePosition[0] = glm::vec4(mCamera.position(), 0.0f);

	mMatrixBuffer->update(&mMatrixStructure);
}

void VMRenderFramework::mouseMove(void * sender, MouseMoveEvent * eventArg)
{
	//button is up
	if (mMouseButtonState[0] == false && mMouseButtonState[1] == false && mMouseButtonState[2] == false)
		return;

	auto centerPosition = glm::vec2(mWidth * 0.5f, mHeight * 0.5f);
	auto offset = eventArg->getPosition() - centerPosition;

	if (offset.length() != 0) mCamera.rotate(glm::vec2(offset.x, -offset.y), getDeltaTime());

	mInput->setCursorPosition(centerPosition);
}

void VMRenderFramework::mouseUp(void * sender, MouseClickEvent * eventArg)
{
	mMouseButtonState[(int)eventArg->getMouseButton()] = false;

	mInput->unlockCursor();
	mInput->showCursor(true);
}

void VMRenderFramework::mouseDown(void * sender, MouseClickEvent * eventArg)
{
	mMouseButtonState[(int)eventArg->getMouseButton()] = true;

	mInput->lockCursor();
	mInput->showCursor(false);
	mInput->setCursorPosition(glm::vec2(mWidth * 0.5f, mHeight * 0.5f));
}

void VMRenderFramework::initializeInputStage()
{
	//initialize the input layout
	mInputLayout = mFactory->createInputLayout();

	mInputLayout->addElement(InputLayoutElement("POSITION", sizeof(glm::vec3), 0));
	mInputLayout->addElement(InputLayoutElement("TEXCOORD", sizeof(glm::vec3), 0));

	//initialize the vertex and index buffer
	mIndexBuffer = mFactory->createIndexBuffer(36 * sizeof(unsigned int), ResourceInfo::IndexBuffer());
	mVertexBuffer = mFactory->createVertexBuffer(8 * sizeof(Mesh::Vertex), sizeof(Mesh::Vertex), ResourceInfo::VertexBuffer());

	//initialize the matrix buffer
	mMatrixBuffer = mFactory->createConstantBuffer(sizeof(MatrixStructure), ResourceInfo::ConstantBuffer());

	//init cube's data
	mCubeMesh = Mesh::Cube(glm::vec3(10));

	//update cube's data
	mIndexBuffer->update(mCubeMesh.indices().data());
	mVertexBuffer->update(mCubeMesh.vertices().data());

	//set camera
	mCamera = OrbitCamera(glm::vec3(0, 0, 0), 20.0f);
	mCamera.perspective(glm::pi<float>() * 0.3f, (float)mWidth / mHeight);
	mCamera.resize(mWidth, mHeight);

	mCamera.setZoomSpeed(20.0f);
	mCamera.setRotateSpeed(glm::vec2(glm::pi<float>() * 0.01f));
	mCamera.setZoomLimit(5.10f, 100.0f);

	mMatrixStructure.WorldTransform = glm::mat4(1);
	mMatrixStructure.CameraTransform = mCamera.viewMatrix();
	mMatrixStructure.ProjectTransform = mCamera.projectionMatrix();

	//update matrix's data
	mMatrixBuffer->update(&mMatrixStructure);
}

void VMRenderFramework::initializeShaderStage()
{
	//initialize shader
	mPixelShader = mFactory->createPixelShader(Helper::readFile("RayCastPixelShader.cso"), true);
	mVertexShader = mFactory->createVertexShader(Helper::readFile("RayCastVertexShader.cso"), true);
}

void VMRenderFramework::initializeRasterizerStage()
{
	//initialize rasterizer state
	mRasterizerState = mFactory->createRasterizerState();

	mRasterizerState->setFillMode(FillMode::Solid);
	mRasterizerState->setCullMode(CullMode::Back);
}

void VMRenderFramework::destoryInputStage()
{
	//destory input layout
	mFactory->destoryInputLayout(mInputLayout);

	//destory vertex and index buffer
	mFactory->destoryIndexBuffer(mIndexBuffer);
	mFactory->destoryVertexbuffer(mVertexBuffer);
}

void VMRenderFramework::destoryShaderStage()
{
	//destory shader
	mFactory->destoryPixelShader(mPixelShader);
	mFactory->destoryVertexShader(mVertexShader);
}

void VMRenderFramework::destoryRasterizerStage()
{
	//destory rasterizer state
	mFactory->destoryRasterizerState(mRasterizerState);
}

VMRenderFramework::VMRenderFramework(const std::string &name, int width, int height) : WindowsFramework(name, width, height)
{
	mInput = mFactory->createInput(this);
	
	mVirtualMemoryManager = new VirtualMemoryManager(mFactory, mGraphics, mWidth, mHeight);
}

VMRenderFramework::~VMRenderFramework()
{
	mFactory->destoryInput(mInput);

	mVirtualMemoryManager->finalize();
	delete mVirtualMemoryManager;

	destoryInputStage();
	destoryShaderStage();
	destoryRasterizerStage();
}

void VMRenderFramework::initialize()
{
	initializeInputStage();
	initializeShaderStage();
	initializeRasterizerStage();

	//set multi-resolution
	std::vector<glm::vec3> multiResolution;
	multiResolution.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

	mVirtualMemoryManager->initialize("Teddybear.raw", multiResolution);
}

