#include "camera.h"


Camera::Camera()
{
	position = D3DXVECTOR3(0, 0, 0);
	rotation = D3DXVECTOR3(0, 0, 0);
}


Camera::~Camera()
{
}

void Camera::SetPosition(float posx, float posy, float posz)
{
	position = D3DXVECTOR3(posx, posy, posz);
}

void Camera::SetRotation(float rotx, float roty, float rotz)
{
	rotation = D3DXVECTOR3(rotx, roty, rotz);
}

D3DXVECTOR3 Camera::GetPosition()
{
	return position;
}

D3DXVECTOR3 Camera::GetRotation()
{
	return rotation;
}

void Camera::Render()
{
	D3DXVECTOR3 up, pos, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// 设置up向量为(0,1,0).
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// 设置摄像机的位置.
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;

	// 设置摄像机lookat的方向.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// 得到弧度单位的欧拉旋转 yaw (Y axis), pitch (X axis), 以及 roll (Z axis) .
	pitch = rotation.x * 0.0174532925f;
	yaw = rotation.y * 0.0174532925f;
	roll = rotation.z * 0.0174532925f;

	// 得到旋转矩阵.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// 变换lookat和up向量，一般view能够在原点被正确旋转.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// 平移旋转后的摄像机位置.
	lookAt = position + lookAt;

	// 创建view矩阵.
	D3DXMatrixLookAtLH(&viewMatrix, &position, &lookAt, &up);

	return;
}

void Camera::GetViewMatrix(D3DXMATRIX& vMatrix)
{
	vMatrix = viewMatrix;
	return;
}