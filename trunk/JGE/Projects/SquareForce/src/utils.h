#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef PSP
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspdisplay_kernel.h>
#include <pspsysevent.h>
#include <psppower.h>
#else
#include <windows.h>
#include <assert.h>
#endif

#include "Box2D.h"
#include "Vector2D.h"
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
- cam follow sur les missiles si on reste appuyé apres le tir
*/

//#define HACK_SKIP_LOGO
//#define NO_SOUND
#define DEBUG_PRINT
#define USE_MULTITHREADED_LOADER


#ifdef PSP
extern string exceptionDebugString;
#define DebugLog(exp) do{exceptionDebugString = exp;}while(0)
#else
#define DebugLog(exp)
#endif

inline PIXEL_TYPE DeserializeColor(PIXEL_TYPE c)// from ARGB
{
	JColor color;
	color.a = (u8)(c>>24);
	color.r = (u8)(c>>16);
	color.g = (u8)(c>>8);
	color.b = (u8)c;
	return ARGB(color.a, color.r, color.g, color.b);
}

#ifdef PSP
inline PIXEL_TYPE SerializeColor(PIXEL_TYPE c)// to ARGB
{
	JColor color;
	color.a = (u8)(c>>24);
	color.b = (u8)(c>>16);
	color.g = (u8)(c>>8);
	color.r = (u8)c;

	return ARGB(color.a, color.b, color.g, color.r);// grosse flemme pr réécrire une macro...
}
#else
inline PIXEL_TYPE SerializeColor(PIXEL_TYPE c)// to ARGB
{
	return c;
}
#endif

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
#define NB_OBJECTS						50
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

#define popCast(type, var) (*((type*)&var))


template<typename T> inline void Swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

// Random number in range [-1,1]
inline float Random()
{
#ifdef PSP
	return vfpu_randf(-1.0f, 1.0f);
#else
	float r = (float)rand();
	r /= RAND_MAX;
	r = 2.0f * r - 1.0f;
	return r;
#endif
}

inline float Random(float lo, float hi)
{
	if(lo > hi)
		Swap<float>(lo, hi);
#ifdef PSP
	return vfpu_randf(lo, hi);
#else
	float r = (float32)rand();
	r /= RAND_MAX;
	r = (hi - lo) * r + lo;
	return r;
#endif
}


// MATH
// projection scalaire de u sur v
inline Vector2D ScalarProj(const Vector2D& u, const Vector2D& v)
{
	Vector2D base = v;
	base.Normalize();
	return (u*base)*base;
}

// vecteur direction le plus court d'un point ref à un segment ab
inline Vector2D DirectionToSegment(const Vector2D& ref, const Vector2D& a, const Vector2D& b)
{
	Vector2D segment = b-a;
	Vector2D base = segment;
	float baseSize = base.Normalize();
	Vector2D a2ref = ref-a;
	float flatDist = a2ref*base;
	if(flatDist <= 0.0f)
		return -a2ref;
	else if (flatDist >= baseSize)
		return (b-ref);

	return (a + flatDist*base - ref); 
}

// distance du point ref au segment ab
inline float DistanceFromSegment(const Vector2D& ref, const Vector2D& a, const Vector2D& b)
{
	return DirectionToSegment(ref, a, b).Length(); 
}

// distance² du point ref au segment ab
inline float Distance2FromSegment(const Vector2D& ref, const Vector2D& a, const Vector2D& b)
{
	return DirectionToSegment(ref, a, b).Length2(); 
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

#define RAM_BLOCK (1024 * 1024)
/* RAM simple check functions source */

// *** FUNCTIONS ***

inline u32 ramAvailableLineareMax(void)
{
	u32 size, sizeblock;
	u8 *ram;

	// Init variables
	size = 0;
	sizeblock = RAM_BLOCK;

#ifdef PSP
	int disableInterrupts = pspSdkDisableInterrupts();
#endif

	// Check loop
	while (sizeblock)
	{
		// Increment size
		size += sizeblock;

		// Allocate ram
		ram = (u8 *) malloc(size);

		// Check allocate
		if (!(ram))
		{
			// Restore old size
			size -= sizeblock;

			// Size block / 2
			sizeblock >>= 1;
		}
		else
			free(ram);
	}

#ifdef PSP
	pspSdkEnableInterrupts(disableInterrupts);
#endif

	return size;
}

inline u32 ramAvailable(void)
{
	u8 **ram, **temp;
	u32 size, count, x;

	// Init variables
	ram = NULL;
	size = 0;
	count = 0;

#ifdef PSP
	int disableInterrupts = pspSdkDisableInterrupts();
#endif

	// Check loop
	for (;;)
	{
		// Check size entries
		if (!(count % 10))
		{
			// Allocate more entries if needed
			temp = (u8**) realloc(ram, sizeof(u8 *) * (count + 10));
			if (!(temp)) break;

			// Update entries and size (size contains also size of entries)
			ram = temp;
			size += (sizeof(u8 *) * 10);
		}

		// Find max lineare size available
		x = ramAvailableLineareMax();
		if (!(x)) break;

		// Allocate ram
		ram[count] = (u8 *) malloc(x);
		if (!(ram[count])) break;

		// Update variables
		size += x;
		count++;
	}

	// Free ram
	if (ram)
	{
		for (x = 0; x < count; x++)
			free(ram[x]);
		free(ram);
	}

#ifdef PSP
	pspSdkEnableInterrupts(disableInterrupts);
#endif
	return size;
}

#ifdef PSP
#define PspAssert(x) ((x) ? (void) 0 : _PspAssert(__FILE__, __LINE__, #x))
inline void _PspAssert(const char *filename, long line, const char *exp)
{
	SceCtrlData pad;
	SceCtrlData oldPad;
	sceCtrlReadBufferPositive(&oldPad, 1);

	pspDebugScreenInit();
	pspDebugScreenSetBackColor(0x00FF0000);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();
	pspDebugScreenPrintf("PSP ASSERT !\n");

	pspDebugScreenPrintf("\nfile %s, line %ld, expression \"%s\"", filename, line, exp);
	
	pspDebugScreenPrintf("\n\nPress X to continue");
	pspDebugScreenPrintf("\nPress O to quit");

	for (;;)
	{
		sceCtrlReadBufferPositive(&pad, 1);
		if ((pad.Buttons ^ oldPad.Buttons) & pad.Buttons & PSP_CTRL_CROSS)
			break;
		else if ((pad.Buttons ^ oldPad.Buttons) & pad.Buttons & PSP_CTRL_CIRCLE)
			sceKernelExitGame();

		oldPad = pad;
		sceKernelDelayThread(100000);
	}
}
#else
#define PspAssert(cond) assert(cond)
#endif

#endif
