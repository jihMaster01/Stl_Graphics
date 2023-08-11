#include "Window.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPSTR pCmdLine/*PWSTR = wide character(2Byte)*/,_In_ int nCmdShow)
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