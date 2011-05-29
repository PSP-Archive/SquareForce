//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _PLANETMAINPANEL_H_
#define _PLANETMAINPANEL_H_

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JTTFont.h>
#include <JTypes.h>

#include "IPanel.h"
#include "GameLevel.h"

class PlanetMainPanel: public IPanel
{
	
public:
	PlanetMainPanel(GameLevel* gameLevel);
	virtual ~PlanetMainPanel();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();

protected:
	JTTFont* mFont;

	GameLevel* mGameLevel;

	int mLine;
};

#endif
