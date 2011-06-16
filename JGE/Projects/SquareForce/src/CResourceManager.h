#ifndef _CRESOURCEMANAGER_H_
#define _CRESOURCEMANAGER_H_

#include "utils.h"

//#include "CSquareShip.h"
//#include "CPlanet.h"

#define PLANET_TEXTURE_SIZE 256.0f


class hgeDistortionMesh;
class CSquareShip;
class CSquareTile;
class CSquareShipData;
class CPlanet;

class CResourceManager
{
public:
	inline static CResourceManager* GetInstance() {return mInstance?mInstance:(mInstance = new CResourceManager());}
	inline static void DestroyInstance() {if(mInstance) delete mInstance; mInstance = NULL;}

	inline JTexture* GetSquareTilesTex() {return mSquareTilesTex;}
	inline JTexture* GetParticlesTex() {return mParticlesTex;}
	inline JTexture* GetLightningTex() {return mLightningTex;}
	inline JTexture* GetPlasmaTex() {return mPlasmaTex;}

	inline JQuad* GetSquareTilesQuad(int num) {return (num>=0 && num<2*NB_SQUARETILES_QUADS)?mSquareTilesQuads[num]:NULL;}
	inline JQuad* GetLightningQuad(int num) {return (num>=0 && num<NB_LIGHTNING_FRAMES)?mLightningQuads[num]:NULL;}
	JQuad* GetParticlesQuad(int num);

	inline JQuad** GetSquareTilesQuads() {return mSquareTilesQuads;}
	inline JQuad** GetLightningQuads() {return mLightningQuads;}
	inline JQuad** GetParticlesQuads() {return mParticlesQuads;}

	inline hgeDistortionMesh* GetPlanetMesh(float zoom) 
	{
		if(zoom < 1.0f)
			return mPlanetMesh12;
		else if (zoom < 1.5f)
			return mPlanetMesh16;
		else
			return mPlanetMesh20;
	}

	inline hgeDistortionMesh* GetPlasmaMesh() {return mPlasmaMesh;}

	inline JTexture* GetPlanetTex() {return mCurrentPlanetTex;}
	inline JTexture* GetCloudsTex() {return mCurrentCloudsTex;}
	inline JTexture* GetShadowsTex() {return mShadowsTex;}
	inline JTexture* GetLightsTex() {return mLightsTex;}

	void UpdatePlasmaMesh(float dt);

	bool LoadPlanet(CPlanet* planet);
	inline bool IsLoading() {return mIsloadingTextures;}
	inline string GetCurrentPlanetTexName()
	{
		char name[100];
		sprintf(name, "planets/planet%d.png", (unsigned int)mDemandedPlanetTexId);
		string filename = name;
	
		return filename;
	}
	inline string GetCurrentCloudsTexName()
	{
		char name[100];
		sprintf(name, "planets/clouds%d.png", (unsigned int)mDemandedCloudsTexId);
		string filename = name;

		return filename;
	}
	
	vector<CSquareTile*> mListTiles;
	vector<CSquareShipData*> mListShipsDatas;


	inline float TCosf(float angle) 
	{
		float fAngleDeg = RAD2DEG*fmodf(angle, 2*M_PI);
		int iAngleDeg = (int)(fAngleDeg+0.5f);
		if(iAngleDeg < 0)
			iAngleDeg += 360;
		return mCosTable[iAngleDeg];
	}

	inline float TSinf(float angle) 
	{
		float fAngleDeg = RAD2DEG*fmodf(M_PI_2-angle, 2*M_PI);
		int iAngleDeg = (int)(fAngleDeg+0.5f);
		if(iAngleDeg < 0)
			iAngleDeg += 360;
		return mCosTable[iAngleDeg];
	}

	enum EFaction 
	{
		FACTION_CIVILIAN,
		FACTION_SQUAREFORCE,
		FACTION_BANDIT,
		FACTION_COP,
		FACTION_SQUAREMASTER,

		NUM_FACTIONS
	};
	inline float GetFactionConflict(EFaction fac1, EFaction fac2)
	{
		return mTableFactionConficts[fac1+fac2*NUM_FACTIONS];
	}
	inline void SetFactionConflict(EFaction fac1, EFaction fac2, float val)
	{
		mTableFactionConficts[fac1+fac2*NUM_FACTIONS] = mTableFactionConficts[fac2+fac1*NUM_FACTIONS] = (val<0.0f)?0.0f:(val<1.0f)?val:1.0f;
	}
	
	static bool HasloadTextures;

protected:
	CResourceManager();
	~CResourceManager();

	bool ReadTilesTxt();
	bool WriteTilesRes();
	bool ReadTilesRes();
	bool ReadTilesDesc();

	bool ReadShipsTxt();
	bool WriteShipsRes();
	bool ReadShipsRes();
	bool ReadShipsDesc();


	static CResourceManager* mInstance;

	JTexture* mSquareTilesTex;
	JQuad** mSquareTilesQuads;

	JTexture* mParticlesTex;
	JQuad** mParticlesQuads;

	JTexture* mLightningTex;
	JQuad** mLightningQuads;

	JTexture* mPlasmaTex;

	hgeDistortionMesh* mPlasmaMesh;

	hgeDistortionMesh* mPlanetMesh20;
	hgeDistortionMesh* mPlanetMesh16;
	hgeDistortionMesh* mPlanetMesh12;

	JTexture* mShadowsTex;
	JTexture* mLightsTex;

	float mCosTable[361];

#ifdef PSP
	int mThreadId;
	static int _ThreadLoadTextures(SceSize args, void *argp);
#else
	DWORD mThreadId;
	static DWORD WINAPI _ThreadLoadTextures(LPVOID lpParameter);
#endif
	void _InitLoaderThread();
	static void _UpdateLoadTextures();

	static bool mIsloadingTextures;

	static u32 mCurrentPlanetTexId;
	static u32 mDemandedPlanetTexId;
	static JTexture* mCurrentPlanetTex;
	static JTexture* mDemandedPlanetTex;

	static u32 mCurrentCloudsTexId;
	static u32 mDemandedCloudsTexId;
	static JTexture* mCurrentCloudsTex;
	static JTexture* mDemandedCloudsTex;


	// table 2d de taille NUM_FACTIONS contenant un indice de 0 � 1 mod�lisant le taux de paix entre 2 factions
	float* mTableFactionConficts;

};

#endif
