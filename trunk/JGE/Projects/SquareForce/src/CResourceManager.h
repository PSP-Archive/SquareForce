#ifndef _CRESOURCEMANAGER_H_
#define _CRESOURCEMANAGER_H_

#include "utils.h"

#include "CSquareShip.h"

#define PLANET_TEXTURE_WIDTH 128
#define PLANET_TEXTURE_HEIGHT 128


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

	inline JTexture* GetPlanetTex(unsigned int id) {return mPlanetTex;}
	inline JTexture* GetCloudsTex(unsigned int id) {return mCloudsTex;}
	inline JTexture* GetShadowsTex() {return mShadowsTex;}
	inline JTexture* GetLightsTex() {return mLightsTex;}

	void UpdatePlasmaMesh(float dt);
	
	vector<CSquareTile*> mListTiles;
	vector<CSquareShipData*> mListShipsDatas;

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
};

#endif
