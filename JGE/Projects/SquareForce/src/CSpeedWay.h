#ifndef _CSPEEDWAY_H_
#define _CSPEEDWAY_H_
#include "utils.h"

#include "Box2D.h"

#include "CSquareTile.h"
#include "CPlanet.h"


class hgeDistortionMesh;

class CSpeedGate
{
public:
	CSpeedGate(const Vector2D& originPosition, const float& rotation, const Matrix22& matrixRot);
	~CSpeedGate();

	inline const Vector2D& GetOriginPosition() const {return mOriginPosition;}
	inline void SetOriginPosition(const Vector2D& position) {mOriginPosition = position;}

	void Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat);
	void Update(float dt);


protected:
	CSquareTile* mGate[2];

	Vector2D mOriginPosition;
	float mRotation;
	Matrix22 mMatrixRot;

	JQuad** mLightningQuads;
	JQuad** mQuad;

};

class CSpeedWay
{
public:
	CSpeedWay(CPlanet* planet1, CPlanet* planet2);
	~CSpeedWay();

	void Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat);
	void Update(float dt);


protected:
	hgeDistortionMesh* mDistortMesh;

	CPlanet* mPlanet1;
	CPlanet* mPlanet2;

	Vector2D mDir;

	float mPlasmaSizeX;
	float mPlasmaSizeY;

	float mRotation;
	Matrix22 mMatrixRot;

	vector<CSpeedGate*> mListGates;

};

#endif

