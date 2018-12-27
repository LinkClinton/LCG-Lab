#pragma once

#include <WindowsFramework.hpp>
#include <Camera.hpp>

#include "Helper.hpp"

class VMRenderFramework : public WindowsFramework {
private:
	InputLayout* mInputLayout;
	IndexBuffer* mIndexBuffer;
	VertexBuffer* mVertexBuffer;
	ConstantBuffer* mMatrixBuffer;

	PixelShader* mPixelShader;
	VertexShader* mVertexShader;

	RasterizerState* mRasterizerState;

	Camera mCamera;
	MatrixStructure mMatrixStructure;

	virtual void render(void* sender, float mDeltaTime)override;
	virtual void update(void* sender, float mDeltaTime)override;
	
	virtual void mouseMove(void* sender, MouseMoveEvent* eventArg)override;

	void initializeInputStage();
	void initializeShaderStage();
	void initializeRasterizerStage();

	void destoryInputStage();
	void destoryShaderStage();
	void destoryRasterizerStage();
public:
	VMRenderFramework(const std::string &name, int width, int height);

	~VMRenderFramework();

	void initialize();
};