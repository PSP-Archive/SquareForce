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

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JGBKFont.h>
#include <JSoundSystem.h>

#include <JTTFont.h>
#include <Encoding.h>

#include "GameIntro.h"

GameIntro::GameIntro(): IGame()
{

}


GameIntro::~GameIntro()
{
	
}


void GameIntro::Create()
{
	
}


void GameIntro::Destroy()
{

}


void GameIntro::Pause()
{

}

void GameIntro::Resume()
{

}


void GameIntro::Update()
{
	JGE* engine = JGE::GetInstance();

	if (engine->GetButtonClick(PSP_CTRL_CROSS) || engine->GetButtonClick(PSP_CTRL_START))	
	{
		mQuit = true;
	}
}


void GameIntro::Render()
{

}
