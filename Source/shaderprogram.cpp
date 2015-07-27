#include "shaderprogram.h"


ShaderProgram::ShaderProgram()
{
	vertexShader = 0;
	pixelShader = 0;
	layout = 0;
	matrixBuffer = 0;
}


ShaderProgram::~ShaderProgram()
{
}


bool ShaderProgram::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// 初始化vs 和ps.
	LPCTSTR vs = "../Assets/Shaders/color.vs";
	LPCTSTR ps = "../Assets/Shaders/color.ps";
	result = InitializeShader(device, hwnd, vs, ps);
	if (!result)
	{
		return false;
	}

	return true;
}
void ShaderProgram::Shutdown()
{
	// 释放shader资源.
	ShutdownShader();

	return;
}
bool ShaderProgram::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool result;


	// 设置shader参数.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// 用shader渲染指定缓冲顶点
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ShaderProgram::InitializeShader(ID3D11Device* device, HWND hwnd, LPCTSTR& vsFilename, LPCTSTR& psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// 初始化指针为空.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 编译vs代码.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMessage, NULL);

		/*result = D3DX11CompileFromFile("shaders.shader", 0, 0, "VShader", "vs_5_0", 0, 0, 0, &vertexShaderBuffer, 0, 0);*/

	if (FAILED(result))
	{
		// 如果vs编译失败，输出错误消息.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 如果没有任何错误消息，可能是shader文件丢失.
		else
		{
			MessageBox(hwnd, _T(vsFilename), "Missing Shader File", MB_OK);
		}

		return false;
	}

	// 编译ps.

	/*result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);*/
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);

	if (FAILED(result))
	{
		// 如果ps编译失败，输出错误信息.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 如果没有任何错误消息，可能是shader文件丢失.
		else
		{
			MessageBox(hwnd, psFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}


	// 从缓冲创建vs shader.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 从缓冲创建ps shader.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 设置数据布局，以便在shader中使用.
	// 定义要和ModelClass中的顶点结构一致.
	polygonLayout[0].SemanticName = "POSITION";//vs中的输入参数
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 得到layout中的元素数量
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 创建顶点输入布局.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		return false;
	}

	//释放顶点和像素缓冲.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 设置动态矩阵描述.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 创建const buffer指针，以便访问shader常量.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ShaderProgram::ShutdownShader()
{
	// 释放常量缓冲
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	//释放顶点布局
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// 释放ps .
	if (pixelShader)
	{
		pixelShader->Release();
		pixelShader = 0;
	}

	// 释放vs
	if (vertexShader)
	{
		vertexShader->Release();
		vertexShader = 0;
	}

	return;
}

void ShaderProgram::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, LPCTSTR& shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;


	// 得到错误信息buffer指针.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 得到buffer的长度.
	bufferSize = errorMessage->GetBufferSize();

	// 打开一个文件写错误信息.
	fout.open("shader-error.txt");

	// 输出错误信心.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 关闭文件
	fout.close();

	// 释放错误消息.
	errorMessage->Release();
	errorMessage = 0;

	//弹出错误提示.
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool ShaderProgram::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// 传入shader前，确保矩阵转置，这是D3D11的要求.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	//  锁定常量缓冲，以便能够写入.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 得到const buffer指针.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 设置world,view以及projection矩阵.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 解锁常量缓冲.
	deviceContext->Unmap(matrixBuffer, 0);

	// 设置常量缓冲位置.
	bufferNumber = 0;

	// 用更新后的值设置常量缓冲.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	return true;
}

void ShaderProgram::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 绑定顶点布局.
	deviceContext->IASetInputLayout(layout);

	// 设置渲染使用vs和ps.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// 渲染三角形
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}