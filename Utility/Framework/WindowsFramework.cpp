#include "WindowsFramework.hpp"
#include "WindowsGraphics.hpp"
#include "WindowsSwapChain.hpp"
#include "WindowsFactory.hpp"

#ifdef _WIN32

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

void WindowsFramework::update(void * sender)
{

}

void WindowsFramework::render(void * sender)
{
	mSwapChain->present();
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

			if (message.message == WM_QUIT) mIsWindowExist = false;
		}

		update(this);

		render(this);
	}
}

#endif // _WIN32
