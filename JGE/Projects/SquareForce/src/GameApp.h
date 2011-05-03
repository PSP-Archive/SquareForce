//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _GAMEAPP_H_
#define _GAMEAPP_H_

#include <JApp.h>

#include "IGame.h"


class GameApp:	public JApp
{
	
public:
	GameApp();
	virtual ~GameApp();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

private:
	IGame* mGame;

	enum ETypeGame{TYPE_MENU_LANG, TYPE_LOGO_GENESIS, TYPE_LOGO_XTC, TYPE_LOGO_DREAMRAISER, TYPE_GAME, TYPE_PROLOG, TYPE_MENU, TYPE_MENU_CREDITS, TYPE_COUNT};
	ETypeGame mTypeGame;

};

#endif
