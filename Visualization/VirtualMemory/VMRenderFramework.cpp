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
	static float moveSpeed = 1.0f;

	glm::vec3 moveVector = glm::vec3(0, 0, 0);

	//for move vector
	if (Input::isKeyDown(KeyCode::W) == true) moveVector = moveVector + Camera::forward();
	if (Input::isKeyDown(KeyCode::S) == true) moveVector = moveVector + Camera::back();
	if (Input::isKeyDown(KeyCode::A) == true) moveVector = moveVector + Camera::left();
	if (Input::isKeyDown(KeyCode::D) == true) moveVector = moveVector + Camera::right();

	//move with constant speed
	if (glm::length(moveVector) != 0) mCamera.move(glm::normalize(moveVector) * moveSpeed * mDeltaTime);

	//update matrix
	mMatrixStructure.WorldTransform = glm::mat4(1);
	mMatrixStructure.CameraTransform = mCamera.getView();
	mMatrixStructure.ProjectTransform = mCamera.getPerspective();

	mMatrixBuffer->update(&mMatrixStructure);
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
	mCamera.setPosition(glm::vec3(0, 0, -3));
	mCamera.setForward(glm::vec3(0, 0, 1));
	mCamera.setPerspective(glm::perspectiveFov(glm::pi<float>() * 0.55f, (float)mWidth, (float)mHeight, 0.0f, 100.0f));

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

}

VMRenderFramework::~VMRenderFramework()
{
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

