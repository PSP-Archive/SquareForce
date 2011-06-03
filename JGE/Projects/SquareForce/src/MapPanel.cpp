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

#include "MapPanel.h"
#include "utils.h"

#include "CLocalization.h"
#include "CResourceManager.h"

#include "CObject.h"
#include "CPlanet.h"
#include "CSpeedWay.h"


//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
MapPanel::MapPanel(GameLevel* gameLevel): IPanel(), mGameLevel(gameLevel)
{
	mFont = NULL;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
MapPanel::~MapPanel()
{
}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void MapPanel::Create()
{
	mFont = new JTTFont();
	mFont->Load("font.ttf", 16);

	mQuit = false;
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void MapPanel::Destroy()
{
	SAFE_DELETE(mFont);
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void MapPanel::Update()
{
	JGE* engine = JGE::GetInstance();
	mDeltaTime = engine->GetDelta();		// Get time elapsed since last update.

	
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void MapPanel::Render()
{
	static int centerX = SCREEN_SIZE_X2;
	static int centerY = SCREEN_SIZE_Y2;

	static float minimapRatio = SCREEN_SIZE_Y2/25000.0f;

	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();	

	renderer->FillRect(0, 0, SCREEN_SIZE_X, SCREEN_SIZE_Y, ARGB(255, 0, 0, 0));

	CSpawnManager* mgr = mGameLevel->mWorldObjects->mSpawnMgr;

	CSpeedWay* sw = NULL;
	int i = 0;
	while((sw = mgr->GetSpeedWay(i++)))
	{
		Vector2D swPos1 = minimapRatio*sw->GetStart();
		Vector2D swPos2 = minimapRatio*sw->GetEnd();
		renderer->DrawLine(centerX+swPos1.x, centerY-swPos1.y, centerX+swPos2.x, centerY-swPos2.y, 1.0f, ARGB(255,0,0,255));
	}

	i = 0;
	CPlanet* planet = NULL;
	while((planet = mgr->GetPlanet(i++)))
	{
		Vector2D planetPos = minimapRatio*planet->GetOriginPosition();
		float planetRadius = planet->GetSize()*PLANET_TEXTURE_SIZE/2;
		renderer->FillCircle(centerX+planetPos.x, centerY-planetPos.y, planetRadius*minimapRatio, ARGB(255,128,128,255));
		mFont->SetColor(ARGB(255,255,255,255));
		mFont->RenderString(planet->GetName().c_str(), centerX+planetPos.x, centerY-planetPos.y-20.0f, JGETEXT_CENTER);
	}

	Vector2D shipPos = minimapRatio*mgr->GetHero()->GetOriginPosition();
	renderer->FillCircle(centerX+shipPos.x, centerY-shipPos.y, 2.0f, ARGB(255,0,255,0));

// 	CObject* obj = NULL;
// 	i = 1;
// 	while((obj = mgr->GetObject(i++)))
// 	{
// 		Vector2D shipPos = minimapRatio*obj->GetOriginPosition();
// 		renderer->FillCircle(centerX+shipPos.x, centerY-shipPos.y, 1.0f, ARGB(255,255,0,0));
// 	}

}
