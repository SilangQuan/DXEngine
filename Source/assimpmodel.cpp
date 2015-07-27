#include "assimpmodel.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

Assimpmodel::Assimpmodel()
{
	vertexBuffer = 0;
	indexBuffer = 0;
}


Assimpmodel::~Assimpmodel()
{
}

bool Assimpmodel::LoadModel(ID3D11Device* device, const std::string& filename)
{

	HRESULT result;
	Assimp::Importer importer;
	VertexType2* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality);
	if (!scene)
	{
		MessageBoxA(NULL, importer.GetErrorString(), "Error", MB_OK);
		return false;
	}

	int n = 0;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		vertexCount += aiMesh->mNumVertices;
		indexCount += aiMesh->mNumFaces * 3;
	}

	vertices = new VertexType2[vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	int index1 = 0;
	int index2 = 0;

	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh *submesh = scene->mMeshes[i];
		if (!submesh->HasNormals() || !submesh->HasTextureCoords(0))
		{
			MessageBoxA(NULL, "No UV OR NO normals!", "Error", MB_OK);
			return false;
		}

		int vertexCount = submesh->mNumVertices;
		for (int i = 0; i < vertexCount; ++i)
		{
			vertices[index1].position = D3DXVECTOR3(submesh->mVertices[i].x, submesh->mVertices[i].y, submesh->mVertices[i].z);
			vertices[index1].position = D3DXVECTOR3(submesh->mNormals[i].x, submesh->mNormals[i].y, submesh->mNormals[i].z);
			vertices[index1].texture = D3DXVECTOR2(submesh->mTextureCoords[0][i].x, submesh->mTextureCoords[0][i].y);
			vertices[index1].Kd = D3DXVECTOR4(1.0, 1.0, 1.0, 1.0);
			vertices[index1].Kd = D3DXVECTOR4(1.0, 1.0, 1.0, 1.0);
			++index1;
		}

		for (int i = 0; i < submesh->mNumFaces; ++i)
		{
			const aiFace& face = submesh->mFaces[i];
			indices[index2] = face.mIndices[0];
			++index2;
			indices[index2] = face.mIndices[0];
			++index2;
			indices[index2] = face.mIndices[0];
			++index2;
		}
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType2) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		//HR(result);
		return false;
	}

	// 设置索引缓冲描述. 
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 指向存临时索引缓冲. 
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建索引缓冲. 
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		//HR(result);
		return false;
	}
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

bool Assimpmodel::Initialize(ID3D11Device* device)
{
	bool result;
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	return true;
}

void Assimpmodel::Shutdown()
{
	ShutdownBuffers();
	return;
}

void Assimpmodel::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
	return;
}

int Assimpmodel::GetIndexCount()
{
	return indexCount;
}

bool Assimpmodel::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//首先，我们创建2个临时缓冲存放顶点和索引数据，以便后面使用。.

	// 设置顶点缓冲大小为3，一个三角形.
	vertexCount = 3;

	// 设置索引缓冲大小.
	indexCount = 3;

	// 创建顶点临时缓冲.
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 创建索引缓冲.
	indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}
	//创建顺时针方向的三角形，左手规则
	// 设置顶点数据.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // 左下
	vertices[0].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // 中上.
	vertices[1].color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // 底右
	vertices[2].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	// 设置索引缓冲数据.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	// 设置顶点缓冲描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 指向保存顶点数据的临时缓冲.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 设置索引缓冲描述.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 指向存临时索引缓冲.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建索引缓冲.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 释放临时缓冲.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void Assimpmodel::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return;
}

void Assimpmodel::ShutdownBuffers()
{
	// 释放顶点缓冲.
	if (indexBuffer)
	{
			indexBuffer->Release();
			indexBuffer = 0;
	}
		// 释放索引缓冲
	if (vertexBuffer)
	{
			vertexBuffer->Release();
			vertexBuffer = 0;
	}
	return;
}
