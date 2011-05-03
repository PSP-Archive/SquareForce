//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _GAMEINTRO_H_
#define _GAMEINTRO_H_

#include "IGame.h"


class GameIntro : public IGame
{

private:

public:
	GameIntro();
	virtual ~GameIntro();
	virtual void Create();
	virtual void Destroy();
	virtual void Pause();
	virtual void Resume();
	virtual void Update();
	virtual void Render();

};



#endif
