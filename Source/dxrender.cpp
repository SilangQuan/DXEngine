#include "dxrender.h"


DxRender::DxRender()
{
	context = NULL;
	camera = NULL;
	model = NULL;
	shaderprogram = NULL;
}


DxRender::~DxRender()
{
}

bool DxRender::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	context = new D3DContext();
	if (!context)
	{
		return false;
	}

	result = context->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	camera = new Camera();
	if (!camera)
	{
		return false;
	}

	camera->SetPosition(0.0f, 0.0f, -6.0f);
	

		// 创建shader对象
	shaderprogram = new ShaderProgram();
	if (!shaderprogram)
	{
			return false;
	}
		// 初始化shader对象
	result = shaderprogram->Initialize(context->GetDevice(), hwnd);
	if (!result)
	{
			MessageBox(hwnd, "Could not initialize the color shader object.", "Error", MB_OK);
			return false;
	}

	model = new Assimpmodel();
	if (!model)
	{
		return false;
	}

	result = model->Initialize(context->GetDevice());

	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}

		return true;
	
}

void DxRender::ShutDown()
{
	if (shaderprogram)
	{
		shaderprogram->Shutdown();
		delete shaderprogram;
		shaderprogram = NULL;
	}

	if (model)
	{
		model->Shutdown();
		delete model;
		model = NULL;
	}
	if (camera)
	{
		delete camera;
		camera = NULL;
	}
	if (context)
	{
		context->Shutdown();
		delete context;
		context = NULL;
	}
	return;
}

bool DxRender::Update()
{
	bool result;
	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}

bool DxRender::Render()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result = true;
	context->BeginScene(0.0f, 1.0f, 1.0f, 1.0f);

	camera->Render();

	camera->GetViewMatrix(viewMatrix);
	context->GetWorldMatrix(worldMatrix);
	context->GetProjectionMatrix(projectionMatrix);

	model->Render(context->GetDeviceContext());

	result = shaderprogram->Render(context->GetDeviceContext(), model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	
	D3DXMATRIX MVP = worldMatrix * viewMatrix * projectionMatrix;
	VertexType* vertices = new VertexType[3];
	// 设置顶点数据.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // 左下
	vertices[0].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // 中上.
	vertices[1].color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // 底右
	vertices[2].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	
	for (int i = 0; i < 3; i++)
	{
	//	D3DXVECTOR4 pos0 = MVP * D3DXMATRIX(vertices[0].position.x, vertices[0].position.y, vertices[0].position.z, 1);

	}

	if (!result)
	{
			return false;
	}
	//把framebuffer中的图像present到屏幕上.
	context->EndScene();
	return true;
}

