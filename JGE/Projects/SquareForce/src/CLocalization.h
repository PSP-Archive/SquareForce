//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _CLOCALIZATION_H_
#define _CLOCALIZATION_H_

#include <vector>
#include <string>

using namespace std;

class CLocalization
{
	
public:
	enum ELangToken
	{
		TITLE_TOKEN_SUBTITLE, 
		TITLE_TOKEN_1, 
		TITLE_TOKEN_2, 
		TITLE_TOKEN_3,
		MAIN_PANEL_TOKEN_1,
		MAIN_PANEL_TOKEN_2,
		MAIN_PANEL_TOKEN_3,
		NUM_TOKENS
	};

	enum ELangTokenTiles
	{
		TILE_TOKEN_HULL,
		TILE_TOKEN_REGEN_SPEED,
		TILE_TOKEN_RECOVERY_TIME,
		TILE_TOKEN_LINEAR_ACCELERATION,
		TILE_TOKEN_MISSILE_SPEED,
		TILE_TOKEN_MISSILE_CADENCY,
		TILE_TOKEN_MISSILE_DAMAGES,
		NUM_TOKENS_TILES
	};

	inline static CLocalization* GetInstance() {return mInstance?mInstance:(mInstance = new CLocalization());}
	inline static void DestroyInstance() {if(mInstance) delete mInstance; mInstance = NULL;}

	bool GetLangList();
	bool LoadStrings();
	bool LoadSquareTilesStrings();
	inline string GetLanguages(int i) {return (i>=0 && i<mLanguages.size())?mLanguages[i]:"";}
	inline string GetPath() {return mPath;}
	inline void SetLanguage(int i) {if(i>=0 && i<mLanguages.size()) mPath = mPathList[i];}
	inline string GetToken(ELangToken token) {return mTokens[(int)token];}
	inline string GetSquareTilesToken(ELangTokenTiles token) {return mSquareTilesTokens[(int)token];}

	
protected:
	CLocalization():mPath("") {}
	~CLocalization() {}

	static CLocalization* mInstance;

	string mPath;
	vector<string> mLanguages;
	vector<string> mPathList;

	vector<string> mTokens;
	vector<string> mSquareTilesTokens;

};

#endif
