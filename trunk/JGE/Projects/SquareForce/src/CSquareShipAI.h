#ifndef _CSQUARESHIPAI_H_
#define _CSQUARESHIPAI_H_
#include "utils.h"

#include <list>

#include "CResourceManager.h"
#include "Box2D.h"
class CSquareShip;
class CSpawnManager;

using namespace std;

class CGroupData
{
public:
	CGroupData():
		mId(0),
		mSSDId(0),
		mNbShips(5),
		mSpawnPoint(Vector2D(0,0)), 
		mSpawnPointRadius(1000.0f), 
		mPatrolPoint(Vector2D(0,0)), 
		mPatrolPointRadius(1000.0f),
		mIsSpawned(false) {}
	~CGroupData() {}

	unsigned int mId;// id du groupe

	unsigned int mSSDId;// id de squareship data

	unsigned int mNbShips;

	CResourceManager::EFaction mFaction;

	Vector2D mSpawnPoint;
	float mSpawnPointRadius;

	Vector2D mPatrolPoint;
	float mPatrolPointRadius;

	bool mIsSpawned;

	list<CSquareShip*> m_listShips;
};

class CSquareShipAI
{
public:
	CSquareShipAI(CSpawnManager *spawnMgr, CGroupData* groupData);
	~CSquareShipAI();

	void Update(float dt, CSquareShip *owner);

	inline CSquareShip* GetOwner(unsigned int i) const {return (i<mOwners.size())?mOwners[i]:NULL;}
	bool IsOwner(CSquareShip* ship);
	bool IsLeader(CSquareShip* ship);

	inline void AddOwner(CSquareShip* owner) {mOwners.push_back(owner); mGroupRadius = GetGroupRadius();}
	bool RemoveOwner(CSquareShip* owner);

	inline CGroupData* GetGroupData() {return mGroupData;}
	//inline void SetGroupData(CGroupData* groupData) {mGroupData = groupData;}

	inline CSquareShip* GetCurrentTarget() const {return mCurrentTarget;}
	inline void SetCurrentTarget(CSquareShip* target) {mCurrentTarget = target;}

protected:
	inline float GetGroupRadius()
	{
// 		unsigned int nbOwners = 1;
// 		unsigned int i;
// 		for(i = 1; nbOwners < mOwners.size(); ++i)
// 			nbOwners += i*4;
// 
// 		return 100.0f*(i-1);

		return 50.0f * (mOwners.size()-1);
	}
	//CSquareShip *mOwner;
	CSpawnManager *mSpawnMgr;

	CGroupData *mGroupData;

	Vector2D mCurrentDest;
	CSquareShip *mCurrentTarget;

	float mRatioErrorFiring;

	vector<CSquareShip*> mOwners;

	Vector2D mDir;

	float mGroupRadius;
};

#endif

