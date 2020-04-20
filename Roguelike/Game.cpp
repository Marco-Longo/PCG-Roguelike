#include "pch.h"
#include "Game.h"

#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ImGui;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_input.Initialise(window);
	mouseSensitivity = 5.0f;

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	//setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());

	m_fullscreenRect.left = 0;
	m_fullscreenRect.top = 0;
	m_fullscreenRect.right = 800;
	m_fullscreenRect.bottom = 600;

	m_CameraViewRect.left = 500;
	m_CameraViewRect.top = 0;
	m_CameraViewRect.right = 800;
	m_CameraViewRect.bottom = 240;

	//setup light
	m_Light.setAmbientColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
//	m_Light.setPosition(2.0f, 1.0f, 1.0f);
//	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	//setup camera
	m_Camera.setPosition(Vector3(0.0f, 0.0f, 5.0f));
	m_Camera.setRotation(Vector3(-90.0f, -270.0f, 0.0f));
	m_cameraLock = true;
	
#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game
	
	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	//Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//Update delta time and fps
	dt = float(timer.GetElapsedSeconds());
	if (timer.GetFrameCount() == 1 || timer.GetFrameCount() % 120 == 0)
	{
		fps = int(1.0f / dt);
		fps_text = L"FPS: " + std::to_wstring(fps);
	}

	//Update Mouse Position
	GetCursorPos(&mousePos);
	//Handle Input
	if (m_gameInputCommands.resetView)
	{
		m_Camera.setPosition(Vector3(0.0f, 0.0f, 5.0f));
		m_Camera.setRotation(Vector3(-90.0f, -270.0f, 0.0f));
	}
	if (m_gameInputCommands.resetLevel)
	{
		m_MapGen->ResetLevel();
	}
	/*
	if (!m_cameraLock)
	{
		if (m_gameInputCommands.right)
		{
			Vector3 position = m_Camera01.getPosition();
			position += (m_Camera01.getRight() * m_Camera01.getMoveSpeed() * dt);
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.left)
		{
			Vector3 position = m_Camera01.getPosition();
			position -= (m_Camera01.getRight() * m_Camera01.getMoveSpeed() * dt);
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.down)
		{
			Vector3 position = m_Camera01.getPosition();
			position += (m_Camera01.getUp() * m_Camera01.getMoveSpeed() * dt);
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.up)
		{
			Vector3 position = m_Camera01.getPosition();
			position -= (m_Camera01.getUp() * m_Camera01.getMoveSpeed() * dt);
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.forward)
		{
			Vector3 position = m_Camera01.getPosition();
			position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * mouseSensitivity * 2 * dt);
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.back)
		{
			Vector3 position = m_Camera01.getPosition();
			position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * mouseSensitivity * 2 * dt);
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.rotate)
		{
			Vector3 rotation = m_Camera01.getRotation();
			rotation.y += (mousePos.x - lastFrameCursorPos.x) / mouseSensitivity * m_Camera01.getRotationSpeed() * dt;
			rotation.x += (lastFrameCursorPos.y - mousePos.y) / mouseSensitivity * m_Camera01.getRotationSpeed() * dt;
			if (rotation.x < -170.0f)
				rotation.x = -170.0f;
			if (rotation.x > -10.0f)
				rotation.x = -10.0f;
			m_Camera01.setRotation(rotation);
		}
	}
	*/
	//Camera forward and back movement (DEBUG)
	if (m_gameInputCommands.forward)
	{
		Vector3 position = m_Camera.getPosition();
		position += (m_Camera.getForward() * m_Camera.getMoveSpeed() * mouseSensitivity * 2 * dt);
		m_Camera.setPosition(position);
	}
	else if (m_gameInputCommands.back)
	{
		Vector3 position = m_Camera.getPosition();
		position -= (m_Camera.getForward() * m_Camera.getMoveSpeed() * mouseSensitivity * 2 * dt);
		m_Camera.setPosition(position);
	}

	//Player commands
	Vector3 inputPos = m_Player->GetPosition();
	if (m_gameInputCommands.right)
	{
		inputPos.x += m_Player->GetSpeed() *dt;
	}
	if (m_gameInputCommands.left)
	{
		inputPos.x -= m_Player->GetSpeed() * dt;
	}
	if (m_gameInputCommands.down)
	{
		inputPos.y -= m_Player->GetSpeed() * dt;
	}
	if (m_gameInputCommands.up)
	{
		inputPos.y += m_Player->GetSpeed() * dt;
	}
	m_Player->SetPosition(inputPos);

	//Check for collisions between scene objects
	if (CollisionDetection::SAT(m_Player, *(m_MapGen->GetRoomsList())->begin()))
		debugLine = L"DEBUG: Collision has occured";
	else
		debugLine = L"DEBUG: NO collision has occurred";
//	m_Player->CheckBoundaries(*(m_MapGen->GetRoomsList())->begin());

	m_Camera.setPosition(m_Player->GetPosition().x, m_Player->GetPosition().y, m_Camera.getPosition().z);
	m_Camera.Update();
	m_view = m_Camera.getCameraMatrix();
	m_world = Matrix::Identity;
	lastFrameCursorPos = Vector2(mousePos.x, mousePos.y);

	//create our UI
	SetupGUI();

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

	if (m_input.Quit())
	{
		ExitGame();
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{	
    // Don't try to render anything before the first Update
    if (m_timer.GetFrameCount() == 0)
        return;

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();

	// Set Rendering states
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_states->CullClockwise());
//	context->RSSetState(m_states->Wireframe());

	// Background
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(m_Background->GetPosition());
	m_world = m_world * newPosition;

	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_Background->GetTexture());
	m_Background->Render(context);

	// Generated Map
	std::vector<Boundary*>* roomsList = m_MapGen->GetRoomsList();
	for (std::vector<Boundary*>::iterator it = roomsList->begin(); it != roomsList->end(); ++it)
	{
		Boundary* bound = *it;
		m_world = SimpleMath::Matrix::Identity;
		SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(bound->GetPosition());
		m_world = m_world * newPosition;

		m_BasicShaderPair.EnableShader(context);
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, bound->GetTexture());
		bound->Render(context);
	}

	// Player
	m_world = SimpleMath::Matrix::Identity;
	newPosition = SimpleMath::Matrix::CreateTranslation(m_Player->GetPosition());
	m_world = m_world * newPosition;

	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_Player->GetTexture());
	m_Player->Render(context);

	// Draw Text to the screen
	m_sprites->Begin();
	m_font->DrawString(m_sprites.get(), L"Advanced Procedural Methods", XMFLOAT2(10, 10), Colors::Yellow);
//	m_font->DrawString(m_sprites.get(), debugLine.c_str(), XMFLOAT2(10, 40), Colors::Yellow);
	m_font->DrawString(m_sprites.get(), m_MapGen->debugLine.c_str(), XMFLOAT2(10, 40), Colors::Yellow);
	m_font->DrawString(m_sprites.get(), fps_text.c_str(), XMFLOAT2(1810, 10), Colors::Orange);
	m_sprites->End();

	// Render our GUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

/*
//COLLISION DETECTION FUNCTIONS

void Game::MoveAI()
{
	float speed = 1.5f;
	float minX1, maxX1, minY1, maxY1, minZ1, maxZ1;
	float minX2, maxX2, minY2, maxY2, minZ2, maxZ2;

	//AI Rectangle
	minX1 = m_AIModel.GetCentre().x - (m_AIModel.GetWidth() / 2.0f);
	maxX1 = minX1 + m_AIModel.GetWidth();
	minY1 = m_AIModel.GetCentre().y - (m_AIModel.GetHeight() / 2.0f);
	maxY1 = minY1 + m_AIModel.GetHeight();
	minZ1 = m_AIModel.GetCentre().z - (m_AIModel.GetDepth() / 2.0f);
	maxZ1 = minZ1 + m_AIModel.GetDepth();

	//Boundary Rectangle
	minX2 = m_BoundaryModel.GetCentre().x - (m_BoundaryModel.GetWidth() / 2.0f);
	maxX2 = minX2 + m_BoundaryModel.GetWidth();
	minY2 = m_BoundaryModel.GetCentre().y - (m_BoundaryModel.GetHeight() / 2.0f);
	maxY2 = minY2 + m_BoundaryModel.GetHeight();
	minZ2 = m_BoundaryModel.GetCentre().z - (m_BoundaryModel.GetDepth() / 2.0f);
	maxZ2 = minZ2 + m_BoundaryModel.GetDepth();

	if (maxZ1 > maxZ2 || (minZ1 < minZ2) || (maxX1 > maxX2) || (minX1 < minX2)) //Collision with borders
		m_aiDirection = (m_aiDirection + (rand() % 4)) % 4;

	if (m_aiDirection == 0)
	{
		m_aiPosition.x += speed * dt; //Up
		m_aiPosition.z += speed * dt; //Right
	}
	else if (m_aiDirection == 1)
	{
		m_aiPosition.x -= speed * dt; //Down
		m_aiPosition.z += speed * dt; //Right
	}
	else if (m_aiDirection == 2)
	{
		m_aiPosition.x += speed * dt; //Up
		m_aiPosition.z -= speed * dt; //Left
	}
	else if (m_aiDirection == 3)
	{
		m_aiPosition.x -= speed * dt; //Down
		m_aiPosition.z -= speed * dt; //Left
	}
}

bool Game::SAT()
{
	float minX1, maxX1, minY1, maxY1, minZ1, maxZ1;
	float minX2, maxX2, minY2, maxY2, minZ2, maxZ2;

	minX1 = m_PlayerModel.GetCentre().x - (m_PlayerModel.GetWidth() / 2.0f);
	maxX1 = minX1 + m_PlayerModel.GetWidth();
	minY1 = m_PlayerModel.GetCentre().y - (m_PlayerModel.GetHeight() / 2.0f);
	maxY1 = minY1 + m_PlayerModel.GetHeight();
	minZ1 = m_PlayerModel.GetCentre().z - (m_PlayerModel.GetDepth() / 2.0f);
	maxZ1 = minZ1 + m_PlayerModel.GetDepth();

	minX2 = m_BoundaryModel.GetCentre().x - (m_BoundaryModel.GetWidth() / 2.0f);
	maxX2 = minX2 + m_BoundaryModel.GetWidth();
	minY2 = m_BoundaryModel.GetCentre().y - (m_BoundaryModel.GetHeight() / 2.0f);
	maxY2 = minY2 + m_BoundaryModel.GetHeight();
	minZ2 = m_BoundaryModel.GetCentre().z - (m_BoundaryModel.GetDepth() / 2.0f);
	maxZ2 = minZ2 + m_BoundaryModel.GetDepth();

	return  (minX1 <= maxX2 && maxX1 >= minX2) &&
			(minY1 <= maxY2 && maxY1 >= minY2) &&
			(minZ1 <= maxZ2 && maxZ1 >= minZ2);
}

void Game::CheckBoundaries()
{
	float minX1, maxX1, minY1, maxY1, minZ1, maxZ1;
	float minX2, maxX2, minY2, maxY2, minZ2, maxZ2;

	//Player Rectangle
	minX1 = m_PlayerModel.GetCentre().x - (m_PlayerModel.GetWidth() / 2.0f);
	maxX1 = minX1 + m_PlayerModel.GetWidth();
	minY1 = m_PlayerModel.GetCentre().y - (m_PlayerModel.GetHeight() / 2.0f);
	maxY1 = minY1 + m_PlayerModel.GetHeight();
	minZ1 = m_PlayerModel.GetCentre().z - (m_PlayerModel.GetDepth() / 2.0f);
	maxZ1 = minZ1 + m_PlayerModel.GetDepth();

	//Boundary Rectangle
	minX2 = m_BoundaryModel.GetCentre().x - (m_BoundaryModel.GetWidth() / 2.0f);
	maxX2 = minX2 + m_BoundaryModel.GetWidth();
	minY2 = m_BoundaryModel.GetCentre().y - (m_BoundaryModel.GetHeight() / 2.0f);
	maxY2 = minY2 + m_BoundaryModel.GetHeight();
	minZ2 = m_BoundaryModel.GetCentre().z - (m_BoundaryModel.GetDepth() / 2.0f);
	maxZ2 = minZ2 + m_BoundaryModel.GetDepth();

	if (maxZ1 > maxZ2) //Right border
		m_playerPosition.z = maxZ2 - (m_PlayerModel.GetDepth() / 2.0f);
	if (minZ1 < minZ2) //Left border
		m_playerPosition.z = minZ2 + (m_PlayerModel.GetDepth() / 2.0f);
	if (maxX1 > maxX2) //Top border
		m_playerPosition.x = maxX2 - (m_PlayerModel.GetWidth() / 2.0f);
	if (minX1 < minX2) //Bottom border
		m_playerPosition.x = minX2 + (m_PlayerModel.GetWidth() / 2.0f);
}

void Game::CheckCollisions(ModelClass player, ModelClass other)
{
	float minX1, maxX1, minY1, maxY1, minZ1, maxZ1;
	float minX2, maxX2, minY2, maxY2, minZ2, maxZ2;

	//Player Rectangle
	minX1 = player.GetCentre().x - (player.GetWidth() / 2.0f);
	maxX1 = minX1 + player.GetWidth();
	minY1 = player.GetCentre().y - (player.GetHeight() / 2.0f);
	maxY1 = minY1 + player.GetHeight();
	minZ1 = player.GetCentre().z - (player.GetDepth() / 2.0f);
	maxZ1 = minZ1 + player.GetDepth();

	//Other Rectangle
	minX2 = other.GetCentre().x - (other.GetWidth() / 2.0f);
	maxX2 = minX2 + other.GetWidth();
	minY2 = other.GetCentre().y - (other.GetHeight() / 2.0f);
	maxY2 = minY2 + other.GetHeight();
	minZ2 = other.GetCentre().z - (other.GetDepth() / 2.0f);
	maxZ2 = minZ2 + other.GetDepth();

	if ((minX1 <= maxX2 && maxX1 >= minX2) && (minY1 <= maxY2 && maxY1 >= minY2) && (minZ1 <= maxZ2 && maxZ1 >= minZ2)) //Collision Occurred
	{
		//...
	}	
}
*/
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//setup our player and boundary models
	m_Player = new Player(device);
	m_Background = new Boundary(device, 10, 10, L"blackBG.dds"); //DEBUG (background)
	m_MapGen = MapGenerator::GetInstance(device);

	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"./light_vs.cso", L"./light_ps.cso");

	//load Textures
	CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"grass.dds", nullptr,	m_texture2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"rock.dds", nullptr,	m_texture3.ReleaseAndGetAddressOf());

	//initialise Render to texture
	m_FirstRenderPass = new RenderTexture(device, 800, 600, 1, 2);	//for our rendering, we dont use the last two properties, but they can't be zero and they can't be the same. 
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );
}

void Game::SetupGUI()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("GUI Parameters");
		ImGui::SliderFloat("PlayerX", m_Player->GetPositionX(), -3.0f, 3.0f);
		ImGui::SliderFloat("PlayerY", m_Player->GetPositionY(), -3.0f, 3.0f);
	ImGui::End();
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}
#pragma endregion
