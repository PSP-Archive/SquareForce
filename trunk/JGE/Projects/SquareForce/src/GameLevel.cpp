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
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "CLocalization.h"
#include "CResourceManager.h"

#include "GameLevel.h"
#include "utils.h"



//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameLevel::GameLevel()
{
	mWorldObjects = NULL;
	mFont = NULL;

	mTargetReticleTex = NULL;
	mTargetReticleQuad = NULL;

	mMinimapTex = NULL;
	mMinimapQuad = NULL;

	mMusic = NULL;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameLevel::~GameLevel()
{

}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameLevel::Create()
{
	mFont = new JTTFont();
	mFont->Load("font.ttf", 12);

	mTargetReticleTex = JRenderer::GetInstance()->LoadTexture("reticle.png", TEX_TYPE_USE_VRAM);
	mTargetReticleQuad = new JQuad(mTargetReticleTex, 0, 0, (float)mTargetReticleTex->mWidth, (float)mTargetReticleTex->mHeight);
	mTargetReticleQuad->SetHotSpot((float)mTargetReticleTex->mWidth*0.5f, (float)mTargetReticleTex->mHeight*0.5f);

	mMinimapTex = JRenderer::GetInstance()->LoadTexture("minimap.png", TEX_TYPE_USE_VRAM);
	mMinimapQuad = new JQuad(mMinimapTex, 0, 0, (float)mMinimapTex->mWidth, (float)mMinimapTex->mHeight);
	mMinimapQuad->SetHotSpot((float)mMinimapTex->mWidth*0.5f, (float)mMinimapTex->mHeight*0.5f);

	mMinimapSight = PLAYER_MAX_SIGHT;

	mPaused = false;
	mQuit = false;

#ifndef NO_SOUND
	JSoundSystem* sound = JSoundSystem::GetInstance();

	if (sound)
		mMusic = sound->LoadMusic("2-11 bLiNd - JENOVA Celestial (J-E-N-O-V-A).mp3");	// Load a background music.

	if (mMusic)
		sound->PlayMusic(mMusic, true);
	sound->SetMusicVolume(50);
#endif

	CResourceManager* resMgr = CResourceManager::GetInstance();

	for(int i=0; i<SQUARETILE_INVENTORY_SIZE; i++)
		mInventory[i] = NULL;
	for(int i=0; i<9; i++)
		mInventory[i] = new CSquareTileHull((CSquareTileHull*)(resMgr->mListTiles[1]));
	for(int i=9; i<15; i++)
		mInventory[i] = new CSquareTileEngine((CSquareTileEngine*)(resMgr->mListTiles[2]));
	for(int i=15; i<21; i++)
		mInventory[i] = new CSquareTileGun((CSquareTileGun*)(resMgr->mListTiles[3]));

	Reset();

}


void GameLevel::Reset()
{
	//PspAssert(false && "begin reset");
	SAFE_DELETE(mWorldObjects);

	mWorldObjects = new CWorldObjects();
	//PspAssert(false && "CWorldObjects new");
	
	mWorldObjects->Create();
	//PspAssert(false && "CWorldObjects created");

	Matrix22 camMat = mWorldObjects->mCamMat;

	mTargetReticlePos = Vector2D(0, 0);
	mTargetReticleWorldPos = mWorldObjects->mCamPos + camMat * mTargetReticlePos;

	JGE* engine = JGE::GetInstance();
	mDeltaTime = engine->GetDelta(); // on reset le timer (un coup ds le vent)
	mDeltaTime = 0.0f;

	mTarget = NULL;
	//PspAssert(false && "end reset");
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameLevel::Destroy()
{
	SAFE_DELETE(mWorldObjects);

	if(mFont)
		mFont->Unload();
	SAFE_DELETE(mFont);

	SAFE_DELETE(mMinimapQuad);
	SAFE_DELETE(mMinimapTex);

	SAFE_DELETE(mTargetReticleQuad);
	SAFE_DELETE(mTargetReticleTex);

#ifndef NO_SOUND
	JSoundSystem* sound = JSoundSystem::GetInstance();
	sound->StopMusic(mMusic);
 	SAFE_DELETE(mMusic);
#endif

	for(int i=0; i<SQUARETILE_INVENTORY_SIZE; i++)
		SAFE_DELETE(mInventory[i]);

}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameLevel::Update()
{
	JGE* engine = JGE::GetInstance();
	mDeltaTime = engine->GetDelta();		// Get time elapsed since last update.

	if(mPaused)
		return;

	// on update tout
	if (mWorldObjects)
		mWorldObjects->Update(mDeltaTime);

	UpdateControler();

}

void GameLevel::UpdateControler()
{
	if(mWorldObjects->mHero->IsLanded() || mWorldObjects->mHero->GetAI())
		return;

	JGE* engine = JGE::GetInstance();

	mWorldObjects->mHero->mAngularPower = 0.0f;

	// on crée une rotation proportionnelle à la position du réticule % au centre du vaisseau
// 	Vector2D reticleDir = mTargetReticlePos - 
// 		(mWorldObjects->mHero->GetCenterPosition()-mWorldObjects->mCamPos).Rotate(-mWorldObjects->mCamRot);
// 	float l = reticleDir.Normalize();
// 	float dr = b2Dot(Vector2D(-1, 0), reticleDir);
// 	dr *= abs(dr);// on utilise une courbe parabole
// 	if(l < RETICLE_DEAD_ZONE)// on crée une zone morte
// 		dr = 0.0f;
// 	else if(b2Dot(Vector2D(0, 1), reticleDir) < 0)
// 		dr = (dr > 0.0f)?1.0f:-1.0f;
// 	mWorldObjects->mHero->mAngularPower = dr;
// 	if(mWorldObjects->mHero->mAngularPower > 1.0f)
// 		mWorldObjects->mHero->mAngularPower = 1.0f;
// 	else if(mWorldObjects->mHero->mAngularPower < -1.0f)
// 		mWorldObjects->mHero->mAngularPower = -1.0f;
	Vector2D reticleDir = mTargetReticlePos;
		/*-(mWorldObjects->mHero->GetCenterPosition()-mWorldObjects->mCamPos).Rotate(-mWorldObjects->mCamRot);*/
	float l = reticleDir.Length();
	float dr;
	if(l<RETICLE_DEAD_ZONE)
		dr = 0;
	else
	{
		if(reticleDir.y<-30.0f)
		{
			if(reticleDir.x>0.0f)
				dr = 1.0f;
			else
				dr = -1.0f;
		}
		else
		{
			if(abs(reticleDir.x)<RETICLE_DEAD_ZONE)
				dr = 0.0f;
			else if(reticleDir.x>200.0f)
				dr = 1.0f;
			else if(reticleDir.x<-200.0f)
				dr = -1.0f;
			else
				dr = reticleDir.x/200.0f;
		}
	}
	mWorldObjects->mHero->mAngularPower = -dr;



	if (engine->GetButtonClick(PSP_CTRL_TRIANGLE))		
	{
		mWorldObjects->mHero->ToggleStopEngine();
	}

	if (engine->GetButtonState(PSP_CTRL_CROSS))	
	{
		mWorldObjects->mHero->mStopEngine = false;
		mWorldObjects->mHero->mEnginePower += 0.05f;
		if(mWorldObjects->mHero->mEnginePower > 1.0f)
			mWorldObjects->mHero->mEnginePower = 1.0f;
	}
	else if(!mWorldObjects->mHero->mStopEngine)
	{
		mWorldObjects->mHero->mEnginePower -= 0.05f;
		if(mWorldObjects->mHero->mEnginePower < 0.0f)
			mWorldObjects->mHero->mEnginePower = 0.0f;
	}

	if (engine->GetButtonClick(PSP_CTRL_SQUARE))	
	{
		mWorldObjects->mHero->mStopEngine = false;
		mWorldObjects->mHero->DashL();
	}

	if (engine->GetButtonClick(PSP_CTRL_CIRCLE))	
	{
		mWorldObjects->mHero->mStopEngine = false;
		mWorldObjects->mHero->DashR();
	}

	if (engine->GetButtonClick(PSP_CTRL_START))	
	{
// 		Reset();
// 		return;
	}

	if (engine->GetButtonClick(PSP_CTRL_SELECT))	
	{
		mMinimapSight += PLAYER_MAX_SIGHT/MINIMAP_NB_ZOOM_LEVELS;
		if(mMinimapSight> PLAYER_MAX_SIGHT + 1.0f)
			mMinimapSight = PLAYER_MAX_SIGHT/MINIMAP_NB_ZOOM_LEVELS;
	}

	if (engine->GetButtonState(PSP_CTRL_UP))
	{

	}

	if (engine->GetButtonClick(PSP_CTRL_DOWN))
	{
		CPlanet *planet = NULL;
		float dist2 = mWorldObjects->GetNearestPlanet(mWorldObjects->mHero->GetOriginPosition(), planet);
		if(planet)// on atterri sur la planete si on est au dessus
		{
			float distTest = planet->GetSize()*PLANET_TEXTURE_SIZE/2;
			distTest *= distTest;
			if(dist2 < distTest)
			{
				mWorldObjects->mHero->Land(true);
			}
		}
		// on fait une requete d'attache au speedway 
		mWorldObjects->mHero->RequestDock(true);
	}

	if (engine->GetButtonState(PSP_CTRL_RIGHT))
	{
		mWorldObjects->mHero->mAngularPower -= 1.0f;
		if(mWorldObjects->mHero->mAngularPower < -1.0f)
			mWorldObjects->mHero->mAngularPower = -1.0f;
	}

	if (engine->GetButtonState(PSP_CTRL_LEFT))
	{
		mWorldObjects->mHero->mAngularPower += 1.0f;
		if(mWorldObjects->mHero->mAngularPower > 1.0f)
			mWorldObjects->mHero->mAngularPower = 1.0f;
	}
	
	Matrix22 camMat = Matrix22(mWorldObjects->mCamRot);

	// lock
	if(engine->GetButtonClick(PSP_CTRL_LTRIGGER))
	{
		mTarget = mWorldObjects->GetNearestObject(mTargetReticleWorldPos, mWorldObjects->mHero);
		if(mTarget && (mTarget->GetOriginPosition()-mTargetReticleWorldPos).Length2() > 40000.0f)
			mTarget = NULL;
	}
	// unlock
	if(engine->GetButtonState(PSP_CTRL_LTRIGGER))
	{
		if(mTarget && (mTarget->IsDestroyed() || 
			(mTarget->GetOriginPosition()-mWorldObjects->mHero->GetOriginPosition()).Length2() > 250000.0f))
			mTarget = NULL;
	}
	else
		mTarget = NULL;

	// on déplace le réticule de visée
	float dvx = (float)(engine->GetAnalogX()-0x80)/181.0f;
	if(abs(dvx) < 0.15f) dvx = 0.0f;
	float dvy = -(float)(engine->GetAnalogY()-0x80)/181.0f;
	if(abs(dvy) < 0.15f) dvy = 0.0f;
	Vector2D dv = Vector2D(dvx, dvy);// dv max normalisé (181=sqrt(128²+128²))
	
	float cursorMaxSpeed;
	float offset;
	float ratio;
	Vector2D magneticPoint;
	if(mTarget)
	{
		cursorMaxSpeed = 50.0f;
		offset = 100.0f;
		ratio = 2.0f;
		magneticPoint = mTarget->GetOriginPosition();
	}
	else
	{
		cursorMaxSpeed = 20.0f;
		offset = (float)RETICLE_DEAD_ZONE;
		ratio = 2.0f;
		magneticPoint = mWorldObjects->mCamPos;
	}
	//dv *= cursorMaxSpeed;
	//mTargetReticlePos += 0.016f*dv;
	Vector2D localMagneticPoint = camMat / (magneticPoint-mWorldObjects->mCamPos);
	if(mTarget)
		localMagneticPoint += 100.0f*dv;
	Vector2D localDir = mTargetReticlePos - localMagneticPoint;
	float f = localDir.Normalize();
	//mTargetReticlePos -= cursorMaxSpeed*0.016f*f/maxOffset * localDir;
	if(f < offset)
	{
		float val = f/offset;
		val = (1.0f-(1.0f-val)*(1.0f-val));
		if(!mTarget)
			mTargetReticlePos += cursorMaxSpeed*(0.1f+0.9f*val)*ratio*dv;
		mTargetReticlePos -= cursorMaxSpeed*val*localDir;
	}
	else
	{
		if(!mTarget)
			mTargetReticlePos += cursorMaxSpeed*ratio*dv;
		mTargetReticlePos -= cursorMaxSpeed*localDir;
	}

	if(!mTarget)
	{
		if(mTargetReticlePos.x > SCREEN_SIZE_X2)
			mTargetReticlePos.x = SCREEN_SIZE_X2;
		else if(mTargetReticlePos.x < -SCREEN_SIZE_X2)
			mTargetReticlePos.x = -SCREEN_SIZE_X2;
		if(mTargetReticlePos.y > SCREEN_SIZE_Y2)
			mTargetReticlePos.y = SCREEN_SIZE_Y2;
		else if(mTargetReticlePos.y < -SCREEN_SIZE_Y2)
			mTargetReticlePos.y = -SCREEN_SIZE_Y2;
	}


	mTargetReticleWorldPos = mWorldObjects->mCamPos + camMat * mTargetReticlePos;



	if (engine->GetButtonState(PSP_CTRL_RTRIGGER))
	{
		Vector2D shootPos = mTargetReticleWorldPos;
		if(mTarget)
		{
			shootPos = mWorldObjects->mHero->GetShootPoint(mTarget->GetOriginPosition(), mTarget->GetLinearVelocity());
			shootPos += mTargetReticleWorldPos - mTarget->GetOriginPosition();
		}
		mWorldObjects->mHero->FireAt(shootPos, 0.0f);
	}
}

//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameLevel::Render()
{
	DebugLog("Begin GameLevel Render()");
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();		

	// clear screen to black
	renderer->ClearScreen(ARGB(255, 0, 0, 0));
	//renderer->Enable3D(); renderer->Set3DFlag(true); 
	//renderer->Translate(-SCREEN_SIZE_X2 , -SCREEN_SIZE_Y2, -177);

	//
	// Your rendering code here...
	//

	if (mWorldObjects)
	{
		mWorldObjects->Render();
		//mWorldObjects->RenderShape();
	}


	DrawGui();	

	char txt[100];

	int val = 0;
	if(mDeltaTime>0.0f)
		val = (int)(1.0f/mDeltaTime);
	sprintf(txt, "fps=%d", val);
	mFont->SetColor(ARGB(255,255,255,255));
	mFont->RenderString(txt, 0, 0);

	sprintf(txt, "engine=%.3f, speed=%.2f", mWorldObjects->mHero->mEnginePower, 
		mWorldObjects->mHero->GetLinearVelocity().Length());
	mFont->SetColor(ARGB(255,0,255,255));
	mFont->RenderString(txt, 0, 260);

// 	if(CResourceManager::GetInstance()->IsLoading())
// 	{
// 		mFont->SetColor(ARGB(255,255,0,0));
// 		mFont->RenderString("loading...", 0, 20);
// 	}
// 	mFont->SetColor(ARGB(255,0,255,0));
// 	mFont->RenderString(CResourceManager::GetInstance()->GetCurrentPlanetTexName().c_str(), 0, 30);
// 	mFont->RenderString(CResourceManager::GetInstance()->GetCurrentCloudsTexName().c_str(), 0, 40);

// 	JGE* engine = JGE::GetInstance();
// 	sprintf(txt, "heroRot=%f", mWorldObjects->mHero->GetRotation());
// 	mFont->SetColor(ARGB(255,0,255,255));
// 	mFont->RenderString(txt, 350, 0);
// 	sprintf(txt, "camRot=%f", mWorldObjects->mCamRot);
// 	mFont->SetColor(ARGB(255,0,255,255));
// 	mFont->RenderString(txt, 350, 10);

	DebugLog("End GameLevel Render()");
}


void GameLevel::DrawGui()
{
	DebugLog("Begin GameLevel DrawGui()");
	static int centerX = SCREEN_SIZE_X-mMinimapTex->mWidth/2-1;
	static int centerY = mMinimapTex->mHeight/2+1;
	
	float minimapRatio = MINIMAP_RADIUS/mMinimapSight;

	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();	

	renderer->RenderQuad(mMinimapQuad, (float)centerX, (float)centerY);
	DebugLog("Minimap hud rendered");

	float sightMax2 = mMinimapSight*mMinimapSight;
	CSpawnManager* mgr = mWorldObjects->mSpawnMgr;

	Vector2D refPos = mgr->GetHero()->GetCenterPosition();
	Matrix22 camMat(mWorldObjects->mCamRot);
	Vector2D dir = camMat / Vector2D(0, MINIMAP_RADIUS);

	int i = 0;
	CPlanet* planet = NULL;
	while((planet = mgr->GetPlanet(i++)))
	{
		Vector2D planetPos = planet->GetOriginPosition();
		Vector2D planetDir = planetPos - refPos;
		if(planetDir.Length2() <= sightMax2)
		{
			planetDir = camMat / (minimapRatio * planetDir);
			renderer->FillCircle(centerX+planetDir.x, centerY-planetDir.y, 4.0f, ARGB(255,255,255,255));
		}
	}
	DebugLog("Minimap planets rendered");

	CObject* obj = NULL;
	i = 1;
	while((obj = mgr->GetObject(i++)))
	{
		Vector2D shipPos = obj->GetOriginPosition();
		Vector2D shipDir = shipPos - refPos;
		if(shipDir.Length2() <= sightMax2)
		{
			shipDir = camMat / (minimapRatio * shipDir);
			renderer->FillCircle(centerX+shipDir.x, centerY-shipDir.y, 1.0f, ARGB(255,255,0,0));
		}
	}
	DebugLog("Minimap objects rendered");

	renderer->DrawLine((float)centerX, (float)centerY, (float)centerX+dir.x, (float)centerY-dir.y, ARGB(255,255,0,0));
	renderer->DrawLine((float)centerX, (float)centerY, (float)centerX-dir.x, (float)centerY+dir.y, ARGB(255,0,255,0));

	char txt[50] = "";
	sprintf(txt, "%d", (int)(mMinimapSight));
	mFont->SetColor(ARGB(255,0,255,0));
	mFont->RenderString(txt, (float)centerX, (float)centerY+MINIMAP_RADIUS);

	// targeting
	renderer->EnableTextureFilter(false);
	PIXEL_TYPE color;
	if(mTarget)
		color = ARGB(255, 255, 0, 0);
	else
		color = ARGB(255, 255, 255, 255);
	dir = mTargetReticlePos;
	float size = dir.Normalize();
	float l = 3.0f;
	for(int i=1; i<4; i++)
	{
		renderer->DrawLine((float)SCREEN_SIZE_X2+dir.x*size*0.25f*i, (float)SCREEN_SIZE_Y2-dir.y*size*0.25f*i, (float)SCREEN_SIZE_X2+dir.x*size*0.25f*i-dir.y*l-dir.x*l, (float)SCREEN_SIZE_Y2-dir.y*size*0.25f*i-dir.x*l+dir.y*l, color);
		renderer->DrawLine((float)SCREEN_SIZE_X2+dir.x*size*0.25f*i, (float)SCREEN_SIZE_Y2-dir.y*size*0.25f*i, (float)SCREEN_SIZE_X2+dir.x*size*0.25f*i+dir.y*l-dir.x*l, (float)SCREEN_SIZE_Y2-dir.y*size*0.25f*i+dir.x*l+dir.y*l, color);
	}
	renderer->DrawCircle((float)SCREEN_SIZE_X2, (float)SCREEN_SIZE_Y2, 1.0f, color);
	mTargetReticleQuad->SetColor(color);
	renderer->RenderQuad(mTargetReticleQuad, (float)(SCREEN_SIZE_X2+(int)mTargetReticlePos.x), (float)(SCREEN_SIZE_Y2-(int)mTargetReticlePos.y));
	renderer->EnableTextureFilter(true);
	DebugLog("Targeting rendered");

	DebugLog("Begin GameLevel DrawGui()");
}


//-------------------------------------------------------------------------------------
// This function is called when the system wants to pause the game. You can set a flag
// here to stop the update loop and audio playback.
//
//-------------------------------------------------------------------------------------
void GameLevel::Pause()
{
	mPaused = true;

#ifndef NO_SOUND
	//JSoundSystem* sound = JSoundSystem::GetInstance();
	//sound->PauseMusic(mMusic);
#endif
}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameLevel::Resume()
{
	mPaused = false;

#ifndef NO_SOUND
	//JSoundSystem* sound = JSoundSystem::GetInstance();
	//sound->ResumeMusic(mMusic);
#endif
}
