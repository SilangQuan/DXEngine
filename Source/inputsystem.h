#pragma once
class InputSystem
{
public:
	InputSystem();
	~InputSystem();
	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool keys[256];
};

