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

	m_fullscreenRect.left = 10;
	m_fullscreenRect.top = 50;
	m_fullscreenRect.right = 510;
	m_fullscreenRect.bottom = 350;

	m_CameraViewRect.left = 10 * (m_deviceResources->GetWindowWidth() / 1920.0f);
	m_CameraViewRect.top = 50 * (m_deviceResources->GetWindowHeight() / 1080.0f);
	m_CameraViewRect.right = 510 * (m_deviceResources->GetWindowWidth() / 1920.0f);
	m_CameraViewRect.bottom = 350 * (m_deviceResources->GetWindowHeight() / 1080.0f);

	//setup light
	m_Light.setAmbientColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
//	m_Light.setPosition(2.0f, 1.0f, 1.0f);
//	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	//setup camera
	m_Camera.setPosition(Vector3(0.0f, 0.0f, 5.0f));
	m_Camera.setRotation(Vector3(-90.0f, -270.0f, 0.0f));
	m_cameraLock = true;

	//setup camera1
	m_Camera1.setPosition(Vector3(96.8f, -24.5f, 78.0f));
	m_Camera1.setRotation(Vector3(-90.0f, -270.0f, 0.0f));
	
	//initialise auxiliary variables
	fps = 0;
	dt = 0;
	gt = 0;
	record = 0;
	mouseSensitivity = 5.0f;
	levelComplete = false;
	m_showGrid = false;
	enableMinimap = false;
	record_text = L"Cleared Levels: " + std::to_wstring(record);
	winLine = L"LEVEL COMPLETED!";
	winSubLine = L"Press 'R' to generate a new level";

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
	//Update delta time, game time and fps
	dt = float(timer.GetElapsedSeconds());
	if (!levelComplete)
		gt += dt;
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
	if (m_gameInputCommands.resetLevel && levelComplete)
	{
		m_MapGen->ResetLevel();
		m_Grid->InitialiseGrid();
		m_Player->SetPosition(m_Grid->GetPlayerPos());
		m_Treasure->SetPosition(m_Grid->GetTreasurePos());
		m_Treasure->SetActive(true);
		levelComplete = false;
		gt = 0;
		record++;
		record_text = L"Cleared Levels: " + std::to_wstring(record);
	}
	//Camera forward and back movement
	if (m_gameInputCommands.forward && !m_cameraLock)
	{
		Vector3 position = m_Camera.getPosition();
		position += (m_Camera.getForward() * m_Camera.getMoveSpeed() * mouseSensitivity * 2 * dt);
		m_Camera.setPosition(position);
	}
	else if (m_gameInputCommands.back && !m_cameraLock)
	{
		Vector3 position = m_Camera.getPosition();
		position -= (m_Camera.getForward() * m_Camera.getMoveSpeed() * mouseSensitivity * 2 * dt);
		m_Camera.setPosition(position);
	}

	//Player commands
	if (!levelComplete)
	{
		Vector3 inputPos = m_Player->GetPosition();
		if (m_gameInputCommands.right)
		{
			inputPos.x += m_Player->GetSpeed() * dt;
			m_Player->FlipSprite(true);
		}
		if (m_gameInputCommands.left)
		{
			inputPos.x -= m_Player->GetSpeed() * dt;
			m_Player->FlipSprite(false);
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
	}

	//Check for collisions
	m_Player->CheckBoundaries(m_Grid->GetMatrix());
	if (CollisionDetection::SAT(m_Player, m_Treasure))
	{
		m_Treasure->SetActive(false);
		levelComplete = true;
	}
	
	//Update Cameras and World Matrices
	m_Camera.setPosition(m_Player->GetPosition().x, m_Player->GetPosition().y, m_Camera.getPosition().z);
	m_Camera.Update();
	m_view = m_Camera.getCameraMatrix();
	m_Camera1.Update();
	m_view1 = m_Camera1.getCameraMatrix();
	m_world = Matrix::Identity;
	lastFrameCursorPos = Vector2(mousePos.x, mousePos.y);

	//Create our UI
	SetupGUI();
	TimeFormat();

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

	// Create our render to texture.
	RenderTexturePass1();

	// Generated Map
	// Corridors
	std::vector<Corridor*>* hallsList = m_MapGen->GetHallsList();
	for (std::vector<Corridor*>::iterator it = hallsList->begin(); it != hallsList->end(); it++)
	{
		Corridor* hall = *it;
		m_world = SimpleMath::Matrix::Identity;
		SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(hall->GetPosition());
		m_world = m_world * newPosition;

		m_BasicShaderPair.EnableShader(context);
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, hall->GetTexture());
		hall->Render(context);
	}

	// Rooms
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

	// Grid
	if (m_showGrid)
	{
		context->RSSetState(m_states->Wireframe());
		ModelClass cell = m_Grid->GetCell();
		int** matrix = m_Grid->GetMatrix();
		for (int i = 0; i < MAP_WIDTH; i++)
			for (int j = 0; j < MAP_HEIGHT; j++)
			{
				if (matrix[i][j] != EMPTY)
					continue;

				m_world = SimpleMath::Matrix::Identity;
				//Since every rectangle is shifted by width/2 and height/2, even the grid needs to be offset by 0.5f (which is 1.0f/2.0f)
				SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(i + 0.5f, j + 0.5f, 0);
				m_world = m_world * newPosition;

				m_BasicShaderPair.EnableShader(context);
				m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get());
				cell.Render(context);
			}
	}

	// Treasure
	context->RSSetState(m_states->CullClockwise());
	if (m_Treasure->IsActive())
	{
		m_world = SimpleMath::Matrix::Identity;
		SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(m_Treasure->GetPosition());
		m_world = m_world * newPosition;

		m_BasicShaderPair.EnableShader(context);
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_Treasure->GetTexture());
		m_Treasure->Render(context);
	}

	// Player
	context->OMSetBlendState(m_PlayerBlendState, 0, 0xFFFFFFFF);
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(m_Player->GetPosition());
	m_world = m_world * newPosition;

	m_BasicShaderPair.EnableShader(context);
	if (m_Player->IsFlipped())
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_Player->GetTextureFlipped());
	else
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_Player->GetTexture());
	m_Player->Render(context);

	// Draw Text to the screen
	m_sprites->Begin();
	m_font->DrawString(m_sprites.get(), L"Advanced Procedural Methods - Roguelike", XMFLOAT2(10, 10), Colors::LightBlue);
	m_font->DrawString(m_sprites.get(), fps_text.c_str(), XMFLOAT2(m_deviceResources->GetWindowWidth() - 110, 10), Colors::LightBlue);
	m_font->DrawString(m_sprites.get(), gt_text.c_str(), XMFLOAT2(10, m_deviceResources->GetWindowHeight() - 40), Colors::LightBlue);
	m_font->DrawString(m_sprites.get(), record_text.c_str(), XMFLOAT2(m_deviceResources->GetWindowWidth() - 205, m_deviceResources->GetWindowHeight() - 40), Colors::LightBlue);
	if (levelComplete)
	{
		m_bigfont->DrawString(m_sprites.get(), winLine.c_str(), Vector2(m_deviceResources->GetWindowWidth() / 2, (m_deviceResources->GetWindowHeight() - 40) / 2), Colors::CornflowerBlue, 0.0f, m_bigfont->MeasureString(winLine.c_str()) / 2.0f);
		m_smallfont->DrawString(m_sprites.get(), winSubLine.c_str(), Vector2(m_deviceResources->GetWindowWidth() / 2, (m_deviceResources->GetWindowHeight() + 80) / 2), Colors::CornflowerBlue, 0.0f, m_smallfont->MeasureString(winSubLine.c_str()) / 2.0f);
	}
	m_sprites->End();

	// Draw our sprite with the render texture displayed on it.
	if (enableMinimap)
	{
		// Render on sprite
		m_sprites->Begin();
		m_sprites->Draw(m_FirstRenderPass->getShaderResourceView(), m_fullscreenRect);
		m_sprites->End();
	}

	// Render our GUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
//	ImGui::EndFrame();

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

    context->ClearRenderTargetView(renderTarget, Colors::Black);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
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
    width = 1920;
    height = 1080;
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
    m_smallfont = std::make_unique<SpriteFont>(device, L"SmallFont.spritefont");
    m_bigfont = std::make_unique<SpriteFont>(device, L"BigFont.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//setup our player and boundary models
	m_MapGen = MapGenerator::GetInstance(device);
	m_Grid = new Grid(device);
	m_Player = new Player(device, m_Grid->GetPlayerPos());
	m_Treasure = new Treasure(device, m_Grid->GetTreasurePos());

	//setup player custom blend state
	D3D11_BLEND_DESC omDesc;
	ZeroMemory(&omDesc,	sizeof(D3D11_BLEND_DESC));
	omDesc.RenderTarget[0].BlendEnable = true;
	omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&omDesc, &m_PlayerBlendState);
	
	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"./light_vs.cso", L"./light_ps.cso");

	//load Textures
	CreateDDSTextureFromFile(device, L"grayBG.dds", nullptr, m_texture1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"greenShape.dds", nullptr, m_texture2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"yellowBG.dds", nullptr, m_texture3.ReleaseAndGetAddressOf());

	//initialise Render to texture
	m_FirstRenderPass = new RenderTexture(device, 500, 300, 1, 2); //for our rendering, we dont use the last two properties, but they can't be zero and they can't be the same. 
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
        1000.0f
    );
}

void Game::SetupGUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
//		ImGui::SliderFloat("PlayerX", m_Player->GetPositionX(), 0, MAP_WIDTH);
//		ImGui::SliderFloat("PlayerY", m_Player->GetPositionY(), 0, MAP_HEIGHT);
		if (ImGui::Button("Toggle Grid View", ImVec2(200, 40)))
			m_showGrid = !m_showGrid;
		if (ImGui::Button("Toggle Camera Lock", ImVec2(200, 40)))
			m_cameraLock = !m_cameraLock;
		if (ImGui::Button("Toggle Minimap", ImVec2(200, 40)))
			enableMinimap = !enableMinimap;
	ImGui::End();
}

void Game::TimeFormat()
{
	std::wstring min_text, sec_text;
	float timeInSecond = gt;
	int time = (int)timeInSecond;

	int seconds = time % 60;
	int minutes = (time / 60) % 60;
	if (std::to_wstring(seconds).length() == 1)
		sec_text = L"0" + std::to_wstring(seconds);
	else
		sec_text = std::to_wstring(seconds);
	if (std::to_wstring(minutes).length() == 1)
		min_text = L"0" + std::to_wstring(minutes);
	else
		min_text = std::to_wstring(minutes);
	
	gt_text = L"Time: " + min_text + L":" + sec_text;
}

void Game::RenderTexturePass1()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();
	// Set the render target to be the render to texture.
	m_FirstRenderPass->setRenderTarget(context);
	// Clear the render to texture.
	m_FirstRenderPass->clearRenderTarget(context, 0.8f, 0.8f, 0.8f, 1.0f);

	// Render the scene using the secondary camera view
	// Corridors
	std::vector<Corridor*>* hallsList = m_MapGen->GetHallsList();
	for (std::vector<Corridor*>::iterator it = hallsList->begin(); it != hallsList->end(); it++)
	{
		Corridor* hall = *it;
		m_world = SimpleMath::Matrix::Identity;
		SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(hall->GetPosition());
		m_world = m_world * newPosition;

		m_BasicShaderPair.EnableShader(context);
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view1, &m_projection, &m_Light, hall->GetTexture());
		hall->Render(context);
	}

	// Rooms
	std::vector<Boundary*>* roomsList = m_MapGen->GetRoomsList();
	for (std::vector<Boundary*>::iterator it = roomsList->begin(); it != roomsList->end(); ++it)
	{
		Boundary* bound = *it;
		m_world = SimpleMath::Matrix::Identity;
		SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(bound->GetPosition());
		m_world = m_world * newPosition;

		m_BasicShaderPair.EnableShader(context);
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view1, &m_projection, &m_Light, bound->GetTexture());
		bound->Render(context);
	}

	// Treasure
	if (m_Treasure->IsActive())
	{
		m_world = SimpleMath::Matrix::Identity;
		SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(m_Treasure->GetPosition());
		m_world = m_world * newPosition;

		m_BasicShaderPair.EnableShader(context);
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view1, &m_projection, &m_Light, m_texture3.Get());
		m_Treasure->Render(context);
	}

	// Player
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(m_Player->GetPosition());
	m_world = m_world * newPosition;

	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view1, &m_projection, &m_Light, m_texture2.Get());
	m_Player->Render(context);

	// Reset the render target back to the original back buffer and not the render to texture anymore.	
	context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
    m_smallfont.reset();
    m_bigfont.reset();
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