#ifndef _UTILS_H_
#define _UTILS_H_

#include "Box2D.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <JLBFont.h>
#include <JTTFont.h>
#include <JTypes.h>

using namespace std;
/* TODO
- faire des canons qui bougent (pr viser en direction du réticule) + limite 180°

- création secteurs avec liste de tous les objets (sans modèle physique) :
	->position ou zone de pop
- instanciation du modèle physique lorsque la position de pop est suffisamment proche du hero
- cam follow sur les missiles si on reste appuyé apres le tir
*/


// screen dimensions
#define SCREEN_SIZE_X					480
#define SCREEN_SIZE_Y					272

#define SCREEN_SIZE_X2					240
#define SCREEN_SIZE_Y2					136

// global particle emitter
#define GLOBAL_PARTICLE_SCALE			0.5f

// missiles particles textures
#define NB_PARTICLE_TEXTURES			16

// squaretiles properties
#define SQUARETILE_SIZE					8
#define NB_SQUARETILES_QUADS			4

// engine properties
#define NB_POINTS_ENGINE_TRAIL			15

// nb frames in the lightning animation texture
#define NB_LIGHTNING_FRAMES				10

// temp : nb objects
#define NB_OBJECTS						200
#define NB_PLANETS						5

// smooth coeff for the camera
#define SMOOTH_CAM_COEFF				0.75f

// minimap
#define MINIMAP_SCALE_MIN				20
#define MINIMAP_SCALE_MAX				500
#define MINIMAP_RADIUS					40

// zone morte du réticule (rayon)
#define RETICLE_DEAD_ZONE				50.0f

// projectiles
#define GUN_MISSILES_SPEED				300.0f

// SquareTiles
#define SQUARETILE_RECOVERY_TIME		2.0f

// Inventaire
#define SQUARETILE_INVENTORY_SIZE		100


// MATH
// projection scalaire de u sur v
inline b2Vec2 ScalarProj(const b2Vec2& u, const b2Vec2& v)
{
	b2Vec2 base = v;
	base.Normalize();
	return b2Dot(u, base)*base;
}


// string parser
inline void StringExplode(const string& str, const string& separator, vector<string>& results)
{
	string localStr = str;
	int found;
	found = localStr.find_first_of(separator);
	while(found != string::npos)
	{
		if(found > 0)
		{
			results.push_back(localStr.substr(0,found));
		}
		localStr = localStr.substr(found+1);
		found = localStr.find_first_of(separator);
	}
	if(localStr.length() > 0)
	{
		results.push_back(localStr);
	}
}

inline string& TrimString(string &str, const string &pre_chars, const string &post_chars)
{
	if (str.length() > 0) 
	{
		string::size_type first = str.find_first_not_of(pre_chars);
		string::size_type last = str.find_last_not_of(post_chars);
		if (first == string::npos)
			str.erase();
		else
			str = str.substr(first, first != string::npos && last != string::npos?last - first + 1:string::npos);
	}
	return str;  
}

inline string& TrimString(string &str) 
{
	return TrimString(str, " \t\r\n", " \t\r\n");
}

inline string SplitString(string &str, const string &delim_chars, bool trim_space)
{
	string token;

	string::size_type n = str.find_first_of(delim_chars);
	if (n == string::npos) 
	{
		token = str;
		str.erase();
	}
	else 
	{
		token = str.substr(0, n);
		str = str.substr(n + 1);
	}

	if (trim_space) 
	{
		TrimString(token);
		TrimString(str);
	}

	return token;
}


inline int StringFormat(const string& str, JTTFont* font, int maxWidth, const string& separator, vector<string>& results)
{
	int nbLines = 0;
	string localStr = str;
	int found;
	int lastfound = 0;
	found = localStr.find_first_of(separator);
	while(found != string::npos)
	{
		int w = font->GetWidth(localStr.substr(0,found).c_str());
		if(w > maxWidth)
		{
			results.push_back(localStr.substr(0, lastfound));
			nbLines++;

			localStr = localStr.substr(lastfound+1);
			found -= (lastfound+1);
		}
		lastfound = found;

		found = localStr.find_first_of(separator, lastfound+1);
	}
	if(localStr.length() > 0)
	{
		results.push_back(localStr);
		nbLines++;
	}
	return nbLines;
}

inline string FloatToString(float val)
{
	char cVal[30] = "";
	sprintf(cVal, "%.2f", val);
	string str = cVal;
	return str;
}

inline int GetFreeMemory()
{
	void* buf[64];
	int i = 0;
	for (i = 0; i < 64 ; i++) {
		buf[i] = malloc(1024 * 1024);
		if (!buf[i]) break;
	}
	int result = i;
	for (; i >= 0; i--) {
		free(buf[i]);
	}
	return result;
}
#endif
