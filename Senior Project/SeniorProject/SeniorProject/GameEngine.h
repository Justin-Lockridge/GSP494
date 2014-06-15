////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GameEngine class for GSP494 - Senior Project
//  By:  Justin Lockridge and Ricky Rivera
//  Date:  08 March 2014
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  KNOWN ISSUES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <stdio.h>
#include <time.h>
#include <vector>
#pragma comment(lib, "winmm.lib")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  INFO:  Game Includes
#include "DefinesAndEnums.h"
#include "Character.h"
#include "BoardSpace.h"
#include "Buttons.h"
#include "Cursor.h"
#include "Unit.h"
#include "ClassAbilityAnimator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Direct3D headers && includes
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  DirectInput headers && includes
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Audio includes using FMOD
#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmodex_vc.lib")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  For safely releasing objects
#define SAFE_RELEASE(x) if(x){x->Release(); x = 0;}

#define MAXBOARDWIDTH 16
#define MAXBOARDHEIGHT 5
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define for buttons used in battle
#define MAX_BATTLE_BUTTONS 6
#define MAX_HOVER_INFO 4
#define MAX_COL_BUTTONS 80
// maps unsigned 8 bits/channel to D3DCOLOR
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

class GameEngine
{
	//////////////////////////////////////////////////////////////////////////
	// Application Variables
	//////////////////////////////////////////////////////////////////////////
	HWND			m_hWnd;												// Handle to the window
	bool			m_bVsync;											// Boolean for vertical syncing
	float			m_currTime;											// Time to render current frame
	float			m_prevTime, m_deltaTime, m_frameTime;				// Time to render previous frame

	int				m_FPS;												// Frames per second  
	int				m_gamePhase, m_randomNumber, selectedHover, unitsAttacked, number;
	bool			m_unitCurrentlyAttacking, m_fireBallActive, m_arrowActive, m_floatingTextActive, m_unitCurrentlyMoving, m_lightningActive, m_combatMessageActive, m_attackWillHitPlayer,
					m_displayingHelpMenu;
	float			m_projectilePosX, m_projectilePosY, m_arrowForAttackingUnitPosX, m_arrowForAttackingUnitPosY;
	float			m_temporaryTimer, m_lightningTimer, m_unitAttackTimer;
	int				m_attackingSpaceX, m_attackingSpaceY, m_attackTargetSpaceX, m_attackTargetSpaceY, m_moveToTarget, m_tester;

	float			m_fireballRotation, m_floatingRectTopMax, m_floatingRectTimer, textCount, m_characterSelectTimer, m_assassinTimer;
	RECT			m_floatingTextRect, m_healthRect, m_lightningRect;

	bool			noGold, firstTurn, dontPlaceUnit, hoveredUnit, player1selected, player2selected;
	//////////////////////////////////////////////////////////////////////////
	// Direct3D Variables
	//////////////////////////////////////////////////////////////////////////
	IDirect3D9*				m_pD3DObject;	// Direct3D 9 Object
	IDirect3DDevice9*		m_pD3DDevice;	// Direct3D 9 Device
	D3DCAPS9				m_D3DCaps;		// Device Capabilities

	//////////////////////////////////////////////////////////////////////////
	// Font Variables
	//////////////////////////////////////////////////////////////////////////
	ID3DXFont*				m_pD3DFont, *m_readableFont;

	//////////////////////////////////////////////////////////////////////////
	// Sprite Variables
	//////////////////////////////////////////////////////////////////////////
	ID3DXSprite*		m_pD3DSprite;
	IDirect3DTexture9   * m_menuButtons,* m_menuBG,*	m_cursor,*m_battleBackgroundOne, *m_gamePiece, *m_archerCharacter, *m_blackMageCharacter, *m_playerUIBackground,
									*m_goldMine, *m_archerUnit, *m_blackMageUnit, *m_arrow, *m_fireball, *m_archerArrow, *m_healthBar, *m_golemUnit, *m_warlock,
									*m_lightning, *m_wolf, *m_thief, *m_warriorCharacter, *m_warriorUnitIcon, *m_marksmanUnit, *m_minotaurUnit, *m_blackHoleAbility,
									*m_flameStrikeAbility, *m_helpMenu, *m_cleaveAbility, *m_bolsterAbility;
	D3DXIMAGE_INFO		m_menuButtonsInfo,m_menuBGInfo,m_cursorInfo,m_battleBackgroundOneInfo, m_gamePieceInfo, m_archerCharacterInfo, m_blackMageCharacterInfo, m_playerUIBackgroundInfo,
									m_goldMineInfo, m_archerUnitInfo, m_blackMageUnitInfo, m_arrowInfo, m_fireballInfo, m_archerArrowInfo, m_healthBarInfo, m_golemUnitInfo,
									m_warlockInfo, m_lightningInfo, m_wolfInfo, m_thiefInfo, m_warriorCharacterInfo, m_warriorUnitIconInfo, m_marksmanUnitInfo, m_minotaurUnitInfo,
									m_blackHoleAbilityInfo, m_flameStrikeAbilityInfo, m_helpMenuInfo, m_cleaveAbilityInfo, m_bolsterAbilityInfo;

	IDirect3DTexture9		*m_archerIcon, *m_golemIcon, *m_blackMageIcon, *m_thiefIcon, *m_wolfIcon, *m_blackHoleIcon, *m_wallIcon, *m_warlockIcon, *m_endTurn, *m_flameStrikeIcon,
										*m_snipeIcon, *m_splitShotIcon, *m_wallIcon2, *m_archerHover, *m_wolfHover, *m_thiefHover, *m_wallHover, *m_blackMageHover, *m_golemHover, *m_warlockHover, 
										*m_warriorIcon, * m_marksmanIcon, *m_minotaurIcon;
	D3DXIMAGE_INFO			m_archerIconInfo, m_golemIconInfo, m_blackMageIconInfo, m_thiefIconInfo, m_wolfIconInfo, m_blackHoleIconInfo, m_wallIconInfo,m_warlockIconInfo, m_endTurnInfo,
										m_flameStrikeIconInfo, m_snipeIconInfo, m_splitShotIconInfo, m_wallIconInfo2,m_archerHoverInfo, m_wolfHoverInfo, m_thiefHoverInfo, m_wallHoverInfo, m_blackMageHoverInfo,
										m_golemHoverInfo, m_warlockHoverInfo, m_warriorIconInfo, m_marksmanIconInfo, m_minotaurIconInfo;
	D3DCOLOR				buttonColor;
	int						selectedUnit;

	//////////////////////////////////////////////////////////////////////////
	// DirectInput
	//////////////////////////////////////////////////////////////////////////
	IDirectInput8*		 m_pDIObject;	
	IDirectInputDevice8* m_pDIKeyboard;	
	IDirectInputDevice8* m_pDIMouse;

	//////////////////////////////////////////////////////////////////////////
	//  Mouse Information
	Cursor			myMouse;
	DIMOUSESTATE2	mouseState;
	D3DXVECTOR2		cursor;
	char			buffer[256];
	wchar_t			m_combatMessage[128];

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Audio Includes
	//  TODO:  
	FMOD::System* fmodSystem;
	FMOD::Sound* themeMusicOne, *battleTheme;
	FMOD::Sound* hitMines, *arrowHit, *castFireball, *shootArrow, *fireballHit, *thunderStrike, *minotaurRoar, *goldCoins, *warlockSpell, *cleaveAbilitySFX,
		*bolsterAbilitySFX, *chop, *blackHoleAbilitySFX, *flameWaveAbilitySFX;
	FMOD::Channel* mainChannel;
	FMOD::Channel* channel;
	bool keyIsDown[255];

	time_t start;

	//////////////////////////////////////////////////////////////////////////
	//  Buttons for game
	//////////////////////////////////////////////////////////////////////////
	std::vector<Buttons> menuButtons;
	std::vector<Buttons> characterButtons;
	std::vector<Buttons> hoverButtons;
	std::vector<Buttons> hoverButtons2;
	std::vector<Buttons> player1_units;
	std::vector<Buttons> player2_units;
	std::vector<Buttons> map_grid_row1;
	std::vector<Buttons> endTurnButton;
public:
	///////////////////////////////////////////////////////////////////////////////////////////
	//  INFO:  Game Data
	int						m_gameState, m_damageType;
	Character				m_player[2];
	BoardSpace				m_gameBoard[MAXBOARDHEIGHT][MAXBOARDWIDTH];
	Unit					m_unit[MAXBOARDHEIGHT][MAXBOARDWIDTH];
	ClassAbilityAnimator	m_classAbilityAnimator;
	//////////////////////////////////////////////////////////////////////////
	// Init and Shutdown are preferred to constructors and destructor,
	// due to having more control when to explicitly call them when global.
	//////////////////////////////////////////////////////////////////////////
	GameEngine(void);
	~GameEngine(void);

	//////////////////////////////////////////////////////////////////////////
	// Name:		Init
	// Parameters:	HWND hWnd - Handle to the window for the application
	//				HINSTANCE hInst - Handle to the application instance
	//				bool bWindowed - Boolean to control windowed or full-screen
	// Return:		void
	// Description:	Ran once at the start.  Initialize DirectX components and 
	//				variables to control the application.  
	//////////////////////////////////////////////////////////////////////////
	void Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed);
	void InitGameBoard();
	void InitMenu();
	//////////////////////////////////////////////////////////////////////////
	// Name:		Update
	// Parameters:	float elapsedTime - Time that has elapsed since the last
	//					update call.
	// Return:		void
	// Description: Runs every frame, use dt to limit functionality called to
	//				a certain amount of elapsed time that has passed.  Used 
	//				for updating variables and processing input commands prior
	//				to calling render.
	//////////////////////////////////////////////////////////////////////////
	void Update(float dt);
	void updateAnimations(float dt);
	void updateEventPhase(float dt);
	void findNextTarget(int row, int col);
	void resetUnitActions(int playerNumber);
	void moveUnit();
	void destroyUnit();
	void changePhase();
	void archerAbility1(bool);
	void blackHoleAbility(int, int, int);
	void flameWaveAbility(int, int, int);
	void cleaveAbility( int activePlayer );
	void bolsterAbility( int activePlayer );
	//////////////////////////////////////////////////////////////////////////
	// Name:		Render
	// Parameters:	float elapsedTime - Time that has elapsed since the last
	//					render call.
	// Return:		void
	// Description: Runs every frame, use dt to limit functionality called to
	//				a certain amount of elapsed time that has passed.  Render
	//				calls all draw call to render objects to the screen.
	//////////////////////////////////////////////////////////////////////////
	void Render(float dt);

	//////////////////////////////////////////////////////////////////////////
	// Name:		Shutdown
	// Parameters:	void
	// Return:		void
	// Description:	Runs once at the end of an application.  Destroy COM 
	//				objects and deallocate dynamic memory.
	//////////////////////////////////////////////////////////////////////////
	void Shutdown();
	void calcDeltaTime();

	void drawBackground();
	void drawGameBoard();
	void drawPlayers();

	void drawUIText(float dt);

	void drawIcons(int, Character, D3DXVECTOR3, D3DCOLOR);
	void drawButtonGrid(D3DXVECTOR3, D3DCOLOR);

	void drawMenu(D3DXVECTOR3, D3DCOLOR, RECT &);
	void drawCharacters(int, D3DXVECTOR3, D3DCOLOR, RECT &);
	void drawPlayerChoice(int);

	void drawWinner(Character);

	void drawEndTurn(D3DXVECTOR3, D3DCOLOR, RECT &);
	void drawHoverInfo(int, Character, D3DXVECTOR3, D3DCOLOR, int, float dt);
	void drawAbilityAnimations();
	void drawHelpMenuBackground();
	void drawHelpMenuText();
};