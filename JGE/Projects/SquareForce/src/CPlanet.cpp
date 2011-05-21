#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CPlanet.h"
#include "CResourceManager.h"

#include <hge/hgedistort.h>

CPlanet::CPlanet()
{
	mOriginPosition = Vector2D(0,0);
	mRotation = 0.1f;
	mCloudsRotation = 0.0f;

	mDeltaTime = 0.0f;
	mTimer = 0.0f;

	mPlanetAngularVelocity = 2.0f;
	mCloudsAngularVelocity = 3.0f;

	mSizeX = b2Random(0.5f, 4.0f);
	mSizeY = mSizeX;

	mPlanetColor = ARGB(255,255,255,255);
	mCloudsColor = ARGB(255,255,255,255);
	mLightsColor = ARGB(255,0,150,255);
	mShadowsColor = ARGB(220,255,255,255);

	CResourceManager* resMgr = CResourceManager::GetInstance();
	mPlanetTex = resMgr->GetPlanetTex();
	mCloudsTex = resMgr->GetCloudsTex();
	mShadowsTex = resMgr->GetShadowsTex();
	mLightsTex = resMgr->GetLightsTex();

	mPlanetMesh = resMgr->GetPlanetMesh(mSizeX);

	mRenderDist = 300.0f+1.07f*b2Max(mSizeX, mSizeY)*PLANET_TEXTURE_HEIGHT*0.5f;
	mRenderDist *= mRenderDist;
}



CPlanet::~CPlanet()
{

}

void CPlanet::Update(float dt)
{
	mDeltaTime = dt;
	mTimer += mDeltaTime;
	mCloudsRotation = fmod(mTimer*0.04f, (float)M_PI*2.0f);
}

void CPlanet::Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat)
{

	if((mOriginPosition-camPos).Length2() > mRenderDist)
		return;

	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();	

	Vector2D position = camMat / (mOriginPosition - camPos);
	float rotation = mRotation-camRot;

	mPlanetMesh->SetTexture(mPlanetTex);
	mPlanetMesh->SetTextureRect(fmod(mTimer*mPlanetAngularVelocity, (float)PLANET_TEXTURE_WIDTH*2.0f),0,PLANET_TEXTURE_WIDTH,PLANET_TEXTURE_HEIGHT);
	mPlanetMesh->Clear(mPlanetColor);
	mPlanetMesh->Render(SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, 1.0f*mSizeX, 1.0f*mSizeY, -rotation);
	mPlanetMesh->SetTexture(mCloudsTex);
	mPlanetMesh->SetTextureRect(fmod(mTimer*mCloudsAngularVelocity, (float)PLANET_TEXTURE_WIDTH*2.0f),0,PLANET_TEXTURE_WIDTH,PLANET_TEXTURE_HEIGHT);
	mPlanetMesh->Clear(mCloudsColor);
	mPlanetMesh->Render(SCREEN_SIZE_X2+position.x,  SCREEN_SIZE_Y2-position.y, 1.02f*mSizeX, 1.02f*mSizeY, -rotation-mCloudsRotation);
	mPlanetMesh->SetTexture(mLightsTex);
	mPlanetMesh->SetTextureRect(0,0,PLANET_TEXTURE_WIDTH,PLANET_TEXTURE_HEIGHT);
	mPlanetMesh->Clear(mLightsColor);
	mPlanetMesh->Render(SCREEN_SIZE_X2+position.x,  SCREEN_SIZE_Y2-position.y, 1.07f*mSizeX, 1.07f*mSizeY, -rotation);
	mPlanetMesh->SetTexture(mShadowsTex);
	//mPlanetMesh->SetTextureRect(0,0,TEXTURE_WIDTH,TEXTURE_HEIGHT);
	mPlanetMesh->Clear(mShadowsColor);
	mPlanetMesh->Render(SCREEN_SIZE_X2+position.x,  SCREEN_SIZE_Y2-position.y, 1.03f*mSizeX, 1.03f*mSizeY, -rotation);
}
