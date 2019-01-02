#include "VMRenderFramework.hpp"
#include "Helper.hpp"

#include <glm/glm.hpp>

#include <Cube.hpp>

void VMRenderFramework::render(void * sender, float mDeltaTime)
{
	float rgba[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	mGraphics->clearState();
	mGraphics->clearRenderTarget(mSwapChain->getRenderTarget(), rgba);

	mGraphics->setInputLayout(mInputLayout);
	mGraphics->setIndexBuffer(mIndexBuffer);
	mGraphics->setVertexBuffer(mVertexBuffer);

	mGraphics->setVertexShader(mVertexShader);
	mGraphics->setPixelShader(mPixelShader);

	mGraphics->setConstantBuffer(mMatrixBuffer, 0);

	mGraphics->setPrimitiveType(PrimitiveType::TriangleList);
	mGraphics->setRasterizerState(mRasterizerState);

	mGraphics->setViewPort(0, 0, (float)mWidth, (float)mHeight);
	mGraphics->setRenderTarget(mSwapChain->getRenderTarget());

	mGraphics->drawIndexed(36, 0, 0);

	mSwapChain->present(true);
}

void VMRenderFramework::update(void * sender, float mDeltaTime)
{
	static float moveSpeed = 2.0f;

	glm::vec3 moveVector = glm::vec3(0, 0, 0);

	//for move vector
	if (mInput->isKeyDown(KeyCode::W) == true) moveVector = moveVector + Camera::forward();
	if (mInput->isKeyDown(KeyCode::S) == true) moveVector = moveVector + Camera::back();
	if (mInput->isKeyDown(KeyCode::A) == true) moveVector = moveVector + Camera::left();
	if (mInput->isKeyDown(KeyCode::D) == true) moveVector = moveVector + Camera::right();
	if (mInput->isKeyDown(KeyCode::F) == true) moveVector = moveVector + Camera::down();
	if (mInput->isKeyDown(KeyCode::Space) == true) moveVector = moveVector + Camera::up();
	
	//move vector is not zero, means we input the key
	if (glm::length(moveVector) != 0) {
		auto length = glm::normalize(moveVector) * moveSpeed * mDeltaTime; //length

		mCamera.walk(length.z);
		mCamera.strafe(-length.x);
		mCamera.fly(length.y);
	}
	
	//update matrix
	mMatrixStructure.WorldTransform = glm::mat4(1);
	mMatrixStructure.CameraTransform = mCamera.getView();
	mMatrixStructure.ProjectTransform = mCamera.getPerspective();

	mMatrixBuffer->update(&mMatrixStructure);
}

void VMRenderFramework::mouseMove(void * sender, MouseMoveEvent * eventArg)
{
	//button is up
	if (mMouseButtonState[0] == false && mMouseButtonState[1] == false && mMouseButtonState[2] == false)
		return;

	//angle speed
	static float angle = glm::pi<float>() * 0.0001f;
	
	auto centerPosition = glm::vec2(mWidth * 0.5f, mHeight * 0.5f);
	auto offset = eventArg->getPosition() - centerPosition;

	if (glm::length(offset) != 0) {
		mCamera.rotateY(angle * offset.x);
		mCamera.rotateRight(angle * offset.y);
	}

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
	mIndexBuffer = mFactory->createIndexBuffer(36 * sizeof(unsigned int));
	mVertexBuffer = mFactory->createVertexBuffer(8 * sizeof(CubeVertex), sizeof(CubeVertex));

	//initialize the matrix buffer
	mMatrixBuffer = mFactory->createConstantBuffer(sizeof(MatrixStructure));

	//update cube's data
	mIndexBuffer->update(&Cube::GetIndices()[0]);
	mVertexBuffer->update(&Cube::GetVertics(1.0f, 1.0f, 1.0f)[0]);

	//set camera
	mCamera.setPosition(glm::vec3(0, 0, -5));
	mCamera.setForward(glm::vec3(0, 0, 1));
	mCamera.setPerspective(glm::perspectiveFov(glm::pi<float>() * 0.1f, (float)mWidth , (float)mHeight, 1.0f, 100.0f));

	mMatrixStructure.WorldTransform = glm::mat4(1);
	mMatrixStructure.CameraTransform = mCamera.getView();
	mMatrixStructure.ProjectTransform = mCamera.getPerspective();

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
}

VMRenderFramework::~VMRenderFramework()
{
	mFactory->destoryInput(mInput);

	destoryInputStage();
	destoryShaderStage();
	destoryRasterizerStage();
}

void VMRenderFramework::initialize()
{
	initializeInputStage();
	initializeShaderStage();
	initializeRasterizerStage();
}

