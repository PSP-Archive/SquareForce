#ifndef _CSPAWNMANAGER_H_
#define _CSPAWNMANAGER_H_

#include <list>
#include <vector>

#include "utils.h"

#include "Box2D.h"
#include "CSquareShip.h"
#include "CPlanet.h"

using namespace std;

class CSpawnManager
{
public:
	CSpawnManager();
	~CSpawnManager();

	inline void AddObject(CObject* obj) {mListObjects.push_back(obj);}
	inline void AddPlanet(CPlanet* planet) {mListPlanets.push_back(planet);}
	inline CObject* GetActiveObject(unsigned int i) {return ((i<mActiveObjects.size())?mActiveObjects[i]:NULL);}
	inline CObject* GetObject(unsigned int i) {return ((i<mListObjects.size())?mListObjects[i]:NULL);}
	inline CPlanet* GetPlanet(unsigned int i) {return ((i<mListPlanets.size())?mListPlanets[i]:NULL);}

	inline void SetHero(CSquareShip* hero) 
	{
		mListObjects.insert(mListObjects.begin(), hero);
		mHero = hero;
	}
	inline CSquareShip* GetHero() {return mHero;}
	
	CSquareShipData* GetEmptyShipDatas(int size);// attention les datas sont allouées

	void Update();

	vector<CSquareTile*> mListTiles;

	vector<CSquareShipData*> mListShipsDatas;

protected:
	bool ReadTilesTxt();
	bool WriteTilesRes();
	bool ReadTilesRes();
	bool ReadTilesDesc();

	bool ReadShipsTxt();
	bool WriteShipsRes();
	bool ReadShipsRes();
	bool ReadShipsDesc();

	bool ReadSectorTxt(unsigned int num);
	bool WriteSectorRes(unsigned int num);
	bool ReadSectorRes(unsigned int num);

	CSquareShip* mHero;
	vector<CObject*> mListObjects;
	vector<CObject*> mActiveObjects;

	vector<CPlanet*> mListPlanets;

};

#endif

