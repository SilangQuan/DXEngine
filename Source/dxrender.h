#pragma once
const bool FULL_SCREEN = false; //是否全屏
const bool VSYNC_ENABLED = true; //是否垂直同步
const float SCREEN_DEPTH = 1000.0f; //深度，远点
const float SCREEN_NEAR = 0.1f; //深度，近点
#include <windows.h>
#include "d3dcontext.h"
#include "assimpmodel.h"
#include "camera.h"
#include "shaderprogram.h"


class DxRender
{
public:
	DxRender();
	~DxRender();
	bool Update();
	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void ShutDown();
	D3DContext* GetContext();
	//bool RenderFrame();

private:
	bool Render();
	void InitPipeline();
	D3DContext* context;
	Camera* camera;
	Assimpmodel* model;
	ShaderProgram* shaderprogram;
	ID3D11Buffer *pVBuffer;
	ID3D11InputLayout *pLayout;            // the pointer to the input layout
	ID3D11VertexShader *pVS;               // the pointer to the vertex shader
	ID3D11PixelShader *pPS;                // the pointer to the pixel shader
};

