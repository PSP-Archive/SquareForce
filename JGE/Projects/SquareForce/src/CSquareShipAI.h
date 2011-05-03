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

	b2Vec2 mSpawnPoint;
	float mSpawnPointRadius;

	b2Vec2 mPatrolPoint;
	float mPatrolPointRadius;

	b2Vec2 mCurrentDest;
	CSquareShip *mCurrentTarget;

	float mRatioErrorFiring;
};

#endif

