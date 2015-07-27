#pragma once
#include <d3d11.h>
#include <D3DX10math.h>
#include <string>


struct VertexType{
	D3DXVECTOR3 position;
	//D3DXVECTOR3 normal;
	D3DXCOLOR color;
	//D3DXVECTOR2 texture;
};

struct VertexType2 
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal; 
	D3DXVECTOR2 texture; 
	D3DXVECTOR4 Kd;   
	D3DXVECTOR4 Ks; 
};

class Assimpmodel
{
public:
	
	Assimpmodel();
	~Assimpmodel();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool LoadModel(ID3D11Device* device, const std::string& filename);
	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;
	int vertexCount;
	int indexCount;

	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texture;
	D3DXVECTOR4 Kd;
	D3DXVECTOR4 Ks;


};

