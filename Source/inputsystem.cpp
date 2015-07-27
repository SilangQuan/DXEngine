#include "inputsystem.h"


InputSystem::InputSystem()
{
}


InputSystem::~InputSystem()
{
}

void InputSystem::Initialize()
{
	for(int i = 0; i < 256;i++)
	{
		keys[i] = false;
	}
}

void InputSystem::KeyDown(unsigned int input)
{
	keys[input] = true;
}

void InputSystem::KeyUp(unsigned int input)
{
	keys[input] = false;
}

bool InputSystem::IsKeyDown(unsigned int key)
{
	return keys[key];
}
