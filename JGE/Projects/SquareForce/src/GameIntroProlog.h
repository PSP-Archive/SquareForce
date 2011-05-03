//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _GAMEINTROPROLOG_H_
#define _GAMEINTROPROLOG_H_

#include "GameIntro.h"
#include <vector>
#include <string>

using namespace std;

class JTTFont;
class JMusic;


class GameIntroProlog:	public GameIntro
{

private:

	JTexture* mTexBG;
	JQuad* mBGMesh;

	JTTFont* mFont;
	float mDeltaTime;

	float mSpeed;
	float mTotalTime;
	
	vector< vector<string> > mTexts;

	JMusic* mMusic;

	float mOffset;
	
public:
	GameIntroProlog();
	virtual ~GameIntroProlog();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

};



#endif
