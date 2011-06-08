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

#include "PlanetMainPanel.h"
#include "FactoryPanel.h"
#include "MapPanel.h"
#include "utils.h"

#include "CLocalization.h"

//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
PlanetMainPanel::PlanetMainPanel(GameLevel* gameLevel): IPanel(), mGameLevel(gameLevel)
{
	mFont = NULL;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
PlanetMainPanel::~PlanetMainPanel()
{
}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void PlanetMainPanel::Create()
{
	mFont = new JTTFont();
	mFont->Load("font.ttf", 20);

	mQuit = false;

	mLine = 0;
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void PlanetMainPanel::Destroy()
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
void PlanetMainPanel::Update()
{
	JGE* engine = JGE::GetInstance();
	if (engine->GetButtonClick(PSP_CTRL_CROSS))	
	{
		mQuit = true;
		switch(mLine)
		{
		case 0:
			{
				mNextPanel = new FactoryPanel(mGameLevel);
				mNextPanel->Create();
			}
			break;
		case 1:
			{
				mQuit = true;// exit the Planet
			}
			break;
		default:
			break;
		}
	}
	if(engine->GetButtonClick(PSP_CTRL_UP))
	{
		mLine--;
		if(mLine<0)
			mLine = 1;
	}
	if(engine->GetButtonClick(PSP_CTRL_DOWN))
	{
		mLine++;
		if(mLine>1)
			mLine = 0;
	}
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void PlanetMainPanel::Render()
{
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();

	renderer->FillRect(0.0f,0.0f,SCREEN_WIDTH_F, SCREEN_HEIGHT_F, ARGB(255, 0, 0, 0));

	static float X = 240.0f;
	static float Y = 170.0f;
	static string str[]=
	{
		CLocalization::GetInstance()->GetToken(CLocalization::PLANET_MAIN_PANEL_TOKEN_1), 
		CLocalization::GetInstance()->GetToken(CLocalization::PLANET_MAIN_PANEL_TOKEN_2)
	};


	CPlanet* planet = NULL;
	mGameLevel->mWorldObjects->GetNearestPlanet(mGameLevel->mWorldObjects->mHero->GetCenterPosition(), planet);
	if(planet)
	{
		planet->Render(planet->GetOriginPosition(), 0, Matrix22::Identity());

		string name = planet ->GetName();
		mFont->SetColor(ARGB(255, 255, 255, 255));
		mFont->RenderString(name.c_str(), X+3.0f, 20+3.0f, JGETEXT_CENTER);
		mFont->SetColor(ARGB(64, 255, 255, 255));
		mFont->RenderString(name.c_str(), X+4.0f, 20+4.0f, JGETEXT_CENTER);
	}

	
	for(int i=0; i<2; i++)
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
}
