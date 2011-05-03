//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _GAME_H_
#define _GAME_H_

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JTTFont.h>
#include <JTypes.h>
#include <JSoundSystem.h>

#include "IGame.h"
#include "IPanel.h"

#include "TutoPanel1.h"

#include "GameLevel.h"

class JTTFont;

class Game: public IGame
{
	
public:
	Game();
	virtual ~Game();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause() {}
	virtual void Resume() {}

protected:

	GameLevel *mGameLevel;

	bool mPaused;

	JTTFont* mFont;

	IPanel* mPanel;

	bool mPlayTuto;
	TutoPanel1* mTutoPanel;

};

#endif
