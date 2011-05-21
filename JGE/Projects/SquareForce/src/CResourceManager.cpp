//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#include "CResourceManager.h"
#include "utils.h"

#include <hge/hgedistort.h>

CResourceManager* CResourceManager::mInstance = NULL;

hgeDistortionMesh* CreatePlanetMesh(int rows, int cols)
{
	float cellW =(float)(PLANET_TEXTURE_WIDTH/(cols-1));
	float cellH=(float)(PLANET_TEXTURE_HEIGHT/(rows-1));

	hgeDistortionMesh* mesh = new hgeDistortionMesh(cols, rows);

	float dx = (float)(cellW*(cols-1))*0.5f;
	float dy = (float)(cellH*(rows-1))*0.5f;

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			float y = i*cellH-dy;
			y = sin(y/dy*M_PI_2)*dy;
			float x = j*cellW -dx;
			x = sin(x/dx*M_PI_2)*dx;
			float a = abs(x);
			float b = dy;
			if(x < 0.0f)
				x = -a*sqrt(1.0f-y*y/(b*b));
			else if (x > 0.0f)
				x = a*sqrt(1.0f-y*y/(b*b));
			else
				x = 0.0f;

			mesh->SetDisplacement(j,i,x,y,HGEDISP_CENTER);
		}
	}

	return mesh;
}

CResourceManager::CResourceManager()
{
	JRenderer* renderer = JRenderer::GetInstance();

	mSquareTilesTex = renderer->LoadTexture("SquareTiles.png", TEX_TYPE_USE_VRAM);
	mParticlesTex = renderer->LoadTexture("Particles.png", TEX_TYPE_USE_VRAM);
	mLightningTex = renderer->LoadTexture("nrj10anims18x16.png", TEX_TYPE_USE_VRAM);

	int tileSize = SQUARETILE_SIZE*3;
	mSquareTilesQuads = new JQuad*[2*NB_SQUARETILES_QUADS];
	for(int i=0; i<NB_SQUARETILES_QUADS; i++)
	{
		mSquareTilesQuads[i] = new JQuad(mSquareTilesTex, i*tileSize+1.0f, 1.0f, tileSize-2.0f, tileSize-2.0f);
		mSquareTilesQuads[i]->SetHotSpot((float)(tileSize-2)*0.5f,(float)(tileSize-2)*0.5f);
		mSquareTilesQuads[i]->SetColor(ARGB(255, 255, 255, 255));
	}
	for(int i=0; i<NB_SQUARETILES_QUADS; i++)
	{
		mSquareTilesQuads[NB_SQUARETILES_QUADS+i] = new JQuad(mSquareTilesTex, i*tileSize+1.0f, tileSize+1.0f, tileSize-2.0f, tileSize-2.0f);
		mSquareTilesQuads[NB_SQUARETILES_QUADS+i]->SetHotSpot((float)(tileSize-2)*0.5f,(float)(tileSize-2)*0.5f);
		mSquareTilesQuads[NB_SQUARETILES_QUADS+i]->SetColor(ARGB(255, 255, 255, 255));
	}

	mLightningQuads = new JQuad*[NB_LIGHTNING_FRAMES];
	for(int i=0; i<NB_LIGHTNING_FRAMES; i++)
	{
		mLightningQuads[i] = new JQuad(mLightningTex, 18*i+0.5f, 0.5f, 18-1, 16-1);
		mLightningQuads[i]->SetHotSpot((18-1)>>1, (16-1)>>1);
	}

	mParticlesQuads = new JQuad*[NB_PARTICLE_TEXTURES];
	for(int i=0; i<NB_PARTICLE_TEXTURES; i++)
		mParticlesQuads[i] = NULL;


	mPlanetMesh12 = CreatePlanetMesh(12, 12);
	mPlanetMesh16 = CreatePlanetMesh(16, 16);
	mPlanetMesh20 = CreatePlanetMesh(20, 20);

	mPlanetTex = renderer->LoadTexture("planet.png");
	mCloudsTex = renderer->LoadTexture("clouds.png");
	mShadowsTex = renderer->LoadTexture("shadows.png");
	mLightsTex = renderer->LoadTexture("lights.png");
}

CResourceManager::~CResourceManager()
{
	SAFE_DELETE(mLightningTex);
	SAFE_DELETE(mParticlesTex);
	SAFE_DELETE(mSquareTilesTex);

	for(int i=0; i<2*NB_SQUARETILES_QUADS; i++)
		SAFE_DELETE(mSquareTilesQuads[i]);
	SAFE_DELETE_ARRAY(mSquareTilesQuads);

	for(int i=0; i<NB_LIGHTNING_FRAMES; i++)
		SAFE_DELETE(mLightningQuads[i]);
	SAFE_DELETE_ARRAY(mLightningQuads);

	for(int i=0; i<NB_PARTICLE_TEXTURES; i++)
		SAFE_DELETE(mParticlesQuads[i]);
	SAFE_DELETE_ARRAY(mParticlesQuads);

	SAFE_DELETE(mPlanetMesh20);
	SAFE_DELETE(mPlanetMesh16);
	SAFE_DELETE(mPlanetMesh12);

	SAFE_DELETE(mPlanetTex);
	SAFE_DELETE(mCloudsTex);
	SAFE_DELETE(mShadowsTex);
	SAFE_DELETE(mLightsTex);
}


JQuad* CResourceManager::GetParticlesQuad(int num)
{
	if(num<0 || num>=NB_PARTICLE_TEXTURES)
		return NULL;

	if(!mParticlesQuads[num])
	{
		int texSizePcl = 32;
		mParticlesQuads[num] = new JQuad(mParticlesTex, (num%4)*texSizePcl+0.5f, (int(num/4))*texSizePcl+0.5f, texSizePcl-1.0f, texSizePcl-1.0f);
		mParticlesQuads[num]->SetHotSpot((float)((texSizePcl-1)/2),(float)((texSizePcl-1)/2));
		mParticlesQuads[num]->SetColor(ARGB(255, 255, 255, 255));
	}
	
	return mParticlesQuads[num];
}
