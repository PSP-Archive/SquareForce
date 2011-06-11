#ifndef _CSPAWNMANAGER_H_
#define _CSPAWNMANAGER_H_

#include <list>
#include <vector>

#include "utils.h"

#include "Box2D.h"
#include "CSquareShip.h"
#include "CPlanet.h"
#include "CSpeedWay.h"

using namespace std;

class CSpawnManager
{
public:
	CSpawnManager(b2World* world, list<CMissile*> &missilesPt);
	~CSpawnManager();

	inline void AddObject(CObject* obj) {mListObjects.push_back(obj);}
	inline void AddPlanet(CPlanet* planet) {mListPlanets.push_back(planet);}
	inline void AddSpeedWay(CSpeedWay* speedWay) {mListSpeedWays.push_back(speedWay);}
	inline CObject* GetActiveObject(unsigned int i) {return ((i<mActiveObjects.size())?mActiveObjects[i]:NULL);}
	inline CObject* GetObject(unsigned int i) {return ((i<mListObjects.size())?mListObjects[i]:NULL);}
	inline CPlanet* GetPlanet(unsigned int i) {return ((i<mListPlanets.size())?mListPlanets[i]:NULL);}
	inline CSpeedWay* GetSpeedWay(unsigned int i) {return ((i<mListSpeedWays.size())?mListSpeedWays[i]:NULL);}

	inline void SetHero(CSquareShip* hero) 
	{
		mListObjects.insert(mListObjects.begin(), hero);
		mHero = hero;
	}
	inline CSquareShip* GetHero() {return mHero;}
	
	CSquareShipData* GetEmptyShipDatas(int size);// attention les datas sont allouées

	void Update(float dt, const Vector2D& camPos);

	void AddGroup(unsigned int nbShips);

protected:
	bool ReadSectorTxt(unsigned int num);
	bool WriteSectorRes(unsigned int num);
	bool ReadSectorRes(unsigned int num);

	b2World* mWorld;
	list<CMissile*> mMissilesPt;

	CSquareShip* mHero;
	vector<CObject*> mListObjects;
	vector<CObject*> mActiveObjects;

	vector<CPlanet*> mListPlanets;

	vector<CSpeedWay*> mListSpeedWays;
};

#endif

