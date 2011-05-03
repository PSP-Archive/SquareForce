//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _FACTORYPANEL_H_
#define _FACTORYPANEL_H_

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JTTFont.h>
#include <JTypes.h>

#include "IPanel.h"
#include "GameLevel.h"

class FactoryPanel: public IPanel
{
	
public:
	FactoryPanel(GameLevel* gameLevel);
	virtual ~FactoryPanel();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();

protected:
	JTTFont* mFont;

	GameLevel* mGameLevel;

	float mDeltaTime;
	float mTimer;

	int mTilePosX;
	int mTilePosY;

	int mInvTilePosX;
	int mInvTilePosY;

	bool mIsChoosingTile;

	CSquareTile* mInv[25];
};

#endif
