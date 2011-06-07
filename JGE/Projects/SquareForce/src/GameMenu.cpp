#include <stdio.h>

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JGBKFont.h>
#include <JSoundSystem.h>

#include <JTTFont.h>
#include <Encoding.h>

#include "GameMenu.h"
#include "utils.h"

#include "CLocalization.h"

//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameMenu::GameMenu() : IGame()
{
	mTexBG = NULL;
	mBGMesh = NULL;

	mFont = NULL;
	mFont2 = NULL;

	mMusic = NULL;

	mSquareTilesTex = NULL;
	mLightningTex = NULL;
	mLightningQuads = NULL;
	mSquareTileQuad = NULL;

	
	mIsFadingIn=true;
	mIsFadingOut=false;
	mFaderTimer=2.0f;
	
	mLine = 0;

	mCpt = 0.0f;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameMenu::~GameMenu()
{

}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameMenu::Create()
{
	JRenderer* renderer = JRenderer::GetInstance();		
	mTexBG=renderer->LoadTexture("BG.png");
	
	mBGMesh = new JQuad(mTexBG, 0, 0, (float)mTexBG->mWidth, (float)mTexBG->mHeight);

	mFont = new JTTFont();
	mFont->Load("font.ttf", 26);
	mFont2 = new JTTFont();
	mFont2->Load("font.ttf", 36);

	mSquareTilesTex = JRenderer::GetInstance()->LoadTexture("SquareTiles.png", TEX_TYPE_USE_VRAM);
	mLightningTex = JRenderer::GetInstance()->LoadTexture("nrj10anims18x16.png", TEX_TYPE_USE_VRAM);

	mSquareTileQuad = new JQuad(mSquareTilesTex, 0.5f, 0.5f, 24.0f-1.0f, 24.0f-1.0f);
	mSquareTileQuad->SetHotSpot((float)((24-1)>>1),(float)((24-1)>>1));
	mSquareTileQuad->SetColor(ARGB(255, 255, 255, 255));
	
	mLightningQuads = new JQuad*[NB_LIGHTNING_FRAMES];
	for(int i=0; i<NB_LIGHTNING_FRAMES; i++)
	{
		mLightningQuads[i] = new JQuad(mLightningTex, 18*i+0.5f, 0.5f, 18-1, 16-1);
		mLightningQuads[i]->SetHotSpot((18-1)>>1, (16-1)>>1);
		mLightningQuads[i]->SetColor(ARGB(255, 100, 255, 200));
	}

	mTiles.push_back(Vector2D(0*8,0*8));
	mTiles.push_back(Vector2D(1*8,0*8));
	mTiles.push_back(Vector2D(2*8,0*8));
	mTiles.push_back(Vector2D(24*8,0*8));
	mTiles.push_back(Vector2D(25*8,0*8));
	mTiles.push_back(Vector2D(26*8,0*8));
	mTiles.push_back(Vector2D(0*8,1*8));
	mTiles.push_back(Vector2D(4*8,1*8));
	mTiles.push_back(Vector2D(5*8,1*8));
	mTiles.push_back(Vector2D(6*8,1*8));
	mTiles.push_back(Vector2D(8*8,1*8));
	mTiles.push_back(Vector2D(10*8,1*8));
	mTiles.push_back(Vector2D(12*8,1*8));
	mTiles.push_back(Vector2D(13*8,1*8));
	mTiles.push_back(Vector2D(14*8,1*8));
	mTiles.push_back(Vector2D(16*8,1*8));
	mTiles.push_back(Vector2D(17*8,1*8));
	mTiles.push_back(Vector2D(18*8,1*8));
	mTiles.push_back(Vector2D(20*8,1*8));
	mTiles.push_back(Vector2D(21*8,1*8));
	mTiles.push_back(Vector2D(22*8,1*8));
	mTiles.push_back(Vector2D(24*8,1*8));
	mTiles.push_back(Vector2D(28*8,1*8));
	mTiles.push_back(Vector2D(29*8,1*8));
	mTiles.push_back(Vector2D(30*8,1*8));
	mTiles.push_back(Vector2D(32*8,1*8));
	mTiles.push_back(Vector2D(33*8,1*8));
	mTiles.push_back(Vector2D(34*8,1*8));
	mTiles.push_back(Vector2D(36*8,1*8));
	mTiles.push_back(Vector2D(37*8,1*8));
	mTiles.push_back(Vector2D(38*8,1*8));
	mTiles.push_back(Vector2D(40*8,1*8));
	mTiles.push_back(Vector2D(41*8,1*8));
	mTiles.push_back(Vector2D(42*8,1*8));
	mTiles.push_back(Vector2D(0*8,2*8));
	mTiles.push_back(Vector2D(1*8,2*8));
	mTiles.push_back(Vector2D(2*8,2*8));
	mTiles.push_back(Vector2D(4*8,2*8));
	mTiles.push_back(Vector2D(6*8,2*8));
	mTiles.push_back(Vector2D(8*8,2*8));
	mTiles.push_back(Vector2D(10*8,2*8));
	mTiles.push_back(Vector2D(12*8,2*8));
	mTiles.push_back(Vector2D(14*8,2*8));
	mTiles.push_back(Vector2D(16*8,2*8));
	mTiles.push_back(Vector2D(18*8,2*8));
	mTiles.push_back(Vector2D(20*8,2*8));
	mTiles.push_back(Vector2D(21*8,2*8));
	mTiles.push_back(Vector2D(22*8,2*8));
	mTiles.push_back(Vector2D(24*8,2*8));
	mTiles.push_back(Vector2D(25*8,2*8));
	mTiles.push_back(Vector2D(28*8,2*8));
	mTiles.push_back(Vector2D(30*8,2*8));
	mTiles.push_back(Vector2D(32*8,2*8));
	mTiles.push_back(Vector2D(34*8,2*8));
	mTiles.push_back(Vector2D(36*8,2*8));
	mTiles.push_back(Vector2D(40*8,2*8));
	mTiles.push_back(Vector2D(41*8,2*8));
	mTiles.push_back(Vector2D(42*8,2*8));
	mTiles.push_back(Vector2D(2*8,3*8));
	mTiles.push_back(Vector2D(4*8,3*8));
	mTiles.push_back(Vector2D(6*8,3*8));
	mTiles.push_back(Vector2D(8*8,3*8));
	mTiles.push_back(Vector2D(10*8,3*8));
	mTiles.push_back(Vector2D(12*8,3*8));
	mTiles.push_back(Vector2D(13*8,3*8));
	mTiles.push_back(Vector2D(14*8,3*8));
	mTiles.push_back(Vector2D(16*8,3*8));
	mTiles.push_back(Vector2D(17*8,3*8));
	mTiles.push_back(Vector2D(20*8,3*8));
	mTiles.push_back(Vector2D(24*8,3*8));
	mTiles.push_back(Vector2D(28*8,3*8));
	mTiles.push_back(Vector2D(30*8,3*8));
	mTiles.push_back(Vector2D(32*8,3*8));
	mTiles.push_back(Vector2D(33*8,3*8));
	mTiles.push_back(Vector2D(36*8,3*8));
	mTiles.push_back(Vector2D(40*8,3*8));
	mTiles.push_back(Vector2D(0*8,4*8));
	mTiles.push_back(Vector2D(1*8,4*8));
	mTiles.push_back(Vector2D(2*8,4*8));
	mTiles.push_back(Vector2D(4*8,4*8));
	mTiles.push_back(Vector2D(5*8,4*8));
	mTiles.push_back(Vector2D(6*8,4*8));
	mTiles.push_back(Vector2D(8*8,4*8));
	mTiles.push_back(Vector2D(9*8,4*8));
	mTiles.push_back(Vector2D(10*8,4*8));
	mTiles.push_back(Vector2D(12*8,4*8));
	mTiles.push_back(Vector2D(14*8,4*8));
	mTiles.push_back(Vector2D(16*8,4*8));
	mTiles.push_back(Vector2D(18*8,4*8));
	mTiles.push_back(Vector2D(20*8,4*8));
	mTiles.push_back(Vector2D(21*8,4*8));
	mTiles.push_back(Vector2D(22*8,4*8));
	mTiles.push_back(Vector2D(24*8,4*8));
	mTiles.push_back(Vector2D(28*8,4*8));
	mTiles.push_back(Vector2D(29*8,4*8));
	mTiles.push_back(Vector2D(30*8,4*8));
	mTiles.push_back(Vector2D(32*8,4*8));
	mTiles.push_back(Vector2D(34*8,4*8));
	mTiles.push_back(Vector2D(36*8,4*8));
	mTiles.push_back(Vector2D(37*8,4*8));
	mTiles.push_back(Vector2D(38*8,4*8));
	mTiles.push_back(Vector2D(40*8,4*8));
	mTiles.push_back(Vector2D(41*8,4*8));
	mTiles.push_back(Vector2D(42*8,4*8));
	mTiles.push_back(Vector2D(6*8,5*8));

#ifndef NO_SOUND
	JSoundSystem* sound = JSoundSystem::GetInstance();

 	if (sound)
 		mMusic = sound->LoadMusic("2-10 Tweek - Sephiroth's Wake (Trail of Blood).mp3");	// Load a background music.
 
 	if (mMusic)
 		JSoundSystem::GetInstance()->PlayMusic(mMusic, true);
#endif
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameMenu::Destroy()
{
	SAFE_DELETE(mTexBG);

	SAFE_DELETE(mBGMesh);

	if(mFont)
		mFont->Unload();
	SAFE_DELETE(mFont);
	if(mFont2)
		mFont2->Unload();
	SAFE_DELETE(mFont2);

	SAFE_DELETE(mSquareTilesTex);
	SAFE_DELETE(mLightningTex);
	for(int i=0; i<NB_LIGHTNING_FRAMES; i++)
	{
		SAFE_DELETE(mLightningQuads[i]);
	}
	SAFE_DELETE_ARRAY(mLightningQuads);
	SAFE_DELETE(mSquareTileQuad);

	mTiles.clear();

#ifndef NO_SOUND
	JSoundSystem* sound = JSoundSystem::GetInstance();
	sound->StopMusic(mMusic);
 	SAFE_DELETE(mMusic);
#endif
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameMenu::Update()
{
	JGE* engine = JGE::GetInstance();

	if (engine->GetButtonClick(PSP_CTRL_CROSS) || engine->GetButtonClick(PSP_CTRL_START))	
	{
		if(mLine!=1)
			mQuit = true;
	}
	if(engine->GetButtonClick(PSP_CTRL_UP))
	{
		mLine--;
		if(mLine<0)
			mLine = 2;
	}
	if(engine->GetButtonClick(PSP_CTRL_DOWN))
	{
		mLine++;
		if(mLine>2)
			mLine = 0;
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

	JSoundSystem* sound = JSoundSystem::GetInstance();
	if(mIsFadingOut)
		sound->SetMusicVolume((int)(100*mFaderTimer/2.0f));
	else if(mIsFadingIn)
		sound->SetMusicVolume((int)(100*(2.0f-mFaderTimer)/2.0f));
	else
		sound->SetMusicVolume(100);
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameMenu::Render()
{
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();

	// clear screen to black
	//renderer->ClearScreen(ARGB(0,0,0,0));

	renderer->RenderQuad(mBGMesh, 0, 0);

	static float X = 240.0f;
	static float Y = 180.0f;
	static string str[]=
	{
		CLocalization::GetInstance()->GetToken(CLocalization::TITLE_TOKEN_1), 
		CLocalization::GetInstance()->GetToken(CLocalization::TITLE_TOKEN_2), 
		CLocalization::GetInstance()->GetToken(CLocalization::TITLE_TOKEN_3)
	};

	for(int i=0; i<3; i++)
	{
		const char *lStr = str[i].c_str();
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


	static string sSubTitle = CLocalization::GetInstance()->GetToken(CLocalization::TITLE_TOKEN_SUBTITLE);
	const char* subTitle = sSubTitle.c_str();
	mFont2->SetColor(ARGB(64, 255, 255, 255));
	mFont2->RenderString(subTitle, 240+3.0f, 50+3.0f, JGETEXT_CENTER);
	mFont2->RenderString(subTitle, 240+4.0f, 50+4.0f, JGETEXT_CENTER);
	mFont2->SetColor(ARGB(128, 0, 255, 50));
	mFont2->RenderString(subTitle, 240+1.0f, 50+1.0f, JGETEXT_CENTER);
	mFont2->RenderString(subTitle, 240+2.0f, 50+2.0f, JGETEXT_CENTER);
	mFont2->SetColor(ARGB(200, 0, 255, 100));
	mFont2->RenderString(subTitle, 240, 50, JGETEXT_CENTER);


	// title
	float r = 100, g = 255, b = 200;
	static float x = 70.0f;
	static float y = 20.0f;
	list<Vector2D>::const_iterator it = mTiles.begin();
	const list<Vector2D>::const_iterator itEnd = mTiles.end();
	while(it!=itEnd)
	{
		mSquareTileQuad->SetColor(ARGB(128, 255, 255, 255));
		renderer->RenderQuad(mSquareTileQuad, x+it->x+1.0f, y+it->y+1.0f);
		mSquareTileQuad->SetColor(ARGB(255, 255, 255, 255));
		renderer->RenderQuad(mSquareTileQuad, x+it->x, y+it->y);

		int num = (int)(b2Random(0, 1)*20*NB_LIGHTNING_FRAMES);
		if(num<NB_LIGHTNING_FRAMES)
		{
			b += mCpt * 10.0f;
			if(b > 255.0f) b = 0.0f;
			mLightningQuads[num]->SetColor(ARGB(255, ((int)r), ((int)g), ((int)b)));
			renderer->RenderQuad(mLightningQuads[num], x+it->x, y+it->y);
		}
		++it;
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
void GameMenu::Pause()
{

}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameMenu::Resume()
{
	
}
