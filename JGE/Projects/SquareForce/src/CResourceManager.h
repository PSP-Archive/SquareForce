#ifndef _CRESOURCEMANAGER_H_
#define _CRESOURCEMANAGER_H_

#include "utils.h"


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

	inline JTexture* GetPlanetTex() {return mPlanetTex;}
	inline JTexture* GetCloudsTex() {return mCloudsTex;}
	inline JTexture* GetShadowsTex() {return mShadowsTex;}
	inline JTexture* GetLightsTex() {return mLightsTex;}
	
protected:
	CResourceManager();
	~CResourceManager();

	static CResourceManager* mInstance;

	JTexture* mSquareTilesTex;
	JQuad** mSquareTilesQuads;

	JTexture* mParticlesTex;
	JQuad** mParticlesQuads;

	JTexture* mLightningTex;
	JQuad** mLightningQuads;

	hgeDistortionMesh* mPlanetMesh20;
	hgeDistortionMesh* mPlanetMesh16;
	hgeDistortionMesh* mPlanetMesh12;

	JTexture* mPlanetTex;
	JTexture* mCloudsTex;
	JTexture* mShadowsTex;
	JTexture* mLightsTex;
};

#endif
