#include "pch.h"
#include "Input.h"


Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_quitApp = false;
	m_scrollValue = 0;

	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.right		= false;
	m_GameInput.left		= false;
	m_GameInput.down		= false;
	m_GameInput.up			= false;
	m_GameInput.rotRight	= false;
	m_GameInput.rotLeft		= false;
	m_GameInput.generate	= false;
	m_GameInput.smooth		= false;
	m_GameInput.voronoi		= false;
	m_GameInput.rotate		= false;
	m_GameInput.resetView	= false;
}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	//Right Click
	m_mouse->SetMode(mouse.rightButton ? DirectX::Mouse::MODE_RELATIVE : DirectX::Mouse::MODE_ABSOLUTE);
	if (mouse.positionMode == DirectX::Mouse::MODE_RELATIVE)
		m_GameInput.rotate = true;
	else
		m_GameInput.rotate = false;

	//Scroll Wheel
	int currScrollValue = mouse.scrollWheelValue;
	if ((currScrollValue - m_scrollValue) > 0)
		m_GameInput.forward = true;
	else
		m_GameInput.forward = false;
	if ((currScrollValue - m_scrollValue) < 0)
		m_GameInput.back = true;
	else
		m_GameInput.back = false;
	m_scrollValue = currScrollValue;

	if (kb.Escape) 
	{
		m_quitApp = true;
	}
	
	//A key
	if (kb.A)	m_GameInput.left = true;
	else		m_GameInput.left = false;

	//D key
	if (kb.D)	m_GameInput.right = true;
	else		m_GameInput.right = false;

	//W key
	if (kb.W)	m_GameInput.up = true;
	else		m_GameInput.up = false;

	//S key
	if (kb.S)	m_GameInput.down = true;
	else		m_GameInput.down = false;

	//R key
	if (kb.R)
		m_GameInput.resetView = true;
	else
		m_GameInput.resetView = false;

	//space
	if (m_KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::Space))
		m_GameInput.generate = true;
	else		
		m_GameInput.generate = false;

	//P key
	if (m_KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::P))	
		m_GameInput.smooth = true;
	else
		m_GameInput.smooth = false;

	//V key
	if (m_KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::V))
		m_GameInput.voronoi = true;
	else
		m_GameInput.voronoi = false;
}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
