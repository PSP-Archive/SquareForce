//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#include <stdio.h>

#include "Game.h"
#include "utils.h"

#include "CResourceManager.h"

#include "MainPanel.h"
#include "FactoryPanel.h"



//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
Game::Game(): IGame()
{
	mFont = NULL;
	mGameLevel = NULL;

	mTutoPanel = NULL;
	mPanel = NULL;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
Game::~Game()
{
	
}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void Game::Create()
{
	CResourceManager* resMgr = CResourceManager::GetInstance();


	mFont = new JTTFont();
	mFont->Load("font.ttf", 22);

	mPaused = false;

	mPanel = NULL;

	mPlayTuto = false;

	mGameLevel = new GameLevel();
	mGameLevel->Create();

	mPlayTuto = true;
	mTutoPanel = new TutoPanel1(mGameLevel);
	mTutoPanel->Create();

	// on balance le panel d'assemblage des squareships
	mPaused = true;
	mGameLevel->Pause();
	mPanel = new FactoryPanel(mGameLevel);
	mPanel->Create();
}

//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void Game::Destroy()
{
	if(mGameLevel)
		mGameLevel->Destroy();

	if(mPanel)
		mPanel->Destroy();

	if(mTutoPanel)
		mTutoPanel->Destroy();

	SAFE_DELETE(mFont);

	SAFE_DELETE(mGameLevel);

	SAFE_DELETE(mPanel);

	SAFE_DELETE(mTutoPanel);
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void Game::Update()
{
	JGE* engine = JGE::GetInstance();

	if(mPaused && mPanel)
	{
		if(mPlayTuto && mTutoPanel)
		{
			mPanel->EnableSkipControls(true);

			mTutoPanel->Update();
			if(mTutoPanel->HasQuit())
			{
				mTutoPanel->Destroy();
				SAFE_DELETE(mTutoPanel);
				mPlayTuto = false;
				mPanel->EnableSkipControls(false);
			}

		}

		mPanel->Update();
		if(mPanel->HasQuit())
		{
			IPanel* newPanel = mPanel->GetNextPanel();
			if(!newPanel)
			{
				mPaused = false;
				mGameLevel->Resume();
			}
			mPanel->Destroy();
			SAFE_DELETE(mPanel);
			mPanel = newPanel;
		}
	}

	if(mGameLevel->HasQuit())
		mQuit = true;

	if (engine->GetButtonClick(PSP_CTRL_START) && !mPlayTuto)	
	{
		if(!mPaused)// launch main panel
		{
			SAFE_DELETE(mPanel);
			mPaused = true;
			mGameLevel->Pause();
			mPanel = new MainPanel(mGameLevel);
			mPanel->Create();
		}
		else// stop current panel
		{
			mPaused = false;
			mGameLevel->Resume();
			mPanel->Destroy();
			SAFE_DELETE(mPanel);
		}
	}

	mGameLevel->Update();
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void Game::Render()
{
	mGameLevel->Render();

	// en overlay
	if(mPaused && mPanel)
	{
		mPanel->Render();
	}

	// en overoverlay :P
	if(mPlayTuto && mTutoPanel)
	{
		mTutoPanel->Render();
	}
}