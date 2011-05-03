#include <stdio.h>

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JGBKFont.h>
#include <JSoundSystem.h>

#include <JTTFont.h>
#include <Encoding.h>

#include "GameMenuCredits.h"
#include "utils.h"


//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameMenuCredits::GameMenuCredits() : IGame()
{
	mTexBG = NULL;
	mBGMesh = NULL;

	mFont = NULL;
	mFont2 = NULL;
	mFont3 = NULL;
	
	mIsFadingIn=true;
	mIsFadingOut=false;
	mFaderTimer=2.0f;
	
	mCpt = 0.0f;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameMenuCredits::~GameMenuCredits()
{

}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameMenuCredits::Create()
{
	JRenderer* renderer = JRenderer::GetInstance();		
	mTexBG=renderer->LoadTexture("bg2.png");
	
	mBGMesh = new JQuad(mTexBG, 0, 0, (float)mTexBG->mWidth, (float)mTexBG->mHeight);

	mFont = new JTTFont();
	mFont->Load("font.ttf", 22);
	mFont2 = new JTTFont();
	mFont2->Load("font.ttf", 16);
	mFont3 = new JTTFont();
	mFont3->Load("font.ttf", 12);
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameMenuCredits::Destroy()
{
	SAFE_DELETE(mTexBG);

	SAFE_DELETE(mBGMesh);

	SAFE_DELETE(mFont);
	SAFE_DELETE(mFont2);
	SAFE_DELETE(mFont3);
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameMenuCredits::Update()
{
	JGE* engine = JGE::GetInstance();

	if (engine->GetButtonClick(PSP_CTRL_CROSS) || engine->GetButtonClick(PSP_CTRL_START))	
	{
		mQuit = true;
	}
	

	float dt = engine->GetDelta();		// Get time elapsed since last update.
	if((mIsFadingIn || mIsFadingOut) && mFaderTimer>0.0f)
		mFaderTimer -= dt;
	if(mIsFadingIn && mFaderTimer<=0.0f)
	{
		mFaderTimer = 2.0f;
		mIsFadingIn = false;
	}

	if(mIsFadingIn)// overwrite mQuit while fading in
		mQuit = false;
	if(mQuit)// we want to quit and we are allowed : start fading out
	{
		mIsFadingOut = true;
		mQuit = false;
	}
	if(mFaderTimer <= 0.0f)// if the fade out is finished, we force quit
	{
		mQuit = true;
		mFaderTimer = 0.0f;
	}

	
	mCpt+=dt;
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameMenuCredits::Render()
{
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();

	// clear screen to black
	//renderer->ClearScreen(ARGB(0,0,0,0));

	renderer->RenderQuad(mBGMesh, 0, 0);

	char name[50] = "Akabane87";

	static float offset = 30.0f;
	{
		char lStr[50] = "Programmers";
		float X = offset, Y = 10;
		mFont->SetColor(ARGB(64, 255, 255, 255));
		mFont->RenderString(lStr, X+3.0f, Y+3.0f, JGETEXT_LEFT);
		mFont->RenderString(lStr, X+4.0f, Y+4.0f, JGETEXT_LEFT);

		mFont->SetColor(ARGB(128, 0, 255, 255));
		mFont->RenderString(lStr, X+1.0f, Y+1.0f, JGETEXT_LEFT);
		mFont->RenderString(lStr, X+2.0f, Y+2.0f, JGETEXT_LEFT);

		mFont->SetColor(ARGB(255, 0, 255, 50));
		mFont->RenderString(lStr, X, Y, JGETEXT_LEFT);

		Y += 25;
		mFont2->SetColor(ARGB(255, 0, 255, 0));
		mFont2->RenderString(name, X, Y, JGETEXT_LEFT);
	}

	{
		char lStr[50] = "Designers";
		float X = 480 - offset, Y = 60;
		mFont->SetColor(ARGB(64, 255, 255, 255));
		mFont->RenderString(lStr, X+3.0f, Y+3.0f, JGETEXT_RIGHT);
		mFont->RenderString(lStr, X+4.0f, Y+4.0f, JGETEXT_RIGHT);

		mFont->SetColor(ARGB(128, 0, 255, 255));
		mFont->RenderString(lStr, X+1.0f, Y+1.0f, JGETEXT_RIGHT);
		mFont->RenderString(lStr, X+2.0f, Y+2.0f, JGETEXT_RIGHT);

		mFont->SetColor(ARGB(255, 0, 255, 50));
		mFont->RenderString(lStr, X, Y, JGETEXT_RIGHT);

		Y += 25;
		mFont2->SetColor(ARGB(255, 0, 255, 0));
		mFont2->RenderString(name, X, Y, JGETEXT_RIGHT);
	}

	{
		char lStr[50] = "Graphists";
		float X = offset*0.5f, Y = 110;
		mFont->SetColor(ARGB(64, 255, 255, 255));
		mFont->RenderString(lStr, X+3.0f, Y+3.0f, JGETEXT_LEFT);
		mFont->RenderString(lStr, X+4.0f, Y+4.0f, JGETEXT_LEFT);

		mFont->SetColor(ARGB(128, 0, 255, 255));
		mFont->RenderString(lStr, X+1.0f, Y+1.0f, JGETEXT_LEFT);
		mFont->RenderString(lStr, X+2.0f, Y+2.0f, JGETEXT_LEFT);

		mFont->SetColor(ARGB(255, 0, 255, 50));
		mFont->RenderString(lStr, X, Y, JGETEXT_LEFT);

		Y += 25;
		mFont2->SetColor(ARGB(255, 0, 255, 0));
		mFont2->RenderString(name, X, Y, JGETEXT_LEFT);
	}

	{
		char lStr[50] = "Music";
		float X = 480 - offset, Y = 160;
		mFont->SetColor(ARGB(64, 255, 255, 255));
		mFont->RenderString(lStr, X+3.0f, Y+3.0f, JGETEXT_RIGHT);
		mFont->RenderString(lStr, X+4.0f, Y+4.0f, JGETEXT_RIGHT);

		mFont->SetColor(ARGB(128, 0, 255, 255));
		mFont->RenderString(lStr, X+1.0f, Y+1.0f, JGETEXT_RIGHT);
		mFont->RenderString(lStr, X+2.0f, Y+2.0f, JGETEXT_RIGHT);

		mFont->SetColor(ARGB(255, 0, 255, 50));
		mFont->RenderString(lStr, X, Y, JGETEXT_RIGHT);

		sprintf(name, "http://ff7.ocremix.org/");
		Y += 25;
		mFont2->SetColor(ARGB(255, 0, 255, 0));
		mFont2->RenderString(name, X, Y, JGETEXT_RIGHT);
	}

	{
		char lStr[50] = "Testers";
		float X = offset, Y = 210;
		mFont->SetColor(ARGB(64, 255, 255, 255));
		mFont->RenderString(lStr, X+3.0f, Y+3.0f, JGETEXT_LEFT);
		mFont->RenderString(lStr, X+4.0f, Y+4.0f, JGETEXT_LEFT);

		mFont->SetColor(ARGB(128, 0, 255, 255));
		mFont->RenderString(lStr, X+1.0f, Y+1.0f, JGETEXT_LEFT);
		mFont->RenderString(lStr, X+2.0f, Y+2.0f, JGETEXT_LEFT);

		mFont->SetColor(ARGB(255, 0, 255, 50));
		mFont->RenderString(lStr, X, Y, JGETEXT_LEFT);

		sprintf(name, "Léo");
		Y += 25;
		mFont2->SetColor(ARGB(255, 0, 255, 0));
		mFont2->RenderString(name, X, Y, JGETEXT_LEFT);
	}

	{
		sprintf(name, "A DreamRaiser's production");
		float X = 240, Y = -5;
		mFont2->SetColor(ARGB(255, 0, 255, 100));
		mFont2->RenderString(name, X, Y, JGETEXT_CENTER);
		sprintf(name, "http://dreamraiser.forumactif.com");
		X = 480; Y = 260;
		mFont3->SetColor(ARGB(255, 0, 255, 100));
		mFont3->RenderString(name, X, Y, JGETEXT_RIGHT);
	}
	
	
	// fader
	int a = 0;
	float val = (mFaderTimer>=0.0f)?mFaderTimer:0.0f;
	if(mIsFadingIn)
		a = (int)(255.0f*val/2.0f);
	else if(mIsFadingOut)
		a = 255-(int)(255.0f*val/2.0f);
	renderer->FillRect(0, 0, SCREEN_WIDTH_F, SCREEN_HEIGHT_F, ARGB(a,0,0,0));
}


//-------------------------------------------------------------------------------------
// This function is called when the system wants to pause the game. You can set a flag
// here to stop the update loop and audio playback.
//
//-------------------------------------------------------------------------------------
void GameMenuCredits::Pause()
{

}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameMenuCredits::Resume()
{
	
}
