#ifndef _CPLANET_H_
#define _CPLANET_H_
#include "utils.h"

#include "Box2D.h"


class hgeDistortionMesh;

class CPlanet
{
public:
	CPlanet();
	CPlanet(string name, float size, unsigned int idTexPlanet, unsigned int idTexClouds);
	~CPlanet();

	void Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat);
	void Update(float dt);

	inline const Vector2D& GetOriginPosition() const {return mOriginPosition;}
	inline void SetOriginPosition(const Vector2D& position) {mOriginPosition = position;}
	
	inline const float& GetRotation() const {return mRotation;}
	inline const float& GetCloudsRotation() const {return mCloudsRotation;}
	inline const float& GetPlanetAngularVelocity() const {return mPlanetAngularVelocity;}
	inline const float& GetCloudsAngularVelocity() const {return mCloudsAngularVelocity;}

	inline const string& GetName() const {return mName;}
	inline void SetName(const string& name) {mName = name;}

	inline const unsigned int& GetIdTexPlanet() const {return mIdTexPlanet;}
	inline void SetIdTexPlanet(const unsigned int& id) {mIdTexPlanet = id;}
	inline const unsigned int& GetIdTexClouds() const {return mIdTexClouds;}
	inline void SetIdTexClouds(const unsigned int& id) {mIdTexClouds = id;}

	inline const unsigned int& GetAlignment() const {return mAlignment;}
	inline void SetAlignment(const unsigned int& alignment) {mAlignment = alignment;}

	inline const PIXEL_TYPE& GetPlanetColor() const {return mPlanetColor;}
	inline void SetPlanetColor(const PIXEL_TYPE& color) {mPlanetColor = color;}
	inline const PIXEL_TYPE& GetCloudsColor() const {return mCloudsColor;}
	inline void SetCloudsColor(const PIXEL_TYPE& color) {mCloudsColor = color;}
	inline const PIXEL_TYPE& GetShadowsColor() const {return mShadowsColor;}
	inline void SetShadowsColor(const PIXEL_TYPE& color) {mShadowsColor = color;}
	inline const PIXEL_TYPE& GetLightsColor() const {return mLightsColor;}
	inline void SetLightsColor(const PIXEL_TYPE& color) {mLightsColor = color;}

	inline const float& GetSize() const {return mSizeX;}

protected:
	hgeDistortionMesh* mPlanetMesh;

	JTexture* mPlanetTex;
	JTexture* mCloudsTex;
	JTexture* mShadowsTex;
	JTexture* mLightsTex;

	float mDeltaTime;
	float mTimer;

	Vector2D mOriginPosition;
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

	string mName;

	unsigned int mIdTexPlanet;
	unsigned int mIdTexClouds;

	unsigned int mAlignment;// todo : mettre les types ds un enum

};

#endif

