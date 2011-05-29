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

#include "FactoryPanel.h"
#include "PlanetMainPanel.h"

#include "utils.h"

#include "CLocalization.h"
#include "CResourceManager.h"


//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
FactoryPanel::FactoryPanel(GameLevel* gameLevel): IPanel(), mGameLevel(gameLevel)
{
	mFont = NULL;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
FactoryPanel::~FactoryPanel()
{
}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void FactoryPanel::Create()
{
	mFont = new JTTFont();
	mFont->Load("font.ttf", 16);

	mQuit = false;
	mTimer = 0.0f;

	mTilePosX = 0;
	mTilePosY = 0;

	mInvTilePosX = -2;
	mInvTilePosY = -2;

	mIsChoosingTile = true;

	for(int i=0; i<25; i++)
		mInv[i] = mGameLevel->mInventory[i];

	mSkipControls = false;

	mGameLevel->mWorldObjects->mHero->UnloadPhysic();
	mGameLevel->mWorldObjects->mCamPos = mGameLevel->mWorldObjects->mHero->GetCenterPosition();
	mGameLevel->mWorldObjects->mCamRot = M_PI_4;
	mGameLevel->mWorldObjects->mHero->mEnginePower = 1.0f;
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void FactoryPanel::Destroy()
{
	SAFE_DELETE(mFont);
	mGameLevel->mWorldObjects->mHero->mEnginePower = 0.0f;
	mGameLevel->mWorldObjects->mHero->LoadPhysic();
	mGameLevel->mWorldObjects->mCamPos = mGameLevel->mWorldObjects->mHero->GetCenterPosition();
	mGameLevel->mWorldObjects->mCamRot = M_PI_4;
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void FactoryPanel::Update()
{
	JGE* engine = JGE::GetInstance();
	mDeltaTime = engine->GetDelta();		// Get time elapsed since last update.

	CSquareShip* hero = mGameLevel->mWorldObjects->mHero;

	if(!mSkipControls)
	{
		if (engine->GetButtonClick(PSP_CTRL_CROSS))
		{
			if(mIsChoosingTile)
			{
				//mInvTilePosX = -2;
				//mInvTilePosY = -2;
				mIsChoosingTile = false;
			}
			else
			{
				int invSlot = 2+mInvTilePosX+(2+mInvTilePosY)*5;
				int slot = (hero->mSize>>1)+mTilePosX+((hero->mSize>>1)+mTilePosY)*hero->mSize;
				CSquareTile* newTile = mInv[invSlot];
				if(newTile && !newTile->IsEquipped())
				{
					CSquareTile* lastTile = hero->SetSquareTile(newTile, slot);
//  				hero->UnloadPhysic();
//  				hero->LoadPhysic();
// 					mGameLevel->mWorldObjects->mCamPos = hero->GetCenterPosition();
// 					mGameLevel->mWorldObjects->mCamRot = M_PI_4;

					mIsChoosingTile = true;
				}
			}
		}

		if (engine->GetButtonClick(PSP_CTRL_CIRCLE))
		{
			if(mIsChoosingTile)
			{
				int slot = (hero->mSize>>1)+mTilePosX+((hero->mSize>>1)+mTilePosY)*hero->mSize;
				CSquareTile* lastTile = hero->SetSquareTile(NULL, slot);
				if(lastTile)
					lastTile->SetEquipped(false);
// 				hero->UnloadPhysic();
// 				hero->LoadPhysic();
// 				mGameLevel->mWorldObjects->mCamPos = hero->GetCenterPosition();
// 				mGameLevel->mWorldObjects->mCamRot = M_PI_4;
			}
			else
			{
				mIsChoosingTile = true;
			}
		}

		int size;
		int *pTilePosX, *pTilePosY;
		if(mIsChoosingTile)
		{
			size = (int)(hero->mSize/2);
			pTilePosX = &mTilePosX;
			pTilePosY = &mTilePosY;
		}
		else
		{
			size = 2;
			pTilePosX = &mInvTilePosX;
			pTilePosY = &mInvTilePosY;
		}

		if (engine->GetButtonClick(PSP_CTRL_UP))	
		{
			(*pTilePosY)--;
			if((*pTilePosY)<-size)
				(*pTilePosY) = size;
		}
		if (engine->GetButtonClick(PSP_CTRL_DOWN))	
		{
			(*pTilePosY)++;
			if((*pTilePosY)>size)
				(*pTilePosY) = -size;
		}
		if (engine->GetButtonClick(PSP_CTRL_RIGHT))	
		{
			(*pTilePosX)++;
			if((*pTilePosX)>size)
				(*pTilePosX) = -size;
		}
		if (engine->GetButtonClick(PSP_CTRL_LEFT))	
		{
			(*pTilePosX)--;
			if((*pTilePosX)<-size)
				(*pTilePosX) = size;
		}


		if (engine->GetButtonClick(PSP_CTRL_START))
		{
			mNextPanel = new PlanetMainPanel(mGameLevel);
			mNextPanel->Create();
			mQuit = true;
		}
	}
	
	hero->LightUpdate(mDeltaTime, true);

	mTimer += mDeltaTime;
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void FactoryPanel::Render()
{
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();		

	renderer->FillRect(0.0f,0.0f,SCREEN_WIDTH_F, SCREEN_HEIGHT_F, ARGB(255, 0, 0, 0));

	int size;
	int width;
	float ratio;

	size = 4;
	width = 64;
	ratio = M_PI_2/(16.0f*size);
	for(int x=-size;x<=size;x++)
	{
		for(int y=-size;y<=size;y++)
		{
			renderer->DrawRect(
				(float)SCREEN_SIZE_X2+fmod(mTimer*4.0f, 64.0f)+(x-0.5f)*width, 
				(float)SCREEN_SIZE_Y2+fmod(mTimer*5.0f, 64.0f)+(y-0.5f)*width,
				(float)width, 
				(float)width,
				ARGB((12-(int)(8.0f*sinf(M_PI_4 + mTimer*4.0f+(x+y)*ratio))), 0, 255, (128-(int)(128.0f*sinf(mTimer*10.0f+(x+y)*16*ratio))))
				);
			renderer->DrawRect(
				(float)SCREEN_SIZE_X2+1+fmod(mTimer*4.0f, 64.0f)+(x-0.5f)*width, 
				(float)SCREEN_SIZE_Y2+1+fmod(mTimer*5.0f, 64.0f)+(y-0.5f)*width,
				(float)width, 
				(float)width,
				ARGB((12-(int)(8.0f*sinf(M_PI_4 + mTimer*4.0f+(x+y)*ratio))), 0, 255, (128-(int)(128.0f*sinf(mTimer*10.0f+(x+y)*16*ratio))))
				);
		}
	}

	size = 6;
	width = 48;
	ratio = M_PI_2/(8.0f*size);
	for(int x=-size;x<=size;x++)
	{
		for(int y=-size;y<=size;y++)
		{
			renderer->DrawRect(
				(float)SCREEN_SIZE_X2-fmod(mTimer*10.0f, 48.0f)+(x-0.5f)*width, 
				(float)SCREEN_SIZE_Y2-fmod(1.0f+mTimer*6.0f, 48.0f)+(y-0.5f)*width,
				(float)width, 
				(float)width,
				ARGB((20-(int)(12.0f*sinf(mTimer*3.0f+(x+y)*ratio))), 0, 255, (128-(int)(128.0f*sinf(mTimer*10.0f+(x+y)*16*ratio))))
				);
			renderer->DrawRect(
				(float)SCREEN_SIZE_X2+1-fmod(mTimer*10.0f, 48.0f)+(x-0.5f)*width, 
				(float)SCREEN_SIZE_Y2+1-fmod(1.0f+mTimer*6.0f, 48.0f)+(y-0.5f)*width,
				(float)width, 
				(float)width,
				ARGB((20-(int)(12.0f*sinf(mTimer*3.0f+(x+y)*ratio))), 0, 255, (128-(int)(128.0f*sinf(mTimer*10.0f+(x+y)*16*ratio))))
				);
		}
	}

	size = 5;
	width = 56;
	ratio = M_PI_2/(8.0f*size);
	for(int x=-size;x<=size;x++)
	{
		for(int y=-size;y<=size;y++)
		{
			renderer->DrawRect(
				(float)SCREEN_SIZE_X2+fmod(1.0f+mTimer*5.0f, 56.0f)+(x-0.5f)*width, 
				(float)SCREEN_SIZE_Y2-fmod(mTimer*3.0f, 56.0f)+(y-0.5f)*width,
				(float)width, 
				(float)width,
				ARGB((20-(int)(12.0f*sinf(mTimer*3.0f+(x+y)*ratio))), 0, 255, (128-(int)(128.0f*sinf(mTimer*10.0f+(x+y)*16*ratio))))
				);
			renderer->DrawRect(
				(float)SCREEN_SIZE_X2+1+fmod(1.0f+mTimer*5.0f, 56.0f)+(x-0.5f)*width, 
				(float)SCREEN_SIZE_Y2+1-fmod(mTimer*3.0f, 56.0f)+(y-0.5f)*width,
				(float)width, 
				(float)width,
				ARGB((20-(int)(12.0f*sinf(mTimer*3.0f+(x+y)*ratio))), 0, 255, (128-(int)(128.0f*sinf(mTimer*10.0f+(x+y)*16*ratio))))
				);
		}
	}

	CSquareShip* hero = mGameLevel->mWorldObjects->mHero;
	hero->Render(hero->GetOriginPosition(), hero->GetRotation(), hero->GetRotationMatrix());


	// tile sélection
	size = (int)(hero->mSize/2);
	width = SQUARETILE_SIZE;
	ratio = M_PI_2/(2.0f*size);
	for(int x=-size;x<=size;x++)
		for(int y=-size;y<=size;y++)
			renderer->DrawRect(
				(float)SCREEN_SIZE_X2+(x-0.5f)*width, 
				(float)SCREEN_SIZE_Y2+(y-0.5f)*width,
				(float)width, 
				(float)width,
				ARGB((64-(int)(64.0f*sinf(mTimer*6.0f+(x+y)*ratio))), (100-(int)(100.0f*sinf(mTimer*6.0f+(x+y)*ratio))), 255, 0)
				);

	int x = mTilePosX;
	int y = mTilePosY;
	renderer->DrawRect(
		(float)SCREEN_SIZE_X2+(x-0.5f)*width, 
		(float)SCREEN_SIZE_Y2+(y-0.5f)*width,
		(float)width, 
		(float)width,
		ARGB(128, 255, 0, 0)
		);
	renderer->FillRect(
		(float)SCREEN_SIZE_X2+(x-0.5f)*width, 
		(float)SCREEN_SIZE_Y2+(y-0.5f)*width,
		(float)width, 
		(float)width,
		ARGB(64, 255, (128-(int)(128.0f*sinf(mTimer*6.0f+(x+y)*ratio))), 0)
		);

	// inventaire sélection
	mFont->SetColor(ARGB(255, 255, (100-(int)(100.0f*sinf(mTimer*3.0f))), 0));
	mFont->RenderString("Arsenal", 100.0f, (float)SCREEN_SIZE_Y2-80.0f, JGETEXT_CENTER);
	mFont->SetColor(ARGB((100-(int)(50.0f*sinf(1.0f+mTimer*3.0f))), 255, (100-(int)(100.0f*sinf(mTimer*3.0f))), 0));
	mFont->RenderString("Arsenal", 101.0f, (float)SCREEN_SIZE_Y2-79.0f, JGETEXT_CENTER);

	renderer->FillRect(40.0f, 
		(float)SCREEN_SIZE_Y2-60.0f,
		5*24, 
		5*24,
		ARGB(100, 100, 255, 200)
		);
	size = 2;
	width = SQUARETILE_SIZE*3;
	ratio = M_PI_2/(2.0f*size);
	int i = 0;
	for(int y=-size;y<=size;y++)
	{
		for(int x=-size;x<=size;x++)
		{
			renderer->DrawRect(
			(float)100+(x-0.5f)*width, 
			(float)SCREEN_SIZE_Y2+(y-0.5f)*width,
			(float)width, 
			(float)width,
			ARGB(255, (100-(int)(100.0f*sinf(mTimer*6.0f+(x+y)*ratio))), 255, 0)
			);

			renderer->EnableTextureFilter(false);
			JQuad* quad = (mInv[i])?CResourceManager::GetInstance()->GetSquareTilesQuad(mInv[i]->GetType()):NULL;
			if(quad)
			{
#ifdef PSP
				quad->mBlend=GU_TFX_MODULATE;
#endif
				quad->SetColor(ARGB(255, 255, 255, 255));
				
				renderer->RenderQuad(
					quad,
					(float)(100+x*width), 
					(float)(SCREEN_SIZE_Y2+y*width)
					);
			}
			renderer->EnableTextureFilter(true);
			if(mInv[i] && mInv[i]->IsEquipped())
			{
				mFont->SetColor(ARGB(255, 255, (100-(int)(100.0f*sinf(mTimer*6.0f+(x+y)*ratio))), 0));
				mFont->RenderString("E", (float)100+x*width, (float)SCREEN_SIZE_Y2+y*width - 16.0f, JGETEXT_CENTER);
				//mFont->RenderString("E", (float)101+x*width, (float)SCREEN_SIZE_Y2+y*width - 15.0f, JGETEXT_CENTER);
			}
			i++;
		}
	}

	if(!mIsChoosingTile)
	{
		x = mInvTilePosX;
		y = mInvTilePosY;
		renderer->DrawRect(
			(float)100+(x-0.5f)*width, 
			(float)SCREEN_SIZE_Y2+(y-0.5f)*width,
			(float)width, 
			(float)width,
			ARGB(128, 255, 0, 0)
			);
		renderer->FillRect(
			(float)100+(x-0.5f)*width, 
			(float)SCREEN_SIZE_Y2+(y-0.5f)*width,
			(float)width, 
			(float)width,
			ARGB(64, 0, (128-(int)(128.0f*sinf(mTimer*6.0f+(x+y)*ratio))), 0)
			);
	}

	int invSlot = 2+mInvTilePosX+(2+mInvTilePosY)*5;
	int slot = (hero->mSize>>1)+mTilePosX+((hero->mSize>>1)+mTilePosY)*hero->mSize;
	const CSquareTile* selectedTile = mIsChoosingTile?hero->GetSquareTile(slot):mInv[invSlot];
	if(selectedTile)
	{
		int line = 0;
		mFont->SetColor(ARGB(255, 255, (100-(int)(100.0f*sinf(mTimer*3.0f))), 0));

		string str = selectedTile->GetName() + "\n" 
			+ selectedTile->GetDesc() + "\n"
			+ selectedTile->GetStatDesc() + "\n";
		vector<string> paragraphs;
		StringExplode(str, "\n", paragraphs);
		vector<string>::const_iterator itP = paragraphs.begin();
		const vector<string>::const_iterator itPEnd = paragraphs.end();
		while(itP != itPEnd)
		{
			vector<string> lines;
			int nbLines = StringFormat(*itP, mFont, 160, " \t", lines);

			vector<string>::const_iterator itL = lines.begin();
			const vector<string>::const_iterator itLEnd = lines.end();
			while(itL != itLEnd)
			{
				mFont->RenderString(itL->c_str(), 300.0f, 10.0f+line, JGETEXT_LEFT);
				line += 12;
				++itL;
			}
			line += 10;
			++itP;
		}
	}

}
