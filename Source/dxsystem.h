#pragma once
#include <windows.h>
#include "InputSystem.h"
#include "DxRender.h"

class DxSystem
{
public:
	DxSystem();
	~DxSystem();
	
	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	void InitializeWindows(int&, int&);
	void ShutDownWindow();

	LPCSTR applicationName;
	HINSTANCE hinstance;
	HWND hwnd;

	InputSystem* inputSystem;
	DxRender* dxRender;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static DxSystem* ApplicationHandle = 0;

