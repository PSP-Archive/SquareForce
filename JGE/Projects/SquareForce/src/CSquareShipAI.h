#ifndef _CSQUARESHIPAI_H_
#define _CSQUARESHIPAI_H_
#include "utils.h"

#include "Box2D.h"
class CSquareShip;
class CWorldObjects;

class CSquareShipAI
{
public:
	CSquareShipAI(CWorldObjects *worldObjects, CSquareShip *owner);
	~CSquareShipAI();

	void Update(float dt);

protected:
	CSquareShip *mOwner;
	CWorldObjects *mWorldObjects;

	Vector2D mSpawnPoint;
	float mSpawnPointRadius;

	Vector2D mPatrolPoint;
	float mPatrolPointRadius;

	Vector2D mCurrentDest;
	CSquareShip *mCurrentTarget;

	float mRatioErrorFiring;
};

#endif

