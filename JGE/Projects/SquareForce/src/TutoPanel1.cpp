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

#include "TutoPanel1.h"
#include "utils.h"

#include "CLocalization.h"

//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
TutoPanel1::TutoPanel1(GameLevel* gameLevel): IPanel(), mGameLevel(gameLevel)
{
	mFont = NULL;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
TutoPanel1::~TutoPanel1()
{
}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void TutoPanel1::Create()
{
	mFont = new JTTFont();
	mFont->Load("font.ttf", 16);

	mQuit = false;

	string file = CLocalization::GetInstance()->GetPath();
	file += "introtuto.txt";
	ifstream fichier(file.c_str(), ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		string paragraph;
		while(getline(fichier, paragraph))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			vector<string> lines;
			int nbLines = StringFormat(paragraph, mFont, 400, " \t", lines);
			mTexts.push_back(lines);
		}

		fichier.close();
	}
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void TutoPanel1::Destroy()
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
void TutoPanel1::Update()
{
	JGE* engine = JGE::GetInstance();
	if (engine->GetButtonClick(PSP_CTRL_CROSS))	
	{
		mQuit = true;
	}
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void TutoPanel1::Render()
{
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();		

	renderer->FillRect(0.0f,0.0f,SCREEN_WIDTH_F, SCREEN_HEIGHT_F, ARGB(200, 0, 0, 0));

	mFont->SetColor(ARGB(255, 0, 255, 50));
	
	vector< vector<string> >::const_iterator itP = mTexts.begin();
	const vector< vector<string> >::const_iterator itPEnd = mTexts.end();
	float line = 60;
	while(itP != itPEnd)
	{
		vector<string>::const_iterator itL = itP->begin();
		const vector<string>::const_iterator itLEnd = itP->end();
		while(itL != itLEnd)
		{
			mFont->RenderString(itL->c_str(), 240.0f, line, JGETEXT_CENTER);
			line += 10;
			++itL;
		}
		line += 10;
		++itP;
	}
}
