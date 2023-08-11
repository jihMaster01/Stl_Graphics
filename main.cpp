#include "Window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine/*PWSTR = wide character(2Byte)*/, int nCmdShow)
{
	if (Window::InitializeWindow(hInstance, 1280, 800, L"그래픽스 엔진") == false)
	{
		exit(-1);
	}

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//do
		}
	}
}