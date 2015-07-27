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

	// ����up����Ϊ(0,1,0).
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// �����������λ��.
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;

	// ���������lookat�ķ���.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// �õ����ȵ�λ��ŷ����ת yaw (Y axis), pitch (X axis), �Լ� roll (Z axis) .
	pitch = rotation.x * 0.0174532925f;
	yaw = rotation.y * 0.0174532925f;
	roll = rotation.z * 0.0174532925f;

	// �õ���ת����.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// �任lookat��up������һ��view�ܹ���ԭ�㱻��ȷ��ת.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// ƽ����ת��������λ��.
	lookAt = position + lookAt;

	// ����view����.
	D3DXMatrixLookAtLH(&viewMatrix, &position, &lookAt, &up);

	return;
}

void Camera::GetViewMatrix(D3DXMATRIX& vMatrix)
{
	vMatrix = viewMatrix;
	return;
}