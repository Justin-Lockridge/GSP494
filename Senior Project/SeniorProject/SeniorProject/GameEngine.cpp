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

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"cursor.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_cursorInfo, 0, &m_cursor);

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

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"ArcherAnimations.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_archerUnitInfo, 0, &m_archerUnit);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"BlackMageSheet.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_blackMageUnitInfo, 0, &m_blackMageUnit);

	/////////////////////////////////////////////////////
	// Icons: Units, abilities
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"ArcherIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_archerIconInfo, 0, &m_archerIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"BlackHoleIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_blackHoleIconInfo, 0, &m_blackHoleIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"BlackmageIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_blackMageIconInfo, 0, &m_blackMageIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"GolemIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_golemIconInfo, 0, &m_golemIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"GolemIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_golemIconInfo, 0, &m_golemIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"ThiefIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_thiefIconInfo, 0, &m_thiefIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"ThiefIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_thiefIconInfo, 0, &m_thiefIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WallIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_wallIconInfo, 0, &m_wallIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WolfIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_wolfIconInfo, 0, &m_wolfIcon);

	/////////////////////////////////////////////////////
	// Attacks
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Arrow.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_arrowInfo, 0, &m_arrow);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"fireball.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_fireballInfo, 0, &m_fireball);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"ArcherArrow.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_archerArrowInfo, 0, &m_archerArrow);

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
	m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	m_pDIMouse->SetDataFormat(&c_dfDIMouse2);

	//  Set up Audio
	FMOD::System_Create(&fmodSystem);
	fmodSystem->init(10, FMOD_INIT_NORMAL, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  TODO:  Get streaming sound working
	fmodSystem->createStream("BattleTheme.mp3", FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE, 0, &battleTheme);
	fmodSystem->createSound("HitMines.mp3", FMOD_DEFAULT, 0, &hitMines);
	fmodSystem->createSound("ArrowHit.wav", FMOD_DEFAULT, 0, &arrowHit);
	fmodSystem->createSound("CastFireball.mp3", FMOD_DEFAULT, 0, &castFireball);
	fmodSystem->createSound("ShootArrow.wav", FMOD_DEFAULT, 0, &shootArrow);
	fmodSystem->createSound("FireballHit.wav", FMOD_DEFAULT, 0, &fireballHit);
	//fmodSystem->createStream // for streaming audio
	for(int i = 0; i < 255; ++i){
		keyIsDown[i] = false;
	};
	start = (time(0));
	m_frameTime = 0;
	m_deltaTime = 0;

	cursor.x = 500;
	cursor.y = 300;

	myMouse.init();

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
	m_unitCurrentlyAttacking = false;
	m_fireballRotation = 0.0f;
	////////////////////////////////////////////////////////
	//  INFO:  Changed the gamestate manually for testing
	m_gameState = BATTLE;
	m_gameBoard[1][5].setOccupiedBy(ARCHERUNIT);
	m_gameBoard[1][5].setAnimationRect(15, 10, 70, 70);
	m_gameBoard[1][5].setAnimationTimer(1.34);
	m_gameBoard[1][5].setWhoUnitBelongsTo(PLAYERTWO);
	m_gameBoard[1][5].setUnitCanTakeAction(true);

	m_gameBoard[1][14].setOccupiedBy(ARCHERUNIT);
	m_gameBoard[1][14].setAnimationRect(15, 10, 70, 70);
	m_gameBoard[1][14].setAnimationTimer(0.25);
	m_gameBoard[1][14].setWhoUnitBelongsTo(PLAYERONE);
	m_gameBoard[1][14].setUnitCanTakeAction(true);

	m_gameBoard[3][9].setOccupiedBy(ARCHERUNIT);
	m_gameBoard[3][9].setAnimationRect(15, 10, 70, 70);
	m_gameBoard[3][9].setAnimationTimer(0.10);
	m_gameBoard[3][9].setWhoUnitBelongsTo(PLAYERONE);
	m_gameBoard[3][9].setUnitCanTakeAction(true);

	m_gameBoard[1][1].setOccupiedBy(BLACKMAGEUNIT);
	m_gameBoard[1][1].setAnimationRect(75, 5, 70, 145);
	m_gameBoard[1][1].setWhoUnitBelongsTo(PLAYERONE);
	m_gameBoard[1][1].setUnitCanTakeAction(true);

	m_gameBoard[2][6].setOccupiedBy(BLACKMAGEUNIT);
	m_gameBoard[2][6].setAnimationRect(75, 5, 70, 145);
	m_gameBoard[2][6].setWhoUnitBelongsTo(PLAYERONE);
	m_gameBoard[2][6].setUnitCanTakeAction(true);

}

void GameEngine::InitGameBoard(){
	for(int i = 0; i < MAXBOARDHEIGHT; ++i){
		for(int j = 0; j < MAXBOARDWIDTH; ++j){
			m_gameBoard[i][j].setPosX(75 + j * 43);
			m_gameBoard[i][j].setPosY(190 + i * 50);
			m_gameBoard[i][j].setSpaceNumber( i * MAXBOARDWIDTH + j );
			m_gameBoard[i][j].setWhoUnitBelongsTo(-1);
			//////////////////////////////////////////////
			//  INFO:  Sets the end game spaces to be occupied by gold mines
			if(j == 0 || j == MAXBOARDWIDTH-1){
				m_gameBoard[i][j].setOccupiedBy(GOLDMINES);
			}
			int testerX, testerY;
			if(i == 0 && j == 14){
				testerX = m_gameBoard[i][j].getPosX();
				testerY = m_gameBoard[i][j].getPosY();
				testerX += 0;
			}
		}
	}

	player1_units.clear();

	// T, L, B, R, X, Y, Highlighted
	RectData temp_units_pos[] =
	{
		{480, 120, 520, 160, 140, 500, false}, //unit 1
		{480, 170, 520, 210, 190, 500, false}, //unit 2
		{480, 220, 520, 260, 240, 500, false}, //unit 3
		{480, 270, 520, 310, 290, 500, false}, //unit 4
		{540, 120, 580, 160, 140, 560, false}, //ability 1
		{540, 170, 580, 210, 190, 560, false}, //ability 2
	};

	for(int i = 0; i < MAX_BATTLE_BUTTONS; i++)
	{
		Buttons temp;
		RECT rect;

		rect.top = temp_units_pos[i].t;
		rect.left = temp_units_pos[i].l;
		rect.bottom = temp_units_pos[i].b;
		rect.right = temp_units_pos[i].r;

		temp.setPosition(temp_units_pos[i].x, temp_units_pos[i].y);
		temp.setHighlight(temp_units_pos[i].highlight);
		temp.setRect(rect);

		player1_units.push_back(temp);		
	}

	player2_units.clear();

	RectData temp_units_pos2[] =
	{
		{480, 120, 520, 160, 640, 500, false}, //player 2 unit 1
		{480, 120, 520, 160, 590, 500, false}, //player 2 unit 2
		{480, 120, 520, 160, 540, 500, false}, //player 2 unit 3
		{480, 120, 520, 160, 490, 500, false}, //player 2 unit 4
		{480, 120, 520, 160, 640, 560, false}, //player 2 ability 1
		{480, 120, 520, 160, 590, 560, false}, //player 2 ability 2
	};

	for(int i = 0; i < MAX_BATTLE_BUTTONS; i++)
	{
		Buttons temp;
		RECT rect;

		rect.top = temp_units_pos2[i].t;
		rect.left = temp_units_pos2[i].l;
		rect.bottom = temp_units_pos2[i].b;
		rect.right = temp_units_pos2[i].r;

		temp.setPosition(temp_units_pos2[i].x, temp_units_pos2[i].y);
		temp.setHighlight(temp_units_pos2[i].highlight);
		temp.setRect(rect);

		player2_units.push_back(temp);		
	}

	//////////////////////////////////////////////////////////
	//  Reset m_gamePhase
	m_gamePhase						=		PLAYERONE_PLAYPHASE;
	m_fireBallActive				=		false;
	m_arrowActive					=		false;
	m_floatingTextActive			=		false;
	m_projectilePosX				=		0.0f;
	m_projectilePosY				=		0.0f;
	m_arrowForAttackingUnitPosX		=		0.0f;
	m_arrowForAttackingUnitPosY		=		0.0f;
	m_temporaryTimer				=		0.0f;
	m_attackingSpaceX				=		0;
	m_attackingSpaceY				=		0;
	m_attackTargetSpaceX			=		0;
	m_attackTargetSpaceY			=		0;
	m_floatingTextRect.top			=		0.1f;
	m_floatingTextRect.left			=		0.0f;
	m_floatingTextRect.right		=		0.0f;
	m_floatingTextRect.bottom		=		0.0f;
	m_floatingRectTopMax			=		0.0f;
	m_floatingRectTimer				=		0.0f;
	////////////////////////////////////////////////////
	//  INFO:  Set ownership of right side goldmines
	m_gameBoard[0][15].setWhoUnitBelongsTo(PLAYERTWO);
	m_gameBoard[1][15].setWhoUnitBelongsTo(PLAYERTWO);
	m_gameBoard[2][15].setWhoUnitBelongsTo(PLAYERTWO);
	m_gameBoard[3][15].setWhoUnitBelongsTo(PLAYERTWO);
};

void GameEngine::Update(float dt)
{
	//////////////////////////////////////////////////////////////
	//  INFO:  Start audio automatically.  Remove or move later
	//  TODO:  Fix
	if(!channel){
		fmodSystem->playSound( FMOD_CHANNEL_FREE, battleTheme, false, &channel);
	}
	fmodSystem->update();
	//////////////////////////////////////////////////////////////////////////
	// Get and Acquire Keyboard Input
	//////////////////////////////////////////////////////////////////////////
	// Get the input device state
	HRESULT hr;
	hr = m_pDIKeyboard->Acquire();
	if( FAILED(hr) )							//if keyboard fails to acquire
	{
		ZeroMemory(buffer, sizeof(buffer) );	//clear keyboard buffer memory
		hr = m_pDIKeyboard->Acquire();				//and re-acquire
	}else										//else get the state of the keyboard
		hr = m_pDIKeyboard->GetDeviceState( sizeof(buffer),  (LPVOID)&buffer ); 

	//////////////////////////////////////////////////////////////////////////
	// Get and Acquire Mouse Input
	//////////////////////////////////////////////////////////////////////////
	hr = m_pDIMouse->Acquire();
	//check for failure to acquire mouse
	if( FAILED(hr) )
	{
		ZeroMemory(&mouseState, sizeof( DIMOUSESTATE2 ) );  //if mouse fails, clear the memory out
		hr = m_pDIMouse->Acquire();								//and re-acquire
	}else													//else get the state of the mouse
		hr = m_pDIMouse->GetDeviceState( sizeof( DIMOUSESTATE2 ), &mouseState); 


	calcDeltaTime();

	fmodSystem->update();
	//////////////////////////////////////////////////////////////////////////
	// Mouse/Cursor updates
	//////////////////////////////////////////////////////////////////////////
	myMouse.update(mouseState.lX, mouseState.lY);
	cursor.x = myMouse.cursorPos.x;
	cursor.y = myMouse.cursorPos.y;

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
		switch(m_gamePhase){
		case PLAYERONE_PLAYPHASE:
			break;
		case PLAYERONE_EVENTPHASE:
			updateEventPhase(dt);
			break;
		case PLAYERTWO_PLAYPHASE:
			break;
		case PLAYERTWO_EVENTPHASE:
			updateEventPhase(dt);
			break;
		};
		if(buffer[DIK_RIGHT] & 0x80){
			if(!keyIsDown[DIK_RIGHT]){
				keyIsDown[DIK_RIGHT] = true;
				m_gamePhase += 1;
				if(m_gamePhase > 3){
					m_gameBoard[1][4].setUnitCanTakeAction(true);
					m_gameBoard[1][14].setUnitCanTakeAction(true);
					m_gameBoard[3][9].setUnitCanTakeAction(true);
					m_gameBoard[0][1].setUnitCanTakeAction(true);
					m_gameBoard[2][6].setUnitCanTakeAction(true);
					m_gamePhase = 0;
				}
				//m_gameBoard[4][14].setAnimationRect(m_gameBoard[4][14].getAnimationRect().top, m_gameBoard[4][14].getAnimationRect().left + 70, m_gameBoard[4][14].getAnimationRect().right + 70, m_gameBoard[4][14].getAnimationRect().bottom);
			}
		}
		else
			keyIsDown[DIK_RIGHT] = false;
		if(buffer[DIK_LEFT] & 0x80){
			if(!keyIsDown[DIK_LEFT]){
				keyIsDown[DIK_LEFT] = true;
				m_gamePhase -= 1;
				if(m_gamePhase < 0)
					m_gamePhase = 3;
				//m_gameBoard[4][14].setAnimationRect(m_gameBoard[4][14].getAnimationRect().top, m_gameBoard[4][14].getAnimationRect().left - 70, m_gameBoard[4][14].getAnimationRect().right - 70, m_gameBoard[4][14].getAnimationRect().bottom);
			}
		}
		else
			keyIsDown[DIK_LEFT] = false;
		if(buffer[DIK_UP] & 0x80){
			if(!keyIsDown[DIK_UP]){
				keyIsDown[DIK_UP] = true;
				m_gamePhase = PLAYERTWO_EVENTPHASE;
				//if(m_gamePhase < 0)
				//m_gamePhase = 3;
				//m_gameBoard[4][14].setAnimationRect(m_gameBoard[4][14].getAnimationRect().top, m_gameBoard[4][14].getAnimationRect().left - 70, m_gameBoard[4][14].getAnimationRect().right - 70, m_gameBoard[4][14].getAnimationRect().bottom);
			}
		}
		else
			keyIsDown[DIK_UP] = false;

		///////////////////////////////////////////////////////////
		//  INFO:  Updates unit animations
		updateAnimations(dt);

		//////////////////////////////////////////////////
		//// Check for units icon hover over and pressing
		if(m_player[0].checkIfActivePlayer())
		{
			for(auto &Buttons: player1_units)
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3))
				{
					Buttons.setHighlight(true);
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 0, 0));
				}else
				{
					Buttons.setHighlight(false);
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 255));
				}

				if(mouseState.rgbButtons[0])
				{
					int selected = 99;
					for(int i = 0; i < 6; i++)
						if(player1_units[i].isHighlighted())
							selected = i;
					//Switch states accordingly
					switch(selected)
					{
					case 0: // first unit(s)
						m_gameBoard[2][4].setOccupiedBy(ARCHERUNIT);
						m_gameBoard[2][4].setAnimationRect(15, 10, 70, 70);
						break;
					case 1: // second unit(s)
						m_gameBoard[3][4].setOccupiedBy(THIEF);
						m_gameBoard[3][4].setAnimationRect(15, 10, 70, 70);
						break;
					case 2: // third unit(s)
						break;
					case 3: // special unit(s)
						break;
					case 4: // Ability 1
						break;
					case 5: // Ability 2
						break;
					default:
						break;
					}
				}
			}
		}

		if(m_player[1].checkIfActivePlayer())
		{
			for(auto &Buttons: player2_units)
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3))
				{
					Buttons.setHighlight(true);
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 0, 0));
				}else
				{
					Buttons.setHighlight(false);
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 255));
				}

				if(mouseState.rgbButtons[0])
				{
					int selected = 99;
					for(int i = 0; i < 6; i++)
						if(player1_units[i].isHighlighted())
							selected = i;
					//Switch states accordingly
					switch(selected)
					{
					case 0: // first unit(s)
						m_gameBoard[2][10].setOccupiedBy(BLACKMAGEUNIT);
						m_gameBoard[2][10].setAnimationRect(75, 5, 70, 145);
						break;
					case 1: // second unit(s)
						m_gameBoard[3][8].setOccupiedBy(THIEF); // CHANGE THEIF
						m_gameBoard[3][8].setAnimationRect(15, 10, 70, 70);
						break;
					case 2: // third unit(s)
						break;
					case 3: // special unit(s)
						break;
					case 4: // Ability 1
						break;
					case 5: // Ability 2
						break;
					default:
						break;
					}
				}
			}
		}
		break;
	}
}

void GameEngine::updateAnimations(float dt){

	for(int i = 0; i < MAXBOARDHEIGHT; ++i){
		for(int j = 0; j < MAXBOARDWIDTH; ++j){


			//////////////////////////////////////////////////////////////////////////////////
			//  INFO:  If the gamespace is occupied by a unit, draw that unit.  
			//  DETAIL:  Each gamespace will have
			switch(m_gameBoard[i][j].getOccupiedBy()){
			case NOUNIT:
				break;
			case GOLDMINES:
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
				m_gameBoard[i][j].adjustAnimationTimer(dt);
				if(m_gameBoard[i][j].getAnimationTimer() > 0.15f){
					m_gameBoard[i][j].setAnimationRect(m_gameBoard[i][j].getAnimationRect().top, m_gameBoard[i][j].getAnimationRect().left + 64, m_gameBoard[i][j].getAnimationRect().right + 64, m_gameBoard[i][j].getAnimationRect().bottom);
					m_gameBoard[i][j].adjustAnimationTimer(-0.15f);
					if(m_gameBoard[i][j].getAnimationRect().right > 250)
						m_gameBoard[i][j].setAnimationRect(15, 10, 70, 70);
				}
				break;
			case THIEF:
				break;
			case GOLEM:
				break;
			case BLACKMAGEUNIT:
				m_gameBoard[i][j].adjustAnimationTimer(dt);
				if(m_gameBoard[i][j].getAnimationTimer() > 0.15f){
					m_gameBoard[i][j].adjustAnimationTimer(-0.15f);
					switch(m_gameBoard[i][j].getAnimationRect().left){
					case 5:
						m_gameBoard[i][j].setAnimationRect(m_gameBoard[i][j].getAnimationRect().top, m_gameBoard[i][j].getAnimationRect().left + 70, m_gameBoard[i][j].getAnimationRect().right + 70, m_gameBoard[i][j].getAnimationRect().bottom);
						break;
					case 75:
						m_gameBoard[i][j].setAnimationRect(m_gameBoard[i][j].getAnimationRect().top, m_gameBoard[i][j].getAnimationRect().left + 65, m_gameBoard[i][j].getAnimationRect().right + 65, m_gameBoard[i][j].getAnimationRect().bottom);
						break;
					case 140:
						m_gameBoard[i][j].setAnimationRect(m_gameBoard[i][j].getAnimationRect().top, m_gameBoard[i][j].getAnimationRect().left + 65, m_gameBoard[i][j].getAnimationRect().right + 65, m_gameBoard[i][j].getAnimationRect().bottom);
						break;
					case 205:
						m_gameBoard[i][j].setAnimationRect(75, 5, 70, 145);
						break;
					}

				}
				break;
			case WARLOCK:
				break;
			}
		}
	}
};

void GameEngine::updateEventPhase(float dt){
	if(m_floatingTextActive){
		m_floatingRectTimer += 100 * dt;
		if(m_floatingRectTimer > 2.0f){
			m_floatingTextRect.top -=  1;
			m_floatingRectTimer -= 2.0f;
		}
		if(m_floatingRectTopMax > m_floatingTextRect.top){
			m_floatingRectTopMax = 0.0f;
			m_floatingTextRect.top		=		0.0f;
			m_floatingTextRect.left		=		0.0f;
			m_floatingTextRect.right	=		0.0f;
			m_floatingTextRect.bottom	=		0.0f;
			m_floatingTextActive = false;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	//  INFO:  If no unit is attacking, find the next unit to take action
	if(!m_unitCurrentlyAttacking){
		if(m_gamePhase == PLAYERONE_EVENTPHASE){
			for(int i = 0; i < MAXBOARDHEIGHT; ++i){
				int trial = 0;
				for(int j = MAXBOARDWIDTH-1; j > 0; --j){
					if(m_gameBoard[i][j].getWhoUnitBelongsTo() == PLAYERONE && m_gameBoard[i][j].canUnitTakeAction()){
						m_unitCurrentlyAttacking = true;
						m_arrowForAttackingUnitPosX	=	m_gameBoard[i][j].getPosX() + 10;
						m_arrowForAttackingUnitPosY	=	m_gameBoard[i][j].getPosY() - 40;
						switch(m_gameBoard[i][j].getOccupiedBy()){
						case NOUNIT:
							break;
						case GOLDMINES:
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
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_arrowActive = true;
							findNextTarget(i);
							fmodSystem->playSound( FMOD_CHANNEL_FREE, shootArrow, false, 0 );
							return;
							//break;
						case THIEF:
							break;
						case GOLEM:
							break;
						case BLACKMAGEUNIT:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_fireBallActive = true;
							findNextTarget(i);
							fmodSystem->playSound( FMOD_CHANNEL_FREE, castFireball, false, 0);
							return;
							//break;
						case WARLOCK:
							break;
						}
					}

				}
			}
		}
		else if(m_gamePhase == PLAYERTWO_EVENTPHASE){
			for(int i = 0; i < MAXBOARDHEIGHT; ++i){
				int trial = 0;
				for(int j = 0; j < MAXBOARDWIDTH; ++j){
					if(m_gameBoard[i][j].getWhoUnitBelongsTo() == PLAYERTWO && m_gameBoard[i][j].canUnitTakeAction()){
						m_unitCurrentlyAttacking = true;
						m_arrowForAttackingUnitPosX	=	m_gameBoard[i][j].getPosX() + 10;
						m_arrowForAttackingUnitPosY	=	m_gameBoard[i][j].getPosY() - 40;
						switch(m_gameBoard[i][j].getOccupiedBy()){
						case NOUNIT:
							break;
						case GOLDMINES:
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
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_arrowActive = true;
							findNextTarget(i);
							fmodSystem->playSound( FMOD_CHANNEL_FREE, shootArrow, false, 0 );
							return;
							//break;
						case THIEF:
							break;
						case GOLEM:
							break;
						case BLACKMAGEUNIT:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_fireBallActive = true;
							findNextTarget(i);
							fmodSystem->playSound( FMOD_CHANNEL_FREE, castFireball, false, 0);
							return;
							//break;
						case WARLOCK:
							break;
						}
					}

				}
			}
		}
	}
	else{
		m_temporaryTimer += dt;
		int flip = 1;
		if(m_arrowActive){
			if(m_gamePhase == PLAYERTWO_EVENTPHASE)
				flip *= -1;
			m_projectilePosX += 100 * dt * flip;
		}
		else if(m_fireBallActive){
			if(m_gamePhase == PLAYERTWO_EVENTPHASE)
				flip *= -1;
			m_projectilePosX += 100 * dt * flip;
			m_fireballRotation += 800 * dt;
		}
		//	if(m_temporaryTimer > 2.0 ){
		if( ( ( m_projectilePosX > m_gameBoard[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosX() - 5 ) && ( m_projectilePosX < m_gameBoard[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosX() + 5 ) ) || m_projectilePosX > 700 ){

			////////////////////////////////////////////////////////////////
			//  INFO:  Set data for floating text
			m_floatingTextActive = true;
			m_floatingRectTopMax	=	m_gameBoard[m_attackingSpaceX][m_attackingSpaceY].getPosY() - 35;
			m_floatingTextRect.top = m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosY() - 10;
			m_floatingTextRect.left = m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosX() - 10;
			m_floatingTextRect.right = m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosX() + 10;
			m_floatingTextRect.bottom = m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosY() + 10;

			////////////////////////////////////////////////////////////////
			//  INFO:  Play sound effects based on collision
			if(m_arrowActive ){//&& m_gameBoard[m_attackingSpaceX][m_attackingSpaceY].getOccupiedBy() == GOLDMINES){
				switch(m_gameBoard[m_attackingSpaceX][m_attackingSpaceY].getOccupiedBy()){
				case GOLDMINES:
					fmodSystem->playSound( FMOD_CHANNEL_FREE, hitMines, false, 0 );
					break;
				case BLACKMAGEUNIT:
					fmodSystem->playSound( FMOD_CHANNEL_FREE, arrowHit, false, 0 );
					break;
				case ARCHERUNIT:
					fmodSystem->playSound( FMOD_CHANNEL_FREE, arrowHit, false, 0 );
					break;
				}
			}
			else if(m_fireBallActive){
				fmodSystem->playSound( FMOD_CHANNEL_FREE, fireballHit, false, 0 );
			}
			m_gameBoard[m_attackingSpaceX][m_attackingSpaceY].setUnitCanTakeAction(false);
			m_projectilePosX = 0;
			m_projectilePosY = 0;
			m_attackTargetSpaceX = 0;
			m_attackTargetSpaceY = 0;
			m_temporaryTimer = 0.0f;
			m_unitCurrentlyAttacking = false;
			m_arrowActive = false;
			m_fireBallActive = false;
			//m_attackingSpaceX	=	-1;
			//m_attackingSpaceY	=	-1;
		}
		//}
	}
	int test;
	if(m_gamePhase == PLAYERTWO_EVENTPHASE)
		test = 0;
};

void GameEngine::findNextTarget(int row){
	//if(m_gamePhase == PLAYERONE_EVENTPHASE){
	//	///////////////////////////////////////////////////////////////////////////////////
	//	//  INFO:  Finds the first available target and sets it, then returns
	//	for(int i = 0; i < MAXBOARDHEIGHT; ++i){
	//		for(int j = 0; j < MAXBOARDWIDTH; ++j){
	//			if(m_gameBoard[i][j].getWhoUnitBelongsTo() == PLAYERTWO){
	//				m_attackTargetSpaceX = i;
	//				m_attackTargetSpaceY = j;
	//				return;
	//			}
	//		}
	//	}
	//}
	if(m_gamePhase == PLAYERONE_EVENTPHASE){
		///////////////////////////////////////////////////////////////////////////////////
		//  INFO:  Finds the first available target and sets it, then returns
		//for(int i = 0; i < MAXBOARDHEIGHT; ++i){
		for(int i = 0; i < MAXBOARDWIDTH; ++i){
			if(m_gameBoard[row][i].getWhoUnitBelongsTo() == PLAYERTWO){
				m_attackTargetSpaceX = row;
				m_attackTargetSpaceY = i;
				break;
			}
		}
	}
	else if(m_gamePhase == PLAYERTWO_EVENTPHASE){
		for(int i = MAXBOARDWIDTH; i > 0; --i){
			if(m_gameBoard[row][i].getWhoUnitBelongsTo() == PLAYERONE){
				if(i > m_attackingSpaceX )
					continue;
				m_attackTargetSpaceX = row;
				m_attackTargetSpaceY = i;
				break;
			}
		}
	}
	//}
};

void GameEngine::Render()
{
	// If the device was not created successfully, return
	if(!m_pD3DDevice)
		return;
	if(SUCCEEDED(m_pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0)))
	{
		if(SUCCEEDED(m_pD3DDevice->BeginScene()))
		{
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
					//Check which character player is
					if(m_player[0].characterType == ARCHER)
						drawIcons(0, m_player[0]);
					else if(m_player[0].characterType == BLACKMAGE)
						drawIcons(6, m_player[0]);
					else if(m_player[0].characterType == WARRIOR)
						drawIcons(12, m_player[0]);

					if(m_player[1].characterType == BLACKMAGE)
						drawIcons(6, m_player[1]);
					else if(m_player[1].characterType == WARRIOR)
						drawIcons(12, m_player[1]);
					else if(m_player[1].characterType == ARCHER)
						drawIcons(0, m_player[1]);

					break;
				}
				myMouse.render(m_pD3DSprite, m_cursor, m_cursorInfo);

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
	SAFE_RELEASE(m_archerArrow);
	SAFE_RELEASE(m_fireball);
	SAFE_RELEASE(m_arrow);
	SAFE_RELEASE(m_blackMageUnit);
	SAFE_RELEASE(m_archerUnit);
	SAFE_RELEASE(m_goldMine);
	SAFE_RELEASE(m_playerUIBackground);
	SAFE_RELEASE(m_blackMageCharacter);
	SAFE_RELEASE(m_archerCharacter);
	SAFE_RELEASE(m_gamePiece);
	SAFE_RELEASE(m_battleBackgroundOne);
	// Icons
	SAFE_RELEASE(m_wallIcon);
	SAFE_RELEASE(m_blackHoleIcon);
	SAFE_RELEASE(m_wolfIcon);
	SAFE_RELEASE(m_thiefIcon);
	SAFE_RELEASE(m_blackMageIcon);
	SAFE_RELEASE(m_golemIcon);
	SAFE_RELEASE(m_archerIcon);


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
		int flip = 1;
		for(int j = 0; j < MAXBOARDWIDTH; ++j){
			/////////////////////////////////////////////////////////
			//  INFO:  For inverting images for 2nd player
			flip = 1;
			D3DXMatrixIdentity(&transMat);
			D3DXMatrixIdentity(&scaleMat);
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixIdentity(&worldMat);

			D3DXMatrixScaling(&scaleMat, 0.7f, 0.80f, 0.0f);			// Scaling
			D3DXMatrixTranslation(&transMat, m_gameBoard[i][j].getPosX(), m_gameBoard[i][j].getPosY(), 0.0f);			// Translation
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

			// Set Transform
			m_pD3DSprite->SetTransform(&worldMat);


			m_pD3DSprite->Draw(m_gamePiece, 0, &D3DXVECTOR3(m_gamePieceInfo.Width * 0.5f, m_gamePieceInfo.Height * 0.5f, 0.0f),
				0, D3DCOLOR_ARGB(255, 255, 255, 255));

			D3DXMatrixIdentity(&transMat);
			D3DXMatrixIdentity(&scaleMat);
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixIdentity(&worldMat);

			////////////////////////////////////////////////////////////////////////////////
			//  INFO:  For inverting images for player 2
			if(m_gameBoard[i][j].getWhoUnitBelongsTo() == 1)
				flip *= -1;
			int offset = 0;
			//int testX, testY;
			//////////////////////////////////////////////////////////////////////////////////
			//  INFO:  If the gamespace is occupied by a unit, draw that unit.  
			switch(m_gameBoard[i][j].getOccupiedBy()){

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
				//////////////////////////////////////////////////////////////////////////////////////////////////////////
				//  INFO:  Adds an offset to draw unit in the center of the gamespace
				if(m_gameBoard[i][j].getWhoUnitBelongsTo() == PLAYERONE )
					offset = 33;
				if(m_gameBoard[i][j].getWhoUnitBelongsTo() == PLAYERTWO )
					offset = -15;
				D3DXMatrixScaling(&scaleMat, 0.75f * flip, 0.74f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_gameBoard[i][j].getPosX() + offset, m_gameBoard[i][j].getPosY()+33, 0.0f);			// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				// Set Transform
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_archerUnit, &m_gameBoard[i][j].getAnimationRect(), &D3DXVECTOR3(m_gameBoard[i][j].getAnimationRect().right - m_gameBoard[i][j].getAnimationRect().left, m_gameBoard[i][j].getAnimationRect().bottom - m_gameBoard[i][j].getAnimationRect().top, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				break;
			case THIEF:
				break;
			case GOLEM:
				break;
			case BLACKMAGEUNIT:
				//////////////////////////////////////////////////////////////////////////////////////////////////////////
				//  INFO:  Adds an offset to draw unit in the center of the gamespace
				if(m_gameBoard[i][j].getWhoUnitBelongsTo() == PLAYERONE)
					offset = 35;
				if(m_gameBoard[i][j].getWhoUnitBelongsTo() == PLAYERTWO)
					offset = -15;
				D3DXMatrixScaling(&scaleMat, 0.75f * flip, 0.74f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_gameBoard[i][j].getPosX() + offset, m_gameBoard[i][j].getPosY()+35, 0.0f);			// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				// Set Transform
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_blackMageUnit, &m_gameBoard[i][j].getAnimationRect(), &D3DXVECTOR3(m_gameBoard[i][j].getAnimationRect().right - m_gameBoard[i][j].getAnimationRect().left, m_gameBoard[i][j].getAnimationRect().bottom - m_gameBoard[i][j].getAnimationRect().top, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
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
	if(m_unitCurrentlyAttacking){
		/////////////////////////////////////////////////////////////
		//  INFO:  For inverting images if they belong to player 2
		int flip = 1;
		if(m_gamePhase == PLAYERTWO_EVENTPHASE)
			flip *= -1;
		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);
		D3DXMatrixScaling(&scaleMat, 0.03f, 0.03f, 0.0f);			// Scaling
		D3DXMatrixTranslation(&transMat, m_arrowForAttackingUnitPosX, m_arrowForAttackingUnitPosY, 0.0f);			// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
		m_pD3DSprite->SetTransform(&worldMat);
		m_pD3DSprite->Draw(m_arrow, 0, &D3DXVECTOR3(m_arrowInfo.Width * 0.5f, m_arrowInfo.Height * 0.5f, 0.0f),
			0, D3DCOLOR_ARGB(255, 255, 255, 255));
		if(m_fireBallActive){
			D3DXMatrixIdentity(&transMat);
			D3DXMatrixIdentity(&scaleMat);
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixIdentity(&worldMat);
			D3DXMatrixScaling(&scaleMat, 0.05f * flip, 0.04f, 0.0f);			// Scaling
			D3DXMatrixTranslation(&transMat, m_projectilePosX, m_projectilePosY, 0.0f);			// Translation
			D3DXMatrixRotationZ(&rotMat, D3DXToRadian(m_fireballRotation));		// Rotation on Z axis, value in radians, converting from degrees
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
			m_pD3DSprite->SetTransform(&worldMat);
			m_pD3DSprite->Draw(m_fireball, 0, &D3DXVECTOR3(m_fireballInfo.Width * 0.5f, m_fireballInfo.Height * 0.5f, 0.0f),
				0, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		if(m_arrowActive){
			D3DXMatrixIdentity(&transMat);
			D3DXMatrixIdentity(&scaleMat);
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixIdentity(&worldMat);
			D3DXMatrixScaling(&scaleMat, 1.0f * flip, 1.0f, 1.0f);			// Scaling
			D3DXMatrixTranslation(&transMat, m_projectilePosX, m_projectilePosY, 0.0f);			// Translation
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
			m_pD3DSprite->SetTransform(&worldMat);
			m_pD3DSprite->Draw(m_archerArrow, 0, &D3DXVECTOR3(m_archerArrowInfo.Width * 0.5f, m_archerArrowInfo.Height * 0.5f, 0.0f),
				0, D3DCOLOR_ARGB(255, 255, 255, 255));
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

	for(int i = 0; i < 2; ++i)
	{
		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		D3DXMatrixScaling(&scaleMat, 1.60f, 0.35f, 0.0f);			// Scaling
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

	if(m_floatingTextActive){
		swprintf_s(buffer, 128, L"%d", 20);
		m_pD3DFont->DrawText(0, buffer, -1, &m_floatingTextRect, DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	}

	//Drawing cursor positions onto screen
	RECT cursorRect;
	GetClientRect(m_hWnd, &cursorRect);
	cursorRect.top = 470;
	cursorRect.left = 300;
	swprintf_s(buffer, 128, L"%f", cursor.x);
	m_pD3DFont->DrawText(0, buffer, -1, &cursorRect, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
	cursorRect.top = 500;
	swprintf_s(buffer, 128, L"%f", cursor.y);
	m_pD3DFont->DrawText(0, buffer, -1, &cursorRect, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
};

void GameEngine::calcDeltaTime(){
	m_deltaTime = difftime(time(0), start);
};

void GameEngine::drawIcons(int thisButton , Character thisPlayer)
{
	//ARCHER UNIT BUTTONS
	if(thisButton == 0 && thisPlayer.getPlayerNumber() == 0)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);
		for(auto &Buttons: player1_units)
		{
			if(thisButton == ARCHERBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_archerIcon, 0, &D3DXVECTOR3(m_archerIconInfo.Width * 0.5f, m_archerIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == THIEFBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_thiefIcon, 0, &D3DXVECTOR3(m_thiefIconInfo.Width * 0.5f, m_thiefIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == WOLFBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wolfIcon, 0, &D3DXVECTOR3(m_wolfIconInfo.Width * 0.5f, m_wolfIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == WALLBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
			thisButton += 1;
		}
	}
	else if( thisButton == 0  && thisPlayer.getPlayerNumber() == 1)
	{
		for(auto &Buttons: player2_units)
		{
			D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
			D3DXMatrixIdentity(&transMat);
			D3DXMatrixIdentity(&scaleMat);
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixIdentity(&worldMat);
			if(thisButton == ARCHERBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_archerIcon, 0, &D3DXVECTOR3(m_archerIconInfo.Width * 0.5f, m_archerIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == THIEFBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_thiefIcon, 0, &D3DXVECTOR3(m_thiefIconInfo.Width * 0.5f, m_thiefIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == WOLFBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wolfIcon, 0, &D3DXVECTOR3(m_wolfIconInfo.Width * 0.5f, m_wolfIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == WALLBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
			thisButton +=1;
		}
	}

	//BLACKMAGE UNIT BUTTONS
	if(thisButton == 6 && thisPlayer.getPlayerNumber() == 0)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		for(auto &Buttons: player1_units)
		{
			if(thisButton == BLACKMAGEBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_blackMageIcon, 0, &D3DXVECTOR3(m_blackMageIconInfo.Width * 0.5f, m_blackMageIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == GOLEMBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_golemIcon, 0, &D3DXVECTOR3(m_golemIconInfo.Width * 0.5f, m_golemIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == BLACKMAGEABILITY2)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_blackHoleIcon, 0, &D3DXVECTOR3(m_blackHoleIconInfo.Width * 0.5f, m_blackHoleIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
			thisButton +=1;
		}
	}
	else if(thisButton == 6 && thisPlayer.getPlayerNumber() == 1)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		for(auto &Buttons: player2_units)
		{
			if(thisButton == BLACKMAGEBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_blackMageIcon, 0, &D3DXVECTOR3(m_blackMageIconInfo.Width * 0.5f, m_blackMageIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == GOLEMBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_golemIcon, 0, &D3DXVECTOR3(m_golemIconInfo.Width * 0.5f, m_golemIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if(thisButton == BLACKMAGEABILITY2)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
				D3DXMatrixTranslation(&transMat, Buttons.getPosition().x, Buttons.getPosition().y, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_blackHoleIcon, 0, &D3DXVECTOR3(m_blackHoleIconInfo.Width * 0.5f, m_blackHoleIconInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
			thisButton +=1;
		}
	}
}
