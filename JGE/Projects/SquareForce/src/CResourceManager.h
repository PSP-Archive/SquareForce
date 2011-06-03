#ifndef _CRESOURCEMANAGER_H_
#define _CRESOURCEMANAGER_H_

#include "utils.h"

#include "CSquareShip.h"
#include "CPlanet.h"

#define PLANET_TEXTURE_WIDTH 256
#define PLANET_TEXTURE_HEIGHT 256


class hgeDistortionMesh;

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
		if(zoom < 2.0f)
			return mPlanetMesh12;
		else if (zoom < 3.0f)
			return mPlanetMesh16;
		else
			return mPlanetMesh20;
	}

	inline hgeDistortionMesh* GetPlasmaMesh() {return mPlasmaMesh;}

	//inline JTexture* GetPlanetTex(unsigned int id) {return mPlanetTex;}
	inline JTexture* GetPlanetTex() {return mCurrentPlanetTex;}
	//inline JTexture* GetCloudsTex(unsigned int id) {return mCloudsTex;}
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
		int iAngleDeg = (int)(fAngleDeg);
		if(iAngleDeg < 0)
			iAngleDeg += 360;
		return mCosTable[iAngleDeg];
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

	JTexture* mPlanetTex;
	JTexture* mCloudsTex;
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

};

#endif
