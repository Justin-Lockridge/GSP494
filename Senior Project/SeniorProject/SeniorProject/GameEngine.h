////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GameEngine class for GSP494 - Senior Project
//  By:  Justin Lockridge and Ricky Rivera
//  Date:  08 March 2014
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  KNOWN ISSUES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TODO:   []
//			[]
//			[]
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

class GameEngine{
	//////////////////////////////////////////////////////////////////////////
	// Application Variables
	//////////////////////////////////////////////////////////////////////////
	HWND				m_hWnd;					// Handle to the window
	bool				m_bVsync;				// Boolean for vertical syncing
	float				m_currTime;				// Time to render current frame
	float				m_prevTime, m_deltaTime, m_frameTime;				// Time to render previous frame
	int					m_FPS;			// Frames per second  

	//////////////////////////////////////////////////////////////////////////
	// Direct3D Variables
	//////////////////////////////////////////////////////////////////////////
	IDirect3D9*			m_pD3DObject;	// Direct3D 9 Object
	IDirect3DDevice9*	m_pD3DDevice;	// Direct3D 9 Device
	D3DCAPS9			m_D3DCaps;		// Device Capabilities

	//////////////////////////////////////////////////////////////////////////
	// Font Variables
	//////////////////////////////////////////////////////////////////////////
	ID3DXFont*			m_pD3DFont;		// Font Object

	//////////////////////////////////////////////////////////////////////////
	// Sprite Variables
	//////////////////////////////////////////////////////////////////////////
	ID3DXSprite*		m_pD3DSprite;	// Sprite Object
	IDirect3DTexture9*	m_battleBackgroundOne, *m_gamePiece, *m_archerCharacter, *m_blackMageCharacter, *m_playerUIBackground,
						*m_goldMine;		// Texture Object for a sprite
	D3DXIMAGE_INFO		m_battleBackgroundOneInfo, m_gamePieceInfo, m_archerCharacterInfo, m_blackMageCharacterInfo, m_playerUIBackgroundInfo,
						m_goldMineInfo;	// File details of a texture

	//////////////////////////////////////////////////////////////////////////
	// DirectInput
	//////////////////////////////////////////////////////////////////////////
	IDirectInput8*		 m_pDIObject;	// DirectInput Object
	IDirectInputDevice8* m_pDIKeyboard;	// Keyboard Device
	IDirectInputDevice8* m_pDIMouse;	// Mouse Device 

	//////////////////////////////////////////////////////////////////////////
	//  Mouse pos
	D3DVECTOR cursor;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Audio Includes
	//  TODO:  
	FMOD::System* fmodSystem;
	FMOD::Sound* themeMusicOne;
	FMOD::Channel* mainChannel;
	FMOD::Channel* channel;
	bool keyIsDown[255];

	time_t start;

	///////////////////////////////////////////////////////////////////////////////////////////
	//  INFO:  Game Data
	int m_gameState;
	Character m_player[2];
	BoardSpace m_gameBoard[MAXBOARDHEIGHT][MAXBOARDWIDTH];

	public:
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

	//////////////////////////////////////////////////////////////////////////
	// Name:		Render
	// Parameters:	float elapsedTime - Time that has elapsed since the last
	//					render call.
	// Return:		void
	// Description: Runs every frame, use dt to limit functionality called to
	//				a certain amount of elapsed time that has passed.  Render
	//				calls all draw call to render objects to the screen.
	//////////////////////////////////////////////////////////////////////////
	void Render();

	//////////////////////////////////////////////////////////////////////////
	// Name:		Shutdown
	// Parameters:	void
	// Return:		void
	// Description:	Runs once at the end of an application.  Destroy COM 
	//				objects and deallocate dynamic memory.
	//////////////////////////////////////////////////////////////////////////
	void Shutdown();
	void drawBackground();
	void drawGameBoard();
	void drawPlayers();
	void drawUIText();
	void calcDeltaTime();

};