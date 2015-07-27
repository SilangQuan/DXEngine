#pragma once
#include <D3DX10math.h>

class Camera
{
public:
	Camera();
	~Camera();
	void SetPosition(float posx, float posy, float posz);
	void SetRotation(float rotx, float roty, float rotz);
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	D3DXMATRIX viewMatrix;
	
};

