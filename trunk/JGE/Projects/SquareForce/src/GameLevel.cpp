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

	mSpawnMgr = NULL;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameLevel::~GameLevel()
{
	SAFE_DELETE(mSpawnMgr);
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
	mTargetReticleQuad = new JQuad(mTargetReticleTex, 0, 0, mTargetReticleTex->mWidth, mTargetReticleTex->mHeight);
	mTargetReticleQuad->SetHotSpot((float)mTargetReticleTex->mWidth*0.5f, (float)mTargetReticleTex->mHeight*0.5f);

	mMinimapTex = JRenderer::GetInstance()->LoadTexture("minimap.png", TEX_TYPE_USE_VRAM);
	mMinimapQuad = new JQuad(mMinimapTex, 0, 0, (float)mMinimapTex->mWidth, (float)mMinimapTex->mHeight);
	mMinimapQuad->SetHotSpot((float)mMinimapTex->mWidth*0.5f, (float)mMinimapTex->mHeight*0.5f);

	mMinimapScale = MINIMAP_SCALE_MIN;

	mPaused = false;
	mQuit = false;

	JSoundSystem* sound = JSoundSystem::GetInstance();

	if (sound)
		mMusic = sound->LoadMusic("2-11 bLiNd - JENOVA Celestial (J-E-N-O-V-A).mp3");	// Load a background music.

	if (mMusic)
		JSoundSystem::GetInstance()->PlayMusic(mMusic, true);
	sound->SetMusicVolume(50);

	mSpawnMgr = new CSpawnManager;

	for(int i=0; i<SQUARETILE_INVENTORY_SIZE; i++)
		mInventory[i] = NULL;
	for(int i=0; i<9; i++)
		mInventory[i] = new CSquareTileHull((CSquareTileHull*)(mSpawnMgr->mListTiles[1]));
	for(int i=9; i<15; i++)
		mInventory[i] = new CSquareTileEngine((CSquareTileEngine*)(mSpawnMgr->mListTiles[2]));
	for(int i=15; i<21; i++)
		mInventory[i] = new CSquareTileGun((CSquareTileGun*)(mSpawnMgr->mListTiles[3]));

	Reset();

}


void GameLevel::Reset()
{
	SAFE_DELETE(mWorldObjects);

	mWorldObjects = new CWorldObjects(mSpawnMgr);
	
	mWorldObjects->Create();

	b2Mat22 camMat = b2Mat22(mWorldObjects->mCamRot);

	mTargetReticlePos = b2Vec2(0, 0);
	mTargetReticleWorldPos = mWorldObjects->mCamPos 
		+ b2Mul(camMat, mTargetReticlePos);

	JGE* engine = JGE::GetInstance();
	mDeltaTime = engine->GetDelta();		// Get time elapsed since last update.
	mDeltaTime = 0.0f;

	mTarget = NULL;
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameLevel::Destroy()
{
	SAFE_DELETE(mWorldObjects);

	SAFE_DELETE(mFont);

	SAFE_DELETE(mMinimapQuad);
	SAFE_DELETE(mMinimapTex);

	SAFE_DELETE(mTargetReticleQuad);
	SAFE_DELETE(mTargetReticleTex);

 	SAFE_DELETE(mMusic);

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

	UpdateControler();


	// on update tout
	if (mWorldObjects)
		mWorldObjects->Update(mDeltaTime);

}

void GameLevel::UpdateControler()
{
	JGE* engine = JGE::GetInstance();

	mWorldObjects->mHero->mAngularPower = 0.0f;

	// on crée une rotation proportionnelle à la position du réticule % au centre du vaisseau
// 	b2Vec2 reticleDir = mTargetReticlePos - 
// 		(mWorldObjects->mHero->GetCenterPosition()-mWorldObjects->mCamPos).Rotate(-mWorldObjects->mCamRot);
// 	float l = reticleDir.Normalize();
// 	float dr = b2Dot(b2Vec2(-1, 0), reticleDir);
// 	dr *= abs(dr);// on utilise une courbe parabole
// 	if(l < RETICLE_DEAD_ZONE)// on crée une zone morte
// 		dr = 0.0f;
// 	else if(b2Dot(b2Vec2(0, 1), reticleDir) < 0)
// 		dr = (dr > 0.0f)?1.0f:-1.0f;
// 	mWorldObjects->mHero->mAngularPower = dr;
// 	if(mWorldObjects->mHero->mAngularPower > 1.0f)
// 		mWorldObjects->mHero->mAngularPower = 1.0f;
// 	else if(mWorldObjects->mHero->mAngularPower < -1.0f)
// 		mWorldObjects->mHero->mAngularPower = -1.0f;
	b2Vec2 reticleDir = mTargetReticlePos;
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
		mMinimapScale *= 5.0f;
		if(mMinimapScale > MINIMAP_SCALE_MAX)
			mMinimapScale = MINIMAP_SCALE_MIN;
	}

	if (engine->GetButtonState(PSP_CTRL_UP))
	{

	}

	if (engine->GetButtonState(PSP_CTRL_DOWN))
	{
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
	
	b2Mat22 camMat = b2Mat22(mWorldObjects->mCamRot);

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
	b2Vec2 dv = b2Vec2(dvx, dvy);// dv max normalisé (181=sqrt(128²+128²))
	
	float cursorMaxSpeed;
	float offset;
	float ratio;
	b2Vec2 magneticPoint;
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
	b2Vec2 localMagneticPoint = b2MulT(camMat, magneticPoint-mWorldObjects->mCamPos);
	if(mTarget)
		localMagneticPoint += 100.0f*dv;
	b2Vec2 localDir = mTargetReticlePos - localMagneticPoint;
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


	mTargetReticleWorldPos = mWorldObjects->mCamPos 
		+ b2Mul(camMat, mTargetReticlePos);



	if (engine->GetButtonState(PSP_CTRL_RTRIGGER))
	{
		b2Vec2 shootPos = mTargetReticleWorldPos;
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

// 	JGE* engine = JGE::GetInstance();
// 	sprintf(txt, "heroRot=%f", mWorldObjects->mHero->GetRotation());
// 	mFont->SetColor(ARGB(255,0,255,255));
// 	mFont->RenderString(txt, 350, 0);
// 	sprintf(txt, "camRot=%f", mWorldObjects->mCamRot);
// 	mFont->SetColor(ARGB(255,0,255,255));
// 	mFont->RenderString(txt, 350, 10);
}


void GameLevel::DrawGui()
{
	static int centerX = SCREEN_SIZE_X-mMinimapTex->mWidth/2-1;
	static int centerY = mMinimapTex->mHeight/2+1;
	
	float minimapDistMax = (float)MINIMAP_RADIUS*mMinimapScale;
	float minimapRatio = 1.0f/mMinimapScale;

	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();	

	renderer->RenderQuad(mMinimapQuad, (float)centerX, (float)centerY);

// 	renderer->FillCircle(centerX, centerY, MINIMAP_RADIUS, ARGB(50,0,0,255));
// 	renderer->DrawCircle(centerX, centerY, MINIMAP_RADIUS/2, ARGB(128,0,0,255));
// 	renderer->DrawCircle(centerX, centerY, MINIMAP_RADIUS/4, ARGB(128,0,0,255));
// 	renderer->DrawCircle(centerX, centerY, MINIMAP_RADIUS*3/4, ARGB(128,0,0,255));
// 
// 	renderer->DrawLine(centerX-MINIMAP_RADIUS, centerY, centerX+MINIMAP_RADIUS, centerY, ARGB(128,0,0,255));
// 	renderer->DrawLine(centerX, centerY-MINIMAP_RADIUS, centerX, centerY+MINIMAP_RADIUS, ARGB(128,0,0,255));

	b2Vec2 dir = b2Vec2(0, MINIMAP_RADIUS).Rotate(-mWorldObjects->mCamRot);
	renderer->DrawLine((float)centerX, (float)centerY, (float)centerX+dir.x, (float)centerY-dir.y, ARGB(255,255,0,0));
	renderer->DrawLine((float)centerX, (float)centerY, (float)centerX-dir.x, (float)centerY+dir.y, ARGB(255,0,255,0));

	//renderer->DrawCircle(SCREEN_SIZE_X2, SCREEN_SIZE_Y2, RETICLE_DEAD_ZONE, ARGB(128,255,200,0));

	for(int i=0; i<mWorldObjects->mNbObjects; i++)
	{
		if(mWorldObjects->mObjects[i] == (CObject*)mWorldObjects->mHero)
			continue;

		b2Vec2 shipPos = mWorldObjects->mObjects[i]->GetOriginPosition();
		b2Vec2 shipDir = shipPos - mWorldObjects->mCamPos;
		if(shipDir.Length() <= minimapDistMax)
		{
			shipDir = (minimapRatio * shipDir).Rotate(-mWorldObjects->mCamRot);
			renderer->DrawCircle(centerX+shipDir.x, centerY-shipDir.y, 1, ARGB(255,255,0,0));
		}
// 		shipDir = shipPos - mWorldObjects->mCamPos;
// 		if(shipDir.Length() <= MINIMAP_SCALE_MAX)
// 		{
// 			shipDir.Rotate(-mWorldObjects->mCamRot);
// 			shipDir.Normalize();
// 			shipDir *= RETICLE_DEAD_ZONE;
// 			renderer->DrawCircle(SCREEN_SIZE_X2+shipDir.x, SCREEN_SIZE_Y2-shipDir.y, 1, ARGB(255,255,0,0));
// 		}
// 
// 		b2Vec2 shipVel = mWorldObjects->mObjects[i]->GetLinearVelocity();
// 		b2Vec2 shootPos = mWorldObjects->mHero->GetShootPoint(shipPos, shipVel);
// 		b2Vec2 localShootPos = ((shootPos-mWorldObjects->mCamPos).Rotate(-mWorldObjects->mCamRot));
// 		int x = (int)(SCREEN_SIZE_X2+localShootPos.x);
// 		int y = (int)(SCREEN_SIZE_Y2-localShootPos.y);
// 		renderer->DrawCircle((float)x, (float)y, 2, ARGB(255,255,0,0));
		//mWorldObjects->mObjects[i]->RenderWire(mWorldObjects->mCamPos, mWorldObjects->mCamRot, shootPos, ARGB(128,255,255,0));
	}

	char txt[50] = "";
	sprintf(txt, "1/%d", (int)(mMinimapScale+0.5f));
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
}


//-------------------------------------------------------------------------------------
// This function is called when the system wants to pause the game. You can set a flag
// here to stop the update loop and audio playback.
//
//-------------------------------------------------------------------------------------
void GameLevel::Pause()
{
	mPaused = true;

	JSoundSystem* sound = JSoundSystem::GetInstance();
	//sound->PauseMusic(mMusic);
}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameLevel::Resume()
{
	mPaused = false;

	JSoundSystem* sound = JSoundSystem::GetInstance();
	//sound->ResumeMusic(mMusic);
}
