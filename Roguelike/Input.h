#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;
	bool up;
	bool down;
	bool rotRight;
	bool rotLeft;
	bool generate;
	bool smooth;
	bool voronoi;
	bool rotate;
	bool resetView;
};

class Input
{
public:
	Input();
	~Input();
	void Initialise(HWND window);
	void Update();
	bool Quit();
	InputCommands getGameInput();

private:
	bool								m_quitApp;
	int									m_scrollValue;
	std::unique_ptr<DirectX::Keyboard>	m_keyboard;
	std::unique_ptr<DirectX::Mouse>		m_mouse;
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;
	DirectX::Mouse::ButtonStateTracker		m_MouseTracker;
	InputCommands							m_GameInput;
};

