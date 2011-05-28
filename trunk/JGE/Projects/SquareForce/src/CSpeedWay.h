#ifndef _CSPEEDWAY_H_
#define _CSPEEDWAY_H_
#include "utils.h"

#include "Box2D.h"

#include "CSquareTile.h"
#include "CPlanet.h"
#include "CSquareShip.h"

#include <vector>
#include <list>

using namespace std;

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

	inline void DockShip(CObject* obj, bool state) 
	{
		if(state) 
			mDockedObjects.push_back(obj); 
		else
			mDockedObjects.remove(obj);
	}

	inline CPlanet* GetPlanet(int num) {return (num==0)?mPlanet1:((num==1)?mPlanet2:NULL);}
	inline const Vector2D& GetStart() const {return mStart;}
	inline const Vector2D& GetEnd() const {return mEnd;}

protected:
	hgeDistortionMesh* mDistortMesh;

	CPlanet* mPlanet1;
	CPlanet* mPlanet2;

	Vector2D mStart;
	Vector2D mEnd;

	Vector2D mDir;

	float mPlasmaSizeX;
	float mPlasmaSizeY;

	float mRotation;
	Matrix22 mMatrixRot;

	vector<CSpeedGate*> mListGates;

	list<CObject*> mDockedObjects;
};

#endif

