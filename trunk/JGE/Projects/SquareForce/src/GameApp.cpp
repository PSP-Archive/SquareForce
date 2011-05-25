//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#include "GameApp.h"


#include "Game.h"
#include "GameMenuLang.h"
#include "GameIntroLogoGenesis.h"
#include "GameIntroLogo.h"
#include "GameIntroXTC.h"
#include "GameMenu.h"
#include "GameMenuCredits.h"
#include "GameIntroProlog.h"

#include "CLocalization.h"
#include "CResourceManager.h"

#define HACK_SKIP_LOGO


//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameApp::GameApp()
{
	mGame = NULL;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameApp::~GameApp()
{
	CLocalization::DestroyInstance();
	CResourceManager::DestroyInstance();
}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Create()
{
	JRenderer* renderer = JRenderer::GetInstance();
	
#ifdef HACK_SKIP_LOGO
	mTypeGame = TYPE_GAME;
	mGame = new Game;

	// on met le langage en FR
	CLocalization* instance = CLocalization::GetInstance();
	instance->GetLangList();
	instance->SetLanguage(1);
	instance->LoadStrings();
	instance->LoadSquareTilesStrings();
#else 	
	mTypeGame = TYPE_MENU_LANG;
 	mGame = new GameMenuLang;
#endif

	mGame->Create();
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Destroy()
{
	if(mGame)
		mGame->Destroy();

	SAFE_DELETE(mGame);
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameApp::Update()
{
	if(!mGame)
		return;

	JGE* engine = JGE::GetInstance();

	if(mGame->HasQuit())
	{
		switch (mTypeGame)
		{
		case TYPE_MENU_LANG:
			{
				mGame->Destroy();
				delete mGame;
				mGame = new GameLogoGenesis;
				mTypeGame = TYPE_LOGO_GENESIS;
			}
			break;
		case TYPE_LOGO_GENESIS:
			{
				mGame->Destroy();
				delete mGame;
				mGame = new GameIntroXTC;
				mTypeGame = TYPE_LOGO_XTC;
			}
			break;
		case TYPE_LOGO_XTC:
			{
				mGame->Destroy();
				delete mGame;
				mGame = new GameIntroLogo;
				mTypeGame = TYPE_LOGO_DREAMRAISER;
			}
			break;
		case TYPE_LOGO_DREAMRAISER:
			{
				mGame->Destroy();
				delete mGame;
				mGame = new GameMenu;
				mTypeGame = TYPE_MENU;
			}
			break;
		case TYPE_MENU:
			{
				if(((GameMenu*)mGame)->GetLine() == 0)
				{
					mGame->Destroy();
					delete mGame;
					mGame = new GameIntroProlog;
					mTypeGame = TYPE_PROLOG;
				}
				else if(((GameMenu*)mGame)->GetLine() == 2)
				{
					mGame->Destroy();
					delete mGame;
					mGame = new GameMenuCredits;
					mTypeGame = TYPE_MENU_CREDITS;
				}
			}
			break;
		case TYPE_MENU_CREDITS:
			{
				mGame->Destroy();
				delete mGame;
				mGame = new GameMenu;
				mTypeGame = TYPE_MENU;
			}
			break;
		case TYPE_PROLOG:
			{
				mGame->Destroy();
				delete mGame;
				mGame = new Game;
				mTypeGame = TYPE_GAME;
			}
			break;
		case TYPE_GAME:
			{
				mGame->Destroy();
				delete mGame;
				mGame = new GameMenu;
				mTypeGame = TYPE_MENU;
			}
			break;
		default:
			{
				engine->End();
			}
			break;
		}	
		mGame->Create();
	}


	mGame->Update();

	if (engine->GetButtonState(PSP_CTRL_LTRIGGER) && 
		engine->GetButtonState(PSP_CTRL_RTRIGGER) && 
		engine->GetButtonState(PSP_CTRL_SELECT) )
		engine->End();
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameApp::Render()
{
	if(!mGame)
		return;

	mGame->Render();
}


//-------------------------------------------------------------------------------------
// This function is called when the system wants to pause the game. You can set a flag
// here to stop the update loop and audio playback.
//
//-------------------------------------------------------------------------------------
void GameApp::Pause()
{
	if(!mGame)
		return;

	mGame->Pause();
}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameApp::Resume()
{
	if(!mGame)
		return;

	mGame->Resume();	
}
