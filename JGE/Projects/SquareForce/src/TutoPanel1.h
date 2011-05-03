//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _TUTOPANEL1_H_
#define _TUTOPANEL1_H_

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JTTFont.h>
#include <JTypes.h>

#include "IPanel.h"
#include "GameLevel.h"

#include <vector>
#include <string>

using namespace std;

class TutoPanel1: public IPanel
{
	
public:
	TutoPanel1(GameLevel* gameLevel);
	virtual ~TutoPanel1();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();

protected:
	JTTFont* mFont;

	GameLevel* mGameLevel;

	vector< vector<string> > mTexts;
};

#endif
