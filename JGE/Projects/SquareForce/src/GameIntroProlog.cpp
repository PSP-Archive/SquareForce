#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>


#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>
#include <JGBKFont.h>
#include <JSoundSystem.h>

#include <JTTFont.h>
#include <Encoding.h>

#include "GameIntroProlog.h"
#include "utils.h"

#include "CLocalization.h"

GameIntroProlog::GameIntroProlog()
{
	mTexBG = NULL;
	mBGMesh = NULL;

	mFont = NULL;
	mMusic = NULL;
}


GameIntroProlog::~GameIntroProlog()
{
}


void GameIntroProlog::Create()
{
	GameIntro::Create();

	JRenderer* renderer = JRenderer::GetInstance();		
	mTexBG=renderer->LoadTexture("bg2.png");

	mBGMesh = new JQuad(mTexBG, 0, 0, (float)mTexBG->mWidth, (float)mTexBG->mHeight);

	mFont = new JTTFont();
	mFont->Load("font.ttf", 18);


	mSpeed = 10.0f;
	mTotalTime = 0.0f;
	
	string file = CLocalization::GetInstance()->GetPath();
	file += "prolog.txt";
	ifstream fichier(file.c_str(), ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		string paragraph;
		while(getline(fichier, paragraph))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			vector<string> lines;
			int nbLines = StringFormat(paragraph, mFont, 400, " \t", lines);
			mTexts.push_back(lines);
			mTotalTime += ((float)nbLines*20.0f + 10.0f)/mSpeed;
		}

		fichier.close();
	}

#ifndef NO_SOUND
	JSoundSystem* sound = JSoundSystem::GetInstance();

	if (sound)
		mMusic = sound->LoadMusic("1-10 Xaleph - Son of Chaos (Shinra Company).mp3");	// Load a background music.

	if (mMusic)
		JSoundSystem::GetInstance()->PlayMusic(mMusic, true);
#endif

	mTotalTime += 272.0f/mSpeed;
	mDeltaTime = 0.0f;

	mOffset = 272;
}


void GameIntroProlog::Destroy()
{
	SAFE_DELETE(mTexBG);

	SAFE_DELETE(mBGMesh);

#ifndef NO_SOUND
	JSoundSystem* sound = JSoundSystem::GetInstance();
	sound->StopMusic(mMusic);
 	SAFE_DELETE(mMusic);
#endif
	if(mFont)
		mFont->Unload();
	SAFE_DELETE(mFont);

	GameIntro::Destroy();
}


void GameIntroProlog::Update()
{
	GameIntro::Update();

	JGE* engine = JGE::GetInstance();

	mDeltaTime = engine->GetDelta();

	static float timeElapsed = 0.0f;
	timeElapsed += mDeltaTime;
	//mQuit = false;

	if(timeElapsed > mTotalTime + 3.0f)
	{
		mQuit = true;
		timeElapsed = mTotalTime + 3.0f;
	}

	JSoundSystem* sound = JSoundSystem::GetInstance();
	if(timeElapsed<3.0f)
		sound->SetMusicVolume((int)(100*timeElapsed/3.0f));
 	else if(timeElapsed > mTotalTime)
 		sound->SetMusicVolume((int)(100*(3.0f-(timeElapsed-mTotalTime))/3.0f));
	else
		sound->SetMusicVolume(100);

}


void GameIntroProlog::Render()
{
	GameIntro::Render();

	JRenderer* renderer = JRenderer::GetInstance();

	renderer->RenderQuad(mBGMesh, 0, 0);

	renderer->EnableTextureFilter(false);

	mFont->SetColor(ARGB(255,255,255,255));
	
	vector< vector<string> >::const_iterator itP = mTexts.begin();
	const vector< vector<string> >::const_iterator itPEnd = mTexts.end();
	float line = mOffset;
	while(itP != itPEnd)
	{
		vector<string>::const_iterator itL = itP->begin();
		const vector<string>::const_iterator itLEnd = itP->end();
		while(itL != itLEnd)
		{
			if(line < 300.0f && line > -30.0f )
			{
				mFont->SetColor(ARGB(64, 255, 255, 255));
				mFont->RenderString(itL->c_str(), 242.0f, (float)((int)(line+2.0f)), JGETEXT_CENTER);
				mFont->SetColor(ARGB(128, 0, 255, 255));
				mFont->RenderString(itL->c_str(), 241.0f, (float)((int)(line+1.0f)), JGETEXT_CENTER);
				mFont->SetColor(ARGB(255, 0, 255, 0));
				mFont->RenderString(itL->c_str(), 240.0f, (float)((int)(line)), JGETEXT_CENTER);
			}
			line += 20;
			++itL;
		}
		line += 10;
		++itP;
	}
	mOffset-=mDeltaTime*mSpeed;
}


void GameIntroProlog::Pause()
{
	GameIntro::Pause();
}


void GameIntroProlog::Resume()
{
	GameIntro::Resume();
}