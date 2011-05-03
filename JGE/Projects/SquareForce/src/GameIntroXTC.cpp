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

#include "GameIntroXTC.h"

//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameIntroXTC::GameIntroXTC() : GameIntro()
{
	mTex = NULL;
	mDistortionMesh = NULL;
	mTexMascotte = NULL;
	mMascotteMesh = NULL;
 	
	mIsFadingIn=true;
	mIsFadingOut=false;
	mFaderTimer=2.0f;
	mQuitTimer=3.0f;

	mCpt = 0.0f;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameIntroXTC::~GameIntroXTC()
{

}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameIntroXTC::Create()
{
	GameIntro::Create();

	JRenderer* renderer = JRenderer::GetInstance();		
	mTex=renderer->LoadTexture("XTCLogo.png");
	mTexMascotte=renderer->LoadTexture("XTCMascotte.png");

	mMascotteMesh = new JQuad(mTexMascotte, 0, 0, (float)mTexMascotte->mWidth, (float)mTexMascotte->mHeight);


	mRows=20;
	mCols=12;
	mCellw=(float)(mTex->mWidth/(mCols-1));
	mCellh=(float)(mTex->mHeight/(mRows-1));
	mMeshx=150;
	mMeshy=100;


	// Create a distortion mesh
	mDistortionMesh=new hgeDistortionMesh(mCols, mRows);
	mDistortionMesh->SetTexture(mTex);
	mDistortionMesh->SetTextureRect(0,0,(float)mTex->mWidth,(float)mTex->mHeight);
	mDistortionMesh->Clear(ARGB(0xFF,0xFF,0xFF,0xFF));

}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameIntroXTC::Destroy()
{
	GameIntro::Destroy();

	SAFE_DELETE(mTex);

	SAFE_DELETE(mDistortionMesh);

	SAFE_DELETE(mTexMascotte);

	SAFE_DELETE(mMascotteMesh);

}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameIntroXTC::Update()
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
		for(j=1;j<mCols;j++)
		{
			float r = t*7;
			float dx = cosf(r+(0.2f*i+j));
			float dy = sinf(r+(0.2f*i+j));
			mDistortionMesh->SetDisplacement(j,i,dx*10,dy*2,HGEDISP_NODE);

			mDistortionMesh->SetColor(j,i,
				ARGB(255, 
					(255-32-(int)(dy*32)), 
					(255-32-(int)(dy*32)), 
					(255-32-(int)(dy*32)))	);
		}
	}

	mCpt+=dt;
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameIntroXTC::Render()
{
	GameIntro::Render();

	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();

	static int pp=0;
	if(mCpt>0.5f)
	{
		pp++;
		pp%=2;
		mCpt = 0.0f;
	}

	//renderer->Enable3D();
	//renderer->Translate(-240 , -136, -300);//-240 , -136, -177

	// clear screen to black
	renderer->ClearScreen(ARGB(0,0,0,0));

	renderer->FillRect(mMeshx-4.0f, mMeshy-2.0f+pp, 5.0f, 150.0f, ARGB(255,80,50,0));
	renderer->FillRect(mMeshx-1.0f, mMeshy-2.0f+pp, 2.0f, 150.0f, ARGB(255,100,60,0));

	renderer->EnableTextureFilter(true);
	// render the mesh
 	mDistortionMesh->Render(mMeshx, mMeshy+pp, 0.5f, 0.5f);
	renderer->EnableTextureFilter(false);
	
	//renderer->Enable2D();

	renderer->RenderQuad(mMascotteMesh, mMeshx, mMeshy+137.0f+2.0f*pp, 0.0f, 1.0f, 1.0f-pp*0.099f);
	
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
void GameIntroXTC::Pause()
{
	GameIntro::Pause();
}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameIntroXTC::Resume()
{
	GameIntro::Resume();
}
