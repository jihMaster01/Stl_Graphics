#pragma once

#include"Window.h"
#include <d3d11.h>

//#pragma comment(lib,"d3d11.lib")
//#pragma comment(lib,"d3dcomliler.lib")

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

class DXApp
{
public:
	DXApp(HINSTANCE hInstance, int width, int height, std::wstring title);

	virtual ~DXApp();
	virtual bool Initialize();

	// interface
	virtual int Run() = 0; // perfect virtual funtion

protected:
	bool fullscreen = false;
	bool m_vsync_enabled = false;
	int m_videoCardMemory = 0;
	char m_videoCardDescription[128] = { 0, };

	virtual void Update() = 0;
	virtual void DrawScene() = 0;

	bool InitializeDirect3D();

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;

	D3D11_VIEWPORT viewport;
};

