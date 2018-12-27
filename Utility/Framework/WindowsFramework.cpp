#include "WindowsFramework.hpp"
#include "WindowsGraphics.hpp"
#include "WindowsSwapChain.hpp"
#include "WindowsFactory.hpp"

#ifdef _WIN32

#define convert_position_to_vec2(message) glm::vec2(GET_X_LPARAM(message.lParam), GET_Y_LPARAM(message.lParam))

#include <windowsx.h>

LRESULT WindowsFramework::DefaultWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void WindowsFramework::processMessage(WindowsFramework * framework, const MSG & message)
{
	switch (message.message)
	{
	case WM_KEYUP: framework->keyUp(framework, &KeyBoardEvent(false, (KeyCode)message.wParam)); break;
	case WM_KEYDOWN: framework->keyDown(framework, &KeyBoardEvent(true, (KeyCode)message.wParam)); break;
	case WM_MOUSEMOVE: framework->mouseMove(framework, &MouseMoveEvent(convert_position_to_vec2(message))); break;
	case WM_LBUTTONUP: framework->mouseUp(framework, &MouseClickEvent(convert_position_to_vec2(message), MouseButton::Left, false)); break;
	case WM_MBUTTONUP: framework->mouseUp(framework, &MouseClickEvent(convert_position_to_vec2(message), MouseButton::Middle, false)); break;
	case WM_RBUTTONUP: framework->mouseUp(framework, &MouseClickEvent(convert_position_to_vec2(message), MouseButton::Right, false)); break;
	case WM_LBUTTONDOWN: framework->mouseDown(framework, &MouseClickEvent(convert_position_to_vec2(message), MouseButton::Left, true)); break;
	case WM_MBUTTONDOWN: framework->mouseDown(framework, &MouseClickEvent(convert_position_to_vec2(message), MouseButton::Middle, true)); break;
	case WM_RBUTTONDOWN: framework->mouseDown(framework, &MouseClickEvent(convert_position_to_vec2(message), MouseButton::Right, true)); break;
	default:
		break;
	}
}

void WindowsFramework::update(void * sender, float deltaTime)
{

}

void WindowsFramework::render(void * sender, float deltaTime)
{
	mSwapChain->present();
}

void WindowsFramework::keyUp(void * sender, KeyBoardEvent* eventArg)
{
}

void WindowsFramework::keyDown(void * sender, KeyBoardEvent* eventArg)
{
}

void WindowsFramework::mouseMove(void * sender, MouseMoveEvent* eventArg)
{
}

void WindowsFramework::mouseUp(void * sender, MouseClickEvent * eventArg)
{
}

void WindowsFramework::mouseDown(void * sender, MouseClickEvent * eventArg)
{
}

WindowsFramework::WindowsFramework(const std::string &name, int width, int height)
	:Framework(name, width, height)
{
	auto hInstance = GetModuleHandle(0);

	WNDCLASS appInfo;

	appInfo.style = CS_DBLCLKS;
	appInfo.lpfnWndProc = DefaultWindowProc;
	appInfo.cbClsExtra = 0;
	appInfo.cbWndExtra = 0;
	appInfo.hInstance = hInstance;
	appInfo.hIcon = (HICON)LoadImage(0, "", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	appInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
	appInfo.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	appInfo.lpszMenuName = nullptr;
	appInfo.lpszClassName = &mName[0];

	RegisterClass(&appInfo);

	RECT rect;

	rect.top = 0;
	rect.left = 0;
	rect.right = mWidth;
	rect.bottom = mHeight;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	mHwnd = CreateWindow(&mName[0], &mName[0], WS_OVERLAPPEDWINDOW ^
		WS_SIZEBOX ^ WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

	mIsWindowExist = true;

	mGraphics = new WindowsGraphics();
	mFactory = new WindowsFactory(mGraphics);
	mSwapChain = new WindowsSwapChain(mGraphics, mHwnd, mWidth, mHeight);
}

WindowsFramework::~WindowsFramework()
{
	Utility::Delete(mGraphics);
	Utility::Delete(mFactory);
	Utility::Delete(mSwapChain);
}

void WindowsFramework::showWindow()
{
	ShowWindow(mHwnd, SW_SHOW);
}

void WindowsFramework::hideWindow()
{
	ShowWindow(mHwnd, SW_HIDE);
}

void WindowsFramework::runLoop()
{
	while (mIsWindowExist == true) {
		MSG message;

		message.hwnd = mHwnd;

		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message); 
			DispatchMessage(&message);

			processMessage(this, message);

			if (message.message == WM_QUIT) mIsWindowExist = false;
		}

		mDeltaTime = mTimer.getTime();
		mTimer.start();

		update(this, mDeltaTime);

		render(this, mDeltaTime);
	}
}

auto WindowsFramework::getDeltaTime() -> float 
{
	return mDeltaTime;
}

auto WindowsFramework::getHandle() -> void *
{
	return mHwnd;
}

#endif // _WIN32
