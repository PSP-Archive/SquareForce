//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------
//
// Adapted from "tutorial05" of HGE. To use HGE's hepler classes, you need to include
// libhgetools in the PSP makefile.
//
//-------------------------------------------------------------------------------------

#include <stdio.h>

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>

#include <hge/hgedistort.h>
#include <hge/hgefont.h>

#include "GameIntroLogo.h"


//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameIntroLogo::GameIntroLogo() : GameIntro()
{
	mTex = NULL;
	mDistortionMesh = NULL;
	mFont = NULL;

	
	mIsFadingIn=true;
	mIsFadingOut=false;
	mFaderTimer=2.0f;
	mQuitTimer=3.0f;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameIntroLogo::~GameIntroLogo()
{

}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameIntroLogo::Create()
{
	GameIntro::Create();

	JRenderer* renderer = JRenderer::GetInstance();		
	mTex=renderer->LoadTexture("DreamRaiserLogo.png");

	mRows=20;
	mCols=12;
	mCellw=(float)(mTex->mWidth/(mCols-1));
	mCellh=(float)(mTex->mHeight/(mRows-1));
	mMeshx=SCREEN_WIDTH_F/2;
	mMeshy=SCREEN_HEIGHT_F/2;


	// Create a distortion mesh
	mDistortionMesh=new hgeDistortionMesh(mCols, mRows);
	mDistortionMesh->SetTexture(mTex);
	mDistortionMesh->SetTextureRect(0,0,(float)mTex->mWidth,(float)mTex->mHeight);
	mDistortionMesh->Clear(ARGB(0xFF,0xFF,0xFF,0xFF));
	mDistortionMesh->Reset();
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameIntroLogo::Destroy()
{
	GameIntro::Destroy();

	SAFE_DELETE(mTex);

	SAFE_DELETE(mDistortionMesh);

}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameIntroLogo::Update()
{
	GameIntro::Update();

	JGE* engine = JGE::GetInstance();

	float dt = engine->GetDelta();		// Get time elapsed since last update.
	if(mQuitTimer>0.0f)// update timer
		mQuitTimer -= dt;
	if((mIsFadingIn || mIsFadingOut) && mFaderTimer>0.0f)
		mFaderTimer -= dt;
	if(mIsFadingIn && mFaderTimer<=0.0f)
	{
		mFaderTimer = 2.0f;
		mIsFadingIn = false;
	}

	if(mQuitTimer>0.0f || mIsFadingIn)// overwrite mQuit while mQuitTimer is not 0
		mQuit = false;
	if(mQuit)// we want to quit and we are allowed : start fading out
	{
		mIsFadingOut = true;
		mQuit = false;
	}
	if(mFaderTimer <= 0.0f)// if the fade out is finished, we force quit
		mQuit = true;

	static float t=0.0f;
	
	int i, j;
	
	t+=dt;

	// Calculate new displacements and coloring for one of the three effects
	for(i=0;i<mRows;i++)
	{
		for(j=0;j<mCols;j++)
		{
			float r = t*6+(float)(i*j);
			float rx = cosf(r)*3.0f+5.0f*cosf(t*2);
			float ry = sinf(r)*2.0f+2.0f*sinf(t*3);
			if((i>0 && i<mRows-1) && (j>0 && j<mCols-1))
				mDistortionMesh->SetDisplacement(j,i,rx,ry,HGEDISP_NODE);

			mDistortionMesh->SetColor(j,i,
				ARGB(255, 
					(100-32-(int)(0.7f*(sinf(r)+0.3f*sinf(t*2))*32)), 
					(205-32-(int)(sinf(r)*32)), 
					(255-24-(int)(sinf(r)*24)))	);
		}
	}
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameIntroLogo::Render()
{
	GameIntro::Render();

	float zoom = mIsFadingIn?(2.0f-mFaderTimer)*0.25f:0.5f;
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();
	
	// clear screen to black
	renderer->ClearScreen(ARGB(0,0,0,0));

	float w = (float)mTex->mWidth*zoom;
	float h = (float)mTex->mHeight*zoom;

	// render the mesh
 	mDistortionMesh->Render(mMeshx, mMeshy, zoom, zoom);

	// fader
	int a = 0;
	float val = (mFaderTimer>=0.0f)?mFaderTimer:0.0f;
	if(mIsFadingIn)
		a = (int)(255.0f*val/2.0f);
	else if(mIsFadingOut)
		a = 255-(int)(255.0f*val/2.0f);
	
	renderer->FillRect(mMeshx-w/2, mMeshy-h/2, w, h, ARGB(a,255,255,200));

}


//-------------------------------------------------------------------------------------
// This function is called when the system wants to pause the game. You can set a flag
// here to stop the update loop and audio playback.
//
//-------------------------------------------------------------------------------------
void GameIntroLogo::Pause()
{
	GameIntro::Pause();
}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameIntroLogo::Resume()
{
	GameIntro::Resume();
}
