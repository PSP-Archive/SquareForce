//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _GAMEINTROLOGO_H_
#define _GAMEINTROLOGO_H_

#include "GameIntro.h"


class JTexture;
class hgeDistortionMesh;
class hgeFont;

class GameIntroLogo:	public GameIntro
{
	
public:
	GameIntroLogo();
	virtual ~GameIntroLogo();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

private:
 	JTexture* mTex;

 	hgeDistortionMesh* mDistortionMesh;
 	hgeFont* mFont;

 	int mRows;
 	int mCols;
 	float mCellw;
 	float mCellh;

 	float mMeshx;
 	float mMeshy;

	bool mIsFadingIn;
	bool mIsFadingOut;
	float mFaderTimer;
	float mQuitTimer;
};

#endif
