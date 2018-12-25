#pragma once

#include "Framework.hpp"
#include "Timer.hpp"

#ifdef _WIN32

#include <d3d11.h>
#include <Windows.h>

class WindowsFramework : public Framework {
private:
	HWND mHwnd;

	Timer mTimer;
	float mDeltaTime;

	static LRESULT CALLBACK DefaultWindowProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);

	static void processMessage(WindowsFramework* framework, const MSG &message);

protected:
	virtual void update(void* sender, float deltaTime)override;

	virtual void render(void* sender, float deltaTime)override;

	virtual void keyUp(void* sender, KeyBoardEvent* eventArg)override;

	virtual void keyDown(void* sender, KeyBoardEvent* eventArg)override;

	virtual void mouseMove(void* sender, MouseMoveEvent* eventArg)override;
public:
	WindowsFramework(const std::string &name, int width, int height);

	~WindowsFramework();

	virtual void showWindow()override;

	virtual void hideWindow()override;

	virtual void runLoop()override;

	virtual auto getDeltaTime() -> float override;
};

#endif // _WIN32

#include "WindowsInputLayout.hpp"
#include "WindowsRenderTarget.hpp"
#include "WindowsDepthStencil.hpp"
#include "WindowsSwapChain.hpp"
#include "WindowsConstantBuffer.hpp"
#include "WindowsVertexBuffer.hpp"
#include "WindowsIndexBuffer.hpp"
#include "WindowsTexture2D.hpp"
#include "WindowsTexture3D.hpp"
#include "WindowsVertexShader.hpp"
#include "WindowsPixelShader.hpp"
#include "WindowsRasterizerState.hpp"
#include "WindowsDepthStencilState.hpp"
#include "WindowsResourceUsage.hpp"
#include "WindowsUnorderedAccessUsage.hpp"