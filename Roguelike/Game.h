#pragma once
#include "DeviceResources.h"
#include "StepTimer.h"
#include "Shader.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "Player.h"
#include "CollisionDetection.h"
#include "MapGenerator.h"
#include "Grid.h"
#include "Treasure.h"
#include "PostProcess.h"

class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);
#ifdef DXTK_AUDIO
    void NewAudioDevice();
#endif

    // Properties
    void GetDefaultSize( int& width, int& height ) const;
	
private:

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	}; 

    void Update(DX::StepTimer const& timer);
    void Render();
    void Clear();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
	void SetupGUI();
	void TimeFormat();
	void RenderTexturePass1();
	void PostProcess();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	//input manager. 
	Input									m_input;
	InputCommands							m_gameInputCommands;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;	
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;
    std::unique_ptr<DirectX::SpriteFont>                                    m_smallfont;
    std::unique_ptr<DirectX::SpriteFont>                                    m_bigfont;

	//Scene Objects
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	//Lights
	Light																	m_Light;

	//Cameras
	Camera																	m_Camera;
	Camera																	m_Camera1;
	bool																	m_cameraLock;
	bool																	m_showGrid;

	//Textures 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture3;

	//Shaders
	Shader																	m_BasicShaderPair;
	ID3D11BlendState*														m_PlayerBlendState;

	//Scene Objects
	Player*																	m_Player;
	Treasure*																m_Treasure;
	MapGenerator*															m_MapGen;
	Grid*																	m_Grid;

	//RenderTextures
	RenderTexture*															m_FirstRenderPass;
	RECT																	m_fullscreenRect;
	RECT																	m_CameraViewRect;
	
	//Auxiliary Variables
	bool																	levelComplete;
	bool																	enableMinimap;
	int																		record;
	std::wstring															debugLine;
	std::wstring															winLine;
	std::wstring															winSubLine;
	std::wstring															record_text;

#ifdef DXTK_AUDIO
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
#endif
    

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

	//Matrices
    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_view1;
    DirectX::SimpleMath::Matrix                                             m_projection;

	//Time Variables
	float																	dt;
	float																	gt;
	int																		fps;
	std::wstring															fps_text;
	std::wstring															gt_text;

	//Mouse State
	DirectX::SimpleMath::Vector2											lastFrameCursorPos;
	POINT																	mousePos;
	float																	mouseSensitivity;

	//Post Processing
	bool																	enablePostProcess;
	int																		postProcessType;
	std::unique_ptr<DirectX::BasicPostProcess>								m_postprocess;
	RenderTexture*															m_RenderTexture;
};