#include <stdio.h>

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JGBKFont.h>

#include <JTTFont.h>
#include <Encoding.h>

#include "GameMenuLang.h"
#include "utils.h"

#include "CLocalization.h"

//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameMenuLang::GameMenuLang() : IGame()
{
	mFont = NULL;
	
	mIsFadingIn=true;
	mIsFadingOut=false;
	mFaderTimer=0.2f;

	mLine = 0;

	mCpt = 0.0f;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameMenuLang::~GameMenuLang()
{

}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameMenuLang::Create()
{
	JRenderer* renderer = JRenderer::GetInstance();		

	mFont = new JTTFont();
	mFont->Load("font.ttf", 26);

	CLocalization* instance = CLocalization::GetInstance();
	instance->GetLangList();
	string lang = "";
	int i=0;
	while((lang = instance->GetLanguages(i)) != "")
	{
		mLanguages.push_back(lang);
		++i;
	}
	mNbLanguages = mLanguages.size();

}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameMenuLang::Destroy()
{
	if(mFont)
		mFont->Unload();
	SAFE_DELETE(mFont);
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameMenuLang::Update()
{
	JGE* engine = JGE::GetInstance();

	if (engine->GetButtonClick(PSP_CTRL_CROSS) || engine->GetButtonClick(PSP_CTRL_START))	
	{
		mQuit = true;
	}
	if(engine->GetButtonClick(PSP_CTRL_UP))
	{
		mLine--;
		if(mLine<0)
			mLine = mNbLanguages-1;
	}
	if(engine->GetButtonClick(PSP_CTRL_DOWN))
	{
		mLine++;
		if(mLine>=mNbLanguages)
			mLine = 0;
	}

	float dt = engine->GetDelta();		// Get time elapsed since last update.
	if((mIsFadingIn || mIsFadingOut) && mFaderTimer>0.0f)
		mFaderTimer -= dt;
	if(mIsFadingIn && mFaderTimer<=0.0f)
	{
		mFaderTimer = 1.0f;
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
		CLocalization* instance = CLocalization::GetInstance();
		instance->SetLanguage(mLine);
		instance->LoadStrings();
		instance->LoadSquareTilesStrings();

		mQuit = true;
		mFaderTimer = 0.0f;
	}

	
	mCpt+=dt;
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameMenuLang::Render()
{
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();

	// clear screen to black
	renderer->ClearScreen(ARGB(0,0,0,0));

	static float X = 240.0f;
	static float Y = 136.0f - (mNbLanguages-1) * 10.0f;
	
	for(int i=0; i<mNbLanguages; i++)
	{
		const char *lStr = mLanguages[i].c_str();
		mFont->SetColor(ARGB(64, 255, 255, 255));
		if(mLine!=i)
			mFont->RenderString(lStr, X+1.0f+2.0f, Y+2.0f+i*20+1.0f, JGETEXT_CENTER);
		else
		{
			mFont->RenderString(lStr, X+3.0f, Y+3.0f+i*20, JGETEXT_CENTER);
			mFont->RenderString(lStr, X+4.0f, Y+4.0f+i*20, JGETEXT_CENTER);
		}

		mFont->SetColor(ARGB(128, 0, 255, 255));
		if(mLine!=i)
			mFont->RenderString(lStr, X+1.0f+1.0f, Y+1.0f+i*20+1.0f, JGETEXT_CENTER);
		else
		{
			mFont->RenderString(lStr, X+1.0f, Y+1.0f+i*20, JGETEXT_CENTER);
			mFont->RenderString(lStr, X+2.0f, Y+2.0f+i*20, JGETEXT_CENTER);
		}

		if(mLine!=i)
		{
			mFont->SetColor(ARGB(255, 0, 255, 0));
			mFont->RenderString(lStr, X+1.0f, Y+i*20+1.0f, JGETEXT_CENTER);
		}
		else
		{
			mFont->SetColor(ARGB(255, 0, 255, 50));
			mFont->RenderString(lStr, X, Y+i*20, JGETEXT_CENTER);
		}
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
void GameMenuLang::Pause()
{

}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameMenuLang::Resume()
{
	
}
