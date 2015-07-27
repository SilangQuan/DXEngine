#include "dxsystem.h"


DxSystem::DxSystem()
{
	inputSystem = NULL;
	dxRender = NULL;
}


DxSystem::~DxSystem()
{
}

bool DxSystem::Initialize()
{
	bool result;
	int screenWidth = 0, screenHeight = 0;
	InitializeWindows(screenWidth, screenHeight);

	inputSystem = new InputSystem();
	if (!inputSystem)
	{
		return false;
	}

	inputSystem->Initialize();

	dxRender = new DxRender();

	if (!dxRender)
	{
		return false;
	}

	result = dxRender->Initialize(screenWidth, screenHeight, hwnd);

	if (!result)
	{
		return false;
	}
	return true;
}

void DxSystem::Shutdown()
{
	if (dxRender)
	{
		dxRender->ShutDown();
		delete dxRender;
		dxRender = NULL;
	}

	if (inputSystem)
	{
		delete inputSystem;
		inputSystem = NULL;
	}
	
	ShutDownWindow();
}

void DxSystem::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Update();
			if (!result)
			{
				done = true;
			}
		}
	}
	return;
}

bool DxSystem::Update()
{
	bool result;
	if (inputSystem->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	result = dxRender->Update();
	if (!result)
	{
		return false;
	}
	return true;
}

void DxSystem::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;
	//�õ�Ӧ�ó������ָ��
		
	ApplicationHandle = this;
	// �õ�Ӧ�ó���ʵ�����
	hinstance = GetModuleHandle(NULL);
	// Ӧ�ó�������
	applicationName = "Engine";

	// ���ô��������.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc; //ָ���ص�����
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //Ĭ�Ϻ�ɫ���ں�ɫ����
	wc.lpszMenuName = NULL;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);
	// ע�ᴰ����
	RegisterClassEx(&wc);
	// �õ�windows����ֱ���
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	// �����Ƿ�ȫ�����ò�ͬ�ķֱ���.
	if (FULL_SCREEN)
	{
			//ȫ��ģʽ�£����ô��ڴ�СΪwindows����ֱ���.
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			// ��ʱ������ʾ�豸Ϊȫ��ģʽ��ע�⣺Ӧ�ó����˳�ʱ�򣬽��ָ�ϵͳĬ�����á�
			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
			// ���ô��ڵ�Ĭ��λ��Ϊ(0,0).
			posX = posY = 0;
	}
	else
	{
			// ����ģʽ��800*600.
			screenWidth = 800;
			screenHeight = 600;
			// ����λ��,posX, posY�������Ͻ�����
			posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}
		// ȫ���ʹ���ʹ�ò�ͬ�Ĳ���.
	if (FULL_SCREEN)
	{
			hwnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			posX, posY, screenWidth, screenHeight, NULL, NULL, hinstance, NULL);
	}
	else
	{
			hwnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
			WS_OVERLAPPEDWINDOW,
			posX, posY, screenWidth, screenHeight, NULL, NULL, hinstance, NULL);
	}
		// ��ʾ���ڲ�������Ϊ����.
		ShowWindow(hwnd, SW_SHOW);
		SetForegroundWindow(hwnd);
		SetFocus(hwnd);
}

void DxSystem::ShutDownWindow()
{
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(hwnd);
	hwnd = NULL;

	UnregisterClass(applicationName, hinstance);
	ApplicationHandle = NULL;
	return;
}

LRESULT CALLBACK DxSystem::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case  WM_KEYDOWN:
		inputSystem->KeyDown((unsigned int)wparam);
		return 0;
	case WM_KEYUP:
		inputSystem->KeyUp((unsigned int)wparam);
		return 0;
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		return ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}
}