#ifndef _CPLANET_H_
#define _CPLANET_H_
#include "utils.h"

#include "Box2D.h"


class hgeDistortionMesh;

class CPlanet
{
public:
	CPlanet();
	~CPlanet();

	void Render(const b2Vec2& camPos, const float32& camRot);
	void Update(float dt);

	inline const b2Vec2& GetOriginPosition() const {return mOriginPosition;}
	inline void SetOriginPosition(const b2Vec2& position) {mOriginPosition = position;}
	
	inline const float& GetRotation() const {return mRotation;}
	inline const float& GetCloudsRotation() const {return mCloudsRotation;}
	inline const float& GetPlanetAngularVelocity() const {return mPlanetAngularVelocity;}
	inline const float& GetCloudsAngularVelocity() const {return mCloudsAngularVelocity;}

protected:
	hgeDistortionMesh* mPlanetMesh;

	JTexture* mPlanetTex;
	JTexture* mCloudsTex;
	JTexture* mShadowsTex;
	JTexture* mLightsTex;

	float mDeltaTime;
	float mTimer;

	b2Vec2 mOriginPosition;
	float mRotation;
	float mCloudsRotation;

	float mPlanetAngularVelocity;
	float mCloudsAngularVelocity;

	float mSizeX;
	float mSizeY;

	PIXEL_TYPE mPlanetColor;
	PIXEL_TYPE mCloudsColor;
	PIXEL_TYPE mLightsColor;
	PIXEL_TYPE mShadowsColor;

	float mRenderDist;
};

#endif

