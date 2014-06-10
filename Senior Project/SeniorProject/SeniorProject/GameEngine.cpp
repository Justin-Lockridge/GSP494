#include "GameEngine.h"

GameEngine::GameEngine(void)
{
	// Init or NULL objects before use to avoid any undefined behavior
	m_bVsync						=	false;
	m_pD3DObject					=	0;
	m_pD3DDevice					=	0;
	m_currTime						=	0;
	m_prevTime						=	0;
	m_FPS							=	0;
	m_deltaTime						=	0;
	m_gameState						=	0;
	m_lightningActive				=	false;
	m_combatMessageActive			=	false;
	m_lightningRect.top				=	0;
	m_lightningRect.left			=	0;
	m_lightningRect.right			=	0;
	m_lightningRect.bottom			=	0;
	m_lightningTimer				=	0.0f;
	m_randomNumber					=	0;
	selectedUnit					=	99;
	textCount						=	0;
	noGold							=	false;
	dontPlaceUnit					=	false;
	firstTurn						=  true;
	hoveredUnit						=	false;
	selectedHover					=	0;
	m_tester						=	0;
	unitsAttacked					=	0;
	player1selected					= false;
	player2selected					= false;
	number							= 0;
}

GameEngine::~GameEngine()
{
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

	D3Dpp.hDeviceWindow					= hWnd;												// Handle to the focus window
	D3Dpp.Windowed						= bWindowed;											// Windowed or Full-screen boolean
	D3Dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;								// Format of depth/stencil buffer, 24 bit depth, 8 bit stencil
	D3Dpp.EnableAutoDepthStencil		= TRUE;													// Enables Z-Buffer (Depth Buffer)
	D3Dpp.BackBufferCount				= 1;															// Change if need of > 1 is required at a later date
	D3Dpp.BackBufferFormat				= D3DFMT_X8R8G8B8;								// Back-buffer format, 8 bits for each pixel
	D3Dpp.BackBufferHeight				= height;													// Make sure resolution is supported, use adapter modes
	D3Dpp.BackBufferWidth				= width;													// (Same as above)
	D3Dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;						// Discard back-buffer, must stay discard to support multi-sample
	D3Dpp.PresentationInterval			= m_bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;	// Present back-buffer immediately, unless V-Sync is on								
	D3Dpp.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;																	// This flag should improve performance, if not set to NULL.
	D3Dpp.FullScreen_RefreshRateInHz	= bWindowed ? 0 : D3DPRESENT_RATE_DEFAULT;													// Full-screen refresh rate, use adapter modes or default
	D3Dpp.MultiSampleQuality			= 0;																																	// MSAA currently off, check documentation for support.
	D3Dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;																								// MSAA currently off, check documentation for support.

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
		D3DDEVTYPE_HAL,				// device type to use, set to hardware rasterization
		hWnd,									// handle to the focus window
		deviceBehaviorFlags,			// behavior flags
		&D3Dpp,								// presentation parameters
		&m_pD3DDevice);				// returned device pointer

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

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"knight.tga", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_menuBGInfo, 0, &m_menuBG);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"MenuButtons.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_menuButtonsInfo, 0, &m_menuButtons);

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

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"endTurn.tga", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_endTurnInfo, 0, &m_endTurn);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WallIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_wallIconInfo, 0, &m_wallIcon);

	/////////////////////////////////////////////////////
	// Icons: Units, abilities
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"ArcherIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_archerIconInfo, 0, &m_archerIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"BlackmageIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_blackMageIconInfo, 0, &m_blackMageIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"GolemIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_golemIconInfo, 0, &m_golemIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"ThiefIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_thiefIconInfo, 0, &m_thiefIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WallUnitIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_wallIconInfo2, 0, &m_wallIcon2);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WolfIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_wolfIconInfo, 0, &m_wolfIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WarlockIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_warlockIconInfo, 0, &m_warlockIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"FlamestrikeIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_flameStrikeIconInfo, 0, &m_flameStrikeIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"SnipeIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_snipeIconInfo, 0, &m_snipeIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"SplitshotIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_splitShotIconInfo, 0, &m_splitShotIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"SplitshotIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_splitShotIconInfo, 0, &m_splitShotIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"BlackHoleIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_blackHoleIconInfo, 0, &m_blackHoleIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WarriorIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_warriorIconInfo, 0, &m_warriorIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"MarksmanIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_marksmanIconInfo, 0, &m_marksmanIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"MinotaurIcon.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_minotaurIconInfo, 0, &m_minotaurIcon);


	/////////////////////////////////////////////////////
	// Hover Info
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"ArcherUnitInfo.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_archerHoverInfo, 0, &m_archerHover);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WolfUnitInfo.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_wolfHoverInfo, 0, &m_wolfHover);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WolfUnitInfo.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_thiefHoverInfo, 0, &m_thiefHover);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WallUnitInfo.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_wallHoverInfo, 0, &m_wallHover);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"BlackMageUnitInfo.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_blackMageHoverInfo, 0, &m_blackMageHover);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"GolemUnitInfo.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_golemHoverInfo, 0, &m_golemHover);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WarlockUnitInfo.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_warlockHoverInfo, 0, &m_warlockHover);

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

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"HealthBar.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_healthBarInfo, 0, &m_healthBar);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"EarthElementalSheet.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_golemUnitInfo, 0, &m_golemUnit);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Warlock.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_warlockInfo, 0, &m_warlock);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Lightning.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_lightningInfo, 0, &m_lightning);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"WolfSheet.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_wolfInfo, 0, &m_wolf);

	HRESULT hres = D3DXCreateTextureFromFileEx(m_pD3DDevice, L"ThiefSheet.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_thiefInfo, 0, &m_thief);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Warrior.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_warriorCharacterInfo, 0, &m_warriorCharacter);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Test.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_warriorUnitIconInfo, 0, &m_warriorUnitIcon);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"MarksmanSheetOriginal.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_marksmanUnitInfo, 0, &m_marksmanUnit);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Minotaur.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_minotaurUnitInfo, 0, &m_minotaurUnit);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"BlackHoleSheet.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_blackHoleAbilityInfo, 0, &m_blackHoleAbility);


	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Flames.png", 0, 0, 0, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		D3DCOLOR_XRGB(255, 0, 255), &m_flameStrikeAbilityInfo, 0, &m_flameStrikeAbility);
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
	fmodSystem->createSound("ThunderStrike.wav", FMOD_DEFAULT, 0, &thunderStrike);
	fmodSystem->createSound("MinotaurRoar.wav", FMOD_DEFAULT, 0, &minotaurRoar);
	fmodSystem->createSound("Gold.mp3", FMOD_DEFAULT, 0, &goldCoins);
	fmodSystem->createSound("LimitBreak.wav", FMOD_DEFAULT, 0, &warlockSpell);

	for(int i = 0; i < 255; ++i)
	{
		keyIsDown[i] = false;
	};
	start = (time(0));
	m_frameTime = 0;
	m_deltaTime = 0;

	cursor.x = 500;
	cursor.y = 300;

	myMouse.init();
	InitMenu();

	srand(time(NULL));
	//////////////////////////////////////////////
	//  INFO:  Inits both players
	m_player[0].setActivePlayer(true);
	m_player[0].setPlayerNumber(0);
	m_player[1].setPlayerNumber(1);

	///////////////////////////////////////////////////
	//  INFO:  Initializes gameboard and gold mines
	InitGameBoard();
	m_unitCurrentlyAttacking = false;
	m_fireballRotation = 0.0f;
	////////////////////////////////////////////////////////
	//  INFO:  Changed the gamestate manually for testing
	m_gameState = MENUMAIN;
	//m_unit[2][2].addUnit(MINOTAUR, PLAYERONE);
	//m_unit[3][3].addUnit(THIEF, PLAYERONE);
	//m_unit[2][6].addUnit( WARLOCK, PLAYERONE );
	//m_unit[3][7].addUnit( WARLOCK, PLAYERTWO );

	//m_unit[1][1].addUnit( WARLOCK, PLAYERONE );
	//m_unit[1][6].addUnit( WARLOCK, PLAYERONE );
	//m_unit[0][10].addUnit( WARLOCK, PLAYERTWO );
	//m_unit[0][5].addUnit( MINOTAUR, PLAYERONE );
	//m_unit[2][12].addUnit( WARLOCK, PLAYERTWO );
	//m_unit[1][4].addUnit( MARKSMAN, PLAYERONE );
}

void GameEngine::InitGameBoard()
{
	for(int i = 0; i < MAXBOARDHEIGHT; ++i)
	{
		for(int j = 0; j < MAXBOARDWIDTH; ++j)
		{
			m_gameBoard[i][j].setPosX(75.0f + float(j) * 43.0f);
			m_gameBoard[i][j].setPosY(190.0f + float(i) * 50.0f);
			m_gameBoard[i][j].setSpaceNumber( i * MAXBOARDWIDTH + j );
			m_gameBoard[i][j].setWhoUnitBelongsTo(-1);

			m_unit[i][j].setPosX(75.0f + j * 43.0f );
			m_unit[i][j].setPosY( 190.0f + i * 50.0f );
			m_unit[i][j].setType(NOUNIT);
			m_unit[i][j].setWhoUnitBelongsTo(-1);
			//////////////////////////////////////////////
			//  INFO:  Sets the end game spaces to be occupied by gold mines
			if(j == 0 )
			{
				m_unit[i][j].addUnit(GOLDMINES, PLAYERONE);
				m_unit[i][j].setWhoUnitBelongsTo(PLAYERONE);
				m_unit[i][j].setType(GOLDMINES);
			}
			else if (j == MAXBOARDWIDTH-1 )
			{
				m_unit[i][j].addUnit(GOLDMINES, PLAYERTWO);
				m_unit[i][j].setWhoUnitBelongsTo(PLAYERTWO);
				m_unit[i][j].setType(GOLDMINES);
			}
			float testerX, testerY;
			if(i == 0 && j == 14)
			{
				testerX = m_gameBoard[i][j].getPosX();
				testerY = m_gameBoard[i][j].getPosY();
				testerX += 0;
			}
		}
	}
	/////////////////////////////////////////////////////////
	//  TODO:  Put this back in the loop.  For some reason, the last goldmine for player two wasn't being set so I did it here.
	m_unit[4][MAXBOARDWIDTH].addUnit(GOLDMINES, PLAYERTWO);
	m_unit[4][MAXBOARDWIDTH].setWhoUnitBelongsTo(PLAYERTWO);
	m_unit[4][MAXBOARDWIDTH].setType(GOLDMINES);

	player1_units.clear();
	hoverButtons.clear();
	// T, L, B, R, X, Y, Highlighted
	RectData temp_units_pos[] =
	{
		{480, 120, 580, 205, 140, 500, false}, //unit 1
		{480, 170, 580, 255, 190, 500, false}, //unit 2
		{480, 220, 580, 305, 240, 500, false}, //unit 3
		{480, 270, 580, 355, 290, 500, false}, //unit 4
		{561, 120, 650, 205, 140, 570, false}, //ability 1
		{561, 170, 650, 255, 190, 570, false}, //ability 2
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
		hoverButtons.push_back(temp);
	}


	player2_units.clear();
	hoverButtons2.clear();

	RectData temp_units_pos2[] =
	{
		{480, 620, 580, 705, 640, 500, false}, //player 2 unit 1
		{480, 570, 580, 655, 590, 500, false}, //player 2 unit 2
		{480, 520, 580, 605, 540, 500, false}, //player 2 unit 3
		{480, 470, 580, 555, 490, 500, false}, //player 2 unit 4
		{561, 620, 650, 705, 640, 570, false}, //player 2 ability 1
		{561, 570, 650, 655, 590, 570, false}, //player 2 ability 2
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
		hoverButtons2.push_back(temp);
	}

	//////////////////////////////////////////////////////////
	//  Grid Buttons
	// T, L, B, R, X, Y, Highlighted, row, col
	RectData temp_grid_pos_row1[] =
	{
		//Row 1
		{int(m_gameBoard[0][1].getPosY()), int(m_gameBoard[0][1].getPosX() -20.0f),  int(m_gameBoard[0][1].getPosY() +100.0f), int(m_gameBoard[0][1].getPosX() +50.0f), m_gameBoard[0][1].getPosX(), m_gameBoard[0][1].getPosY(), false, 0, 1}, //1
		{int(m_gameBoard[0][2].getPosY()), int(m_gameBoard[0][2].getPosX() -20.0f),  int(m_gameBoard[0][2].getPosY() +100.0f), int(m_gameBoard[0][2].getPosX() +50.0f), m_gameBoard[0][2].getPosX(), m_gameBoard[0][2].getPosY(), false, 0, 2}, // 2
		{int(m_gameBoard[0][3].getPosY()), int(m_gameBoard[0][3].getPosX() -20.0f),  int(m_gameBoard[0][3].getPosY() +100.0f), int(m_gameBoard[0][3].getPosX() +50.0f), m_gameBoard[0][3].getPosX(), m_gameBoard[0][3].getPosY(), false, 0, 3},// 3
		{int(m_gameBoard[0][4].getPosY()), int(m_gameBoard[0][4].getPosX() -20.0f),  int(m_gameBoard[0][4].getPosY() +100), int(m_gameBoard[0][4].getPosX() +50), m_gameBoard[0][4].getPosX(), m_gameBoard[0][4].getPosY(), false, 0, 4},// 4
		{int(m_gameBoard[0][5].getPosY()), int(m_gameBoard[0][5].getPosX() -20.0f),  int(m_gameBoard[0][5].getPosY() +100), int(m_gameBoard[0][5].getPosX() +50), m_gameBoard[0][5].getPosX(), m_gameBoard[0][5].getPosY(), false, 0, 5},// 5
		{int(m_gameBoard[0][6].getPosY()), int(m_gameBoard[0][6].getPosX() -20.0f),  int(m_gameBoard[0][6].getPosY() +100), int(m_gameBoard[0][6].getPosX() +50), m_gameBoard[0][6].getPosX(), m_gameBoard[0][6].getPosY(), false, 0, 6},// 6
		{int(m_gameBoard[0][7].getPosY()), int(m_gameBoard[0][7].getPosX() -20.0f),	int(m_gameBoard[0][7].getPosY() +100), int(m_gameBoard[0][7].getPosX() +50), m_gameBoard[0][7].getPosX(), m_gameBoard[0][7].getPosY(), false, 0, 7},//7
		{int(m_gameBoard[0][8].getPosY()), int(m_gameBoard[0][8].getPosX() -20.0f),  int(m_gameBoard[0][8].getPosY() +100), int(m_gameBoard[0][8].getPosX() +50), m_gameBoard[0][8].getPosX(), m_gameBoard[0][8].getPosY(), false, 0, 8},//8
		{int(m_gameBoard[0][9].getPosY()), int(m_gameBoard[0][9].getPosX() -20.0f),  int(m_gameBoard[0][9].getPosY() +100), int(m_gameBoard[0][9].getPosX() +50), m_gameBoard[0][9].getPosX(), m_gameBoard[0][9].getPosY(), false, 0, 9},// 9
		{int(m_gameBoard[0][10].getPosY()), int(m_gameBoard[0][10].getPosX() -20.0f),  int(m_gameBoard[0][10].getPosY() +100), int(m_gameBoard[0][10].getPosX() +50),m_gameBoard[0][10].getPosX(), m_gameBoard[0][10].getPosY(), false, 0, 10},// 10
		{int(m_gameBoard[0][11].getPosY()), int(m_gameBoard[0][11].getPosX() -20.0f),  int(m_gameBoard[0][11].getPosY() +100), int(m_gameBoard[0][11].getPosX() +50), m_gameBoard[0][11].getPosX(), m_gameBoard[0][11].getPosY(), false, 0, 11},// 11
		{int(m_gameBoard[0][12].getPosY()), int(m_gameBoard[0][12].getPosX() -20.0f),  int(m_gameBoard[0][12].getPosY() +100), int(m_gameBoard[0][12].getPosX() +50), m_gameBoard[0][12].getPosX(), m_gameBoard[0][12].getPosY(), false, 0, 12},// 12
		{int(m_gameBoard[0][13].getPosY()), int(m_gameBoard[0][13].getPosX() -20.0f),  int(m_gameBoard[0][13].getPosY() +100), int(m_gameBoard[0][13].getPosX() +50), m_gameBoard[0][13].getPosX(), m_gameBoard[0][13].getPosY(), false, 0, 13},// 13
		{int(m_gameBoard[0][14].getPosY()), int(m_gameBoard[0][14].getPosX() -20.0f),  int(m_gameBoard[0][14].getPosY() +100), int(m_gameBoard[0][14].getPosX() +50), m_gameBoard[0][14].getPosX(), m_gameBoard[0][14].getPosY(), false,0, 14},// 14
		//Row 2
		{int(m_gameBoard[1][1].getPosY()), int(m_gameBoard[1][1].getPosX() -20),  int(m_gameBoard[1][1].getPosY() +90), int(m_gameBoard[1][1].getPosX() +50), m_gameBoard[1][1].getPosX(), m_gameBoard[1][1].getPosY(), false, 1, 1}, //1
		{int(m_gameBoard[1][2].getPosY()), int(m_gameBoard[1][2].getPosX() -20),  int(m_gameBoard[1][2].getPosY() +90), int(m_gameBoard[1][2].getPosX() +50), m_gameBoard[1][2].getPosX(), m_gameBoard[1][2].getPosY(), false, 1, 2}, // 2
		{int(m_gameBoard[1][3].getPosY()), int(m_gameBoard[1][3].getPosX() -20),  int(m_gameBoard[1][3].getPosY() +90), int(m_gameBoard[1][3].getPosX() +50), m_gameBoard[1][3].getPosX(), m_gameBoard[1][3].getPosY(), false, 1, 3},// 3
		{int(m_gameBoard[1][4].getPosY()), int(m_gameBoard[1][4].getPosX() -20),  int(m_gameBoard[1][4].getPosY() +90), int(m_gameBoard[1][4].getPosX() +50), m_gameBoard[1][4].getPosX(), m_gameBoard[1][4].getPosY(), false, 1, 4},// 4
		{int(m_gameBoard[1][5].getPosY()), int(m_gameBoard[1][5].getPosX() -20),  int(m_gameBoard[1][5].getPosY() +90), int(m_gameBoard[1][5].getPosX() +50), m_gameBoard[1][5].getPosX(), m_gameBoard[1][5].getPosY(), false, 1, 5},// 5
		{int(m_gameBoard[1][6].getPosY()), int(m_gameBoard[1][6].getPosX() -20),  int(m_gameBoard[1][6].getPosY() +90), int(m_gameBoard[1][6].getPosX() +50), m_gameBoard[1][6].getPosX(), m_gameBoard[1][6].getPosY(), false, 1, 6},// 6
		{int(m_gameBoard[1][7].getPosY()), int(m_gameBoard[1][7].getPosX() -20),  int(m_gameBoard[1][7].getPosY() +90), int(m_gameBoard[1][7].getPosX() +50), m_gameBoard[1][7].getPosX(), m_gameBoard[1][7].getPosY(), false, 1 ,7},//7
		{int(m_gameBoard[1][8].getPosY()), int(m_gameBoard[1][8].getPosX() -20),  int(m_gameBoard[1][8].getPosY() +90), int(m_gameBoard[1][8].getPosX() +50), m_gameBoard[1][8].getPosX(), m_gameBoard[1][8].getPosY(), false, 1, 8},//8
		{int(m_gameBoard[1][9].getPosY()), int(m_gameBoard[1][9].getPosX() -20),  int(m_gameBoard[1][9].getPosY() +90), int(m_gameBoard[1][9].getPosX() +50), m_gameBoard[1][9].getPosX(), m_gameBoard[1][9].getPosY(), false, 1 , 9},// 9
		{int(m_gameBoard[1][10].getPosY()), int(m_gameBoard[1][10].getPosX() -20),  int(m_gameBoard[1][10].getPosY() +90), int(m_gameBoard[1][10].getPosX() +50),m_gameBoard[1][10].getPosX(), m_gameBoard[1][10].getPosY(), false, 1, 10},// 10
		{int(m_gameBoard[1][11].getPosY()), int(m_gameBoard[1][11].getPosX() -20),  int(m_gameBoard[1][11].getPosY() +90), int(m_gameBoard[1][11].getPosX() +50), m_gameBoard[1][11].getPosX(), m_gameBoard[1][11].getPosY(), false, 1, 11},// 11
		{int(m_gameBoard[1][12].getPosY()), int(m_gameBoard[1][12].getPosX() -20),  int(m_gameBoard[1][12].getPosY() +90), int(m_gameBoard[1][12].getPosX() +50), m_gameBoard[1][12].getPosX(), m_gameBoard[1][12].getPosY(), false, 1, 12},// 12
		{int(m_gameBoard[1][13].getPosY()), int(m_gameBoard[1][13].getPosX() -20),  int(m_gameBoard[1][13].getPosY() +90), int(m_gameBoard[1][13].getPosX() +50), m_gameBoard[1][13].getPosX(), m_gameBoard[1][13].getPosY(), false, 1, 13},// 13
		{int(m_gameBoard[1][14].getPosY()), int(m_gameBoard[1][14].getPosX() -20),  int(m_gameBoard[1][14].getPosY() +90), int(m_gameBoard[1][14].getPosX() +50), m_gameBoard[1][14].getPosX(), m_gameBoard[1][14].getPosY(), false, 1, 14},// 14
		//Row 3
		{int(m_gameBoard[2][1].getPosY()), int(m_gameBoard[2][1].getPosX() -20),  int(m_gameBoard[2][1].getPosY() +100), int(m_gameBoard[2][1].getPosX() +50), m_gameBoard[2][1].getPosX(), m_gameBoard[2][1].getPosY(), false, 2, 1}, //1
		{int(m_gameBoard[2][2].getPosY()), int(m_gameBoard[2][2].getPosX() -20),  int(m_gameBoard[2][2].getPosY() +100), int(m_gameBoard[2][2].getPosX() +50), m_gameBoard[2][2].getPosX(), m_gameBoard[2][2].getPosY(), false, 2, 2}, // 2
		{int(m_gameBoard[2][3].getPosY()), int(m_gameBoard[2][3].getPosX() -20),  int(m_gameBoard[2][3].getPosY() +100), int(m_gameBoard[2][3].getPosX() +50), m_gameBoard[2][3].getPosX(), m_gameBoard[2][3].getPosY(), false, 2, 3},// 3
		{int(m_gameBoard[2][4].getPosY()), int(m_gameBoard[2][4].getPosX() -20),  int(m_gameBoard[2][4].getPosY() +100), int(m_gameBoard[2][4].getPosX() +50), m_gameBoard[2][4].getPosX(), m_gameBoard[2][4].getPosY(), false, 2, 4},// 4
		{int(m_gameBoard[2][5].getPosY()), int(m_gameBoard[2][5].getPosX() -20),  int(m_gameBoard[2][5].getPosY() +100), int(m_gameBoard[2][5].getPosX() +50), m_gameBoard[2][5].getPosX(), m_gameBoard[2][5].getPosY(), false, 2, 5},// 5
		{int(m_gameBoard[2][6].getPosY()), int(m_gameBoard[2][6].getPosX() -20),  int(m_gameBoard[2][6].getPosY() +100), int(m_gameBoard[2][6].getPosX() +50), m_gameBoard[2][6].getPosX(), m_gameBoard[2][6].getPosY(), false, 2, 6},// 6
		{int(m_gameBoard[2][7].getPosY()), int(m_gameBoard[2][7].getPosX() -20),  int(m_gameBoard[2][7].getPosY() +100), int(m_gameBoard[2][7].getPosX() +50), m_gameBoard[2][7].getPosX(), m_gameBoard[2][7].getPosY(), false, 2, 7},//7
		{int(m_gameBoard[2][8].getPosY()), int(m_gameBoard[2][8].getPosX() -20),  int(m_gameBoard[2][8].getPosY() +100), int(m_gameBoard[2][8].getPosX() +50), m_gameBoard[2][8].getPosX(), m_gameBoard[2][8].getPosY(), false, 2, 8},//8
		{int(m_gameBoard[2][9].getPosY()), int(m_gameBoard[2][9].getPosX() -20),  int(m_gameBoard[2][9].getPosY() +100), int(m_gameBoard[2][9].getPosX() +50), m_gameBoard[2][9].getPosX(), m_gameBoard[2][9].getPosY(), false, 2, 9},// 9
		{int(m_gameBoard[2][10].getPosY()), int(m_gameBoard[2][10].getPosX() -20),  int(m_gameBoard[2][10].getPosY() +100), int(m_gameBoard[2][10].getPosX() +50),m_gameBoard[2][10].getPosX(), m_gameBoard[2][10].getPosY(), false, 2, 10},// 10
		{int(m_gameBoard[2][11].getPosY()), int(m_gameBoard[2][11].getPosX() -20),  int(m_gameBoard[2][11].getPosY() +100), int(m_gameBoard[2][11].getPosX() +50), m_gameBoard[2][11].getPosX(), m_gameBoard[2][11].getPosY(), false, 2, 11},// 11
		{int(m_gameBoard[2][12].getPosY()), int(m_gameBoard[2][12].getPosX() -20),  int(m_gameBoard[2][12].getPosY() +100), int(m_gameBoard[2][12].getPosX() +50), m_gameBoard[2][12].getPosX(), m_gameBoard[2][12].getPosY(), false, 2, 12},// 12
		{int(m_gameBoard[2][13].getPosY()), int(m_gameBoard[2][13].getPosX() -20),  int(m_gameBoard[2][13].getPosY() +100), int(m_gameBoard[2][13].getPosX() +50), m_gameBoard[2][13].getPosX(), m_gameBoard[2][13].getPosY(), false, 2, 13},// 13
		{int(m_gameBoard[2][14].getPosY()), int(m_gameBoard[2][14].getPosX() -20),  int(m_gameBoard[2][14].getPosY() +100), int(m_gameBoard[2][14].getPosX() +50), m_gameBoard[2][14].getPosX(), m_gameBoard[2][14].getPosY(), false, 2, 14},// 14
		//Row 4
		{int(m_gameBoard[3][1].getPosY()), int(m_gameBoard[3][1].getPosX() -20),  int(m_gameBoard[3][1].getPosY() +100), int(m_gameBoard[3][1].getPosX() +50), m_gameBoard[3][1].getPosX(), m_gameBoard[3][1].getPosY(), false, 3, 1}, //1
		{int(m_gameBoard[3][2].getPosY()), int(m_gameBoard[3][2].getPosX() -20),  int(m_gameBoard[3][2].getPosY() +100), int(m_gameBoard[3][2].getPosX() +50), m_gameBoard[3][2].getPosX(), m_gameBoard[3][2].getPosY(), false, 3, 2}, // 2
		{int(m_gameBoard[3][3].getPosY()), int(m_gameBoard[3][3].getPosX() -20),  int(m_gameBoard[3][3].getPosY() +100), int(m_gameBoard[3][3].getPosX() +50), m_gameBoard[3][3].getPosX(), m_gameBoard[3][3].getPosY(), false, 3, 3},// 3
		{int(m_gameBoard[3][4].getPosY()), int(m_gameBoard[3][4].getPosX() -20),  int(m_gameBoard[3][4].getPosY() +100), int(m_gameBoard[3][4].getPosX() +50), m_gameBoard[3][4].getPosX(), m_gameBoard[3][4].getPosY(), false, 3, 4},// 4
		{int(m_gameBoard[3][5].getPosY()), int(m_gameBoard[3][5].getPosX() -20),  int(m_gameBoard[3][5].getPosY() +100), int(m_gameBoard[3][5].getPosX() +50), m_gameBoard[3][5].getPosX(), m_gameBoard[3][5].getPosY(), false, 3, 5},// 5
		{int(m_gameBoard[3][6].getPosY()), int(m_gameBoard[3][6].getPosX() -20),  int(m_gameBoard[3][6].getPosY() +100), int(m_gameBoard[3][6].getPosX() +50), m_gameBoard[3][6].getPosX(), m_gameBoard[3][6].getPosY(), false, 3, 6},// 6
		{int(m_gameBoard[3][7].getPosY()), int(m_gameBoard[3][7].getPosX() -20),  int(m_gameBoard[3][7].getPosY() +100), int(m_gameBoard[3][7].getPosX() +50), m_gameBoard[3][7].getPosX(), m_gameBoard[3][7].getPosY(), false, 3, 7},//7
		{int(m_gameBoard[3][8].getPosY()), int(m_gameBoard[3][8].getPosX() -20),  int(m_gameBoard[3][8].getPosY() +100), int(m_gameBoard[3][8].getPosX() +50), m_gameBoard[3][8].getPosX(), m_gameBoard[3][8].getPosY(), false, 3, 8},//8
		{int(m_gameBoard[3][9].getPosY()), int(m_gameBoard[3][9].getPosX() -20),  int(m_gameBoard[3][9].getPosY() +100), int(m_gameBoard[3][9].getPosX() +50), m_gameBoard[3][9].getPosX(), m_gameBoard[3][9].getPosY(), false, 3, 9},// 9
		{int(m_gameBoard[3][10].getPosY()), int(m_gameBoard[3][10].getPosX() -20),  int(m_gameBoard[3][10].getPosY() +100), int(m_gameBoard[3][10].getPosX() +50),m_gameBoard[3][10].getPosX(), m_gameBoard[3][10].getPosY(), false, 3, 10},// 10
		{int(m_gameBoard[3][11].getPosY()), int(m_gameBoard[3][11].getPosX() -20),  int(m_gameBoard[3][11].getPosY() +100), int(m_gameBoard[3][11].getPosX() +50), m_gameBoard[3][11].getPosX(), m_gameBoard[3][11].getPosY(), false, 3, 11},// 11
		{int(m_gameBoard[3][12].getPosY()), int(m_gameBoard[3][12].getPosX() -20),  int(m_gameBoard[3][12].getPosY() +100), int(m_gameBoard[3][12].getPosX() +50), m_gameBoard[3][12].getPosX(), m_gameBoard[3][12].getPosY(), false, 3, 12},// 12
		{int(m_gameBoard[3][13].getPosY()), int(m_gameBoard[3][13].getPosX() -20),  int(m_gameBoard[3][13].getPosY() +100), int(m_gameBoard[3][13].getPosX() +50), m_gameBoard[3][13].getPosX(), m_gameBoard[3][13].getPosY(), false, 3, 13},// 13
		{int(m_gameBoard[3][14].getPosY()), int(m_gameBoard[3][14].getPosX() -20),  int(m_gameBoard[3][14].getPosY() +100), int(m_gameBoard[3][14].getPosX() +50), m_gameBoard[3][14].getPosX(), m_gameBoard[3][14].getPosY(), false, 3, 14},// 14
		//Row 5
		{int(m_gameBoard[4][1].getPosY()), int(m_gameBoard[4][1].getPosX() -20),  int(m_gameBoard[4][1].getPosY() +90), int(m_gameBoard[4][1].getPosX() +50), m_gameBoard[4][1].getPosX(), m_gameBoard[4][1].getPosY(), false, 4, 1}, //1
		{int(m_gameBoard[4][2].getPosY()), int(m_gameBoard[4][2].getPosX() -20),  int(m_gameBoard[4][2].getPosY() +90), int(m_gameBoard[4][2].getPosX() +50), m_gameBoard[4][2].getPosX(), m_gameBoard[4][2].getPosY(), false, 4, 2}, // 2
		{int(m_gameBoard[4][3].getPosY()), int(m_gameBoard[4][3].getPosX() -20),  int(m_gameBoard[4][3].getPosY() +90), int(m_gameBoard[4][3].getPosX() +50), m_gameBoard[4][3].getPosX(), m_gameBoard[4][3].getPosY(), false, 4, 3},// 3
		{int(m_gameBoard[4][4].getPosY()), int(m_gameBoard[4][4].getPosX() -20),  int(m_gameBoard[4][4].getPosY() +90), int(m_gameBoard[4][4].getPosX() +50), m_gameBoard[4][4].getPosX(), m_gameBoard[4][4].getPosY(), false, 4, 4},// 4
		{int(m_gameBoard[4][5].getPosY()), int(m_gameBoard[4][5].getPosX() -20),  int(m_gameBoard[4][5].getPosY() +90), int(m_gameBoard[4][5].getPosX() +50), m_gameBoard[4][5].getPosX(), m_gameBoard[4][5].getPosY(), false, 4, 5},// 5
		{int(m_gameBoard[4][6].getPosY()), int(m_gameBoard[4][6].getPosX() -20),  int(m_gameBoard[4][6].getPosY() +90), int(m_gameBoard[4][6].getPosX() +50), m_gameBoard[4][6].getPosX(), m_gameBoard[4][6].getPosY(), false, 4, 6},// 6
		{int(m_gameBoard[4][7].getPosY()), int(m_gameBoard[4][7].getPosX() -20),  int(m_gameBoard[4][7].getPosY() +90), int(m_gameBoard[4][7].getPosX() +50), m_gameBoard[4][7].getPosX(), m_gameBoard[4][7].getPosY(), false, 4, 7},//7
		{int(m_gameBoard[4][8].getPosY()), int(m_gameBoard[4][8].getPosX() -20),  int(m_gameBoard[4][8].getPosY() +90), int(m_gameBoard[4][8].getPosX() +50), m_gameBoard[4][8].getPosX(), m_gameBoard[4][8].getPosY(), false, 4, 8},//8
		{int(m_gameBoard[4][9].getPosY()), int(m_gameBoard[4][9].getPosX() -20),  int(m_gameBoard[4][9].getPosY() +90), int(m_gameBoard[4][9].getPosX() +50), m_gameBoard[4][9].getPosX(), m_gameBoard[4][9].getPosY(), false, 4, 9},// 9
		{int(m_gameBoard[4][10].getPosY()), int(m_gameBoard[4][10].getPosX() -20),  int(m_gameBoard[4][10].getPosY() +90), int(m_gameBoard[4][10].getPosX() +50),m_gameBoard[4][10].getPosX(), m_gameBoard[4][10].getPosY(), false, 4, 10},// 10
		{int(m_gameBoard[4][11].getPosY()), int(m_gameBoard[4][11].getPosX() -20),  int(m_gameBoard[4][11].getPosY() +90), int(m_gameBoard[4][11].getPosX() +50), m_gameBoard[4][11].getPosX(), m_gameBoard[4][11].getPosY(), false, 4, 11},// 11
		{int(m_gameBoard[4][12].getPosY()), int(m_gameBoard[4][12].getPosX() -20),  int(m_gameBoard[4][12].getPosY() +90), int(m_gameBoard[4][12].getPosX() +50), m_gameBoard[4][12].getPosX(), m_gameBoard[4][12].getPosY(), false, 4, 12},// 12
		{int(m_gameBoard[4][13].getPosY()), int(m_gameBoard[4][13].getPosX() -20),  int(m_gameBoard[4][13].getPosY() +90), int(m_gameBoard[4][13].getPosX() +50), m_gameBoard[4][13].getPosX(), m_gameBoard[4][13].getPosY(), false, 4, 13},// 13
		{int(m_gameBoard[4][14].getPosY()), int(m_gameBoard[4][14].getPosX() -20),  int(m_gameBoard[4][14].getPosY() +90), int(m_gameBoard[4][14].getPosX() +50), m_gameBoard[4][14].getPosX(), m_gameBoard[4][14].getPosY(), false, 4, 14},// 14
	};

	for(int i = 0; i < MAX_COL_BUTTONS; i++)
	{
		Buttons temp;
		RECT rect;

		rect.top = temp_grid_pos_row1[i].t;
		rect.left = temp_grid_pos_row1[i].l;
		rect.bottom = temp_grid_pos_row1[i].b;
		rect.right = temp_grid_pos_row1[i].r;

		temp.setPosition(temp_grid_pos_row1[i].x, temp_grid_pos_row1[i].y);
		temp.setHighlight(temp_grid_pos_row1[i].highlight);
		temp.setRect(rect);

		temp.setRow(temp_grid_pos_row1[i].row);
		temp.setCol(temp_grid_pos_row1[i].col);

		map_grid_row1.push_back(temp);		
	}

	endTurnButton.clear();
	// T, L, B, R, X, Y, Highlighted, row, col
	RectData  temp_endButton_pos []=
	{
		{10, 400, 100, 600, 380, 460, false, 0,0}
	};

	Buttons temp;
	RECT rect;

	rect.top = temp_endButton_pos[0].t;
	rect.left = temp_endButton_pos[0].l;
	rect.bottom = temp_endButton_pos[0].b;
	rect.right = temp_endButton_pos[0].r;

	temp.setPosition(temp_endButton_pos[0].x, temp_endButton_pos[0].y);
	temp.setHighlight(temp_endButton_pos[0].highlight);
	temp.setRect(rect);

	endTurnButton.push_back(temp);

	//////////////////////////////////////////////////////////
	//  Reset m_gamePhase
	m_gamePhase								=		PLAYERONE_PLAYPHASE;
	m_fireBallActive						=		false;
	m_arrowActive							=		false;
	m_floatingTextActive					=		false;
	m_unitCurrentlyMoving					=		false;
	m_projectilePosX						=		0.0f;
	m_projectilePosY						=		0.0f;
	m_arrowForAttackingUnitPosX				=		0.0f;
	m_arrowForAttackingUnitPosY				=		0.0f;
	m_temporaryTimer						=		0.0f;
	m_attackingSpaceX						=		0;
	m_attackingSpaceY						=		0;
	m_attackTargetSpaceX					=		0;
	m_attackTargetSpaceY					=		0;
	m_moveToTarget							=		0;
	m_floatingTextRect.top					=		long(0.1);
	m_floatingTextRect.left					=		long(0.0);
	m_floatingTextRect.right				=		long(0.0);
	m_floatingTextRect.bottom				=		long(0.0);
	m_healthRect.top						=		0;
	m_healthRect.left						=		6;
	m_healthRect.right						=		90;
	m_healthRect.bottom						=		11;
	m_floatingRectTopMax					=		0.0f;
	m_floatingRectTimer						=		0.0f;
	m_damageType							=		-1;
	m_attackWillHitPlayer					=		false;
};

void GameEngine::InitMenu()
{
	menuButtons.clear();

	// T, L, B, R, X, Y, HIGHLIGHT, ROW, COL
	RectData menu_buttons_pos[] = 
	{
		{0, 100, 115, 600, 360, 250, false, 0, 0},
		{230, 100, 350, 500, 360, 350, false, 0, 0},
		{121, 100, 230, 500, 360, 420, false, 0, 0},
		{345, 100, 460, 300, 360, 500, false, 0, 0},
	};

	for(int i = 0; i < 4; i++)
	{
		Buttons temp;
		RECT rect;

		rect.top = menu_buttons_pos[i].t; 
		rect.left = menu_buttons_pos[i].l;
		rect.bottom =  menu_buttons_pos[i].b; 
		rect.right =  menu_buttons_pos[i].r;

		temp.setPosition( menu_buttons_pos[i].x, menu_buttons_pos[i].y);
		temp.setHighlight( menu_buttons_pos[i].highlight);
		temp.setRect(rect);

		menuButtons.push_back(temp);
	}

	characterButtons.clear();
	// T, L, B, R, X, Y, HIGHLIGHT, ROW, COL
	RectData character_pos[] =
	{
		{200, 60, 400, 140, 100, 300, false, 0, 0},
		{200, 260, 400, 340, 300, 300, false, 0, 0},
		{200, 460, 400, 540, 500, 300, false, 0, 0},
	};

	for(int i = 0; i < 3; i++)
	{
		Buttons temp;
		RECT rect;

		rect.top = character_pos[i].t;
		rect.left = character_pos[i].l;
		rect.bottom = character_pos[i].b;
		rect.right = character_pos[i].r;

		temp.setPosition( character_pos[i].x, character_pos[i].y);
		temp.setHighlight( character_pos[i].highlight);
		temp.setRect(rect);

		characterButtons.push_back(temp);
	}
}

void GameEngine::Update(float dt)
{
	m_player[0].adjustCurrentSpecial(100);
	m_player[1].adjustCurrentSpecial(100);
	///////////////////////////////////////////////////////////////////////////////
	//  INFO:  Used with tools to get animations working, leave this in
	int animationOffsetLeft = 47;
	int animationOffsetRight = 48;
	//////////////////////////////////////////////////////////////////////////
	// Get and Acquire Keyboard Input
	// Get the input device state
	HRESULT hr;
	hr = m_pDIKeyboard->Acquire();
	if( FAILED(hr) )
	{
		ZeroMemory(buffer, sizeof(buffer));	
		hr = m_pDIKeyboard->Acquire();				
	}else									
		hr = m_pDIKeyboard->GetDeviceState( sizeof(buffer),  (LPVOID)&buffer ); 

	//////////////////////////////////////////////////////////////////////////
	// Get and Acquire Mouse Input
	hr = m_pDIMouse->Acquire();

	if( FAILED(hr) )
	{
		ZeroMemory(&mouseState, sizeof( DIMOUSESTATE2 ) ); 
		hr = m_pDIMouse->Acquire();								
	}else													
		hr = m_pDIMouse->GetDeviceState( sizeof( DIMOUSESTATE2 ), &mouseState); 

	calcDeltaTime();

	fmodSystem->update();
	//////////////////////////////////////////////////////////////////////////
	// Mouse/Cursor updates
	myMouse.update(mouseState.lX, mouseState.lY);
	cursor.x = myMouse.cursorPos.x;
	cursor.y = myMouse.cursorPos.y;

	int selected = 0;

	switch(m_gameState)
	{
	case MENUMAIN:
		for(auto &Buttons: menuButtons)
		{
			if(Buttons.isOn(cursor.x, cursor.y, 3))
			{
				Buttons.setColor(D3DCOLOR_ARGB(255,255,255,0));
				Buttons.setHighlight(true);
			}
			else
			{
				Buttons.setColor(D3DCOLOR_ARGB(255,255,255,255));
				Buttons.setHighlight(false);
			}

			if(mouseState.rgbButtons[0])
			{
				for(int i = 0; i < 4; i++)
					if(menuButtons[i].isHighlighted())
						selected = i;

				switch(selected)
				{
				case 0:
					m_gameState	= MENUCHARACTERSELECT;
					break;
				case 1:
					m_gameState = OPTIONS;
					break;
				case 2:
					m_gameState = MENUCREDITS;
					break;
				case 3:
					m_gameState = QUIT;
					break;
				}
			}
		}
		break;
	case MENUCHARACTERSELECT:
		// HAVE 3 BUTTONS - 1 FOR EACH CHARACTER
		if(!player1selected)
		{
			for(auto &Buttons: characterButtons)
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3))
				{
					Buttons.setColor(D3DCOLOR_ARGB(255,255,255,0));
					Buttons.setHighlight(true);
				}
				else
				{
					Buttons.setColor(D3DCOLOR_ARGB(255,255,255,255));
					Buttons.setHighlight(false);
				}

				if(mouseState.rgbButtons[0])
				{
					if(!keyIsDown[DIK_K])
					{
						keyIsDown[DIK_K] = true;

						for(int i = 0; i < 3; i++)
							if(characterButtons[i].isHighlighted())
								selected = i;

						switch(selected)
						{
						case 0:
							m_player[0].setCharacterType(1);
							player1selected = true;
							break;
						case 1:
							m_player[0].setCharacterType(2);
							player1selected = true;
							break;
						case 2:
							m_player[0].setCharacterType(0);
							player1selected = true;
							break;
						}
					}
				}
				else
					keyIsDown[DIK_K] = false;
			}
		}

		if(!player2selected && player1selected)
		{
			for(auto &Buttons: characterButtons)
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3))
				{
					Buttons.setColor(D3DCOLOR_ARGB(255,255,255,0));
					Buttons.setHighlight(true);
				}
				else
				{
					Buttons.setColor(D3DCOLOR_ARGB(255,255,255,255));
					Buttons.setHighlight(false);
				}

				if(mouseState.rgbButtons[0])
				{
					if(!keyIsDown[DIK_K])
					{
						keyIsDown[DIK_K] = true;

						for(int i = 0; i < 3; i++)
							if(characterButtons[i].isHighlighted())
								selected = i;

						switch(selected)
						{
						case 0:
							m_player[1].setCharacterType(1);
							player2selected = true;
							break;
						case 1:
							m_player[1].setCharacterType(2);
							player2selected = true;
							break;
						case 2:
							m_player[1].setCharacterType(0);
							player2selected = true;
							break;
						}
					}
				}
				else
					keyIsDown[DIK_K] = false;
			}
		}

		if(buffer[DIK_RETURN] & 0x80)
		{
			if(!keyIsDown[DIK_RETURN])
			{
				keyIsDown[DIK_RETURN] = true;
				m_gameState = MENUMAIN;
			}
		}
		else
			keyIsDown[DIK_RETURN] = false;

		if(player1selected && player2selected)
			m_gameState = BATTLE;

		break;
	case OPTIONS:
		if(buffer[DIK_RETURN] & 0x80)
		{
			if(!keyIsDown[DIK_RETURN])
			{
				keyIsDown[DIK_RETURN] = true;
				m_gameState = MENUMAIN;
			}
		}
		else
			keyIsDown[DIK_RETURN] = false;
		break;
	case MENUCREDITS:
		if(buffer[DIK_RETURN] & 0x80)
		{
			if(!keyIsDown[DIK_RETURN])
			{
				keyIsDown[DIK_RETURN] = true;
				m_gameState = MENUMAIN;
			}
		}
		else
			keyIsDown[DIK_RETURN] = false;
		break;
	case OVERWORLD:
		break;
	case BATTLE:
		if(!channel)
			fmodSystem->playSound( FMOD_CHANNEL_FREE, battleTheme, false, &channel);

		fmodSystem->update();

		///////////////////////////////////////////////////////////
		//  INFO:  Check if player(s) click on the end turn button
		for(auto & Buttons: endTurnButton)
		{
			if(Buttons.isOn(cursor.x, cursor.y, 3))
			{
				Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 0));
				Buttons.setHighlight(true);
			}
			else
			{
				Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 255));
				Buttons.setHighlight(false);
			}

			if((mouseState.rgbButtons[0] & 0x80) != 0)
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3))
				{
					if(!keyIsDown[DIK_9])
					{
						keyIsDown[DIK_9]  = true;
						changePhase();
					}
				}
			}
			else
				keyIsDown[DIK_9] = false;
		}

		switch(m_gamePhase)
		{
		case PLAYERONE_PLAYPHASE:
			m_classAbilityAnimator.updateClassAbilityAnimation( dt );
			break;
		case PLAYERONE_EVENTPHASE:
			updateEventPhase(dt);
			break;
		case PLAYERTWO_PLAYPHASE:
			m_classAbilityAnimator.updateClassAbilityAnimation( dt );
			break;
		case PLAYERTWO_EVENTPHASE:
			updateEventPhase(dt);
			break;
		};

		/////////////////////////////////////////////////////////////////////////////
		//  INFO:  Used as tools to get animations working, leave this in.
		if( buffer[DIK_RIGHT] &0x80 )
		{
			if( !keyIsDown[DIK_RIGHT] )
			{
				m_classAbilityAnimator.setAnimationActive( true );


				keyIsDown[DIK_RIGHT] = true;
				m_tester++;
				switch(m_tester)
				{
				case 1:
					m_classAbilityAnimator.setClassAbilityAnimation( FLAMESTRIKE , m_gameBoard[0][0].getPosX(), m_gameBoard[0][0].getPosY() );
					break;
				default:
					m_classAbilityAnimator.adjustAnimationRectLeftRight( animationOffsetLeft, animationOffsetRight );
					animationOffsetLeft	=	140;
					animationOffsetRight	=	142;
					break;
				case 4:
					animationOffsetLeft		=	139;
					animationOffsetRight	=	141;
					break;
				}
				m_unit[1][1].setUnitRect( m_unit[1][1].getUnitRect().top, m_unit[1][1].getUnitRect().left + animationOffsetLeft, m_unit[1][1].getUnitRect().right + animationOffsetRight, m_unit[1][1].getUnitRect().bottom );
			}
		}
		else
			keyIsDown[DIK_RIGHT]	=	false;
		/////////////////////////////////////////////////////////////////////////////
		//  INFO:  Used as tools to get animations working, leave this in.
		if( buffer[DIK_LEFT] &0x80 )
		{
			if( !keyIsDown[DIK_LEFT] )
			{
				m_classAbilityAnimator.setAnimationActive( false );

				keyIsDown[DIK_LEFT] = true;
				m_tester--;
				switch(m_tester){
				default:
					m_classAbilityAnimator.adjustAnimationRectLeftRight( -animationOffsetLeft, -animationOffsetRight );
					animationOffsetLeft		=	140;
					animationOffsetRight	=	142;
					break;
				case 4:
					animationOffsetLeft		=	120;
					animationOffsetRight	=	141;
					break;
				}
				m_unit[1][1].setUnitRect( m_unit[1][1].getUnitRect().top, m_unit[1][1].getUnitRect().left - animationOffsetLeft, m_unit[1][1].getUnitRect().right - animationOffsetRight, m_unit[1][1].getUnitRect().bottom );
			}
		}
		else
			keyIsDown[DIK_LEFT]	=	false;
		///////////////////////////////////////////////////////////
		//  INFO:  Updates unit animations
		updateAnimations(dt);
		//////////////////////////////////////////////////
		// Check for units icon hover over and pressing
		if(m_player[0].checkIfActivePlayer())
		{
			int selected = 99;
			int row = 0;
			int col = 0;

			bool locationFound = false;
			bool unitFound = false;
			// Check for mouse over UNIT buttons
			for(auto &Buttons: player1_units)
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3))
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					Buttons.setHighlight(true);
				}
				else
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 255));
					Buttons.setHighlight(false);
				}
				// Check for mouse click
				if(mouseState.rgbButtons[0])
				{
					// Depending on which UNIT mouse is over we take that number
					for(int i = 0; i < 6; i++)
						if(player1_units[i].isHighlighted())
						{
							selected = i;
							selectedUnit = selected;
						}
				}
				//Checking which unit was selected and checks players gold
				//Each unit has different amount of gold cost
				dontPlaceUnit = false;

				if(selected == 0 && m_player[0].getGold() < 100)
					noGold = true;

				if(selected == 1 && m_player[0].getGold() < 125)
					noGold = true;

				if(selected == 2 && m_player[0].getGold() < 250)
					noGold = true;

				if(selected == 3 && m_player[0].getGold() < 75)
					noGold = true;
			}

			for(auto &Buttons: hoverButtons)
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3))
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					Buttons.setHighlight(true);
				}
				else
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 255));
					Buttons.setHighlight(false);
				}
				// Check for mouse click
				if(mouseState.rgbButtons[0])
				{
					// Depending on which UNIT mouse is over we take that number
					for(int i = 0; i < 4; i++)
						if(hoverButtons[i].isHighlighted())
						{
							selected = i;
							selectedHover = selected;
						}
				}
			}

			// Check for mouse over GRID buttons
			for(auto& Buttons: map_grid_row1) 
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3))
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					Buttons.setHighlight(true);
				}else
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 255));
					Buttons.setHighlight(false);
				}
				// Check if mouse is over GRID button and mouse was clicked
				if(Buttons.isOn(cursor.x, cursor.y, 3) && (mouseState.rgbButtons[0]& 0x80) != 0 && !keyIsDown[DIK_P])
				{
					row = Buttons.row;
					col = Buttons.col;
					locationFound = true;

					keyIsDown[DIK_P] = true;

					if(locationFound)
					{
						bool done = false;
						switch(selectedUnit)
						{
						case 0: // first unit(s)
							if(m_player[0].getGold() < 100)
								dontPlaceUnit = true;

							if(!m_unit[row][col].occupied && !dontPlaceUnit && col < 6)
							{
								if(m_player[0].getCharacterType() == ARCHER)
								{
									m_unit[row][col].addUnit(ARCHERUNIT, PLAYERONE);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[0].adjustGold(-100);
								}

								if(m_player[0].getCharacterType() == BLACKMAGE)
								{
									m_unit[row][col].addUnit(BLACKMAGEUNIT, PLAYERONE);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[0].adjustGold(-100);
								}

								if(m_player[0].getCharacterType() == WARRIOR)
								{
									m_unit[row][col].addUnit(WARRIORUNIT, PLAYERONE);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[0].adjustGold(-100);
								}
							}
							break;
						case 1: // second unit(s)
							if(m_player[0].getGold() < 125)
								dontPlaceUnit = true;

							if(!m_unit[row][col].occupied && !dontPlaceUnit && col < 6)
							{
								if(m_player[0].getCharacterType() == ARCHER) // wolf
								{
									m_unit[row][col].addUnit(WOLF, PLAYERONE);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[0].adjustGold(-125);
								}

								if(m_player[0].getCharacterType() == BLACKMAGE) // golem
								{
									m_unit[row][col].addUnit(GOLEM, PLAYERONE);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[0].adjustGold(-125);
								}

								if(m_player[0].getCharacterType() == WARRIOR) // Marksman
								{
									m_unit[row][col].addUnit(MARKSMAN, PLAYERONE);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[0].adjustGold(-125);
								}
							}
							break;
						case 2: // third unit(s)
							if(m_player[0].getGold() < 250)
								dontPlaceUnit = true;

							if(!m_unit[row][col].occupied && !dontPlaceUnit && col < 6)
							{
								if(m_player[0].getCharacterType() == ARCHER) // thief
								{
									m_unit[row][col].addUnit(THIEF, PLAYERONE);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[0].adjustGold(-250);
								}
								if(m_player[0].getCharacterType() == BLACKMAGE) // warlock
								{
									m_unit[row][col].addUnit(WARLOCK, PLAYERONE);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[0].adjustGold(-250);
								}

								if(m_player[0].getCharacterType() == WARRIOR)
								{
									m_unit[row][col].addUnit(MINOTAUR, PLAYERONE);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[0].adjustGold(-250);
								}
							}
							break;
						case 3: // wall
							if(m_player[0].getGold() < 75)
								dontPlaceUnit = true;

							if(!m_unit[row][col].occupied && !dontPlaceUnit && col < 7)
							{
								m_unit[row][col].addUnit(WALL, PLAYERONE);
								m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
								m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
								m_player[0].adjustGold(-75);
							}
							break;
						case 4: // Ability 1
							if(m_player[0].getCurrentSpecial() >= 50)
							{
								if(m_player[0].getCharacterType() == ARCHER) // Split shot = Kills 2 enemy units of player's choosing
								{
									archerAbility1(done);

									if(!done && unitsAttacked != 2)
										archerAbility1(done);
									else
									{
										unitsAttacked = 0;
										m_player[0].adjustCurrentSpecial(-50);
										break;
									}
								}

								if(m_player[0].getCharacterType() == BLACKMAGE) // Black hole
									blackHoleAbility(row, col, 0);

								if(m_player[0].getCharacterType() == WARRIOR) // ??
								{

								}
							}
							break;
						case 5: // Ability 2
							if(m_player[0].getCharacterType() == ARCHER) // Precision Shot
							{
								if(m_player[0].getCurrentSpecial() == 100)
								{
									m_player[0].adjustCurrentSpecial(-100);
									m_player[1].adjustCurrentHealth(-100);
								}
							}

							if(m_player[0].getCharacterType() == BLACKMAGE) //  Flame Wave
								flameWaveAbility(row, col, 0);

							if(m_player[0].getCharacterType() == WARRIOR)
							{}

							break;
						default:
							break;
						}	
					}

				}
				else
					keyIsDown[DIK_P] = false;
			}
		}
		// All player 2 checks and actions
		if(m_player[1].checkIfActivePlayer())
		{
			int selected = 99;
			int row = 0;
			int col = 0;

			bool unitPlaced = false;
			bool locationFound = false;

			for(auto &Buttons: player2_units)
			{
				if(Buttons.isOn(cursor.x, cursor.y, int(3.25) ) )
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					Buttons.setHighlight(true);
				}
				else
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 255));
					Buttons.setHighlight(false);
				}

				if(mouseState.rgbButtons[0])
				{
					for(int i = 0; i < 6; i++)
						if(player2_units[i].isHighlighted())
						{
							selected = i;
							selectedUnit = selected;
						}
				}
				dontPlaceUnit = false;

				if(selected == 0 && m_player[1].getGold() < 100)
					noGold = true;

				if(selected == 1 && m_player[1].getGold() < 125)
					noGold = true;

				if(selected == 2 && m_player[1].getGold() < 250)
					noGold = true;

				if(selected == 3 && m_player[1].getGold() < 75)
					noGold = true;
			}
			//Used to dispaly hover info for units for second player
			for(auto &Buttons: hoverButtons2)
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3))
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					Buttons.setHighlight(true);
				}
				else
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 255));
					Buttons.setHighlight(false);
				}
				// Check for mouse click
				if(mouseState.rgbButtons[0])
				{
					// Depending on which UNIT mouse is over we take that number
					for(int i = 0; i < 4; i++)
						if(hoverButtons2[i].isHighlighted())
						{
							selected = i;
							selectedHover = selected;
						}
				}
			}

			for(auto& Buttons: map_grid_row1) 
			{
				if(Buttons.isOn(cursor.x, cursor.y, 3) )
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 0));
					Buttons.setHighlight(true);
				}else
				{
					Buttons.setColor(D3DCOLOR_ARGB(255, 255, 255, 255));
					Buttons.setHighlight(false);
				}

				if(Buttons.isOn(cursor.x, cursor.y, 3) && mouseState.rgbButtons[0] && !keyIsDown[DIK_P])
				{
					row = Buttons.row;
					col = Buttons.col;
					locationFound = true;

					keyIsDown[DIK_P] = true;

					if(locationFound)
					{
						bool done = false;
						switch(selectedUnit)
						{
						case 0: // first unit(s)
							if(m_player[1].getGold() < 100)
								dontPlaceUnit = true;

							if(!m_unit[row][col].occupied && !dontPlaceUnit && col > 9)
							{
								if(m_player[1].getCharacterType() == ARCHER)
								{
									m_unit[row][col].addUnit( ARCHERUNIT, PLAYERTWO );
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[1].adjustGold(-100);
								}
								if(m_player[1].getCharacterType() == BLACKMAGE)
								{
									m_unit[row][col].addUnit( BLACKMAGEUNIT, PLAYERTWO );
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[1].adjustGold(-100);
								}

								if(m_player[1].getCharacterType() == WARRIOR)
								{
									m_unit[row][col].addUnit(WARRIORUNIT, PLAYERTWO);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[1].adjustGold(-100);
								}
								unitPlaced = true;
							}
							break;
						case 1: // second unit(s)
							if(m_player[1].getGold() < 125)
								dontPlaceUnit = true;

							if(!m_unit[row][col].occupied && !dontPlaceUnit && col > 9)
							{
								if(m_player[1].getCharacterType() == ARCHER) // wolf
								{
									m_unit[row][col].addUnit( WOLF, PLAYERTWO );
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[1].adjustGold(-125);
								}
								if(m_player[1].getCharacterType() == BLACKMAGE) // golem
								{
									m_unit[row][col].addUnit( GOLEM, PLAYERTWO);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[1].adjustGold(-125);
								}

								if(m_player[1].getCharacterType() == WARRIOR)
								{
									m_unit[row][col].addUnit(MARKSMAN, PLAYERTWO);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[1].adjustGold(-125);
								}
								unitPlaced = true;
							}
							break;
						case 2: // third unit(s)
							if(m_player[1].getGold() < 250)
								dontPlaceUnit = true;

							if(!m_unit[row][col].occupied && !dontPlaceUnit && col > 9)
							{
								if(m_player[1].getCharacterType() == ARCHER) // thief
								{
									m_unit[row][col].addUnit( THIEF, PLAYERTWO );
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[1].adjustGold(-250);
								}

								if(m_player[1].getCharacterType() == BLACKMAGE) // warlock
								{
									m_unit[row][col].addUnit( WARLOCK, PLAYERTWO );
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[1].adjustGold(-250);
								}

								if(m_player[1].getCharacterType() == WARRIOR) // minotaur
								{
									m_unit[row][col].addUnit(MINOTAUR, PLAYERTWO);
									m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
									m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
									m_player[1].adjustGold(-250);
								}
								unitPlaced = true;
							}
							break;
						case 3: // walls
							if(m_player[1].getGold() < 75)
								dontPlaceUnit = true;

							if(!m_unit[row][col].occupied && !dontPlaceUnit && col > 8)
							{
								m_unit[row][col].addUnit( WALL, PLAYERTWO );
								m_unit[row][col].setPosX(m_gameBoard[row][col].getPosX());
								m_unit[row][col].setPosY(m_gameBoard[row][col].getPosY());
								m_player[1].adjustGold(-75);

								unitPlaced = true;
							}
							break;
						case 4: // Ability 1
							if(m_player[1].getCurrentSpecial() >= 50)
							{
								//allow ability or don't based on meter
								if(m_player[1].getCharacterType() == ARCHER) // Split shot = Kills 2 enemy units of player's choosing
								{
									archerAbility1(done);

									if(!done && unitsAttacked != 2)
										archerAbility1(done);
									else
									{
										unitsAttacked = 0;
										m_player[1].adjustCurrentSpecial(-50);
										break;
									}
								}

								if(m_player[1].getCharacterType() == BLACKMAGE) // Black hole
									blackHoleAbility(row, col, 1);

								if(m_player[1].getCharacterType() == WARRIOR) // ??
								{

								}

							}
							break;
						case 5: // Ability 2
							//Get row and destroy all units in that row
							if(m_player[1].getCharacterType() == ARCHER) // Precision Shot
							{
								if(m_player[1].getCurrentSpecial() == 100)
								{
									m_player[1].adjustCurrentSpecial(-100);
									m_player[0].adjustCurrentHealth(-100);
								}
							}

							if(m_player[1].getCharacterType() == BLACKMAGE) //  Flame Wave
								flameWaveAbility(row, col, 1);

							if(m_player[1].getCharacterType() == WARRIOR) // ??
							{}
							break;
						default:
							break;
						}
					}
				}
				else if (!mouseState.rgbButtons[0] )
					keyIsDown[DIK_P] = false;
			}
		}

		if(m_player[0].currentHealth < 0)
			m_gameState = PLAYER2WIN;
		else if( m_player[1].currentHealth < 0)
			m_gameState = PLAYER1WIN;

		break;
	case QUIT:
		DestroyWindow(m_hWnd);
		break;
	case PLAYER1WIN:
		if(buffer[DIK_RETURN] & 0x80)
		{
			if(!keyIsDown[DIK_RETURN])
			{
				keyIsDown[DIK_RETURN] = true;
				m_gameState = MENUMAIN;
			}
		}
		else
			keyIsDown[DIK_RETURN] = false;
		break;
	case PLAYER2WIN:
		if(buffer[DIK_RETURN] & 0x80)
		{
			if(!keyIsDown[DIK_RETURN])
			{
				keyIsDown[DIK_RETURN] = true;
				m_gameState = MENUMAIN;
			}
		}
		else
			keyIsDown[DIK_RETURN] = false;
		break;
	}
	int test = 0;
	if( buffer[DIK_DOWN] & 0x80  )
		test = 1;
}

void GameEngine::updateAnimations(float dt)
{
	for(int i = 0; i < MAXBOARDHEIGHT; ++i)
	{
		for(int j = 0; j < MAXBOARDWIDTH; ++j)
		{
			if(m_unit[i][j].checkIfActive())
				m_unit[i][j].updateAnimations(dt);
		}
	}
};

void GameEngine::updateEventPhase(float dt)
{
	if(m_floatingTextActive)
	{
		m_floatingRectTimer += 100 * dt;
		if(m_floatingRectTimer > 2.0f)
		{
			m_floatingTextRect.top -=  1;
			m_floatingRectTimer -= 2.0f;
		}
		if(m_floatingRectTopMax > m_floatingTextRect.top )
		{
			m_floatingRectTopMax			=		long(0.0);
			m_floatingTextRect.top		=		long(0.0);
			m_floatingTextRect.left		=		long(0.0);
			m_floatingTextRect.right		=		long(0.0);
			m_floatingTextRect.bottom	=		long(0.0);
			m_floatingTextActive			=		false;
			m_combatMessageActive		=		false;
			ZeroMemory( m_combatMessage, 128 );
			if( !m_lightningActive )
			{
				m_damageType				=		-1;
				m_unitCurrentlyAttacking	=		false;
				m_unit[m_attackingSpaceX][m_attackingSpaceY].setUnitCanUseAbility( false );
				m_unit[m_attackingSpaceX][m_attackingSpaceY].setState( IDLE );
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	//  INFO:  If no unit is attacking or moving, find the next unit to take action
	if(!m_unitCurrentlyAttacking && !m_unitCurrentlyMoving && !m_floatingTextActive)
	{
		if(m_gamePhase == PLAYERONE_EVENTPHASE)
		{
			for(int i = 0; i < MAXBOARDHEIGHT; ++i)
			{
				int trial = 0;
				for(int j = MAXBOARDWIDTH-1; j > 0; --j)
				{
					if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERONE && m_unit[i][j].canUnitTakeAction() && m_unit[i][j].getType() > GOLDMINES)
					{
						m_unitCurrentlyAttacking = true;
						m_arrowForAttackingUnitPosX	=	m_gameBoard[i][j].getPosX() + 10;
						m_arrowForAttackingUnitPosY	=	m_gameBoard[i][j].getPosY() - 40;
						switch(m_unit[i][j].getType())
						{
						case NOUNIT:
							break;
						case GOLDMINES:
							break;
						case WALL:
							m_unitCurrentlyAttacking = false;
							break;
						case WARRIORUNIT:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							findNextTarget( i, j );
							return;
						case MARKSMAN:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_arrowActive = true;
							findNextTarget( i, j );
							fmodSystem->playSound( FMOD_CHANNEL_FREE, shootArrow, false, 0 );
							return;
						case MINOTAUR:
							m_unitCurrentlyAttacking = false;
							if( m_unit[i][j].canUnitUseAbility() && !m_floatingTextActive ){
								m_floatingTextActive	=	true;
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								m_unit[i][j].setState( ABILITY );
								int offset			=	0;
								if( m_gamePhase == PLAYERONE_EVENTPHASE ) 
									offset += 600;
								////////////////////////////////////////////////////////////////
								m_randomNumber	=	rand()%2;
								if( m_randomNumber	==	0 )
								{
									m_combatMessageActive = true;
									swprintf_s(m_combatMessage, 128, L"Enrage!!");
									m_unit[i][j].setDamage( m_unit[i][j].getDamage() + 15 );
									m_floatingRectTopMax	=	m_unit[i][j].getPosY() - 50;
									m_floatingTextRect.top = long(m_unit[i][j].getPosY() - 10);
									m_floatingTextRect.left = long(m_unit[i][j].getPosX() - 5);
									m_floatingTextRect.right = long(m_unit[i][j].getPosX() + 10);
									m_floatingTextRect.bottom = long(m_unit[i][j].getPosY() + 10);
									m_damageType	=	m_unit[i][j].getDamage();
									fmodSystem->playSound( FMOD_CHANNEL_FREE, minotaurRoar, false, 0 );
								}
								else
								{
									m_combatMessageActive = true;
									swprintf_s(m_combatMessage, 128, L"Failed!!");
									m_floatingRectTopMax	=	m_unit[i][j].getPosY() - 50;
									m_floatingTextRect.top = long(m_unit[i][j].getPosY() - 10);
									m_floatingTextRect.left = long(m_unit[i][j].getPosX() - 5);
									m_floatingTextRect.right = long(m_unit[i][j].getPosX() + 10);
									m_floatingTextRect.bottom = long(m_unit[i][j].getPosY() + 10);
									m_damageType	=	m_unit[i][j].getDamage();
								}
								//  INFO:  Set data for floating text

							}
							else
							{
								m_unitCurrentlyAttacking = true;
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								findNextTarget( i, j );
							}
							return;
						case WOLF:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							findNextTarget( i, j );
							return;
						case ARCHERUNIT:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_arrowActive = true;
							findNextTarget( i, j );
							fmodSystem->playSound( FMOD_CHANNEL_FREE, shootArrow, false, 0 );
							return;
						case THIEF:
							m_unitCurrentlyAttacking = false;
							if( m_unit[i][j].canUnitUseAbility() && !m_floatingTextActive )
							{
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								m_unit[i][j].setState( ABILITY );
								int offset					=	0;
								if( m_gamePhase == PLAYERONE_EVENTPHASE ) 
									offset += 600;
								////////////////////////////////////////////////////////////////
								//  INFO:  Set data for floating text
								m_combatMessageActive = true;
								swprintf_s(m_combatMessage, 128, L"Steal!!");
								m_floatingRectTopMax	=	m_unit[i][j].getPosY() - 50;
								m_floatingTextRect.top = long(m_unit[i][j].getPosY() - 10);
								m_floatingTextRect.left = long(m_unit[i][j].getPosX() - 5);
								m_floatingTextRect.right = long(m_unit[i][j].getPosX() + 10);
								m_floatingTextRect.bottom = long(m_unit[i][j].getPosY() + 10);
								m_floatingTextActive = true;
								fmodSystem->playSound( FMOD_CHANNEL_FREE, goldCoins, false, 0 );
								if( m_player[1].getGold() > 30 )
									m_damageType	=	30;
								else
									m_damageType	=	m_player[1].getGold();
								if(m_player[1].getGold() > 15)
								{
									m_player[1].adjustGold( - 15 );
									m_player[0].adjustGold( 15 );
								}
								else
								{
									m_player[0].adjustGold(m_player[1].getGold());
									m_player[1].adjustGold(-m_player[1].getGold());
								}
							}
							else
							{
								m_unitCurrentlyAttacking = true;
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								findNextTarget( i, j );
							}
							return;
						case GOLEM:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							findNextTarget( i, j );
							return;
						case BLACKMAGEUNIT:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_fireBallActive = true;
							findNextTarget( i, j );
							fmodSystem->playSound( FMOD_CHANNEL_FREE, castFireball, false, 0);
							return;
						case WARLOCK:
							m_unitCurrentlyAttacking = false;
							if( m_unit[i][j].canUnitUseAbility() && !m_floatingTextActive )
							{
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								m_unit[i][j].setState( ABILITY );
								int offset					=	0;
								if( m_gamePhase == PLAYERONE_EVENTPHASE ) 
									offset += 600;
								////////////////////////////////////////////////////////////////
								//  INFO:  Set data for floating text
								m_combatMessageActive = true;
								swprintf_s(m_combatMessage, 128, L"Drain!!");
								m_floatingRectTopMax	=	m_unit[i][j].getPosY() - 50;
								m_floatingTextRect.top = long(m_unit[i][j].getPosY() - 10);
								m_floatingTextRect.left = long(m_unit[i][j].getPosX() - 5);
								m_floatingTextRect.right = long(m_unit[i][j].getPosX() + 10);
								m_floatingTextRect.bottom = long(m_unit[i][j].getPosY() + 10);
								m_damageType	=	m_unit[i][j].getDamage();
								m_floatingTextActive = true;
								m_damageType	=	15;
								m_player[1].adjustCurrentHealth( - 5 );
								m_player[0].adjustCurrentHealth( 5 );
								fmodSystem->playSound( FMOD_CHANNEL_FREE, warlockSpell, false, 0 );
							}
							else
							{
								findNextTarget( i, j );
								m_unitCurrentlyAttacking = true;
								m_lightningActive	=	true;
								m_unit[i][j].setState( ATTACKING );
								m_unit[i][j].setUnitCanUseAbility( false );
								fmodSystem->playSound( FMOD_CHANNEL_FREE, thunderStrike, false, 0);
								m_lightningRect.left = 0;
								m_lightningRect.right	=	600;
								m_projectilePosX	=	m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosX() ;
								m_projectilePosY	=	m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosY() - 300;
								m_lightningRect.bottom	+=	100;
								m_damageType	=	35;
							}
							return;
						}
					}
					if( i == ( MAXBOARDHEIGHT - 1 ) && j ==  1 )
						changePhase();
				}
			}
		}
		else if(m_gamePhase == PLAYERTWO_EVENTPHASE)
		{
			for(int i = 0; i < MAXBOARDHEIGHT; ++i)
			{
				int trial = 0;
				for(int j = 0; j < MAXBOARDWIDTH; ++j)
				{
					if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERTWO && m_unit[i][j].canUnitTakeAction() && m_unit[i][j].getType() > GOLDMINES)
					{
						m_unitCurrentlyAttacking = true;
						m_arrowForAttackingUnitPosX	=	m_gameBoard[i][j].getPosX() + 10;
						m_arrowForAttackingUnitPosY	=	m_gameBoard[i][j].getPosY() - 40;
						switch(m_unit[i][j].getType())
						{
						case NOUNIT:
							break;
						case GOLDMINES:
							break;
						case WALL:
							m_unitCurrentlyAttacking = false;
							break;
						case WARRIORUNIT:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							findNextTarget( i, j );
							return;
						case MARKSMAN:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_arrowActive = true;
							findNextTarget( i, j );
							fmodSystem->playSound( FMOD_CHANNEL_FREE, shootArrow, false, 0 );
							return;
						case MINOTAUR:
							m_unitCurrentlyAttacking = false;
							if( m_unit[i][j].canUnitUseAbility() && !m_floatingTextActive ){
								m_floatingTextActive	=	true;
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								m_unit[i][j].setState( ABILITY );
								int offset			=	0;
								if( m_gamePhase == PLAYERONE_EVENTPHASE ) 
									offset += 600;
								////////////////////////////////////////////////////////////////
								m_randomNumber	=	rand()%2;
								if( m_randomNumber	==	0 )
								{
									m_combatMessageActive = true;
									swprintf_s(m_combatMessage, 128, L"Enrage!!");
									m_unit[i][j].setDamage( m_unit[i][j].getDamage() + 15 );
									m_floatingRectTopMax	=	m_unit[i][j].getPosY() - 50;
									m_floatingTextRect.top = long(m_unit[i][j].getPosY() - 10);
									m_floatingTextRect.left = long(m_unit[i][j].getPosX() - 5);
									m_floatingTextRect.right = long(m_unit[i][j].getPosX() + 10);
									m_floatingTextRect.bottom = long(m_unit[i][j].getPosY() + 10);
									m_damageType	=	m_unit[i][j].getDamage();
									fmodSystem->playSound( FMOD_CHANNEL_FREE, minotaurRoar, false, 0 );
								}
								else
								{
									m_combatMessageActive = true;
									swprintf_s(m_combatMessage, 128, L"Failed!!");
									m_floatingRectTopMax	=	m_unit[i][j].getPosY() - 50;
									m_floatingTextRect.top = long(m_unit[i][j].getPosY() - 10);
									m_floatingTextRect.left = long(m_unit[i][j].getPosX() - 5);
									m_floatingTextRect.right = long(m_unit[i][j].getPosX() + 10);
									m_floatingTextRect.bottom = long(m_unit[i][j].getPosY() + 10);
									m_damageType	=	m_unit[i][j].getDamage();
								}
								//  INFO:  Set data for floating text

							}
							else
							{
								m_unitCurrentlyAttacking = true;
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								findNextTarget( i, j );
							}
							return;
						case WOLF:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							findNextTarget( i, j );
							return;
						case ARCHERUNIT:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_arrowActive = true;
							findNextTarget( i, j );
							fmodSystem->playSound( FMOD_CHANNEL_FREE, shootArrow, false, 0 );
							return;
						case THIEF:
							m_unitCurrentlyAttacking = false;
							if( m_unit[i][j].canUnitUseAbility() && !m_floatingTextActive ){
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								m_unit[i][j].setState( ABILITY );
								int offset			=	0;
								if( m_gamePhase == PLAYERONE_EVENTPHASE ) 
									offset += 600;
								////////////////////////////////////////////////////////////////
								//  INFO:  Set data for floating text
								m_floatingTextActive = true;
								m_combatMessageActive = true;
								swprintf_s(m_combatMessage, 128, L"Steal!!");
								m_floatingRectTopMax	=	m_unit[i][j].getPosY() - 50;
								m_floatingTextRect.top = long(m_unit[i][j].getPosY() - 10);
								m_floatingTextRect.left = long(m_unit[i][j].getPosX() - 5);
								m_floatingTextRect.right = long(m_unit[i][j].getPosX() + 10);
								m_floatingTextRect.bottom = long(m_unit[i][j].getPosY() + 10);
								m_floatingTextActive = true;
								fmodSystem->playSound( FMOD_CHANNEL_FREE, goldCoins, false, 0 );
								if( m_player[0].getGold() > 30 )
									m_damageType	=	30;
								else
									m_damageType	=	m_player[0].getGold();
								if(m_player[0].getGold() > 15)
								{
									m_player[0].adjustGold( - 15 );
									m_player[1].adjustGold( 15 );
								}
								else
								{
									m_player[0].adjustGold(m_player[0].getGold());
									m_player[1].adjustGold(m_player[0].getGold());
								}

							}
							else{
								m_unitCurrentlyAttacking = true;
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								findNextTarget( i, j );
							}
							return;
						case GOLEM:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							findNextTarget( i, j );
							return;
						case BLACKMAGEUNIT:
							m_attackingSpaceX	=	i;
							m_attackingSpaceY	=	j;
							m_temporaryTimer += 0.0001f;
							m_projectilePosX = m_gameBoard[i][j].getPosX()+5;
							m_projectilePosY = m_gameBoard[i][j].getPosY()+10;
							m_fireBallActive = true;
							findNextTarget( i, j );
							fmodSystem->playSound( FMOD_CHANNEL_FREE, castFireball, false, 0);
							return;
						case WARLOCK:
							m_unitCurrentlyAttacking = false;
							if( m_unit[i][j].canUnitUseAbility() && !m_floatingTextActive )
							{
								m_attackingSpaceX	=	i;
								m_attackingSpaceY	=	j;
								m_unit[i][j].setState( ABILITY );
								int offset					=	0;
								if( m_gamePhase == PLAYERONE_EVENTPHASE ) 
									offset += 600;
								////////////////////////////////////////////////////////////////
								//  INFO:  Set data for floating text
								m_combatMessageActive = true;
								swprintf_s(m_combatMessage, 128, L"Drain!!");
								m_floatingRectTopMax	=	m_unit[i][j].getPosY() - 50;
								m_floatingTextRect.top = long(m_unit[i][j].getPosY() - 10);
								m_floatingTextRect.left = long(m_unit[i][j].getPosX() - 5);
								m_floatingTextRect.right = long(m_unit[i][j].getPosX() + 10);
								m_floatingTextRect.bottom = long(m_unit[i][j].getPosY() + 10);
								m_damageType	=	m_unit[i][j].getDamage();
								m_floatingTextActive = true;
								m_damageType	=	15;
								m_player[0].adjustCurrentHealth( - 5 );
								m_player[1].adjustCurrentHealth( 5 );
								fmodSystem->playSound( FMOD_CHANNEL_FREE, warlockSpell, false, 0 );
							}
							else
							{
								findNextTarget( i, j );
								m_unitCurrentlyAttacking = true;
								m_lightningActive	=	true;
								m_unit[i][j].setState( ATTACKING );
								m_unit[i][j].setUnitCanUseAbility( false );
								fmodSystem->playSound( FMOD_CHANNEL_FREE, thunderStrike, false, 0);
								m_lightningRect.left = 0;
								m_lightningRect.right	=	600;
								m_projectilePosX	=	m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosX() ;
								m_projectilePosY	=	m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosY() - 300;
								m_lightningRect.bottom	+=	100;
								m_damageType	=	35;
							}
							return;
						}
					}
					if( i == ( MAXBOARDHEIGHT - 1 ) && j == ( MAXBOARDWIDTH - 1 ) )
						changePhase();
				}
			}
		}
	}
	else if ( !m_unitCurrentlyMoving && !m_floatingTextActive ) 
	{
		m_temporaryTimer += dt;
		int flip = 1;
		if(m_arrowActive)
		{
			if(m_gamePhase == PLAYERTWO_EVENTPHASE)
				flip *= -1;
			m_projectilePosX += 270 * dt * flip;
		}
		else if(m_fireBallActive)
		{
			if(m_gamePhase == PLAYERTWO_EVENTPHASE)
				flip *= -1;
			m_projectilePosX += 270 * dt * flip;
			m_fireballRotation += 1100 * dt * flip;
		}
		//	if(m_temporaryTimer > 2.0 ){
		if ( m_attackWillHitPlayer )
		{
			//////////////////////////////////////////////////////////////////////////
			//  INFO:  If player one's projectile hits player two
			if( m_unit[m_attackingSpaceX][m_attackingSpaceY].checkIfRanged() )
			{
				if( m_projectilePosX > 700 )
				{
					m_damageType	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage();
					m_player[1].adjustCurrentHealth( -m_damageType ); 
					//m_floatingTextActive	=	true;
					////////////////////////////////////////////////////////////////
					//  INFO:  Set data for floating text
					//  TODO:  Add sound effects for hitting players based on the active projectile
					m_floatingTextActive = true;
					m_floatingRectTopMax	=	50;
					m_floatingTextRect.top = 100;
					m_floatingTextRect.left = 700;
					m_floatingTextRect.right = 750;
					m_floatingTextRect.bottom = 120;
					m_attackWillHitPlayer	=	false;
					m_arrowActive	=	false;
					m_fireBallActive	=	false;
					m_lightningActive	=	false;
					m_unit[m_attackingSpaceX][m_attackingSpaceY].setUnitCanTakeAction( false );
				}
				///////////////////////////////////////////////////////////////////////////
				//  INFO:  If player two's projectile hits player one
				if( m_projectilePosX < 50 )
				{
					m_damageType	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage();
					m_player[0].adjustCurrentHealth( -m_damageType ); 
					//m_floatingTextActive	=	true;
					////////////////////////////////////////////////////////////////
					//  INFO:  Set data for floating text
					m_floatingTextActive = true;
					m_floatingRectTopMax	=	50;
					m_floatingTextRect.top = 100;
					m_floatingTextRect.left = 100;
					m_floatingTextRect.right = 150;
					m_floatingTextRect.bottom = 120;
					m_attackWillHitPlayer	=	false;
					m_arrowActive	=	false;
					m_fireBallActive	=	false;
					m_lightningActive	=	false;
					m_unit[m_attackingSpaceX][m_attackingSpaceY].setUnitCanTakeAction( false );
				}
				if( m_lightningActive )
				{
					m_damageType	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage();

					////////////////////////////////////////////////////////////////
					//  INFO:  Set data for floating text
					m_floatingTextActive = true;
					m_floatingRectTopMax	=	50;
					m_floatingTextRect.top = 100;
					m_floatingTextRect.bottom = 120;
					m_attackWillHitPlayer	=	false;
					//m_lightningActive	=	false;
					m_unit[m_attackingSpaceX][m_attackingSpaceY].setUnitCanTakeAction( false );
					if( m_player[0].checkIfActivePlayer() )
					{
						m_floatingTextRect.left = 700;
						m_floatingTextRect.right = 750;
						m_player[1].adjustCurrentHealth( -m_damageType ); 
					}
					else
					{
						m_floatingTextRect.left = 100;
						m_floatingTextRect.right = 150;
						m_player[0].adjustCurrentHealth( -m_damageType ); 
					}
				}
			}
			else
			{
				if( m_player[0].checkIfActivePlayer() )
				{
					m_damageType	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage();
					m_player[1].adjustCurrentHealth( -m_damageType ); 
					////////////////////////////////////////////////////////////////
					//  INFO:  Set data for floating text
					m_floatingTextActive = true;
					m_floatingRectTopMax	=	50;
					m_floatingTextRect.top = 100;
					m_floatingTextRect.left = 700;
					m_floatingTextRect.right = 750;
					m_floatingTextRect.bottom = 120;
					m_attackWillHitPlayer	=	false;
					m_unit[m_attackingSpaceX][m_attackingSpaceY].setUnitCanTakeAction( false );
				}
				else
				{
					m_damageType	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage();
					m_player[0].adjustCurrentHealth( -m_damageType ); 
					////////////////////////////////////////////////////////////////
					//  INFO:  Set data for floating text
					m_floatingTextActive = true;
					m_floatingRectTopMax	=	50;
					m_floatingTextRect.top = 100;
					m_floatingTextRect.left = 100;
					m_floatingTextRect.right = 150;
					m_floatingTextRect.bottom = 120;
					m_attackWillHitPlayer	=	false;
					m_unit[m_attackingSpaceX][m_attackingSpaceY].setUnitCanTakeAction( false );
				}
			}
		}
		else if( ( ( m_projectilePosX > m_gameBoard[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosX() - 5 ) && ( m_projectilePosX < m_gameBoard[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosX() + 5 ) ) )
		{
			//m_projectilePosX < -200 ){

			////////////////////////////////////////////////////////////////
			//  INFO:  Set data for floating text
			if( !m_lightningActive )
			{
				m_floatingTextActive = true;
				m_floatingRectTopMax	=	m_gameBoard[m_attackingSpaceX][m_attackingSpaceY].getPosY() - 35;
				m_floatingTextRect.top = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosY() - 10);
				m_floatingTextRect.left = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosX() - 10);
				m_floatingTextRect.right = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosX() + 10);
				m_floatingTextRect.bottom = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosY() + 10);
			}
			////////////////////////////////////////////////////////////////
			//  INFO:  Play sound effects based on collision
			if(m_arrowActive )
			{
				if( m_attackTargetSpaceY >= 0 )
				{
					m_damageType	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage();
					if( m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getType() == THIEF )
					{
						m_randomNumber	=	rand()%2;
						if( m_randomNumber == 0 )
							m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].adjustCurrentHealth(-m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage());
						else{
							m_combatMessageActive = true;
							swprintf_s(m_combatMessage, 128, L"Missed!!");
						}
					}
					else 
						m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].adjustCurrentHealth(-m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage());

					//////////////////////////////////////////////////////////////////////
					//  INFO:  Remove attacked unit if health < 1
					if( m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getCurrentHealth() < 1 )
						destroyUnit();
					//m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].removeUnit();
				}
				else
				{
					m_player[0].adjustCurrentHealth(-m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage());
					m_attackTargetSpaceY	=	0;
				}
				/////////////////////////////////////////////////////
				//  INFO:  Set damage to arrow's damage stat
				switch(m_unit[m_attackingSpaceX][m_attackingSpaceY].getType())
				{
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
			else if(m_fireBallActive)
			{
				if( m_attackTargetSpaceX >= 0 ) 
				{
					m_damageType	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage();
					if( m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getType() == THIEF )
					{
						m_randomNumber	=	rand()%2;
						if( m_randomNumber == 0 )
							m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].adjustCurrentHealth(-m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage());
						else{
							m_combatMessageActive	=	true;
							swprintf_s(m_combatMessage, 128, L"Missed!!");
						}
					}
					else 
						m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].adjustCurrentHealth(-m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage());
					//////////////////////////////////////////////////////////////////////
					//  INFO:  Remove attacked unit if health < 1
					if( m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getCurrentHealth() < 1 )
						destroyUnit();
					//m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].removeUnit();
				}
				else
				{
					m_player[0].adjustCurrentHealth(-m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage());
					m_attackTargetSpaceY	=	0;
				}
				fmodSystem->playSound( FMOD_CHANNEL_FREE, fireballHit, false, 0 );
			}
			if( !m_lightningActive )
			{
				m_unit[m_attackingSpaceX][m_attackingSpaceY].setUnitCanTakeAction(false);
				m_projectilePosX = 0;
				m_projectilePosY = 0;
				m_attackTargetSpaceX = 0;
				m_attackTargetSpaceY = 0;
				m_temporaryTimer = 0.0f;
				m_unitCurrentlyAttacking = false;
				m_arrowActive = false;
				m_fireBallActive = false;
			}
		}
	}
	if ( m_lightningActive )
	{
		m_lightningTimer += 100 * dt;
		if(m_lightningTimer > 0.5f)
		{
			m_lightningRect.bottom +=  20;
			m_lightningTimer -= 0.5f;
		}
		if( ( m_lightningRect.bottom - 1300 ) > m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosY() )
		{
			m_unit[m_attackingSpaceX][m_attackingSpaceY].setUnitCanTakeAction( false );
			m_lightningActive = false;
			m_lightningRect.top		=	0;
			m_lightningRect.left	=	0;
			m_lightningRect.right	=	600;
			m_lightningRect.bottom	=	200;
			if( m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getType() == THIEF )
			{
				m_randomNumber	=	rand()%2;
				if( m_randomNumber == 0 )
				{
					m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].adjustCurrentHealth(-m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage());
				}
				else{
					m_combatMessageActive = true;
					swprintf_s(m_combatMessage, 128, L"Missed!!");
				}
			}
			else 
			{
				m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].adjustCurrentHealth(-m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage());
			}
			m_floatingTextActive = true;
			m_floatingRectTopMax	=	m_gameBoard[m_attackingSpaceX][m_attackingSpaceY].getPosY() - 35;
			m_floatingTextRect.top = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosY() - 10);
			m_floatingTextRect.left = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosX() - 10);
			m_floatingTextRect.right = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosX() + 10);
			m_floatingTextRect.bottom = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosY() + 10);
			if( m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getCurrentHealth() < 1 ){
				destroyUnit();
			}
		}
	}
	else if ( m_unitCurrentlyMoving )
	{///////////  INFO:  Melee Unit is moving
		/////////////////////////////////////////////////////////////////////
		//  INFO:  If m_moveToTarget > 0, then an enemy is not in front of this melee unit
		if( m_moveToTarget > -1 ) 
		{
			if( m_unit[m_attackingSpaceX][m_attackingSpaceY].getWhoUnitBelongsTo() == PLAYERONE )
			{
				m_unit[m_attackingSpaceX][m_attackingSpaceY].adjustPosX( 40 * dt );
				m_arrowForAttackingUnitPosX	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getPosX() + 10;
				m_arrowForAttackingUnitPosY	=	m_unit[m_attackingSpaceX][m_attackingSpaceX].getPosY() - 40;
				if( m_unit[m_attackingSpaceX][m_attackingSpaceY].getPosX() > m_gameBoard[m_attackingSpaceX][m_moveToTarget].getPosX())
				{
					moveUnit();
				}
			}
			else 
			{
				m_unit[m_attackingSpaceX][m_attackingSpaceY].adjustPosX( -40 * dt );
				m_arrowForAttackingUnitPosX	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getPosX() + 10;
				m_arrowForAttackingUnitPosY	=	m_unit[m_attackingSpaceX][m_attackingSpaceX].getPosY() - 40;
				if( m_unit[m_attackingSpaceX][m_attackingSpaceY].getPosX() < m_gameBoard[m_attackingSpaceX][m_moveToTarget].getPosX())
				{
					moveUnit();
				}
			}
		}
		else
		{
			////////////////////////////////////////////////////////////////
			//  INFO:  Set data for floating text
			if( !m_floatingTextActive )
			{
				m_floatingTextActive = true;
				m_floatingRectTopMax	=	m_gameBoard[m_attackingSpaceX][m_attackingSpaceY].getPosY() - 35;
				m_floatingTextRect.top = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosY() - 10);
				m_floatingTextRect.left = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosX() - 10);
				m_floatingTextRect.right = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosX() + 10);
				m_floatingTextRect.bottom = long(m_gameBoard[m_attackingSpaceX][m_attackTargetSpaceY].getPosY() + 10);
				m_damageType	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage();
				m_arrowForAttackingUnitPosX	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getPosX() + 10;
				m_arrowForAttackingUnitPosY	=	m_unit[m_attackingSpaceX][m_attackingSpaceY].getPosY() - 40;
				m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].adjustCurrentHealth(-m_unit[m_attackingSpaceX][m_attackingSpaceY].getDamage());
				m_unitCurrentlyMoving	=	false;
				m_unitCurrentlyAttacking	=	true;
				m_unit[m_attackingSpaceX][m_attackingSpaceY].setUnitCanTakeAction(false);
				m_unit[m_attackingSpaceX][m_moveToTarget].setUnitCanTakeAction(false);
				//////////////////////////////////////////////////////////////////////
				//  INFO:  Remove attacked unit if health < 1
				if( m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getCurrentHealth() < 1 )
					destroyUnit();
				//m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].removeUnit();
			}
		}
	}
	int test;
	if(m_gamePhase == PLAYERTWO_EVENTPHASE)
		test = 0;
};

void GameEngine::findNextTarget( int row, int col )
{
	if ( col == 12 )
		col = 12;
	if(m_gamePhase == PLAYERONE_EVENTPHASE)
	{
		///////////////////////////////////////////////////////////////////////////////////
		//  INFO:  Finds the first available target and sets it, then returns
		if( m_unit[row][col].checkIfRanged() )
		{
			for(int i = col; i < MAXBOARDWIDTH; ++i)
			{
				if(m_unit[row][i].getWhoUnitBelongsTo() == PLAYERTWO)
				{
					m_attackTargetSpaceX = row;
					m_attackTargetSpaceY = i;
					break;
				}
				//////////////////////////////////////////////////////////////////////////////////////////
				//  INFO:  If loop is on last iter and there is no unit, deal damage to the player
				if( i == ( MAXBOARDWIDTH - 1 ) ){
					m_attackWillHitPlayer	=	true;
				}
			}
		}
		else{
			//if( ( col + 1 ) > MAXBOARDWIDTH )
			if( ( col + 1 ) == MAXBOARDWIDTH-1 && ( m_unit[row][MAXBOARDWIDTH-1].getWhoUnitBelongsTo() < 0 ) ){
				m_attackWillHitPlayer = true;
				return;
			}
			///////////////////////////////////////////////////////
			//  INFO:  If enemy is in next space, attack that unit
			if( m_unit[row][col + 1].getWhoUnitBelongsTo() == PLAYERTWO ){
				m_attackTargetSpaceX	=	row;
				m_attackTargetSpaceY	=	col + 1;
				m_unitCurrentlyMoving	=	true;
				m_unit[row][col].setState( ATTACKING );
			}
			/////////////////////////////////////////////////////////////////////////////////////
			//  INFO:  If friendly unit is in front of current unit, continue
			else if ( m_unit[row][col + 1].getWhoUnitBelongsTo() == PLAYERONE && !m_unit[row][col].canUnitUseAbility()){
				m_unit[row][col].setUnitCanTakeAction ( false );
				m_unitCurrentlyAttacking	=	false;
				return;
			}
			else {
				m_unitCurrentlyMoving	=	true;
				for( int i = 1; i <= m_unit[row][col].getSpeed(); ++i ){
					if( i == MAXBOARDWIDTH )
						break;
					switch( m_unit[row][col + i].getWhoUnitBelongsTo() ){
					case -1:
						if( m_moveToTarget == MAXBOARDWIDTH )
							m_moveToTarget	=	MAXBOARDWIDTH - 1;
						else{
							m_moveToTarget = col + i;
							m_unit[row][col].setState( MOVING );
						}

						break;
					case PLAYERONE:
						//m_moveToTarget = col + i - 1;
						//m_unit[row][col].setState( MOVING );
						return;
					case PLAYERTWO:
						//m_moveToTarget = col + i;
						//m_unit[row][col].setState( MOVING );
						return;
					}
					//if(m_unit[row][col + i ].getWhoUnitBelongsTo() < PLAYERONE ){
					//m_attackTargetSpaceX	=	row;
					//m_attackTargetSpaceY	=	col;
					//m_moveToTarget = col + i;
					//m_unit[row][col].setState( MOVING );
					//}
					//else
					//break;
				}
				//m_unit[row][col].
			}
		}
	}
	else if(m_gamePhase == PLAYERTWO_EVENTPHASE){
		if(m_unit[row][col].checkIfRanged()){
			m_attackTargetSpaceY	=	-1;
			for(int i = MAXBOARDWIDTH-1; i >= 0; --i){
				if(m_unit[row][i].getWhoUnitBelongsTo() == PLAYERONE){
					//////////////////////////////////////////////////////
					//  INFO:  If the player is
					//  TODO:  FIX this SOON.  m_attackingSpaceY SHOULD be m_attackingSpaceX and vice versa
					if(i > m_attackingSpaceY )
						continue;
					m_attackTargetSpaceX = row;
					m_attackTargetSpaceY = i;
					m_unit[row][col].setState( ATTACKING );
					break;
				}
				if( i == 0 && m_unit[row][0].getWhoUnitBelongsTo() < 0){
					m_attackTargetSpaceX = row;
					m_attackTargetSpaceY = i;
					m_unit[row][col].setState( ATTACKING );
					m_attackWillHitPlayer	=	true;
				}
			}
			//if( m_attackTargetSpaceY == -1 ){
			//m_player[0].adjustCurrentHealth( - 30 );
			//m_attackTargetSpaceY	=	0;
			//	}
		}
		else{
			if( ( col - 1 ) < 1 && m_unit[row][0].getWhoUnitBelongsTo() < PLAYERONE ){
				m_attackWillHitPlayer	=	true;
				return;
			}
			//////////////////////////////////////////////////////////////////
			//  INFO:  If next to enemy, attack
			if( m_unit[row][col - 1].getWhoUnitBelongsTo() == PLAYERONE ){
				m_attackTargetSpaceX	=	row;
				m_attackTargetSpaceY	=	col - 1;
				m_unitCurrentlyMoving	=	true;
				m_unit[row][col].setState( ATTACKING );
			}
			/////////////////////////////////////////////////////////////////////////////////////
			//  INFO:  If friendly unit is in front of current unit, continue
			else if ( m_unit[row][col - 1].getWhoUnitBelongsTo() == PLAYERTWO && !m_unit[row][col].canUnitUseAbility()){
				m_unit[row][col].setUnitCanTakeAction ( false );
				m_unitCurrentlyAttacking	=	false;
				return;
			}
			//////////////////////////////////////////////////////////////////
			//  INFO:  If blocked by friendly unit, return
			//else if ( m_unit[row][col - 1].getWhoUnitBelongsTo() == PLAYERTWO ){
			//	m_unitCurrentlyMoving	=	true;
			//	return;
			//}
			else {
				m_unitCurrentlyMoving	=	true;
				for( int i = 1; i <= m_unit[row][col].getSpeed(); ++i ){
					if( ( col - i ) < 0 )
						break;
					if(m_unit[row][col - i ].getWhoUnitBelongsTo() < PLAYERONE ){
						//m_attackTargetSpaceX	=	row;
						//m_attackTargetSpaceY	=	col;
						m_moveToTarget = col - i;
						if( m_moveToTarget	==	0 )
							m_moveToTarget	=	1;
						m_unit[row][col].setState( MOVING );
					}
					else
						break;
				}
				//m_unit[row][col].
			}
		}
	}
	//}
};

void GameEngine::resetUnitActions(int playerNumber)
{
	for(int i = 0; i < MAXBOARDHEIGHT; ++i)
	{
		for(int j = 0; j < MAXBOARDWIDTH; ++j)
		{
			if(m_unit[i][j].getWhoUnitBelongsTo() == playerNumber && m_unit[i][j].getType() != GOLDMINES)
				m_unit[i][j].setUnitCanTakeAction(true);
			switch(m_unit[i][j].getType())
			{
			case WARLOCK:
				m_unit[i][j].setUnitCanUseAbility( true );
				break;
			case THIEF:
				m_unit[i][j].setUnitCanUseAbility( true );
				break;
			case MINOTAUR:
				m_unit[i][j].setUnitCanUseAbility( true );
				break;
			}
		}
	}
};

void GameEngine::moveUnit()
{
	m_unit[m_attackingSpaceX][m_moveToTarget].addUnit( m_unit[m_attackingSpaceX][m_attackingSpaceY].getType(), m_unit[m_attackingSpaceX][m_attackingSpaceY].getWhoUnitBelongsTo() );
	m_unit[m_attackingSpaceX][m_moveToTarget].setCurrentHealth( m_unit[m_attackingSpaceX][m_attackingSpaceY].getCurrentHealth() );
	m_unit[m_attackingSpaceX][m_moveToTarget].setMaxHealth( m_unit[m_attackingSpaceX][m_attackingSpaceY].getMaxHealth() );
	m_unit[m_attackingSpaceX][m_attackingSpaceY].removeUnit();
	m_unitCurrentlyMoving = false;
	m_unitCurrentlyAttacking	=	false;
	m_unit[m_attackingSpaceX][m_moveToTarget].setPosX( m_gameBoard[m_attackingSpaceX][m_moveToTarget].getPosX());
	m_unit[m_attackingSpaceX][m_moveToTarget].setPosY( m_gameBoard[m_attackingSpaceX][m_moveToTarget].getPosY());
	m_unit[m_attackingSpaceX][m_moveToTarget].setUnitCanTakeAction(false);
	m_attackingSpaceX	=	-1;
	m_attackingSpaceY	=	-1;
	m_moveToTarget		=	-1;

};

void GameEngine::destroyUnit(){
	int currentPlayer = -1;
	if( m_player[0].checkIfActivePlayer() ){
		currentPlayer = PLAYERONE;
	}
	else
		currentPlayer = PLAYERTWO;

	//////////////////////////////////////////////////////////////////////////
	//  INFO:  Gives the player special for killing a unit
	switch(m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getType() ){
	case GOLDMINES:
		m_player[currentPlayer].adjustCurrentSpecial( 25 );
		break;
	case WALL:
		m_player[currentPlayer].adjustCurrentSpecial( 10 );
		break;
	case WARRIORUNIT:
		m_player[currentPlayer].adjustCurrentSpecial( 15 );
		break;
	case MARKSMAN:
		m_player[currentPlayer].adjustCurrentSpecial( 10 );
		break;
	case MINOTAUR:
		m_player[currentPlayer].adjustCurrentSpecial( 25 );
		break;
	case WOLF:
		m_player[currentPlayer].adjustCurrentSpecial( 15 );
		break;
	case ARCHERUNIT:
		m_player[currentPlayer].adjustCurrentSpecial( 10 );
		break;
	case THIEF:
		m_player[currentPlayer].adjustCurrentSpecial( 25 );
		break;
	case GOLEM:
		m_player[currentPlayer].adjustCurrentSpecial( 15 );
		break;
	case BLACKMAGEUNIT:
		m_player[currentPlayer].adjustCurrentSpecial( 10 );
		break;
	case WARLOCK:
		m_player[currentPlayer].adjustCurrentSpecial( 25 );
		break;
	}

	m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].removeUnit();
};

void GameEngine::changePhase()
{
	int test = 0;
	m_gamePhase++;
	if( m_gamePhase > 3 )
		m_gamePhase = 0;
	switch( m_gamePhase )
	{
	case PLAYERONE_PLAYPHASE:
		resetUnitActions(PLAYERONE);					
		m_player[1].setActivePlayer(false);
		m_player[0].setActivePlayer(true);
		for( int i = 0; i < 5; ++i )
		{
			if( m_unit[i][0].getType() == GOLDMINES )
				m_player[0].adjustGold( 50 );
		}
		break;
	case PLAYERONE_EVENTPHASE:
		break;
	case PLAYERTWO_PLAYPHASE:
		resetUnitActions(PLAYERTWO);
		m_player[0].setActivePlayer(false);
		m_player[1].setActivePlayer(true);
		if(!firstTurn)
		{
			for( int i = 0; i < 5; ++i )
			{
				test = m_unit[i][MAXBOARDWIDTH].getType();
				if( m_unit[i][MAXBOARDWIDTH].getType() == GOLDMINES )
					m_player[1].adjustGold( 50 );
			}
		}
		firstTurn = false;
		break;
	case PLAYERTWO_EVENTPHASE:
		break;
	};
};

void GameEngine::archerAbility1(bool done)
{
	int a_row = 0;
	int a_col = 0;

	for(auto& Buttons: map_grid_row1)
	{
		//Check if mouse was pressed
		if((mouseState.rgbButtons[0] & 0x80) != 0)
		{
			//Check which row/col was pressed
			if(Buttons.isOn(cursor.x, cursor.y, 3))
			{
				a_row = Buttons.row;
				a_col = Buttons.col;

				if(!keyIsDown[DIK_8])
				{
					keyIsDown[DIK_8] = true;
					//If unit is in this location remove it
					if(unitsAttacked != 2)
					{
						if(m_unit[a_row][a_col].occupied)
						{
							m_unit[a_row][a_col].removeUnit();
							unitsAttacked += 1;
						}
					}
					else
						done = true;
				}
				else
					keyIsDown[DIK_8] = false;
			}
		}
	}

}

void GameEngine::blackHoleAbility(int row, int col, int a_player)
{
	if(a_player == 0)
	{
		if(m_player[0].getCharacterType() == BLACKMAGE) // Black hole
		{
			m_classAbilityAnimator.setAnimationActive( true );
			m_classAbilityAnimator.setClassAbilityAnimation( BLACKHOLE , m_unit[row][col].getPosX() + 32, m_unit[row][col].getPosY() + 32 );
			if(row != 4 && col != 14)
			{
				if(m_unit[row][col].occupied)
					m_unit[row][col].removeUnit();

				if(m_unit[row][col+1].occupied)
					m_unit[row][col+1].removeUnit();

				if(m_unit[row+1][col].occupied)
					m_unit[row+1][col].removeUnit();

				if(m_unit[row+1][col+1].occupied)
					m_unit[row+1][col+1].removeUnit();
			}
			//last row has different locations to remove units
			if(row == 4 && col != 14)
			{
				if(m_unit[row][col].occupied)
					m_unit[row][col].removeUnit();

				if(m_unit[row-1][col].occupied)
					m_unit[row-1][col].removeUnit();

				if(m_unit[row][col+1].occupied)
					m_unit[row][col+1].removeUnit();

				if(m_unit[row-1][col+1].occupied)
					m_unit[row-1][col+1].removeUnit();
			}
			//last col has different locations to remove units
			if(col == 14 && row != 4)
			{
				if(m_unit[row][col].occupied)
					m_unit[row][col].removeUnit();

				if(m_unit[row+1][col].occupied)
					m_unit[row+1][col].removeUnit();

				if(m_unit[row][col-1].occupied)
					m_unit[row][col-1].removeUnit();

				if(m_unit[row+1][col-1].occupied)
					m_unit[row+1][col-1].removeUnit();
			}

			if(row == 4 && col == 14)
			{
				if(m_unit[row][col].occupied)
					m_unit[row][col].removeUnit();

				if(m_unit[row][col-1].occupied)
					m_unit[row][col-1].removeUnit();

				if(m_unit[row-1][col].occupied)
					m_unit[row-1][col].removeUnit();

				if(m_unit[row-1][col-1].occupied)
					m_unit[row-1][col-1].removeUnit();
			}
			m_player[0].adjustCurrentSpecial(-50);
		}
	}

	if(a_player == 1)
	{
		if(m_player[1].getCharacterType() == BLACKMAGE) // Black hole
		{
			m_classAbilityAnimator.setAnimationActive( true );
			m_classAbilityAnimator.setClassAbilityAnimation( BLACKHOLE , m_unit[row][col].getPosX() + 32, m_unit[row][col].getPosY() + 32 );
			if(row != 4 && col != 14)
			{
				if(m_unit[row][col].occupied)
					m_unit[row][col].removeUnit();

				if(m_unit[row][col+1].occupied)
					m_unit[row][col+1].removeUnit();

				if(m_unit[row+1][col].occupied)
					m_unit[row+1][col].removeUnit();

				if(m_unit[row+1][col+1].occupied)
					m_unit[row+1][col+1].removeUnit();
			}
			//last row has different locations to remove units
			if(row == 4 && col != 14)
			{
				if(m_unit[row][col].occupied)
					m_unit[row][col].removeUnit();

				if(m_unit[row-1][col].occupied)
					m_unit[row-1][col].removeUnit();

				if(m_unit[row][col+1].occupied)
					m_unit[row][col+1].removeUnit();

				if(m_unit[row-1][col+1].occupied)
					m_unit[row-1][col+1].removeUnit();
			}
			//last col has different locations to remove units
			if(col == 14 && row != 4)
			{
				if(m_unit[row][col].occupied)
					m_unit[row][col].removeUnit();

				if(m_unit[row+1][col].occupied)
					m_unit[row+1][col].removeUnit();

				if(m_unit[row][col-1].occupied)
					m_unit[row][col-1].removeUnit();

				if(m_unit[row+1][col-1].occupied)
					m_unit[row+1][col-1].removeUnit();
			}

			if(row == 4 && col == 14)
			{
				if(m_unit[row][col].occupied)
					m_unit[row][col].removeUnit();

				if(m_unit[row][col-1].occupied)
					m_unit[row][col-1].removeUnit();

				if(m_unit[row-1][col].occupied)
					m_unit[row-1][col].removeUnit();

				if(m_unit[row-1][col-1].occupied)
					m_unit[row-1][col-1].removeUnit();
			}
			m_player[1].adjustCurrentSpecial(-50);
		}
	}
}

void GameEngine::flameWaveAbility(int row, int col, int a_player)
{
	if(a_player == 0)
	{
		if(m_player[0].getCurrentSpecial() == 100)
		{
			//Kill everyone in same row including gold mines
			for(int i = 0; i < 16; i++)
				m_unit[row][i].removeUnit();
			m_classAbilityAnimator.setAnimationActive( true );
			m_classAbilityAnimator.setClassAbilityAnimation( FLAMESTRIKE , m_unit[row][col].getPosX() +8 , m_unit[row][col].getPosY() - 12 );
			m_attackTargetSpaceX = row;
			m_player[0].adjustCurrentSpecial(-100);
		}
	}

	if(a_player == 1)
	{
		if(m_player[1].getCurrentSpecial() == 100)
		{
			//Kill everyone in same row including gold mines
			for(int i = 0; i < 16; i++)
				m_unit[row][i].removeUnit();
			m_classAbilityAnimator.setAnimationActive( true );
			m_classAbilityAnimator.setClassAbilityAnimation( FLAMESTRIKE , m_unit[row][col].getPosX() +8 , m_unit[row][col].getPosY() - 12 );
			m_attackTargetSpaceX = row;
			m_player[1].adjustCurrentSpecial(-100);
		}
	}
}

void GameEngine::Render(float dt)
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
				D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
				int fuckyou = 0;
				int fuckyou2 = 0;
				///////////////////////////////////////////////////
				//  INFO:  For drawing graphics
				switch(m_gameState)
				{
				case MENUMAIN:
					D3DXMatrixIdentity(&transMat);
					D3DXMatrixIdentity(&scaleMat);
					D3DXMatrixIdentity(&rotMat);
					D3DXMatrixIdentity(&worldMat);

					D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
					D3DXMatrixTranslation(&transMat, 380.0f, 300.0f, 0.0f);
					D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
					D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

					m_pD3DSprite->SetTransform(&worldMat);

					m_pD3DSprite->Draw(m_menuBG, 0, &D3DXVECTOR3(m_menuBGInfo.Width * 0.5f, m_menuBGInfo.Height * 0.5f, 0.0f),
						0, D3DXCOLOR(255,255, 255,255));

					for(auto& Buttons: menuButtons)
						drawMenu(D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), Buttons.getRect());

					break;
				case MENUCHARACTERSELECT:
					D3DXMatrixIdentity(&transMat);
					D3DXMatrixIdentity(&scaleMat);
					D3DXMatrixIdentity(&rotMat);
					D3DXMatrixIdentity(&worldMat);

					D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
					D3DXMatrixTranslation(&transMat, 380.0f, 300.0f, 0.0f);
					D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
					D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

					m_pD3DSprite->SetTransform(&worldMat);
					//change backgroung image
					m_pD3DSprite->Draw(m_menuBG, 0, &D3DXVECTOR3(m_menuBGInfo.Width * 0.5f, m_menuBGInfo.Height * 0.5f, 0.0f),
						0, D3DXCOLOR(255,255, 255,255));
					//Character select buttons

					for(auto& Buttons: characterButtons)
					{
						if(!player1selected)
							drawCharacters(fuckyou, D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), Buttons.getRect());

						fuckyou += 1;
					}

					for(auto& Buttons: characterButtons)
					{
						if(player1selected)
							drawCharacters(fuckyou2, D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), Buttons.getRect());

						fuckyou2 += 1;
					}
					break;
				case MENUCREDITS:
					break;
				case OVERWORLD:
					break;
				case BATTLE:
					drawBackground();
					drawPlayers();
					//Battle Map Icons
					for(auto& Buttons: map_grid_row1)
						drawButtonGrid(D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor());
					//End Turn Button
					for(auto& Buttons: endTurnButton)
						drawEndTurn(D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), Buttons.getRect());
					//Draws Units onto battle map
					drawGameBoard();
					// Player 1 character options for buttons
					number = 0;
					for(auto& Buttons: player1_units)
					{
						if(m_player[0].characterType == ARCHER)
							drawIcons(number, m_player[0], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor());

						number += 1;
					}

					number = 6;
					for(auto& Buttons: player1_units)
					{
						if(m_player[0].characterType == BLACKMAGE)
							drawIcons(number, m_player[0],D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor());

						number += 1;
					}

					number = 12;
					for(auto& Buttons: player1_units)
					{
						if(m_player[0].characterType == WARRIOR)
							drawIcons(number, m_player[0],D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor());

						number += 1;
					}
					//Hover Info
					int hoverCount = 0;

					if(m_gamePhase == PLAYERONE_PLAYPHASE)
					{
						//hoverCount = 0;
						for(auto&Buttons: hoverButtons)
						{
							if(m_player[0].characterType == ARCHER)
								drawHoverInfo(hoverCount, m_player[0], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), selectedUnit, dt);

							hoverCount += 1;
						}
						hoverCount = 6;
						for(auto&Buttons: hoverButtons)
						{
							if(m_player[0].characterType == BLACKMAGE)
								drawHoverInfo(hoverCount, m_player[0], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), selectedUnit, dt);

							hoverCount += 1;
						}

						hoverCount = 12;
						for(auto&Buttons: hoverButtons)
						{
							if(m_player[0].characterType == WARRIOR)
								drawHoverInfo(hoverCount, m_player[0], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), selectedUnit, dt);

							hoverCount += 1;
						}
					}

					if(m_gamePhase == PLAYERTWO_PLAYPHASE)
					{
						hoverCount = 0;
						for(auto&Buttons: hoverButtons2)
						{
							if(m_player[1].characterType == ARCHER)
								drawHoverInfo(hoverCount, m_player[1], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), selectedUnit, dt);

							hoverCount += 1;
						}

						hoverCount = 6;
						for(auto&Buttons: hoverButtons2)
						{
							if(m_player[1].characterType == BLACKMAGE)
								drawHoverInfo(hoverCount, m_player[1], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), selectedUnit, dt);

							hoverCount += 1;
						}

						hoverCount = 12;
						for(auto&Buttons: hoverButtons2)
						{
							if(m_player[1].characterType == WARRIOR)
								drawHoverInfo(hoverCount, m_player[1], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor(), selectedUnit, dt);

							hoverCount += 1;
						}
					}

					//Player 2 character options for buttons
					number = 0;
					for(auto& Buttons: player2_units)
					{
						if(m_player[1].characterType == ARCHER)
							drawIcons(number, m_player[1], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor());

						number += 1;
					}
					number = 6;
					for(auto& Buttons: player2_units)
					{
						if(m_player[1].characterType == BLACKMAGE)
							drawIcons(number, m_player[1], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor());

						number += 1;
					}
					number = 12;
					for(auto& Buttons: player2_units)
					{
						if(m_player[1].characterType == WARRIOR)
							drawIcons(number, m_player[1], D3DXVECTOR3(Buttons.getPosition().x, Buttons.getPosition().y, 0.0f), Buttons.getColor());

						number += 1;
					}
					break;
				}
				/////////////////////////////////////////////////////////////////////
				//  INFO:  If an ability is active, draw it.
				if( m_classAbilityAnimator.getAnimationActive() ){
					drawAbilityAnimations();
				}

				myMouse.render(m_pD3DSprite, m_cursor, m_cursorInfo);

				m_pD3DSprite->End();
			}
			/////////////////////////////////////////////
			//  INFO:  For drawing text
			switch(m_gameState)
			{
			case MENUMAIN:
				//Drawing cursor positions onto screen
				/*RECT cursorRect;
				GetClientRect(m_hWnd, &cursorRect);
				wchar_t buffer[128];
				cursorRect.top = 470;
				cursorRect.left = 300;
				swprintf_s(buffer, 128, L"%f", cursor.x);
				m_pD3DFont->DrawText(0, buffer, -1, &cursorRect, DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 255, 0));
				cursorRect.top = 500;
				swprintf_s(buffer, 128, L"%f", cursor.y);
				m_pD3DFont->DrawText(0, buffer, -1, &cursorRect, DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 255, 0));*/
				break;
			case MENUCHARACTERSELECT:
				break;
			case MENUCREDITS:
				break;
			case OVERWORLD:
				break;
			case BATTLE:
				drawUIText(dt);
				break;
			case PLAYER1WIN:
				drawWinner(m_player[0]);
				break;
			case PLAYER2WIN:
				drawWinner(m_player[1]);
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
	SAFE_RELEASE(m_flameStrikeAbility);
	SAFE_RELEASE(m_blackHoleAbility);
	SAFE_RELEASE(m_minotaurUnit);
	SAFE_RELEASE(m_marksmanUnit);
	SAFE_RELEASE(m_warriorUnitIcon);
	SAFE_RELEASE(m_warriorCharacter);
	SAFE_RELEASE(m_thief);
	SAFE_RELEASE(m_wolf);
	SAFE_RELEASE(m_lightning);
	SAFE_RELEASE(m_warlock);
	SAFE_RELEASE(m_healthBar);
	SAFE_RELEASE(m_archerArrow);
	SAFE_RELEASE(m_fireball);
	SAFE_RELEASE(m_arrow);
	SAFE_RELEASE(m_blackMageUnit);
	SAFE_RELEASE(m_archerUnit);
	SAFE_RELEASE(m_golemUnit);
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
	SAFE_RELEASE(m_warlockIcon);
	SAFE_RELEASE(m_flameStrikeIcon);
	SAFE_RELEASE(m_snipeIcon);
	SAFE_RELEASE(m_splitShotIcon);
	SAFE_RELEASE(m_wallIcon2);
	SAFE_RELEASE(m_minotaurIcon);
	SAFE_RELEASE(m_warriorIcon);
	SAFE_RELEASE(m_marksmanIcon);
	// Unit Hover
	SAFE_RELEASE(m_archerHover);
	SAFE_RELEASE(m_thiefHover);
	SAFE_RELEASE(m_wolfHover);
	SAFE_RELEASE(m_wallHover);
	SAFE_RELEASE(m_warlockHover);
	SAFE_RELEASE(m_blackMageHover);
	SAFE_RELEASE(m_golemHover);
	//End turn
	SAFE_RELEASE(m_endTurn);
	//Font
	SAFE_RELEASE(m_pD3DFont);
	RemoveFontResourceEx(L"Old English Text MT.ttf", FR_PRIVATE, 0);
	//Cursor & Main menu
	SAFE_RELEASE(m_cursor);
	SAFE_RELEASE(m_menuButtons);
	SAFE_RELEASE(m_menuBG);

	SAFE_RELEASE(m_pD3DDevice);
	SAFE_RELEASE(m_pD3DObject);
	SAFE_RELEASE(m_pD3DSprite);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Releases Audio
	fmodSystem->release();
}

void GameEngine::calcDeltaTime()
{
	m_deltaTime = float(difftime(time(0), start));
};

void GameEngine::drawBackground()
{
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);

	D3DXMatrixScaling(&scaleMat, 1.0f, 3.3f, 0.0f);		
	D3DXMatrixTranslation(&transMat, 220, -200, 0.0f);			
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);	
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	

	m_pD3DSprite->SetTransform(&worldMat);

	m_pD3DSprite->Draw(m_battleBackgroundOne, 0, &D3DXVECTOR3(m_battleBackgroundOneInfo.Width * 0.5f, m_battleBackgroundOneInfo.Height * 0.5f, 0.0f),
		0, D3DCOLOR_ARGB(255, 255, 255, 255));
};

void GameEngine::drawGameBoard()
{
	float healthPercentage = 0;
	int   healthPercentageRight = 0;
	//////////////////////////////////////////////////////////////////////////
	// INFO:  Draws gameboard
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

	for(int i = 0; i < MAXBOARDHEIGHT; ++i)
	{
		int flip = 1;
		for(int j = 0; j < MAXBOARDWIDTH; ++j)
		{
			flip = 1;

			D3DXMatrixIdentity(&transMat);
			D3DXMatrixIdentity(&scaleMat);
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixIdentity(&worldMat);

			////////////////////////////////////////////////////////////////////////////////
			//  INFO:  For inverting images for player 2
			if(!m_unit[i][j].isFacingRight())
				flip *= -1;
			int offset = 0;
			//int testX, testY;
			//////////////////////////////////////////////////////////////////////////////////
			//  INFO:  If the gamespace is occupied by a unit, draw that unit.  
			switch(m_unit[i][j].getType())
			{
			case NOUNIT:
				break;
			case GOLDMINES:
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws Goldmines
				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+4, m_unit[i][j].getPosY(), 0.0f);			// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_goldMine, 0, &D3DXVECTOR3(m_goldMineInfo.Width * 0.5f, m_goldMineInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f)
				{
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case WALL:
				D3DXMatrixScaling(&scaleMat, 1.0f, 1.0f, 0.0f);			
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + 8, m_unit[i][j].getPosY() + 10, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);	
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	

				m_pD3DSprite->SetTransform(&worldMat);

				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERONE)
				{
					int type = m_player[0].getCharacterType();

					switch (type)
					{
					case ARCHER:
						m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
							0, D3DCOLOR_ARGB(255, 255, 255, 255));
						break;
					case BLACKMAGE:
						m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
							0, D3DCOLOR_ARGB(255, 0, 255, 255));
						break;
					case WARRIOR:
						m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
							0, D3DCOLOR_ARGB(255, 255, 0, 0));
						break;
					}
				}

				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERTWO)
				{
					int type = m_player[1].getCharacterType();

					switch (type)
					{
					case ARCHER:
						m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
							0, D3DCOLOR_ARGB(255, 255, 255, 255));
						break;
					case BLACKMAGE:
						m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
							0, D3DCOLOR_ARGB(255, 0, 255, 255));
						break;
					case WARRIOR:
						m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
							0, D3DCOLOR_ARGB(255, 255, 0, 0));
						break;
					}
				}

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);


				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case WARRIORUNIT:
				//////////////////////////////////////////////////////////////////////////////////////////////////////////
				//  INFO:  Adds an offset to draw unit in the center of the gamespace
				//if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERONE )
				offset = 8;
				//if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERTWO )
				//offset = 8;
				D3DXMatrixScaling(&scaleMat, 0.7f * flip, 0.68f, 0.0f);			
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + offset, m_unit[i][j].getPosY()+7, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);	
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_warriorUnitIcon, &m_unit[i][j].getUnitRect(), &D3DXVECTOR3( ( m_unit[i][j].getUnitRect().right - m_unit[i][j].getUnitRect().left)  * 0.5f, ( m_unit[i][j].getUnitRect().bottom - m_unit[i][j].getUnitRect().top ) * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				//if(m_unit[i][j].getWhoUnitBelongsTo() == 1)
				//{
				//	D3DXMatrixScaling(&scaleMat, 1.0f, 1.0f, 0.0f);			
				//	D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + 8, m_unit[i][j].getPosY() + 10, 0.0f);
				//	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);	
				//	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	

				//	m_pD3DSprite->SetTransform(&worldMat);

				//	m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
				//		0, D3DCOLOR_ARGB(255, 0, 255, 255));
				//}
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);


				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case MARKSMAN:
				//////////////////////////////////////////////////////////////////////////////////////////////////////////
				//  INFO:  Adds an offset to draw unit in the center of the gamespace
				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERONE ){
					flip = -1;
					offset = 8;
				}
				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERTWO ){
					flip = 1;
					offset = 8;
				}
				D3DXMatrixScaling(&scaleMat, 0.85f * flip, 0.7f, 0.0f);			
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + offset, m_unit[i][j].getPosY() + 9, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);	
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_marksmanUnit, &m_unit[i][j].getUnitRect(), &D3DXVECTOR3( ( m_unit[i][j].getUnitRect().right - m_unit[i][j].getUnitRect().left ) * 0.5f, ( m_unit[i][j].getUnitRect().bottom - m_unit[i][j].getUnitRect().top ) * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);


				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case MINOTAUR:
				offset = 9;
				//////////////////////////////////////////////////////////////////////////////////////////////////////////
				//  INFO:  Adds an offset to draw unit in the center of the gamespace
				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERONE )
					flip = -1;

				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERTWO )
					flip = 1;
				D3DXMatrixScaling(&scaleMat, 0.27f * flip, 0.28f, 0.0f);			
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + offset, m_unit[i][j].getPosY()+9, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);	
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_minotaurUnit, &m_unit[i][j].getUnitRect(), &D3DXVECTOR3( ( m_unit[i][j].getUnitRect().right - m_unit[i][j].getUnitRect().left)  * 0.5f, ( m_unit[i][j].getUnitRect().bottom - m_unit[i][j].getUnitRect().top ) * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				//if(m_unit[i][j].getWhoUnitBelongsTo() == 1)
				//{
				//	D3DXMatrixScaling(&scaleMat, 1.0f, 1.0f, 0.0f);			
				//	D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + 8, m_unit[i][j].getPosY() + 10, 0.0f);
				//	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);	
				//	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	

				//	m_pD3DSprite->SetTransform(&worldMat);

				//	m_pD3DSprite->Draw(m_wallIcon, 0, &D3DXVECTOR3(m_wallIconInfo.Width * 0.5f, m_wallIconInfo.Height * 0.5f, 0.0f),
				//		0, D3DCOLOR_ARGB(255, 0, 255, 255));
				//}
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);


				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case WOLF:
				//////////////////////////////////////////////////////////////////////////////////////////////////////////
				//  INFO:  Adds an offset to draw unit in the center of the gamespace
				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERONE )
					offset = 33;
				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERTWO )
					offset = -15;
				D3DXMatrixScaling(&scaleMat, 0.35f * flip, 0.35f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + 8, m_unit[i][j].getPosY(), 0.0f);			// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				// Set Transform
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_wolf, &m_unit[i][j].getUnitRect(), &D3DXVECTOR3( ( m_unit[i][j].getUnitRect().right - m_unit[i][j].getUnitRect().left ) * 0.5f, ( m_unit[i][j].getUnitRect().bottom - m_unit[i][j].getUnitRect().top ) * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case ARCHERUNIT:
				//////////////////////////////////////////////////////////////////////////////////////////////////////////
				//  INFO:  Adds an offset to draw unit in the center of the gamespace
				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERONE )
					offset = 33;
				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERTWO )
					offset = -15;
				D3DXMatrixScaling(&scaleMat, 0.75f * flip, 0.74f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + offset, m_unit[i][j].getPosY()+33, 0.0f);			// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				// Set Transform
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_archerUnit, &m_unit[i][j].getUnitRect(), &D3DXVECTOR3(float(m_unit[i][j].getUnitRect().right) - float(m_unit[i][j].getUnitRect().left), float(m_unit[i][j].getUnitRect().bottom) - float(m_unit[i][j].getUnitRect().top), 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case THIEF:
				D3DXMatrixScaling(&scaleMat, 1.2f * flip * -1 , 1.1f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + 10, m_unit[i][j].getPosY() + 6, 0.0f);			// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				// Set Transform
				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_thief, &m_unit[i][j].getUnitRect(), &D3DXVECTOR3( ( m_gamePieceInfo.Width ) * 0.5f, ( m_gamePieceInfo.Height ) * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case GOLEM:
				offset = 5;
				if( flip < 0 )
					offset += 5;
				D3DXMatrixScaling(&scaleMat, 0.55f * flip, 0.40f, 0.0f);			// Scaling
				m_gameBoard[i][j].setPosX(m_gameBoard[i][j].getPosX());
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + offset, m_unit[i][j].getPosY() + 15, 0.0f);			// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				// Set Transform
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_golemUnit, &m_unit[i][j].getUnitRect(), &D3DXVECTOR3( ( m_unit[i][j].getUnitRect().right - m_unit[i][j].getUnitRect().left ) * 0.5f, ( m_unit[i][j].getUnitRect().bottom - m_unit[i][j].getUnitRect().top ) * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);
				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));


				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling

				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);


				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling

				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case BLACKMAGEUNIT:
				//////////////////////////////////////////////////////////////////////////////////////////////////////////
				//  INFO:  Adds an offset to draw unit in the center of the gamespace
				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERONE)
					offset = 35;
				if(m_unit[i][j].getWhoUnitBelongsTo() == PLAYERTWO)
					offset = -15;
				D3DXMatrixScaling(&scaleMat, 0.75f * flip, 0.74f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX() + offset, m_unit[i][j].getPosY()+35, 0.0f);			// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				// Set Transform
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_blackMageUnit, &m_unit[i][j].getUnitRect(), &D3DXVECTOR3(float(m_unit[i][j].getUnitRect().right) - float(m_unit[i][j].getUnitRect().left), float(m_unit[i][j].getUnitRect().bottom) - float(m_unit[i][j].getUnitRect().top), 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);
				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));


				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling

				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);


				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);			// Scaling

				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			case WARLOCK:
				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws Warlock
				D3DXMatrixScaling(&scaleMat, 1.3f * ( flip * -1 ), 1.3f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+10, m_unit[i][j].getPosY()+10, 0.0f);
				//D3DXMatrixTranslation(&transMat, m_gameBoard[i][j].getPosX(), m_gameBoard[i][j].getPosY(), 0.0f);// Translation
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				// Set Transform
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_warlock, &m_unit[i][j].getUnitRect(), &D3DXVECTOR3( ( m_unit[i][j].getUnitRect().right - m_unit[i][j].getUnitRect().left ) * 0.5f, ( m_unit[i][j].getUnitRect().bottom - m_unit[i][j].getUnitRect().top ) * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws black background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);
				D3DXMatrixScaling(&scaleMat, 0.45f, 0.55f, 0.0f);			// Scaling
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+35, m_unit[i][j].getPosY()+48, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
				m_pD3DSprite->SetTransform(&worldMat);
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 0, 0, 0));


				//////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws red background for health bar
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);


				m_healthRect.right += 13;
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));

				m_healthRect.right -= 13;
				///////////////////////////////////////////////////////////////////////////////
				//  INFO:  Draws green health bar based on remaining health
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				D3DXMatrixScaling(&scaleMat, 0.33f, 0.35f, 0.0f);
				D3DXMatrixTranslation(&transMat, m_unit[i][j].getPosX()+25, m_unit[i][j].getPosY()+42, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);

				m_healthRect.left += 120;
				m_healthRect.right += 133;
				///////////////////////////////////////////////////////////////////////
				//  INFO:  Shrinks green health bar based on missing health
				healthPercentage = ( (float) m_unit[i][j].getCurrentHealth() / m_unit[i][j].getMaxHealth() );
				while(healthPercentage < 1.0f){
					healthPercentage += 0.05f;
					m_healthRect.right -= 5;
				}
				m_pD3DSprite->Draw(m_healthBar, &m_healthRect, &D3DXVECTOR3(m_healthBarInfo.Width * 0.5f, m_healthBarInfo.Height * 0.5f, 0.0f),
					0, D3DCOLOR_ARGB(255, 255, 255, 255));
				m_healthRect.left -= 120;
				m_healthRect.right = 90;
				break;
			}
		}
	}
	//}
	if(m_unitCurrentlyAttacking || m_floatingTextActive){
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
		if(m_arrowActive)
		{
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
		if(m_lightningActive){
			D3DXMatrixIdentity(&transMat);
			D3DXMatrixIdentity(&scaleMat);
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixIdentity(&worldMat);
			D3DXMatrixScaling(&scaleMat, 0.2f, 0.2f, 1.0f);			// Scaling
			D3DXMatrixTranslation(&transMat, m_projectilePosX, m_projectilePosY, 0.0f);			// Translation
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world
			m_pD3DSprite->SetTransform(&worldMat);
			m_pD3DSprite->Draw(m_lightning, &m_lightningRect, &D3DXVECTOR3( ( m_lightningRect.right - m_lightningRect.left -40 ) * 0.5f, /*m_lightningInfo.Width * 0.5f,*/  m_unit[m_attackTargetSpaceX][m_attackTargetSpaceY].getPosY() - 300, /* m_lightningInfo.Height * 0.5f,*/ 0.0f),
				0, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
};

void GameEngine::drawPlayers()
{
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
		D3DXMatrixTranslation(&transMat, 210.0f, 48.0f + 425.0f * float(i), 0.0f);			// Translation
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

		m_pD3DSprite->SetTransform(&worldMat);

		m_pD3DSprite->Draw(m_playerUIBackground, 0, &D3DXVECTOR3(m_playerUIBackgroundInfo.Width * 0.5f, m_playerUIBackgroundInfo.Height * 0.5f, 0.0f),
			0, D3DCOLOR_ARGB(255, 255, 255, 255));

		////////////////////////////////////////////
		//  INFO:  Offset for drawing characters on opposite sides of the screen
		int offset = 0;

		switch(m_player[i].getCharacterType())
		{
			////////////////////////////////////////
			//  INFO:  Clear Matrices after drawing character backgrounds
			D3DXMatrixIdentity(&transMat);
			D3DXMatrixIdentity(&scaleMat);
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixIdentity(&worldMat);

		case WARRIOR:
			offset = 0;
			if(m_player[i].getPlayerNumber() == 1)
				offset = 140;

			D3DXMatrixScaling(&scaleMat, 0.10f * invertImage, 0.16f, 0.0f);			// Scaling
			D3DXMatrixTranslation(&transMat, 75.0f + offset + 500.0f * float(i), 85.0f, 0.0f);			// Translation
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_warriorCharacter, 0, &D3DXVECTOR3(m_warriorCharacterInfo.Width * 0.5f, m_warriorCharacterInfo.Height * 0.5f, 0.0f),
				0, D3DCOLOR_ARGB(255, 255, 255, 255));
			break;
		case ARCHER:
			/////////////////////////////////////////////////////////////////////
			//  TODO:  Sloppy way to handle this, but it technically works.  Find a better way later
			//  INFO:  Creates an offset to draw the player appropriately on both sides of the screen
			offset = 0;
			if(m_player[i].getPlayerNumber() == 1)
				offset = 140;

			D3DXMatrixScaling(&scaleMat, 0.10f * invertImage, 0.075f, 0.0f);			// Scaling
			D3DXMatrixTranslation(&transMat, 75.0f + offset + 500.0f * float(i), 60.0f, 0.0f);			// Translation
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_archerCharacter, 0, &D3DXVECTOR3(m_archerCharacterInfo.Width * 0.5f, m_archerCharacterInfo.Height * 0.5f, 0.0f),
				0, D3DCOLOR_ARGB(255, 255, 255, 255));
			break;
		case BLACKMAGE:
			/////////////////////////////////////////////////////////////////////
			//  TODO:  Sloppy way to handle this, but it technically works.  Find a better way later
			//  INFO:  Creates an offset to draw the player appropriately on both sides of the screen
			offset = 0;
			if(m_player[i].getPlayerNumber() == 1)
				offset = 90;

			D3DXMatrixScaling(&scaleMat, 0.15f * invertImage, 0.15f, 0.0f);			// Scaling
			D3DXMatrixTranslation(&transMat, 105.0f + offset + 500.0f * float(i), 80.0f, 0.0f);			// Translation
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);		// Multiply scale and rotation, store in scale
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);		// Multiply scale and translation, store in world

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

void GameEngine::drawUIText(float dt)
{
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	rect.right = rect.right - rect.left;
	rect.bottom = rect.bottom - rect.top;
	rect.top = 40;			
	rect.left = 170;
	wchar_t buffer[128];

	for(int i = 0; i < 2; ++i)
	{
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

	if(m_floatingTextActive && !m_combatMessageActive)
	{
		swprintf_s(buffer, 128, L"%d", m_damageType);
		m_pD3DFont->DrawText(0, buffer, -1, &m_floatingTextRect, DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
	if( m_combatMessageActive )
	{
		m_pD3DFont->DrawText(0, m_combatMessage, -1, &m_floatingTextRect, DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 127, 0));
	}

	//Player can't afford unit
	if(noGold)
	{
		RECT noMoney;
		wchar_t noMonbuffer[64];
		GetClientRect(m_hWnd, &noMoney);
		noMoney.top = 470;
		noMoney.left = 300;
		swprintf_s(noMonbuffer, 64, L"Not Enough GOLD!");
		m_pD3DFont->DrawText(0, noMonbuffer, -1, &noMoney, DT_TOP| DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 255, 0, 0));

		textCount += dt;

		if(textCount >= 2.0f)
			noGold = false;
	}
	else
		textCount = 0;

	////Drawing cursor positions onto screen
	/*RECT cursorRect;
	GetClientRect(m_hWnd, &cursorRect);
	cursorRect.top = 470;
	cursorRect.left = 300;
	swprintf_s(buffer, 128, L"%f", cursor.x);
	m_pD3DFont->DrawText(0, buffer, -1, &cursorRect, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
	cursorRect.top = 500;
	swprintf_s(buffer, 128, L"%f", cursor.y);
	m_pD3DFont->DrawText(0, buffer, -1, &cursorRect, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));*/
};
// Change WARRIOR ABILITY icons to correct icons once recieved!!
void GameEngine::drawIcons(int thisButton , Character thisPlayer, D3DXVECTOR3 position,D3DCOLOR a_color)
{
	//ARCHER UNIT BUTTONS
	if(thisPlayer.getPlayerNumber() == 0 && thisPlayer.characterType == ARCHER)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisButton == ARCHERBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_archerIcon, 0, &D3DXVECTOR3(m_archerIconInfo.Width * 0.5f, m_archerIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == THIEFBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_thiefIcon, 0, &D3DXVECTOR3(m_thiefIconInfo.Width * 0.5f, m_thiefIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WOLFBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_wolfIcon, 0, &D3DXVECTOR3(m_wolfIconInfo.Width * 0.5f, m_wolfIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WALLBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_wallIcon2, 0, &D3DXVECTOR3(m_wallIconInfo2.Width * 0.5f, m_wallIconInfo2.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == ARCHERABILITY1)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_splitShotIcon, 0, &D3DXVECTOR3(m_splitShotIconInfo.Width * 0.5f, m_splitShotIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == ARCHERABILITY2)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw( m_snipeIcon, 0, &D3DXVECTOR3(m_snipeIconInfo.Width * 0.5f, m_snipeIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}
	}

	if(thisPlayer.getPlayerNumber() == 1 && thisPlayer.characterType == ARCHER)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisButton == ARCHERBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_archerIcon, 0, &D3DXVECTOR3(m_archerIconInfo.Width * 0.5f, m_archerIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton== THIEFBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_thiefIcon, 0, &D3DXVECTOR3(m_thiefIconInfo.Width * 0.5f, m_thiefIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WOLFBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat,position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_wolfIcon, 0, &D3DXVECTOR3(m_wolfIconInfo.Width * 0.5f, m_wolfIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WALLBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_wallIcon2, 0, &D3DXVECTOR3(m_wallIconInfo2.Width * 0.5f, m_wallIconInfo2.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == ARCHERABILITY1)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_splitShotIcon, 0, &D3DXVECTOR3(m_splitShotIconInfo.Width * 0.5f, m_splitShotIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == ARCHERABILITY2)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw( m_snipeIcon, 0, &D3DXVECTOR3(m_snipeIconInfo.Width * 0.5f, m_snipeIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}
	}

	//BLACKMAGE UNIT BUTTONS
	if(thisPlayer.getPlayerNumber() == 0 && thisPlayer.characterType == BLACKMAGE)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisButton == BLACKMAGEBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_blackMageIcon, 0, &D3DXVECTOR3(m_blackMageIconInfo.Width * 0.5f, m_blackMageIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == GOLEMBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_golemIcon, 0, &D3DXVECTOR3(m_golemIconInfo.Width * 0.5f, m_golemIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WARLOCKBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_warlockIcon, 0, &D3DXVECTOR3(m_warlockIconInfo.Width * 0.5f, m_warlockIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WALLBUTTON2)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_wallIcon2, 0, &D3DXVECTOR3(m_wallIconInfo2.Width * 0.5f, m_wallIconInfo2.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == BLACKMAGEABILITY2)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_flameStrikeIcon, 0, &D3DXVECTOR3(m_flameStrikeIconInfo.Width * 0.5f, m_flameStrikeIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == BLACKMAGEABILITY1)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_blackHoleIcon, 0, &D3DXVECTOR3(m_blackHoleIconInfo.Width * 0.5f, m_blackHoleIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}
	}

	if(thisPlayer.getPlayerNumber() == 1 && thisPlayer.characterType == BLACKMAGE)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisButton == BLACKMAGEBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_blackMageIcon, 0, &D3DXVECTOR3(m_blackMageIconInfo.Width * 0.5f, m_blackMageIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == GOLEMBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_golemIcon, 0, &D3DXVECTOR3(m_golemIconInfo.Width * 0.5f, m_golemIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WARLOCKBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_warlockIcon, 0, &D3DXVECTOR3(m_warlockIconInfo.Width * 0.5f, m_warlockIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WALLBUTTON2)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_wallIcon2, 0, &D3DXVECTOR3(m_wallIconInfo2.Width * 0.5f, m_wallIconInfo2.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == BLACKMAGEABILITY2)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_flameStrikeIcon, 0, &D3DXVECTOR3(m_flameStrikeIconInfo.Width * 0.5f, m_flameStrikeIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == BLACKMAGEABILITY1)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_blackHoleIcon, 0, &D3DXVECTOR3(m_blackHoleIconInfo.Width * 0.5f, m_blackHoleIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}
	}

	// Warrior Icons
	if(thisPlayer.getPlayerNumber() == 0 && thisPlayer.characterType == WARRIOR)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisButton == WARRIORBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_warriorIcon, 0, &D3DXVECTOR3(m_warriorIconInfo.Width * 0.5f, m_warriorIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == MARKSMANBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_marksmanIcon, 0, &D3DXVECTOR3(m_marksmanIconInfo.Width * 0.5f, m_marksmanIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == MINOTAURBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_minotaurIcon, 0, &D3DXVECTOR3(m_minotaurIconInfo.Width * 0.5f, m_minotaurIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WALLBUTTON3)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_wallIcon2, 0, &D3DXVECTOR3(m_wallIconInfo2.Width * 0.5f, m_wallIconInfo2.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WARRIORABILITY1)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_flameStrikeIcon, 0, &D3DXVECTOR3(m_flameStrikeIconInfo.Width * 0.5f, m_flameStrikeIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WARRIORABILITY2)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_blackHoleIcon, 0, &D3DXVECTOR3(m_blackHoleIconInfo.Width * 0.5f, m_blackHoleIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}
	}

	if(thisPlayer.getPlayerNumber() == 1 && thisPlayer.characterType == WARRIOR)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisButton == WARRIORBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_warriorIcon, 0, &D3DXVECTOR3(m_warriorIconInfo.Width * 0.5f, m_warriorIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == MARKSMANBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_marksmanIcon, 0, &D3DXVECTOR3(m_marksmanIconInfo.Width * 0.5f, m_marksmanIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == MINOTAURBUTTON)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_minotaurIcon, 0, &D3DXVECTOR3(m_minotaurIconInfo.Width * 0.5f, m_minotaurIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WALLBUTTON3)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_wallIcon2, 0, &D3DXVECTOR3(m_wallIconInfo2.Width * 0.5f, m_wallIconInfo2.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WARRIORABILITY1)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_flameStrikeIcon, 0, &D3DXVECTOR3(m_flameStrikeIconInfo.Width * 0.5f, m_flameStrikeIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}

		if(thisButton == WARRIORABILITY2)
		{
			D3DXMatrixScaling(&scaleMat, 0.8f, 0.8f, 0.0f);
			D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

			m_pD3DSprite->SetTransform(&worldMat);

			m_pD3DSprite->Draw(m_blackHoleIcon, 0, &D3DXVECTOR3(m_blackHoleIconInfo.Width * 0.5f, m_blackHoleIconInfo.Height * 0.5f, 0.0f),
				0, a_color);
		}
	}
}

void GameEngine::drawButtonGrid(D3DXVECTOR3 position, D3DCOLOR a_color)
{
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);

	D3DXMatrixScaling(&scaleMat, 0.7f, 0.80f, 0.0f);
	D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

	m_pD3DSprite->SetTransform(&worldMat);

	m_pD3DSprite->Draw(m_gamePiece, 0, &D3DXVECTOR3(m_gamePieceInfo.Width * 0.5f, m_gamePieceInfo.Height * 0.5f, 0.0f),
		0, a_color);
}

void GameEngine::drawMenu(D3DXVECTOR3 position, D3DCOLOR a_color, RECT &a_rect)
{
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);

	D3DXMatrixScaling(&scaleMat, 1.0f, 1.0f, 0.0f);
	D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

	m_pD3DSprite->SetTransform(&worldMat);

	m_pD3DSprite->Draw(m_menuButtons, &a_rect, &D3DXVECTOR3(m_menuButtonsInfo.Width * 0.5f, m_menuButtonsInfo.Height * 0.5f, 0.0f),
		0, a_color);
}

void GameEngine::drawCharacters(int a_number, D3DXVECTOR3 position, D3DCOLOR a_color, RECT &a_rect)
{
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);

	//draw archer
	if(a_number == 0)
	{
		D3DXMatrixScaling(&scaleMat, 0.2f, 0.2f, 0.0f);
		D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

		m_pD3DSprite->SetTransform(&worldMat);

		m_pD3DSprite->Draw(m_archerCharacter, 0, &D3DXVECTOR3(m_archerCharacterInfo.Width * 0.5f, m_archerCharacterInfo.Height * 0.5f, 0.0f),
			0, a_color);
	}
	//draw warrior
	if(a_number == 2)
	{
		D3DXMatrixScaling(&scaleMat, 0.2f, 0.35f, 0.0f);
		D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

		m_pD3DSprite->SetTransform(&worldMat);

		m_pD3DSprite->Draw(m_warriorCharacter, 0, &D3DXVECTOR3(m_warriorCharacterInfo.Width * 0.5f, m_warriorCharacterInfo.Height * 0.5f, 0.0f),
			0, a_color);
	}
	////draw black mage
	if(a_number == 1)
	{
		D3DXMatrixScaling(&scaleMat, 0.2f, 0.35f, 0.0f);
		D3DXMatrixTranslation(&transMat, position.x+100.0f, position.y, 0.0f);
		D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
		D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

		m_pD3DSprite->SetTransform(&worldMat);

		m_pD3DSprite->Draw(m_blackMageCharacter, 0, &D3DXVECTOR3(m_blackMageCharacterInfo.Width * 0.5f, m_blackMageCharacterInfo.Height * 0.5f, 0.0f),
			0, a_color);
	}
}

void GameEngine::drawWinner(Character a_player)
{
	RECT rect;
	rect.right = 500;
	rect.top = 35;
	if(a_player.getPlayerNumber() == 0)
	{
		m_pD3DFont->DrawText(0, L"Player One is VICTORIOUS!\nPress Enter To Return To Menu", -1, &rect, DT_TOP | DT_RIGHT | DT_NOCLIP , D3DCOLOR_ARGB(255, 255, 255, 0));
	}
	else
	{
		m_pD3DFont->DrawText(0, L"Player Two is VICTORIOUS!\nPress Enter To Return To Menu", -1, &rect, DT_TOP | DT_RIGHT | DT_NOCLIP , D3DCOLOR_ARGB(255, 255, 255, 0));
	}
}

void GameEngine::drawEndTurn(D3DXVECTOR3 position, D3DCOLOR a_color, RECT &a_rect)
{
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);

	D3DXMatrixScaling(&scaleMat, 0.75f, 0.75f, 0.0f);
	D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

	m_pD3DSprite->SetTransform(&worldMat);

	m_pD3DSprite->Draw(m_endTurn, 0, &D3DXVECTOR3(float(m_endTurnInfo.Width* 0.5f), float(m_endTurnInfo.Height * 0.5f), 0.0f),
		0, a_color);
}

void GameEngine::drawHoverInfo(int thisButton , Character thisPlayer, D3DXVECTOR3 position,D3DCOLOR a_color, int thisUnit, float dt)
{
	////////////////////////////////
	// ARCHER
	if(thisPlayer.getPlayerNumber() == 0 && thisPlayer.characterType == ARCHER)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisUnit == 0)
		{
			if(thisButton == ARCHERBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y - 50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_archerHover, 0, &D3DXVECTOR3(m_archerHoverInfo.Width * 0.5f, m_archerHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 2)
		{
			if(thisButton == THIEFBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_thiefHover, 0, &D3DXVECTOR3(m_thiefHoverInfo.Width * 0.5f, m_thiefHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 1)
		{
			if(thisButton == WOLFBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wolfHover, 0, &D3DXVECTOR3(m_wolfHoverInfo.Width * 0.5f, m_wolfHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 3)
		{
			if(thisButton == WALLBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wallHover, 0, &D3DXVECTOR3(m_wallHoverInfo.Width * 0.5f, m_wallHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
	}

	if(thisPlayer.getPlayerNumber() == 1 && thisPlayer.characterType == ARCHER)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisUnit == 0)
		{
			if(thisButton == ARCHERBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y - 50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_archerHover, 0, &D3DXVECTOR3(m_archerHoverInfo.Width * 0.5f, m_archerHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 2)
		{
			if(thisButton == THIEFBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_thiefHover, 0, &D3DXVECTOR3(m_thiefHoverInfo.Width * 0.5f, m_thiefHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 1)
		{
			if(thisButton == WOLFBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wolfHover, 0, &D3DXVECTOR3(m_wolfHoverInfo.Width * 0.5f, m_wolfHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 3)
		{
			if(thisButton == WALLBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wallHover, 0, &D3DXVECTOR3(m_wallHoverInfo.Width * 0.5f, m_wallHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
	}

	//////////////////////////////////
	// BLACKMAGE
	if(thisPlayer.getPlayerNumber() == 1 && thisPlayer.characterType == BLACKMAGE)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisUnit == 0)
		{
			if(thisButton == BLACKMAGEBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y - 50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_blackMageHover, 0, &D3DXVECTOR3(m_blackMageHoverInfo.Width * 0.5f, m_blackMageHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 2)
		{
			if(thisButton == WARLOCKBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_warlockHover, 0, &D3DXVECTOR3(m_warlockHoverInfo.Width * 0.5f, m_warlockHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 1)
		{
			if(thisButton == GOLEMBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_golemHover, 0, &D3DXVECTOR3(m_golemHoverInfo.Width * 0.5f, m_golemHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 3)
		{
			if(thisButton == WALLBUTTON2)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wallHover, 0, &D3DXVECTOR3(m_wallHoverInfo.Width * 0.5f, m_wallHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
	}

	if(thisPlayer.getPlayerNumber() == 0 && thisPlayer.characterType == BLACKMAGE)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisUnit == 0)
		{
			if(thisButton == BLACKMAGEBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y - 50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_blackMageHover, 0, &D3DXVECTOR3(m_blackMageHoverInfo.Width * 0.5f, m_blackMageHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 2)
		{
			if(thisButton == WARLOCKBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_warlockHover, 0, &D3DXVECTOR3(m_warlockHoverInfo.Width * 0.5f, m_warlockHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 1)
		{
			if(thisButton == GOLEMBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_golemHover, 0, &D3DXVECTOR3(m_golemHoverInfo.Width * 0.5f, m_golemHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 3)
		{
			if(thisButton == WALLBUTTON2)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wallHover, 0, &D3DXVECTOR3(m_wallHoverInfo.Width * 0.5f, m_wallHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
	}

	//////////////////////////////////
	// WARRIOR
	if(thisPlayer.getPlayerNumber() == 1 && thisPlayer.characterType == WARRIOR)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisUnit == 0)
		{
			if(thisButton == WARRIORBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y - 50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_blackMageHover, 0, &D3DXVECTOR3(m_blackMageHoverInfo.Width * 0.5f, m_blackMageHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 2)
		{
			if(thisButton == MINOTAURBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_warlockHover, 0, &D3DXVECTOR3(m_warlockHoverInfo.Width * 0.5f, m_warlockHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 1)
		{
			if(thisButton == MARKSMANBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_golemHover, 0, &D3DXVECTOR3(m_golemHoverInfo.Width * 0.5f, m_golemHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 3)
		{
			if(thisButton == WALLBUTTON2)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wallHover, 0, &D3DXVECTOR3(m_wallHoverInfo.Width * 0.5f, m_wallHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
	}

	if(thisPlayer.getPlayerNumber() == 0 && thisPlayer.characterType == WARRIOR)
	{
		D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

		D3DXMatrixIdentity(&transMat);
		D3DXMatrixIdentity(&scaleMat);
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixIdentity(&worldMat);

		if(thisUnit == 0)
		{
			if(thisButton == WARRIORBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y - 50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_blackMageHover, 0, &D3DXVECTOR3(m_blackMageHoverInfo.Width * 0.5f, m_blackMageHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 2)
		{
			if(thisButton == MINOTAURBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_warlockHover, 0, &D3DXVECTOR3(m_warlockHoverInfo.Width * 0.5f, m_warlockHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 1)
		{
			if(thisButton == MARKSMANBUTTON)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_golemHover, 0, &D3DXVECTOR3(m_golemHoverInfo.Width * 0.5f, m_golemHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
		if(thisUnit == 3)
		{
			if(thisButton == WALLBUTTON2)
			{
				D3DXMatrixScaling(&scaleMat, 0.8f, 1.0f, 0.0f);
				D3DXMatrixTranslation(&transMat, position.x, position.y-50.0f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wallHover, 0, &D3DXVECTOR3(m_wallHoverInfo.Width * 0.5f, m_wallHoverInfo.Height * 0.5f, 0.0f),
					0, a_color);
			}
		}
	}


}

void GameEngine::drawAbilityAnimations()
{
	D3DXMATRIX transMat, rotMat, scaleMat, worldMat;

	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);

	D3DXMatrixScaling(&scaleMat, m_classAbilityAnimator.getScaleX(), m_classAbilityAnimator.getScaleY(), 0.0f);		
	D3DXMatrixTranslation(&transMat, m_classAbilityAnimator.getPosX(), m_classAbilityAnimator.getPosY(), 0.0f);			
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);	
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	

	m_pD3DSprite->SetTransform(&worldMat);

	switch( m_classAbilityAnimator.getType() ){
	case BLACKHOLE:
		m_pD3DSprite->Draw(m_blackHoleAbility, &m_classAbilityAnimator.getAnimationRect(), &D3DXVECTOR3( ( m_classAbilityAnimator.getAnimationRect().right - m_classAbilityAnimator.getAnimationRect().left ) * 0.5f,
			( m_classAbilityAnimator.getAnimationRect().bottom - m_classAbilityAnimator.getAnimationRect().top ) * 0.5f, 0.0f),
			0, D3DCOLOR_ARGB(255, 255, 255, 255));
		break;
	case FLAMESTRIKE:
		for( int i = 0; i < MAXBOARDWIDTH; ++i ){
			D3DXMatrixIdentity(&transMat);
			D3DXMatrixTranslation(&transMat, m_unit[m_attackTargetSpaceX][i].getPosX() + 8, m_unit[m_attackTargetSpaceX][i].getPosY() - 12, 0.0f);			
			D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);	
			D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);	

			m_pD3DSprite->SetTransform(&worldMat);
			m_pD3DSprite->Draw(m_flameStrikeAbility, &m_classAbilityAnimator.getAnimationRect(), &D3DXVECTOR3( ( m_classAbilityAnimator.getAnimationRect().right - m_classAbilityAnimator.getAnimationRect().left ) * 0.5f,
				( m_classAbilityAnimator.getAnimationRect().bottom - m_classAbilityAnimator.getAnimationRect().top ) * 0.5f, 0.0f),
				0, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		break;
	}
};