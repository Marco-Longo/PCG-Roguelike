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

	m_GameInput.right		= false;
	m_GameInput.left		= false;
	m_GameInput.down		= false;
	m_GameInput.up			= false;
	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.resetView	= false;
	m_GameInput.resetLevel	= false;
}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state.

	if (kb.Escape) 
	{
		m_quitApp = true;
	}

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

	//Space key
	if (kb.Space)
		m_GameInput.resetView = true;
	else
		m_GameInput.resetView = false;

	//R key
	if (m_KeyboardTracker.IsKeyPressed(DirectX::Keyboard::Keys::R))
		m_GameInput.resetLevel = true;
	else
		m_GameInput.resetLevel = false;
}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
