#ifndef _CSPAWNMANAGER_H_
#define _CSPAWNMANAGER_H_

#include <list>
#include <vector>

#include "utils.h"

#include "Box2D.h"
#include "CSquareShip.h"

using namespace std;

class CSpawnManager
{
public:
	CSpawnManager();
	~CSpawnManager();

	inline void AddObject(CObject* obj) {mListObjects.push_back(obj);}
	inline CObject* GetObject(int i) {return ((i>=0 && i<mVisibleObjects.size())?mVisibleObjects[i]:NULL);}

	inline void SetHero(CSquareShip* hero) 
	{
		mListObjects.push_front(hero);
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

	CSquareShip* mHero;
	list<CObject*> mListObjects;
	vector<CObject*> mVisibleObjects;

};

#endif

