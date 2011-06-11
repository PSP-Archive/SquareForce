//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _GAMELEVEL_H_
#define _GAMELEVEL_H_

#include <vector>

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JTTFont.h>
#include <JTypes.h>
#include <JSoundSystem.h>

#include "Box2D.h"

#include "CWorldObjects.h"
#include "CSpawnManager.h"

using namespace std;

class GameLevel
{
	
public:
	GameLevel();
	virtual ~GameLevel();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

	virtual void Reset();

	inline bool HasQuit() {return mQuit;}
	inline void Quit() {mQuit = true;}


	CWorldObjects* mWorldObjects;


	CSquareTile* mInventory[SQUARETILE_INVENTORY_SIZE];

protected:
	void DrawGui();

	void UpdateControler();

	bool mPaused;
	bool mQuit;
	
	JTTFont* mFont;

	JMusic* mMusic;

	JTexture* mTargetReticleTex;
	JQuad* mTargetReticleQuad;

	JTexture* mMinimapTex;
	JQuad* mMinimapQuad;

	Vector2D mTargetReticlePos;// position locale % au centre de l'écran du réticule de visée
	Vector2D mTargetReticleWorldPos;// world position

	float mMinimapSight;

	float mDeltaTime;

	CObject *mTarget;

};

#endif
