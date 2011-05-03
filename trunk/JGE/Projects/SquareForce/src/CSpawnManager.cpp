#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "CLocalization.h"

#include "CSpawnManager.h"


CSpawnManager::CSpawnManager()
{
	mHero = NULL;

	if(!ReadTilesRes())
	{
		ReadTilesTxt();
		WriteTilesRes();
	}
	ReadTilesDesc();

	if(!ReadShipsRes())
	{
		ReadShipsTxt();
		WriteShipsRes();
	}
	ReadShipsDesc();
}

CSpawnManager::~CSpawnManager()
{
	{
		vector<CSquareTile*>::iterator it = mListTiles.begin();
		const vector<CSquareTile*>::const_iterator itEnd = mListTiles.end();
		while(it != itEnd)
		{
			SAFE_DELETE((*it));
			++it;
		}
	}

	{
		vector<CSquareShipData*>::iterator it = mListShipsDatas.begin();
		const vector<CSquareShipData*>::const_iterator itEnd = mListShipsDatas.end();
		while(it != itEnd)
		{
			SAFE_DELETE((*it));
			++it;
		}
	}
}

CSquareShipData* CSpawnManager::GetEmptyShipDatas(int size)
{
	CSquareShipData* datas = new CSquareShipData;
	datas->mSize = size;
	datas->mTilesId = new u32[size*size];
	for(int i=0; i<size*size; ++i)
		datas->mTilesId[i] = 0;

	return datas;
}

void CSpawnManager::Update()
{
	if(mListObjects.empty())
		return;

	// on vide l'ancienne liste d'objets visibles 
	mVisibleObjects.clear();
	mVisibleObjects.push_back(mHero);

	// on charge la nouvelle
	list<CObject*>::const_iterator it = mListObjects.begin();
	++it;
	const list<CObject*>::const_iterator itEnd = mListObjects.end();
	const b2Vec2& selfPos = mHero->GetCenterPosition();
	while(it != itEnd)
	{
		float dist2 = ((*it)->GetCenterPosition() - selfPos).Length2();
		if(dist2 < 90000.0f)// dist<300 : on est dans le champ de vision
		{
			mVisibleObjects.push_back(*it);
			(*it)->LoadPhysic();// on charge la physique
		}
		else
			(*it)->UnloadPhysic();// on décharge la physique
		++it;
	}
}

bool CSpawnManager::ReadTilesTxt()
{
	ifstream fichier("Res/tiles.txt", ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		if(!mListTiles.empty())
			return false;
		mListTiles.push_back(NULL);
		int id = 1;

		string line;
		while(getline(fichier, line))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			if(line == "")
				continue;

			
			string key = SplitString(line, "=", true);
			if(key != "Type")
				return false;
			
			CSquareTile::ESquareTileType type;
			if(line == "HULL")
				type = CSquareTile::HULL;
			else if(line == "ENGINE")
				type = CSquareTile::ENGINE;
			else if(line == "GUN")
				type = CSquareTile::GUN;
			else if(line == "MISSILE")
				type = CSquareTile::MISSILE;
			else if(line == "MINE")
				type = CSquareTile::MINE;
			else
				return false;
			
			CSquareTile* tile = NULL;
			switch(type)
			{
			case CSquareTile::HULL:
				{
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "Hull")
						return false;
					float hull = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RegenSpeed")
						return false;
					float regenSpd = (float)(float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RecoveryTime")
						return false;
					float recovTime = (float)atof(line.c_str());
					CSquareTileHull *t = new CSquareTileHull(b2Vec2(0,0));
					t->SetLifePoints(hull);
					t->SetRegenSpeed(regenSpd);
					t->SetTimeBeforeRecovery(recovTime);
					tile = t;
				}
				break;
			case CSquareTile::ENGINE:
				{
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "Hull")
						return false;
					float hull = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RegenSpeed")
						return false;
					float regenSpd = (float)(float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RecoveryTime")
						return false;
					float recovTime = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "EngineAcceleration")
						return false;
					float accel = (float)atof(line.c_str());
					CSquareTileEngine *t = new CSquareTileEngine(b2Vec2(0,0));
					t->SetLifePoints(hull);
					t->SetRegenSpeed(regenSpd);
					t->SetTimeBeforeRecovery(recovTime);
					t->SetEngineAcceleration(accel);
					tile = t;
				}
				break;
			case CSquareTile::GUN:
				{
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "Hull")
						return false;
					float hull = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RegenSpeed")
						return false;
					float regenSpd = (float)(float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RecoveryTime")
						return false;
					float recovTime = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "MissileSpeed")
						return false;
					float missileSpd = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "MissileFrequency")
						return false;
					float missileCadency = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "HullDamages")
						return false;
					float missileDmg = (float)atof(line.c_str());
					CSquareTileGun *t = new CSquareTileGun(b2Vec2(0,0));
					t->SetLifePoints(hull);
					t->SetRegenSpeed(regenSpd);
					t->SetTimeBeforeRecovery(recovTime);
					t->SetMissileSpeed(missileSpd);
					t->SetMissileFrequency(missileCadency);
					t->SetHullDamages(missileDmg);
					tile = t;
				}
				break;
			case CSquareTile::MISSILE:
				{
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "Hull")
						return false;
					float hull = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RegenSpeed")
						return false;
					float regenSpd = (float)(float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RecoveryTime")
						return false;
					float recovTime = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "MissileSpeed")
						return false;
					float missileSpd = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "MissileFrequency")
						return false;
					float missileCadency = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "HullDamages")
						return false;
					float missileDmg = (float)atof(line.c_str());
					CSquareTileMissile *t = new CSquareTileMissile(b2Vec2(0,0));
					t->SetLifePoints(hull);
					t->SetRegenSpeed(regenSpd);
					t->SetTimeBeforeRecovery(recovTime);
					t->SetMissileSpeed(missileSpd);
					t->SetMissileFrequency(missileCadency);
					t->SetHullDammages(missileDmg);
					tile = t;
				}
				break;
			case CSquareTile::MINE:
				{
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "Hull")
						return false;
					float hull = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RegenSpeed")
						return false;
					float regenSpd = (float)(float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "RecoveryTime")
						return false;
					float recovTime = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "MissileSpeed")
						return false;
					float missileSpd = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "MissileFrequency")
						return false;
					float missileCadency = (float)atof(line.c_str());
					getline(fichier, line);
					key = SplitString(line, "=", true);
					if(key != "HullDamages")
						return false;
					float missileDmg = (float)atof(line.c_str());
					CSquareTileMine *t = new CSquareTileMine(b2Vec2(0,0));
					t->SetLifePoints(hull);
					t->SetRegenSpeed(regenSpd);
					t->SetTimeBeforeRecovery(recovTime);
					t->SetMissileSpeed(missileSpd);
					t->SetMissileFrequency(missileCadency);
					t->SetHullDammages(missileDmg);
					tile = t;
				}
				break;
			default:
				break;
			}
			if(tile)
			{
				tile->SetId(id);
				++id;
				mListTiles.push_back(tile);
			}			
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

bool CSpawnManager::WriteTilesRes()
{
	if(mListTiles.empty())
		return false;

	fstream fichier("Res/tiles.res", ios::out | ios::binary);  // on ouvre ecriture binaire

	if(fichier)  // si l'ouverture a fonctionné
	{
		vector<CSquareTile*>::const_iterator it = mListTiles.begin();
		++it;
		const vector<CSquareTile*>::const_iterator itEnd = mListTiles.end();

		while(it != itEnd)  // tant que l'on peut mettre la ligne dans "contenu"
		{
			if(!(*it))
				continue;

			CSquareTile::ESquareTileType type = (*it)->GetType();
			fichier.write((char*)&type, sizeof(CSquareTile::ESquareTileType));
			switch(type)
			{
			case CSquareTile::HULL:
				{
					float val;
					CSquareTileHull *t = (CSquareTileHull*)(*it);
					val = t->GetLifePoints();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetRegenSpeed();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetTimeBeforeRecovery();
					fichier.write((char*)&val, sizeof(float));
				}
				break;
			case CSquareTile::ENGINE:
				{
					float val;
					CSquareTileEngine *t = (CSquareTileEngine*)(*it);
					val = t->GetLifePoints();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetRegenSpeed();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetTimeBeforeRecovery();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetEngineAcceleration();
					fichier.write((char*)&val, sizeof(float));
				}
				break;
			case CSquareTile::GUN:
				{
					float val;
					CSquareTileGun *t = (CSquareTileGun*)(*it);
					val = t->GetLifePoints();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetRegenSpeed();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetTimeBeforeRecovery();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetMissileSpeed();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetMissileFrequency();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetHullDammages();
					fichier.write((char*)&val, sizeof(float));
				}
				break;
			case CSquareTile::MISSILE:
				{
					float val;
					CSquareTileMissile *t = (CSquareTileMissile*)(*it);
					val = t->GetLifePoints();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetRegenSpeed();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetTimeBeforeRecovery();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetMissileSpeed();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetMissileFrequency();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetHullDammages();
					fichier.write((char*)&val, sizeof(float));
				}
				break;
			case CSquareTile::MINE:
				{
					float val;
					CSquareTileMine *t = (CSquareTileMine*)(*it);
					val = t->GetLifePoints();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetRegenSpeed();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetTimeBeforeRecovery();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetMissileSpeed();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetMissileFrequency();
					fichier.write((char*)&val, sizeof(float));
					val = t->GetHullDammages();
					fichier.write((char*)&val, sizeof(float));
				}
				break;
			default:
				break;
			}
			++it;
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

bool CSpawnManager::ReadTilesRes()
{
	fstream fichier("Res/tiles.res", ios::in | ios::binary);  // on ouvre en lecture binaire

	if(fichier)  // si l'ouverture a fonctionné
	{
		if(!mListTiles.empty())
			return false;
		mListTiles.push_back(NULL);

		int id = 1;

		CSquareTile::ESquareTileType type;
		while(fichier.read((char*)&type, sizeof(CSquareTile::ESquareTileType)))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			CSquareTile* tile = NULL;
			switch(type)
			{
			case CSquareTile::HULL:
				{
					float val;
					CSquareTileHull *t = new CSquareTileHull(b2Vec2(0,0));
					fichier.read((char*)&val, sizeof(float));
					t->SetLifePoints(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetRegenSpeed(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetTimeBeforeRecovery(val);
					tile = t;
				}
				break;
			case CSquareTile::ENGINE:
				{
					float val;
					CSquareTileEngine *t = new CSquareTileEngine(b2Vec2(0,0));
					fichier.read((char*)&val, sizeof(float));
					t->SetLifePoints(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetRegenSpeed(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetTimeBeforeRecovery(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetEngineAcceleration(val);
					tile = t;
				}
				break;
			case CSquareTile::GUN:
				{
					float val;
					CSquareTileGun *t = new CSquareTileGun(b2Vec2(0,0));
					fichier.read((char*)&val, sizeof(float));
					t->SetLifePoints(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetRegenSpeed(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetTimeBeforeRecovery(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetMissileSpeed(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetMissileFrequency(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetHullDamages(val);
					tile = t;
				}
				break;
			case CSquareTile::MISSILE:
				{
					float val;
					CSquareTileMissile *t = new CSquareTileMissile(b2Vec2(0,0));
					fichier.read((char*)&val, sizeof(float));
					t->SetLifePoints(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetRegenSpeed(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetTimeBeforeRecovery(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetMissileSpeed(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetMissileFrequency(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetHullDammages(val);
					tile = t;
				}
				break;
			case CSquareTile::MINE:
				{
					float val;
					CSquareTileMine *t = new CSquareTileMine(b2Vec2(0,0));
					fichier.read((char*)&val, sizeof(float));
					t->SetLifePoints(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetRegenSpeed(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetTimeBeforeRecovery(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetMissileSpeed(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetMissileFrequency(val);
					fichier.read((char*)&val, sizeof(float));
					t->SetHullDammages(val);
					tile = t;
				}
				break;
			default:
				break;
			}
			if(tile)
			{
				tile->SetId(id);
				++id;
				mListTiles.push_back(tile);
			}			
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

bool CSpawnManager::ReadTilesDesc()
{
	string file = CLocalization::GetInstance()->GetPath();
	if(file == "")
		return false;
	file += "tiles_desc.txt";
	ifstream fichier(file.c_str(), ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		vector<CSquareTile*>::const_iterator it = mListTiles.begin();
		++it;
		const vector<CSquareTile*>::const_iterator itEnd = mListTiles.end();
		string line;
		while(getline(fichier, line))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			if(line == "" )
				continue;

			(*it)->SetName(line);
			getline(fichier, line);
			(*it)->SetDesc(line);
			(*it)->ComputeStatDesc();
			++it;
		}
		fichier.close();
		if(it != itEnd)
			return false;
	}
	else
		return false;

	return true;
}

bool CSpawnManager::ReadShipsTxt()
{
	ifstream fichier("Res/ships.txt", ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		if(!mListShipsDatas.empty())
			return false;
		
		string line;
		while(getline(fichier, line))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			if(line == "")
				continue;

			int size = atoi(line.c_str());
			CSquareShipData* datas = new CSquareShipData;
			datas->mSize = size;
			datas->mTilesId = new u32[size*size];
			for(int i=0; i<size*size; ++i)
			{
				getline(fichier, line);
				u32 id = atoi(line.c_str());
				datas->mTilesId[i] = id;				
			}
			mListShipsDatas.push_back(datas);
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

bool CSpawnManager::WriteShipsRes()
{
	if(mListShipsDatas.empty())
		return false;

	fstream fichier("Res/ships.res", ios::out | ios::binary);  // on ouvre ecriture binaire

	if(fichier)  // si l'ouverture a fonctionné
	{
		vector<CSquareShipData*>::const_iterator it = mListShipsDatas.begin();
		const vector<CSquareShipData*>::const_iterator itEnd = mListShipsDatas.end();

		while(it != itEnd)  // tant que l'on peut mettre la ligne dans "contenu"
		{
			if(!(*it))
				continue;

			int size = (*it)->mSize;
			fichier.write((char*)&size, sizeof(char));
			
			for(int i=0; i<size*size; ++i)
			{
				u32 id = (*it)->mTilesId[i];
				fichier.write((char*)&id, sizeof(u32));
			}
			++it;
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

bool CSpawnManager::ReadShipsRes()
{
	fstream fichier("Res/ships.res", ios::in | ios::binary);  // on ouvre en lecture binaire

	if(fichier)  // si l'ouverture a fonctionné
	{
		if(!mListShipsDatas.empty())
			return false;
		
		int size = 0;
		while(fichier.read((char*)&size, sizeof(char)))// tant qu'il y a des vaisseaux
		{
			CSquareShipData* datas = new CSquareShipData;
			datas->mSize = size;
			datas->mTilesId = new u32[size*size];
			for(int i=0; i<size*size; ++i)
			{
				u32 id = 0;
				fichier.read((char*)&id, sizeof(u32));
				datas->mTilesId[i] = id;
			}
			if(datas)
			{
				mListShipsDatas.push_back(datas);
			}
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

bool CSpawnManager::ReadShipsDesc()
{
	/*string file = CLocalization::GetInstance()->GetPath();
	if(file == "")
		return false;
	file += "tiles_desc.txt";
	ifstream fichier(file.c_str(), ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		vector<CSquareTile*>::const_iterator it = mListTiles.begin();
		++it;
		const vector<CSquareTile*>::const_iterator itEnd = mListTiles.end();
		string line;
		while(getline(fichier, line))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			if(line == "" )
				continue;

			(*it)->SetName(line);
			getline(fichier, line);
			(*it)->SetDesc(line);
			(*it)->ComputeStatDesc();
			++it;
		}
		fichier.close();
		if(it != itEnd)
			return false;
	}
	else
		return false;
*/
	return true;
}
