#include "GameEngine.h"

GameEngine::GameEngine(void)
{
	// Init or NULL objects before use to avoid any undefined behavior
	m_bVsync		= false;
	m_pD3DObject	= 0;
	m_pD3DDevice	= 0;
	m_currTime		= 0;
	m_prevTime		= 0;
	m_FPS			= 0;
	m_deltaTime		= 0;
	m_gameState		= 0;
}

GameEngine::~GameEngine(){
	Shutdown();
};

void GameEngine::Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed)
{
	m_hWnd = hWnd;

	//////////////////////////////////////////////////////////////////////////
	// Direct3D Foundations - D3D Object, Present Parameters, and D3D Device
	//////////////////////////////////////////////////////////////////////////

	// Create the D3D Object
	m_pD3DObject = Direct3DCreate9(D3D_SDK_VERSION);

	// Find the width and height of window using hWnd and GetWindowRect()
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Set D3D Device presentation parameters before creating the device
	D3DPRESENT_PARAMETERS D3Dpp;
	ZeroMemory(&D3Dpp, sizeof(D3Dpp));  // NULL the structure's memory

	D3Dpp.hDeviceWindow					= hWnd;										// Handle to the focus window
	D3Dpp.Windowed						= bWindowed;								// Windowed or Full-screen boolean
	D3Dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;								// Format of depth/stencil buffer, 24 bit depth, 8 bit stencil
	D3Dpp.EnableAutoDepthStencil		= TRUE;										// Enables Z-Buffer (Depth Buffer)
	D3Dpp.BackBufferCount				= 1;										// Change if need of > 1 is required at a later date
	D3Dpp.BackBufferFormat				= D3DFMT_X8R8G8B8;							// Back-buffer format, 8 bits for each pixel
	D3Dpp.BackBufferHeight				= height;									// Make sure resolution is supported, use adapter modes
	D3Dpp.BackBufferWidth				= width;									// (Same as above)
	D3Dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;					// Discard back-buffer, must stay discard to support multi-sample
	D3Dpp.PresentationInterval			= m_bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE; // Present back-buffer immediately, unless V-Sync is on								
	D3Dpp.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;		// This flag should improve performance, if not set to NULL.
	D3Dpp.FullScreen_RefreshRateInHz	= bWindowed ? 0 : D3DPRESENT_RATE_DEFAULT;	// Full-screen refresh rate, use adapter modes or default
	D3Dpp.MultiSampleQuality			= 0;										// MSAA currently off, check documentation for support.
	D3Dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;						// MSAA currently off, check documentation for support.

	// Check device capabilities
	DWORD deviceBehaviorFlags = 0;
	m_pD3DObject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_D3DCaps);

	// Determine vertex processing mode
	if(m_D3DCaps.DevCaps & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		// Hardware vertex processing supported? (Video Card)
		deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;	
	}
	else
	{
		// If not, use software (CPU)
		deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; 
	}

	// If hardware vertex processing is on, check pure device support
	if(m_D3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE && deviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;	
	}

	// Create the D3D Device with the present parameters and device flags above
	m_pD3DObject->CreateDevice(
		D3DADAPTER_DEFAULT,		// which adapter to use, set to primary
		D3DDEVTYPE_HAL,			// device type to use, set to hardware rasterization
		hWnd,					// handle to the focus window
		deviceBehaviorFlags,	// behavior flags
		&D3Dpp,					// presentation parameters
		&m_pD3DDevice);			// returned device pointer

	//////////////////////////////////////////////////////////////////////////
	// Create a Font Object
	//////////////////////////////////////////////////////////////////////////

	// Load a font for private use for this process
	AddFontResourceEx(L"Old English Text MT.otf", FR_PRIVATE, 0);

	// Load D3DXFont, each font style you want to support will need an ID3DXFont
	D3DXCreateFont(m_pD3DDevice, 30, 0, FW_BOLD, 0, false, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Old English Text MT"), &m_pD3DFont);

	//////////////////////////////////////////////////////////////////////////
	// Create Sprite Object and Textures
	//////////////////////////////////////////////////////////////////////////

	// Create a sprite object, note you will only need one for all 2D sprites
	D3DXCreateSprite(m_pD3DDevice, &m_pD3DSprite);

	// Create a texture, each different 2D sprite to display to the screen
	// will need a new texture object.  If drawing the same sprite texture
	// multiple times, just call that sprite's Draw() with different 
	// transformation values.
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Dojo.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_battleBackgroundOneInfo, 0, &m_battleBackgroundOne);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"GamePiece.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_gamePieceInfo, 0, &m_gamePiece);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Archer.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_archerCharacterInfo, 0, &m_archerCharacter);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"BlackMage.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_blackMageCharacterInfo, 0, &m_blackMageCharacter);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"PlayerUIBackground - Copy.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_playerUIBackgroundInfo, 0, &m_playerUIBackground);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"GoldMine.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_goldMineInfo, 0, &m_goldMine);
	// Seed rand() with time
	srand(timeGetTime());

	//////////////////////////////////////////////////////////////////////////
	// Initialize DirectInput
	//////////////////////////////////////////////////////////////////////////

	// Create the DI Object
	DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDIObject, NULL);

	// Initialize Keyboard
	m_pDIObject->CreateDevice(GUID_SysKeyboard, &m_pDIKeyboard, NULL);

	// Initialize Mouse
	m_pDIObject->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);

	// Set up Keyboard
	m_pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);

	// Set up Mouse (c_dfDIMouse2 = 8 button mouse)
	m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pDIMouse->SetDataFormat(&c_dfDIMouse2);

	//  Set up Audio
	FMOD::System_Create(&fmodSystem);
	fmodSystem->init(10, FMOD_INIT_NORMAL, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  TODO:  Get streaming sound working
	//fmodSystem->createStream("TMNT_Theme.mp3", FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE, 0, &themeMusic);
	//fmodSystem->createSound("Thwack.mp3", FMOD_DEFAULT, 0, &thwack);
	//fmodSystem->createSound("collide.wav", FMOD_DEFAULT, 0, &collide);
	//fmodSystem->createSound("sizzle.mp3", FMOD_DEFAULT, 0, &sizzle);
	//fmodSystem->createStream // for streaming audio
	for(int i = 0; i < 255; ++i){
		keyIsDown[i] = false;
	};
	start = (time(0));
	m_frameTime = 0;
	m_deltaTime = 0;
	cursor.x = 500;
	cursor.y = 300;
	cursor.z = 0;
	//	if(!themeMusicChannel)
	//{
	//	channel->stop();
	//	fmodSystem->playSound( FMOD_CHANNEL_FREE, themeMusic, false, &themeMusicChannel);
	//}

	//////////////////////////////////////////////
	//  INFO:  Inits both players
	m_player[0].setActivePlayer(true);
	m_player[0].setPlayerNumber(0);
	m_player[1].setPlayerNumber(1);
	m_player[0].setCharacterType(ARCHER);
	m_player[1].setCharacterType(BLACKMAGE);

	///////////////////////////////////////////////////
	//  INFO:  Initializes gameboard and gold mines
	InitGameBoard();
	////////////////////////////////////////////////////////
	//  INFO:  Changed the gamestate manually for testing
	m_gameState = BATTLE;
}

void GameEngine::InitGameBoard(){
	for(int i = 0; i < MAXBOARDHEIGHT; ++i){
		for(int j = 0; j < MAXBOARDWIDTH; ++j){
			m_gameBoard[i][j].setPosX(75 + j * 43);
			m_gameBoard[i][j].setPosY(190 + i * 50);
			m_gameBoard[i][j].setSpaceNumber( i + j );
			//////////////////////////////////////////////
			//  INFO:  Sets the end game spaces to be occupied by gold mines
			if(j == 0 || j == MAXBOARDWIDTH-1){
				m_gameBoard[i][j].setOccupiedBy(GOLDMINES);
			}
		}
	}
};

void GameEngine::Update(float dt)
{

	//////////////////////////////////////////////////////////////////////////
	// Get and Acquire Keyboard Input
	//////////////////////////////////////////////////////////////////////////

	// buffer - Stores our keyboard device state
	char buffer[256];
	ZeroMemory(buffer, sizeof(buffer));

	// Get the input device state
	HRESULT hr;
	hr = m_pDIKeyboard->GetDeviceState( sizeof(buffer), (LPVOID)&buffer );

	if(FAILED(hr))
	{
		hr = m_pDIKeyboard->Acquire();

		// Device has probably been lost if failed, if so keep trying to get it until it’s found.
		while( hr == DIERR_INPUTLOST)
		{
			hr = m_pDIKeyboard->Acquire();
		}

		// If we failed for some other reason
		if(FAILED(hr))
			return;

		// Read the device state again
		m_pDIKeyboard->GetDeviceState(sizeof(buffer), buffer);
	}

	//////////////////////////////////////////////////////////////////////////
	// Get and Acquire Mouse Input
	//////////////////////////////////////////////////////////////////////////
	// Stores our mouse state for an 8 button mouse.
	DIMOUSESTATE2 mouseState;
	ZeroMemory(&mouseState, sizeof(mouseState));

	// Get the input device state
	hr = m_pDIMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &mouseState );
	if(FAILED(hr))
	{
		hr = m_pDIMouse->Acquire();

		// Device has probably been lost if failed, if so keep trying to get it until it’s found.
		while( hr == DIERR_INPUTLOST)
		{
			hr = m_pDIMouse->Acquire();
		}

		// If we failed for some other reason
		if(FAILED(hr))
			return;
		// Read the device state again
		m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);
	}

	//////////////////////////////////////////////////////////////////////////
	//	Keyboard Code Examples: [DIK (DirectInput Key) codes we translate]
	//	DIK_0 – DIK_9
	//	DIK_NUMPAD0 – DIK_NUMPAD9
	//	DIK_A – DIK_Z
	//	DIK_F1 – DIK_F12
	//	DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT	// Arrow Keys
	//	DIK_SPACE, DIK_TAB, DIK_CAPITAL, DIK_LCONTROL (Left Ctrl Key), 
	//  DIK_RCONTROL (Right Ctrl Key), DIK_RETURN, DIK_LMENU (Left Alt Key), 
	//  DIK_LWIN (Left Windows Key), DIK_LSHIFT (Left Shift Key), etc.
	//	Complete list under Keyboard Device in the documentation.
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//	Mouse variables:  [MouseState2 structure supports 8 button mice]
	//	lX				-	X-axis mouse relative coordinates
	//	lY				-	Y-axis mouse relative coordinates
	//	lZ				-	Mouse wheel relative coordinates
	//	rgbButtons[8]	-	Array of 8 mouse buttons
	//
	//	Usually mouse hardware maps the button layout in a standard way for 
	//	the first 4 buttons, after that it depends on the mouse hardware layout
	//	rgbButtons[0]	-	Left Mouse Button
	//	rgbButtons[1]	-	Right Mouse Button
	//	rgbButtons[2]	-	Middle Mouse Button (click scroll wheel)
	//	rgbButtons[3]	-	Side Mouse Button 1
	//	rgbButtons[4]	-	Side Mouse Button 2
	//	rgbButtons[5]	-	Side Mouse Button 3
	//	rgbButtons[6]	-	Side Mouse Button 4
	//	rgbButtons[7]	-	Side Mouse Button 5
	//////////////////////////////////////////////////////////////////////////


	calcDeltaTime();



	fmodSystem->update();
}

void GameEngine::Render()
{
	// If the device was not created successfully, return
	if(!m_pD3DDevice)
		return;

	//////////////////////////////////////////////////////////////////////////
	// All draw calls between swap chain's functions, and pre-render and post- 
	// render functions (Clear and Present, BeginScene and EndScene)
	//////////////////////////////////////////////////////////////////////////

	// Clear the back buffer, call BeginScene()
	if(SUCCEEDED(m_pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DXCOLOR(0.0f, 0.4f, 0.8f, 1.0f), 1.0f, 0)))
	{
		if(SUCCEEDED(m_pD3DDevice->BeginScene()))
		{
			// Call Sprite's Begin to start rendering 2D sprite objects
			if(SUCCEEDED(m_pD3DSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK)))
			{
				///////////////////////////////////////////////////
				//  INFO:  For drawing graphics
				switch(m_gameState){
				case MENUMAIN:
					break;
				case MENUCHARACTERSELECT:
					break;
				case MENUCREDITS:
					break;
				case OVERWORLD:
					break;
				case BATTLE:
					drawBackground();
					drawGameBoard();
					drawPlayers();
					break;
				}
				m_pD3DSprite->End();
			}
			/////////////////////////////////////////////
			//  INFO:  For drawing text
			switch(m_gameState){
			case MENUMAIN:
				break;
			case MENUCHARACTERSELECT:
				break;
			case MENUCREDITS:
				break;
			case OVERWORLD:
				break;
			case BATTLE:
				drawUIText();
				break;
			}

				m_pD3DDevice->EndScene();
		}
		m_pD3DDevice->Present(0, 0, 0, 0);
	}

	// Calculate Frames per Second
	m_currTime = (float)timeGetTime();
	static int fpsCounter = 0;
	if(m_currTime - m_prevTime >= 1000.0f)
	{
		m_prevTime = m_currTime;
		m_FPS = fpsCounter;
		fpsCounter = 0;		
	}
	else
	{
		++fpsCounter;
	}

}

void GameEngine::Shutdown()
{
	// Release COM objects in the opposite order they were created in
	SAFE_RELEASE(m_goldMine);
	SAFE_RELEASE(m_playerUIBackground);
	SAFE_RELEASE(m_blackMageCharacter);
	SAFE_RELEASE(m_archerCharacter);
	SAFE_RELEASE(m_gamePiece);
	SAFE_RELEASE(m_battleBackgroundOne);


	SAFE_RELEASE(m_pD3DFont);
	RemoveFontResourceEx(L"Old English Text MT.ttf", FR_PRIVATE, 0);

	SAFE_RELEASE(m_pD3DDevice);
	SAFE_RELEASE(m_pD3DObject);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Releases Audio
	fmodSystem->release();

}

void GameEngine::drawBackground(){
	//////////////////////////////////////////////////////////////////////////
	// Matrix Transformations to control sprite position, scale, and rotate
	// Set these matrices for each object you want to render to the screen
	//////////////////////////////////////////////////////////////////////////
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);

	D3DXMatrixScaling(&scaleMat, 1.0f, 3.3f, 0.0f);			// Scaling
	//D3DXMatrixRotationZ(&rotMat, D3DXToRadian(90.0f));		// Rotation on Z axis, value in radians, converting from degrees
	D3DXMatrixTranslation(&transMat, 220, -200, 0.0f);			// Translation
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

	// Set Transform
	m_pD3DSprite->SetTransform(&worldMat);


	m_pD3DSprite->Draw(m_battleBackgroundOne, 0, &D3DXVECTOR3(m_battleBackgroundOneInfo.Width * 0.5f, m_battleBackgroundOneInfo.Height * 0.5f, 0.0f),
		0, D3DCOLOR_ARGB(255, 255, 255, 255));

};



void GameEngine::drawGameBoard(){
	//////////////////////////////////////////////////////////////////////////
	// INFO:  Draws gameboard
	//////////////////////////////////////////////////////////////////////////
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
	for(int i = 0; i < MAXBOARDHEIGHT; ++i){
		for(int j = 0; j < MAXBOARDWIDTH; ++j){
			D3DXMatrixIdentity(&transMat);
			D3DXMatrixIdentity(&scaleMat);
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixIdentity(&worldMat);

			D3DXMatrixScaling(&scaleMat, 0.7f, 0.80f, 0.0f);			// Scaling
			//D3DXMatrixRotationZ(&rotMat, D3DXToRadian(90.0f));		// Rotation on Z axis, value in radians, converting from degrees
			D3DXMatrixTranslation(&transMat, m_gameBoard[i][j].getPosX(), m_gameBoard[i][j].getPosY(), 0.0f);			// Translation
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

			// Set Transform
			m_pD3DSprite->SetTransform(&worldMat);


			m_pD3DSprite->Draw(m_gamePiece, 0, &D3DXVECTOR3(m_gamePieceInfo.Width * 0.5f, m_gamePieceInfo.Height * 0.5f, 0.0f),
				0, D3DCOLOR_ARGB(255, 255, 255, 255));


			//////////////////////////////////////////////////////////////////////////////////
			//  INFO:  If the gamespace is occupied by a unit, draw that unit.  
			switch(m_gameBoard[i][j].getOccupiedBy()){
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);
			case NOUNIT:
				break;
			case GOLDMINES:
				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_gameBoard[i][j].getPosX()+4, m_gameBoard[i][j].getPosY(), 0.0f);			// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				// Set Transform
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_goldMine, 0, &D3DXVECTOR3(m_goldMineInfo.Width * 0.5f, m_goldMineInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				break;
			case WALL:
				break;
			case WARRIORUNIT:
				break;
			case MARKSMAN:
				break;
			case CAVALRY:
				break;
			case WOLF:
				break;
			case ARCHERUNIT:
				break;
			case THIEF:
				break;
			case GOLEM:
				break;
			case BLACKMAGEUNIT:
				break;
			case WARLOCK:
				break;
			}
			//D3DXMatrixScaling(&scaleMat, 0.7f, 0.80f, 0.0f);			// Scaling
			////D3DXMatrixRotationZ(&rotMat, D3DXToRadian(90.0f));		// Rotation on Z axis, value in radians, converting from degrees
			//D3DXMatrixTranslation(&transMat, 75 + (j * 43), 190 + i * 50, 0.0f);			// Translation
			//D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
			//D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

			//// Set Transform
			//m_pD3DSprite->SetTransform(&worldMat);


			//m_pD3DSprite->Draw(m_gamePiece, 0, &D3DXVECTOR3(m_gamePieceInfo.Width * 0.5f, m_gamePieceInfo.Height * 0.5f, 0.0f),
			//	0, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
};

void GameEngine::drawPlayers(){
	//////////////////////////////////////////////////////////////////////////
	// Matrix Transformations to control sprite position, scale, and rotate
	// Set these matrices for each object you want to render to the screen
	//////////////////////////////////////////////////////////////////////////
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
	int invertImage = 1;

	for(int i = 0; i < 2; ++i){
	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);
	D3DXMatrixScaling(&scaleMat, 1.60f, 0.35f, 0.0f);			// Scaling
	//D3DXMatrixRotationZ(&rotMat, D3DXToRadian(90.0f));		// Rotation on Z axis, value in radians, converting from degrees
	D3DXMatrixTranslation(&transMat, 210, 48 + 425 * i, 0.0f);			// Translation
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

	// Set Transform
	m_pD3DSprite->SetTransform(&worldMat);


	m_pD3DSprite->Draw(m_playerUIBackground, 0, &D3DXVECTOR3(m_playerUIBackgroundInfo.Width * 0.5f, m_playerUIBackgroundInfo.Height * 0.5f, 0.0f),
		0, D3DCOLOR_ARGB(255, 255, 255, 255));

	////////////////////////////////////////////
	//  INFO:  Offset for drawing characters on opposite sides of the screen
	int offset = 0;

	switch(m_player[i].getCharacterType()){
		////////////////////////////////////////
		//  INFO:  Clear Matrices after drawing character backgrounds
		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);
	case WARRIOR:
		offset = 0;
		//D3DXMatrixScaling(&scaleMat, 1.0f, 1.0f, 0.0f);			// Scaling
		////D3DXMatrixRotationZ(&rotMat, D3DXToRadian(90.0f));		// Rotation on Z axis, value in radians, converting from degrees
		//D3DXMatrixTranslation(&transMat, 120, 200, 0.0f);			// Translation
		//D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		//D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

		//// Set Transform
		//m_pD3DSprite->SetTransform(&worldMat);


		//m_pD3DSprite->Draw(m_, 0, &D3DXVECTOR3(m_battleBackgroundOneInfo.Width * 0.5f, m_battleBackgroundOneInfo.Height * 0.5f, 0.0f),
		//	0, D3DCOLOR_ARGB(255, 255, 255, 255));
		break;
	case ARCHER:
		/////////////////////////////////////////////////////////////////////
		//  TODO:  Sloppy way to handle this, but it technically works.  Find a better way later
		//  INFO:  Creates an offset to draw the player appropriately on both sides of the screen
		offset = 0;
		if(m_player[i].getPlayerNumber() == 1){
			offset = 140;
		}
		D3DXMatrixScaling(&scaleMat, 0.10f * invertImage, 0.075f, 0.0f);			// Scaling
		//D3DXMatrixRotationZ(&rotMat, D3DXToRadian(90.0f));		// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat, 75 + offset + 500 * i, 60, 0.0f);			// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

		// Set Transform
		m_pD3DSprite->SetTransform(&worldMat);


		m_pD3DSprite->Draw(m_archerCharacter, 0, &D3DXVECTOR3(m_archerCharacterInfo.Width * 0.5f, m_archerCharacterInfo.Height * 0.5f, 0.0f),
			0, D3DCOLOR_ARGB(255, 255, 255, 255));
		break;
	case BLACKMAGE:
		/////////////////////////////////////////////////////////////////////
		//  TODO:  Sloppy way to handle this, but it technically works.  Find a better way later
		//  INFO:  Creates an offset to draw the player appropriately on both sides of the screen
		offset = 0;
		if(m_player[i].getPlayerNumber() == 1){
			offset = 90;
		}
		D3DXMatrixScaling(&scaleMat, 0.15f * invertImage, 0.15f, 0.0f);			// Scaling
		//D3DXMatrixRotationZ(&rotMat, D3DXToRadian(90.0f));		// Rotation on Z axis, value in radians, converting from degrees
		D3DXMatrixTranslation(&transMat, 105 + offset + 500 * i, 80, 0.0f);			// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

		// Set Transform
		m_pD3DSprite->SetTransform(&worldMat);


		m_pD3DSprite->Draw(m_blackMageCharacter, 0, &D3DXVECTOR3(m_blackMageCharacterInfo.Width * 0.5f, m_blackMageCharacterInfo.Height * 0.5f, 0.0f),
			0, D3DCOLOR_ARGB(255, 255, 255, 255));
		break;
	case WHITEMAGE:
		break;
	}
	invertImage = -1;
	}
};

void GameEngine::drawUIText(){
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	rect.right = rect.right - rect.left;
	rect.bottom = rect.bottom - rect.top;
	rect.top = 40;			
	rect.left = 170;
	wchar_t buffer[128];

	for(int i = 0; i < 2; ++i){
		rect.left += i * 250;
		rect.right += i * 250;
		swprintf_s(buffer, 128, L"Health:   %d \ %d\nSpecial:  %d / %d\nGold:      %d", m_player[i].getCurrentHealth(), m_player[i].getMaxHealth(), m_player[i].getCurrentSpecial(), m_player[i].getMaxSpecial(), m_player[i].getGold());
		m_pD3DFont->DrawText(0, buffer, -1, &rect, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
	}
	rect.top = 470;
	rect.bottom = 575;
	rect.left += 250;
	rect.right += 250;
	swprintf_s(buffer, 128, L"Units\n\nAbilities");
	m_pD3DFont->DrawText(0, buffer, -1, &rect, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
	rect.left -= 620;
	swprintf_s(buffer, 128, L"Units\n\nAbilities");
	m_pD3DFont->DrawText(0, buffer, -1, &rect, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));

};

void GameEngine::calcDeltaTime(){
	m_deltaTime = difftime(time(0), start);
};
