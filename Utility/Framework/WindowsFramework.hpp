#pragma once

#include "Framework.hpp"

#ifdef _WIN32

#include <d3d11.h>
#include <Windows.h>

class WindowsFramework : public Framework {
private:
	HWND mHwnd;

	static LRESULT CALLBACK DefaultWindowProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);

protected:
	
	virtual void update(void* sender)override;

	virtual void render(void* sender)override;
public:
	WindowsFramework(const std::string &name, int width, int height);

	~WindowsFramework();

	virtual void showWindow()override;

	virtual void hideWindow()override;

	virtual void runLoop()override;
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