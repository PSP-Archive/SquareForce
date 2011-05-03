//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#include "CLocalization.h"
#include "utils.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

CLocalization* CLocalization::mInstance = NULL;

bool CLocalization::GetLangList()
{
	ifstream fichier("Res/languages.ini", ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		string line;
		while(getline(fichier, line))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			int pos = line.find('=');
			if(pos == -1)
				continue;
			mLanguages.push_back(line.substr(0, pos));
			mPathList.push_back(line.substr(pos+1, line.size()));
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

bool CLocalization::LoadStrings()
{
	string file = mPath;
	if(file == "")
		return false;
	file += "strings.txt";
	ifstream fichier(file.c_str(), ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		string line;
		while(getline(fichier, line))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			mTokens.push_back(line);
		}
		fichier.close();
	}
	else
		return false;

	if(mTokens.size() != NUM_TOKENS)
		return false;

	return true;
}

bool CLocalization::LoadSquareTilesStrings()
{
	string file = mPath;
	if(file == "")
		return false;
	file += "tiles_stat_desc.txt";
	ifstream fichier(file.c_str(), ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		string line;
		while(getline(fichier, line))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			mSquareTilesTokens.push_back(line);
		}
		fichier.close();
	}
	else
		return false;

	if(mSquareTilesTokens.size() != NUM_TOKENS_TILES)
		return false;

	return true;
}
