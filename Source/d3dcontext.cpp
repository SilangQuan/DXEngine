#include "d3dcontext.h"


D3DContext::D3DContext()
{
	swapChain = NULL;
	device = NULL;
	deviceContext = NULL;
	renderTargetView = NULL;
	depthStencilView = NULL;
	depthStencilState = NULL;
	depthStencilBuffer = NULL;
	rasterState = NULL;
}


D3DContext::~D3DContext()
{
}

bool D3DContext ::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
	float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	size_t stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;


	// 保存垂直同步设置
	vsyncEnabled = vsync;

	// 创建一个DirectXgraphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// 用接口工厂创建一个主显卡的适配
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// 得到主适配器的输出.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//得到适合 DXGI_FORMAT_R8G8B8A8_UNORM 的显示模式.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 保存显示模式到displayModeList中
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	//遍历所有显示模式，得到刷新率两个参数值numerator 和 denominato
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}
	// 得到显卡描述
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// 保存显存大小.
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//保存显卡描述串
	//wcstombs_s, wide char转化为char
	error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}
	// 释放显示模式列表
	delete[] displayModeList;
	displayModeList = 0;

	//释放适配器输出.
	adapterOutput->Release();
	adapterOutput = 0;

	//释放适配器
	adapter->Release();
	adapter = 0;

	// 释放接口工厂.
	factory->Release();
	factory = 0;

	// 初始化交换链描述
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 用1个后缓冲
	swapChainDesc.BufferCount = 1;

	//帧缓冲的大小和应用程序窗口大小相等.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 后缓冲的surface的格式为DXGI_FORMAT_R8G8B8A8_UNORM.
	// surface的每个像素用4个无符号的8bit[映射到0-1]来表示。NORM表示归一化。
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 如果使用垂直同步，设置后缓冲的刷新率。.
	//刷新率就是一秒钟把后缓冲内容在屏幕上画出的次数。
	//如果开启垂直同步，则锁定刷新率，则fps是固定的
	if (vsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 设置后缓冲的用途
	// 我们的渲染目标缓冲为后缓冲。
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 后缓冲输出的窗口句柄.
	swapChainDesc.OutputWindow = hwnd;

	// 不使用多重采样
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 设置全屏或者窗口模式.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 设定扫描线ordering以及缩放为unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 后缓冲内容呈现到屏幕后，放弃其内容
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//不设置标志
	swapChainDesc.Flags = 0;

	// 设置feature level为D3D11
	// 如果显卡不支持D3D11,我们能够通过设置这个参数，使用D3D10,或者9.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 创建交换链，设备以及设备上下文.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// 得到交换链中的后缓冲指针.
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// 用后缓冲创建渲染目标视图.
	result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	//释放后缓冲.(引用计数减1)
	backBufferPtr->Release();
	backBufferPtr = 0;

	// 初始化深度缓冲描述.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//设置深度缓冲描述
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1; //对于深度缓冲为1
	depthBufferDesc.ArraySize = 1; //对于深度缓冲为1，对于纹理，这2个参数有更多用途
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 创建深度缓冲.
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(result))
	{
		return false;

	}

	// 初始化深度模版状态描述.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 设置深度模版状态描述.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//D3D11_DEPTH_WRITE_MASK_ZERO禁止写深度缓冲
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 对于front face 像素使用的模版操作操作.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 对于back face像素使用的模版操作模式.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 创建深度模版状态，使其生效
	result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
	{
		return false;

	}

	// 设置深度模版状态.
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	// 初始化深度模版视图.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 设置深度模版视图描述.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 创建深度模版视图.
	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 绑定渲染目标视图和深度缓冲到渲染管线.
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);


	// 设置光栅化描述，指定多边形如何被渲染.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 创建光栅化状态
	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
	{
		return false;
	}

	//设置光栅化状态，使其生效
	deviceContext->RSSetState(rasterState);


	// 设置视口，显示全部后缓冲内容
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// 创建视口
	deviceContext->RSSetViewports(1, &viewport);

	// 设置透视投影矩阵
	fieldOfView = (float)D3DX_PI / 4.0f; //90 degree
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 创建透视投影矩阵.
	D3DXMatrixPerspectiveFovLH(&projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	//初始化world矩阵为单位矩阵.
	//该矩阵实现局部坐标到世界坐标的转换
	D3DXMatrixIdentity(&worldMatrix);

	// 创建正交投影矩阵，主要用来实施2D渲染.
	D3DXMatrixOrthoLH(&orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;

}

void D3DContext::Shutdown()
{
	if (swapChain)
	{
		swapChain->SetFullscreenState(false, NULL);
	}

	if (rasterState)
	{
		rasterState->Release();
		rasterState = NULL;
	}

	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = NULL;
	}

	if (depthStencilState)
	{
		depthStencilState->Release();
		depthStencilState = NULL;
	}

	if (depthStencilBuffer)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = NULL;
	}

	if (renderTargetView)
	{
		renderTargetView->Release();
		renderTargetView = NULL;
	}

	if (deviceContext)
	{
		deviceContext->Release();
		deviceContext = NULL;
	}

	if (device)
	{
		device->Release();
		device = NULL;
	}
	if (swapChain)
	{
		swapChain->Release();
		swapChain = 0;
	}
	return;
}

void D3DContext::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	deviceContext->ClearRenderTargetView(renderTargetView, color);

	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	return;
}

void D3DContext::EndScene()
{
	if (vsyncEnabled)
	{
		swapChain->Present(1, 0);
	}
	else
	{
		swapChain->Present(0, 0);
	}
	return;
}

ID3D11Device* D3DContext::GetDevice()
{
	return device;
}

ID3D11DeviceContext* D3DContext::GetDeviceContext()
{
	return deviceContext;
}

void D3DContext::GetProjectionMatrix(D3DXMATRIX& pMatrix)
{
	pMatrix = projectionMatrix;
	return;
}


void D3DContext::GetWorldMatrix(D3DXMATRIX& wMatrix)
{
	wMatrix = worldMatrix;
	return;
}


void D3DContext::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = orthoMatrix;
	return;
}

void D3DContext::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, videoCardDescription);
	memory = videoCardMemory;
	return;
}