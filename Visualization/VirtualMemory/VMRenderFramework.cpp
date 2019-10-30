#include "VMRenderFramework.hpp"
#include "Helper.hpp"

#include <glm/glm.hpp>

void VMRenderFramework::render(void * sender, float mDeltaTime)
{
	float rgba[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	unsigned int uavClear[4] = { 0, 0, 0, 0 };
	auto unorderedAccessUsage = mVirtualMemoryManager->getUnorderedAccessUsage();

	mMatrixBuffer->update(&mMatrixStructure);

	mRasterizerState->setFillMode(FillMode::Solid);
	mRasterizerState->setCullMode(CullMode::Back);

	mGraphics->clearState();
	mGraphics->clearRenderTarget(mSwapChain->getRenderTarget(), rgba);
	mGraphics->clearUnorderedAccessUsageUint(unorderedAccessUsage[0], uavClear);
	mGraphics->clearUnorderedAccessUsageUint(unorderedAccessUsage[1], uavClear);

	mGraphics->setInputLayout(mInputLayout);
	mGraphics->setIndexBuffer(mIndexBuffer);
	mGraphics->setVertexBuffer(mVertexBuffer);

#ifdef _SPARSE_LEAP
	mGraphics->setVertexShader(mSparseLeapVertexShader);
	mGraphics->setPixelShader(mSparseLeapPixelShader);
#else
	mGraphics->setVertexShader(mVertexShader);
	mGraphics->setPixelShader(mPixelShader);
#endif // _SPARSE_LEAP

	mGraphics->setConstantBuffer(mMatrixBuffer, 0);
	mGraphics->setConstantBuffer(mVirtualMemoryManager->getMultiResolutionSizeBuffer(), 1);
	mGraphics->setConstantBuffer(mVirtualMemoryManager->getMultiResolutionBaseBuffer(), 2);
	mGraphics->setConstantBuffer(mVirtualMemoryManager->getMultiResolutionBlockBaseBuffer(), 3);

	mGraphics->setResourceUsage(mVirtualMemoryManager->getPageDirectory()->getTextureUsage(), 0);
	mGraphics->setResourceUsage(mVirtualMemoryManager->getPageTable()->getTextureUsage(), 1);
	mGraphics->setResourceUsage(mVirtualMemoryManager->getBlockTable()->getTextureUsage(), 2);

#ifdef _SPARSE_LEAP
	mGraphics->setResourceUsage(mSparseLeapManager->mRaySegmentListDepthSRVUsage, 3);
	mGraphics->setResourceUsage(mSparseLeapManager->mRaySegmentListCountSRVUsage, 4);
	mGraphics->setResourceUsage(mSparseLeapManager->mRaySegmentListBoxTypeSRVUsage, 5);
	mGraphics->setResourceUsage(mSparseLeapManager->mRaySegmentListEventTypeSRVUsage, 6);
#endif // _SPARSE_LEAP

	mGraphics->setUnorderedAccessUsage(unorderedAccessUsage);

	mGraphics->setPrimitiveType(PrimitiveType::TriangleList);
	mGraphics->setRasterizerState(mRasterizerState);

	mGraphics->setViewPort(0, 0, float(mWidth), float(mHeight));
	mGraphics->setRenderTarget(mSwapChain->getRenderTarget());

	mGraphics->drawIndexed(36, 0, 0);

	mSwapChain->present(true);

	mVirtualMemoryManager->solveCacheMiss();
}

void VMRenderFramework::update(void * sender, float mDeltaTime)
{
	glm::vec2 offset = glm::vec2(0, 0);

	if (mInput->isKeyDown(KeyCode::Space) == true) mViewCamera.zoom(-1.0f, mDeltaTime);
	if (mInput->isKeyDown(KeyCode::F) == true) mViewCamera.zoom(1.0f, mDeltaTime);

	if (mInput->isKeyDown(KeyCode::W) == true) offset += glm::vec2(0, 1);
	if (mInput->isKeyDown(KeyCode::S) == true) offset += glm::vec2(0, -1);
	if (mInput->isKeyDown(KeyCode::A) == true) offset += glm::vec2(1, 0);
	if (mInput->isKeyDown(KeyCode::D) == true) offset += glm::vec2(-1, 0);
	
	if (glm::length(offset) != 0) mViewCamera.pan(glm::normalize(offset), getDeltaTime());

	mViewCamera.update(mDeltaTime);
	
	static auto cubeMeshTriangles = mCubeMesh.triangles();
	static auto cubeMeshVolume = mCubeMesh.volume();

	const auto ratio = Mesh(mCamera->frustum().clip(cubeMeshTriangles)).volume() / cubeMeshVolume;
	const auto resolutionLevel = mVirtualMemoryManager->detectResolutionLevel(ratio);

#ifdef _DEBUG
	printf("Current Resolution Level : %d\n", resolutionLevel);
#endif // _DEBUG

	//update matrix
	mMatrixStructure.WorldTransform = glm::scale(glm::mat4(1), mCubeSize);
	mMatrixStructure.CameraTransform = mCamera->viewMatrix();
	mMatrixStructure.ProjectTransform = mCamera->projectionMatrix();
	mMatrixStructure.RenderConfig[0] = glm::vec4(mCamera->position(), 0.0f);
	mMatrixStructure.RenderConfig[1] = glm::vec4(float(resolutionLevel));
	mMatrixStructure.RenderConfig[2] = glm::vec4(mCubeSize, 0.0f);

#ifdef _SPARSE_LEAP
	mSparseLeapManager->update(mCamera->position());
	renderRaySegmentList();
#endif // _SPARSE_LEAP

}

void VMRenderFramework::mouseMove(void * sender, MouseMoveEvent * eventArg)
{
	//button is up
	if (mMouseButtonState[0] == false && mMouseButtonState[1] == false && mMouseButtonState[2] == false)
		return;

	const auto centerPosition = glm::vec2(mWidth * 0.5f, mHeight * 0.5f);
	auto offset = eventArg->getPosition() - centerPosition;

	if (offset.length() != 0) mViewCamera.rotate(glm::vec2(offset.x, -offset.y), getDeltaTime());

	mInput->setCursorPosition(centerPosition);
}

void VMRenderFramework::mouseUp(void * sender, MouseClickEvent * eventArg)
{
	mMouseButtonState[int(eventArg->getMouseButton())] = false;

	mInput->unlockCursor();
	mInput->showCursor(true);
}

void VMRenderFramework::mouseDown(void * sender, MouseClickEvent * eventArg)
{
	mMouseButtonState[int(eventArg->getMouseButton())] = true;

	mInput->lockCursor();
	mInput->showCursor(false);
	mInput->setCursorPosition(glm::vec2(mWidth * 0.5f, mHeight * 0.5f));
}

void VMRenderFramework::renderRaySegmentList()
{
	float rgba[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	unsigned int clearValue[4] = { 0, 0, 0, 0 };
	
	mGraphics->clearUnorderedAccessUsageUint(mSparseLeapManager->mRaySegmentListCountUAVUsage, clearValue);

	mRasterizerState->setFillMode(FillMode::Solid);
	mRasterizerState->setCullMode(CullMode::None);
	
	mGraphics->clearState();

	mGraphics->setRenderTarget(mSparseLeapManager->mOccupancyGeometryRenderTarget);
	mGraphics->clearRenderTarget(mSparseLeapManager->mOccupancyGeometryRenderTarget, rgba);

	mGraphics->setViewPort(0, 0, float(mWidth), float(mHeight));

	mGraphics->setVertexBuffer(mVertexBuffer);
	mGraphics->setIndexBuffer(mIndexBuffer);
	mGraphics->setInputLayout(mInputLayout);

	mGraphics->setRasterizerState(mRasterizerState);

	mGraphics->setVertexShader(mSparseLeapManager->mOccupancyGeometryVertexShader);
	mGraphics->setPixelShader(mSparseLeapManager->mOccupancyGeometryPixelShader);

	MatrixStructure matrixStructure;

	matrixStructure.WorldTransform = glm::scale(glm::mat4(1), mCubeSize);
	matrixStructure.CameraTransform = mCamera->viewMatrix();
	matrixStructure.ProjectTransform = mCamera->projectionMatrix();
	matrixStructure.RenderConfig[0] = glm::vec4(mCamera->position(), 0.0f);
	
	mGraphics->setConstantBuffer(mMatrixBuffer, 0);

	std::vector<UnorderedAccessUsage*> unorderedAccessUsages(4);

	//commit the RWTexture
	unorderedAccessUsages[0] = mSparseLeapManager->mRaySegmentListCountUAVUsage;
	unorderedAccessUsages[1] = mSparseLeapManager->mRaySegmentListDepthUAVUsage;
	unorderedAccessUsages[2] = mSparseLeapManager->mRaySegmentListBoxTypeUAVUsage;
	unorderedAccessUsages[3] = mSparseLeapManager->mRaySegmentListEventTypeUAVUsage;

	mGraphics->setUnorderedAccessUsage(unorderedAccessUsages);

	mGraphics->setPrimitiveType(PrimitiveType::TriangleList);

	//to do: use the draw instance
	for (auto it = mSparseLeapManager->mOccupancyGeometry.begin(); it != mSparseLeapManager->mOccupancyGeometry.end(); it++) {
		OccupancyHistogramNodeCompareComponent component = *it;
		
		//transform 
		matrixStructure.WorldTransform = glm::translate(glm::mat4(1), 
			(component.Node->AxiallyAlignedBoundingBox.Max + component.Node->AxiallyAlignedBoundingBox.Min) * 0.5f);
		matrixStructure.WorldTransform = glm::scale(glm::mat4(matrixStructure.WorldTransform), 
			component.Node->AxiallyAlignedBoundingBox.Max - component.Node->AxiallyAlignedBoundingBox.Min);

		matrixStructure.RenderConfig[1] = glm::vec4(float(component.IsFrontFace), float(component.Node->Type),
			float(component.Node->Parent != nullptr ? component.Node->Parent->Type : OccupancyType::Empty), 0.0f);

		mMatrixBuffer->update(&matrixStructure);

		mGraphics->setConstantBuffer(mMatrixBuffer, 0);
		mGraphics->drawIndexed(36, 0, 0);
	}
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
	mCubeMesh = Mesh::Cube(glm::vec3(1));
	mCubeSize = glm::vec3(100);

	//update cube's data
	mIndexBuffer->update(mCubeMesh.indices().data());
	mVertexBuffer->update(mCubeMesh.vertices().data());

	//set camera
	mViewCamera = OrbitCamera(glm::vec3(0, 0, 0), 200.0f);
	mViewCamera.perspective(glm::pi<float>() * 0.3f, float(mWidth) / mHeight);
	mViewCamera.resize(mWidth, mHeight);

	mViewCamera.setPanSpeed(glm::vec2(100.0f));
	mViewCamera.setZoomSpeed(20.0f);
	mViewCamera.setRotateSpeed(glm::vec2(glm::pi<float>() * 0.01f));
	mViewCamera.setZoomLimit(100, 200.0f);
	mViewCamera.rotate(glm::vec2(glm::pi<float>(), glm::pi<float>()), 1.0f);

	mCamera = &mViewCamera;

	mMatrixStructure.WorldTransform = glm::mat4(1);
	mMatrixStructure.CameraTransform = mCamera->viewMatrix();
	mMatrixStructure.ProjectTransform = mCamera->projectionMatrix();

	//update matrix's data
	mMatrixBuffer->update(&mMatrixStructure);
}

void VMRenderFramework::initializeShaderStage()
{
	//initialize shader
	mPixelShader = mFactory->createPixelShader(Helper::readFile("RayCastPixelShader.cso"), true);
	mSparseLeapPixelShader = mFactory->createPixelShader(Helper::readFile("SparseLeapRayCastPixelShader.cso"), true);
	mVertexShader = mFactory->createVertexShader(Helper::readFile("RayCastVertexShader.cso"), true);
	mSparseLeapVertexShader = mFactory->createVertexShader(Helper::readFile("SparseLeapRayCastVertexShader.cso"), true);
}

void VMRenderFramework::initializeRasterizerStage()
{
	//initialize rasterizer state
	mRasterizerState = mFactory->createRasterizerState();

	mRasterizerState->setFillMode(FillMode::Solid);
	mRasterizerState->setCullMode(CullMode::Back);
}

void VMRenderFramework::destroyInputStage()
{
	//destroy input layout
	mFactory->destroyInputLayout(mInputLayout);

	//destroy vertex and index buffer
	mFactory->destroyIndexBuffer(mIndexBuffer);
	mFactory->destroyVertexBuffer(mVertexBuffer);
}

void VMRenderFramework::destroyShaderStage()
{
	//destroy shader
	mFactory->destroyPixelShader(mPixelShader);
	mFactory->destroyVertexShader(mVertexShader);
}

void VMRenderFramework::destroyRasterizerStage()
{
	//destroy rasterizer state
	mFactory->destroyRasterizerState(mRasterizerState);
}

VMRenderFramework::VMRenderFramework(const std::string &name, int width, int height) : WindowsFramework(name, width, height)
{
	mInput = mFactory->createInput(this);
	
	mSparseLeapManager = new SparseLeapManager(mFactory, mWidth, mHeight);
	mVirtualMemoryManager = new VirtualMemoryManager(mFactory, mGraphics, mSparseLeapManager, mWidth, mHeight);
}

VMRenderFramework::~VMRenderFramework()
{
	mFactory->destroyInput(mInput);

#ifdef _SPARSE_LEAP
	mSparseLeapManager->finalize();
#endif // _SPARSE_LEAP
	mVirtualMemoryManager->finalize();
	delete mVirtualMemoryManager;
	delete mSparseLeapManager;

	destroyInputStage();
	destroyShaderStage();
	destroyRasterizerStage();
}

void VMRenderFramework::initialize()
{
	initializeInputStage();
	initializeShaderStage();
	initializeRasterizerStage();

	//set multi-resolution
	std::vector<glm::vec3> multiResolution;
	multiResolution.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	multiResolution.push_back(glm::vec3(0.3f, 0.3f, 0.3f));
	multiResolution.push_back(glm::vec3(0.1f, 0.1f, 0.1f));

#ifdef _SPARSE_LEAP
	mSparseLeapManager->initialize(mCubeSize);
#endif // _SPARSE_LEAP
	mVirtualMemoryManager->initialize("volume", multiResolution);
}

