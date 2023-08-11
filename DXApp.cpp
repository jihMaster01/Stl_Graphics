#include "DXApp.h"

DXApp::DXApp(HINSTANCE hInstance, int width, int height, std::wstring title)
{
	Window::SetInstance(hInstance);
	Window::SetWidth(width);
	Window::SetHeight(height);
	Window::SetTitle(title);
}

DXApp::~DXApp()
{

}

bool DXApp::Initialize()
{
	if (Window::InitializeWindow() == false)
	{
		return false;
	}

	if (InitializeDirect3D() == false)
	{
		return false;
	}
	return true;
}

bool DXApp::InitializeDirect3D()
{
	IDXGIFactory* factory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&factory))))
	{
		return false;
	}

	IDXGIAdapter* adapter = nullptr;
	if (FAILED(factory->EnumAdapters(0, &adapter)))
	{
		return false;
	}

	IDXGIOutput* adapterOutput = nullptr;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
	{
		return false;
	}

	unsigned int numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
	{
		return false;
	}

	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
	{
		return false;
	}
	// dispaly mode width and height search.
	unsigned int numerator = 0;
	unsigned int denominator = 0;

	for (unsigned int i = 0; i < numModes; i++)
	{
		DXGI_MODE_DESC* temp = &displayModeList[i];
		if (temp->Width == (unsigned int)Window::Width())
		{
			if (temp->Height == (unsigned int)Window::Height())
			{
				numerator = temp->RefreshRate.Numerator;
				denominator = temp->RefreshRate.Denominator;
			}
		}
	}

	// display card struct get.
	DXGI_ADAPTER_DESC adapterDesc{};
	if (FAILED(adapter->GetDesc(&adapterDesc)))
	{
		return false;
	}

	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)
	{
		return false;
	}

	delete[] displayModeList;
	displayModeList = 0;
	adapterOutput->Release();
	adapterOutput = 0;
	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;


	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1; // backbuffer 1. Double Buffering
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = Window::windowsHandle(); 
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 출력된 다음 백버퍼를 비우도록 지정.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Width = Window::Width();
	swapChainDesc.BufferDesc.Height = Window::Height();

	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// multi sampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}
	
	// 스캔 라인 순서 및 크기를 지정하지 않음으로 설정.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 추가 옵션 플래그를 사용하지 않음.
	swapChainDesc.Flags = 0;

	// 피처레벨을 Direct 11 로 설정.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);

	if (FAILED(result))
	{
		MessageBox(nullptr, L"Device Create Falied.", L"Error", 0);
		return false;
	}

	// 백버퍼는 기본적으로 2D텍스처로 만들어져있다.
	ID3D11Texture2D* backbufferTexture = nullptr;
	result = swapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (void**)(&backbufferTexture));
	if (FAILED(result))
	{
		MessageBox(nullptr, L"Backbuffer Loading error", L"error", 0);
		return false;
	}

	result = device->CreateRenderTargetView(backbufferTexture, nullptr, &renderTargetView);
	if (FAILED(result))
	{
		MessageBox(nullptr, L"렌더타겟뷰 생성 실패", L"오류", 0);
		return false;
	}

	backbufferTexture->Release();
	backbufferTexture = 0;

	deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)Window::Width();
	viewport.Height = (float)Window::Height();

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &viewport);

	return true;
}