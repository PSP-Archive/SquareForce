#include <stdio.h>

#include <JGE.h>
#include <JRenderer.h>

#include "GameIntroLogoGenesis.h"
#include "utils.h"


//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameLogoGenesis::GameLogoGenesis() : IGame()
{
	mTexBG = NULL;
	mBGMesh = NULL;
	
	mIsFadingIn=true;
	mIsFadingOut=false;
	mFaderTimer=2.0f;
	
	mCpt = 0.0f;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameLogoGenesis::~GameLogoGenesis()
{

}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameLogoGenesis::Create()
{
	JRenderer* renderer = JRenderer::GetInstance();		
	mTexBG=renderer->LoadTexture("GenesisLogo.png");
	
	mBGMesh = new JQuad(mTexBG, 0, 0, (float)mTexBG->mWidth, (float)mTexBG->mHeight);
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameLogoGenesis::Destroy()
{
	SAFE_DELETE(mTexBG);

	SAFE_DELETE(mBGMesh);
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameLogoGenesis::Update()
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
void GameLogoGenesis::Render()
{
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();

	// clear screen to black
	//renderer->ClearScreen(ARGB(0,0,0,0));

	renderer->RenderQuad(mBGMesh, 0, 0);
	
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
void GameLogoGenesis::Pause()
{

}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameLogoGenesis::Resume()
{
	
}
