#ifndef _CSQUARESHIPAI_H_
#define _CSQUARESHIPAI_H_
#include "utils.h"

#include "Box2D.h"
class CSquareShip;
class CSpawnManager;

class CSquareShipAI
{
public:
	CSquareShipAI(CSpawnManager *spawnMgr);
	~CSquareShipAI();

	void Update(float dt, CSquareShip *owner);
	void LightUpdate(float dt, CSquareShip *owner);

	inline CSquareShip* GetOwner(unsigned int i) const {return (i<mOwners.size())?mOwners[i]:NULL;}
	inline void AddOwner(CSquareShip* owner) {mOwners.push_back(owner);}

	inline const Vector2D& GetSpawnPoint() const {return mSpawnPoint;}
	inline void SetSpawnPoint(const Vector2D& spawnPoint) {mSpawnPoint = spawnPoint;}
	inline const float& GetSpawnPointRadius() const {return mSpawnPointRadius;}
	inline void SetSpawnPointRadius(const float& spawnPointRadius) {mSpawnPointRadius = spawnPointRadius;}

	inline const Vector2D& GetPatrolPoint() const {return mPatrolPoint;}
	inline void SetPatrolPoint(const Vector2D& patrolPoint) {mPatrolPoint = patrolPoint;}
	inline const float& GetPatrolPointRadius() const {return mPatrolPointRadius;}
	inline void SetPatrolPointRadius(const float& patrolPointRadius) {mPatrolPointRadius = patrolPointRadius;}


	inline CSquareShip* GetCurrentTarget() const {return mCurrentTarget;}
	inline void SetCurrentTarget(CSquareShip* target) {mCurrentTarget = target;}

protected:
	//CSquareShip *mOwner;
	CSpawnManager *mSpawnMgr;

	Vector2D mSpawnPoint;
	float mSpawnPointRadius;

	Vector2D mPatrolPoint;
	float mPatrolPointRadius;

	Vector2D mCurrentDest;
	CSquareShip *mCurrentTarget;

	float mRatioErrorFiring;

	vector<CSquareShip*> mOwners;
};

#endif

