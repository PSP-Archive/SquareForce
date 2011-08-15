#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "CLocalization.h"
#include "CResourceManager.h"

#include "CSpawnManager.h"


CSpawnManager::CSpawnManager(b2World* world, list<CMissile*> &missilesPt)
: mWorld(world), mMissilesPt(missilesPt)
{
	mHero = NULL;

	if(!ReadSectorRes(0))
	{
		ReadSectorTxt(0);
		WriteSectorRes(0);
	}
}

CSpawnManager::~CSpawnManager()
{
	// ne pas supprimer les tiles sur le hero (elles sont dans l'inventaire)
	for(int i=0; i<mHero->mSize*mHero->mSize; i++)
		mHero->SetSquareTile(NULL, i);
	SAFE_DELETE(mHero->mDatas);
	
	vector<CObject*>::iterator itObj = mListObjects.begin();
	const vector<CObject*>::const_iterator itObjEnd = mListObjects.end();
	while(itObj != itObjEnd)
	{
		SAFE_DELETE(*itObj);
		++itObj;
	}
	mListObjects.clear();

	vector<CPlanet*>::iterator itPlanet = mListPlanets.begin();
	const vector<CPlanet*>::const_iterator itPlanetEnd = mListPlanets.end();
	while(itPlanet != itPlanetEnd)
	{
		SAFE_DELETE(*itPlanet);
		++itPlanet;
	}
	mListPlanets.clear();

	vector<CSpeedWay*>::iterator itSpeedWay = mListSpeedWays.begin();
	const vector<CSpeedWay*>::const_iterator itSpeedWayEnd = mListSpeedWays.end();
	while(itSpeedWay != itSpeedWayEnd)
	{
		SAFE_DELETE(*itSpeedWay);
		++itSpeedWay;
	}
	mListSpeedWays.clear();

	vector<CGroupData*>::iterator itgroupData = mPatrols.begin();
	const vector<CGroupData*>::const_iterator itgroupDataEnd = mPatrols.end();
	while(itgroupData != itgroupDataEnd)
	{
		SAFE_DELETE(*itgroupData);
		++itgroupData;
	}
	mPatrols.clear();

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

void CSpawnManager::Update(float dt, const Vector2D& camPos)
{
	if(mListObjects.empty())
		return;

	const Vector2D& selfPos = camPos;

	// update des patrouilles
	vector<CGroupData*>::const_iterator itP = mPatrols.begin();
	const vector<CGroupData*>::const_iterator itPEnd = mPatrols.end();
	while(itP != itPEnd)
	{
		CGroupData* data = *itP;

		float dist2 = (data->mSpawnPoint - selfPos).Length2();
		float dist2Test = data->mPatrolPointRadius+data->mSpawnPointRadius+PLAYER_MAX_SIGHT;
		dist2Test *= dist2Test; 
		if(dist2 < dist2Test)
			SpawnGroup(data);// spawn le groupe s'il ne l'est pas déjà
		else
			UnspawnGroup(data);// despawn le groupe s'il ne l'est pas déjà

		++itP;
	}	

	// on vide l'ancienne liste d'objets visibles 
	mActiveObjects.clear();
	
	// on charge la nouvelle
	vector<CObject*>::iterator it = mListObjects.begin();
	vector<CObject*>::const_iterator itEnd = mListObjects.end();
	while(it != itEnd)
	{
		CObject* obj = *it;
		if(obj->IsUnspawned())
		{
			it = mListObjects.erase(it);
			itEnd = mListObjects.end();
			SAFE_DELETE(obj);
			continue;
		}

		if(!obj->IsDocked() && obj->WantToDock())// docking request : 
		{
			vector<CSpeedWay*>::const_iterator itSW = mListSpeedWays.begin();
			const vector<CSpeedWay*>::const_iterator itSWEnd = mListSpeedWays.end();
			while(itSW != itSWEnd)
			{
				CSpeedWay* sw = *itSW;
				if(Distance2FromSegment(obj->GetCenterPosition(), sw->GetStart(), sw->GetEnd()) < 2500.0f)
				{
					sw->DockShip(obj, true);
					obj->UnloadPhysic();// on décharge la physique
					obj->Dock(true);
					break;
				}
				++itSW;
			}
			obj->RequestDock(false);
		}
		else if(obj->IsDocked() && obj->WantToDock())// undocking request : 
		{
			vector<CSpeedWay*>::const_iterator itSW = mListSpeedWays.begin();
			const vector<CSpeedWay*>::const_iterator itSWEnd = mListSpeedWays.end();
			while(itSW != itSWEnd)
			{
				CSpeedWay* sw = *itSW;
				sw->DockShip(obj, false);
				++itSW;
			}
			obj->Dock(false);
			obj->RequestDock(false);
		}

		float dist2 = (obj->GetOriginPosition() - selfPos).Length2();
		if(dist2 < 90000.0f)// dist<300 : on est dans le champ de vision
		{
			mActiveObjects.push_back(obj);
			if(!obj->IsDocked() && !obj->IsLanded())
				obj->LoadPhysic();// on charge la physique
		}
		else
		{
			if(!obj->IsDocked())
			{
				obj->UnloadPhysic();// on décharge la physique
			}
		}
		++it;
	}
}

void CSpawnManager::AddGroup(unsigned int nbShips)
{
	CResourceManager *resMgr = CResourceManager::GetInstance();

	CGroupData* data = new CGroupData;
	mPatrols.push_back(data);
	data->mNbShips = nbShips;
	data->mSpawnPoint = Vector2D(2000.0f, 4000.0f);
	data->mSpawnPointRadius = 1000.0f;

	//SpawnGroup(data);
}

void CSpawnManager::SpawnGroup(CGroupData* data)
{
	if(!data || data->mIsSpawned)
		return;

	//PspAssert(false&&"Group spawned");

	CResourceManager *resMgr = CResourceManager::GetInstance();

	Vector2D spawnPoint = Vector2D(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f));
	spawnPoint.Normalize();
	spawnPoint = (Random(0.0f, 1.0f)*data->mSpawnPointRadius) * spawnPoint + data->mSpawnPoint;

	CSquareShipAI *ai = new CSquareShipAI(this, data);
	PspAssert(ai);

	data->m_listShips.clear();


	for(unsigned int i=0; i<data->mNbShips; i++)
	{
		CSquareShip *ship = new CSquareShip(mWorld, mMissilesPt);
		PspAssert(ship);
		ship->Create(3);
		ship->LoadShape(resMgr->mListShipsDatas[data->mSSDId], resMgr->mListTiles);
		ship->SetAI(ai);
		ship->SetPosition(spawnPoint);
		ai->AddOwner(ship);
		AddObject(ship);
		data->m_listShips.push_back(ship);
	}

	data->mIsSpawned = true;
}

void CSpawnManager::UnspawnGroup(CGroupData* data)
{
	if(!data || !data->mIsSpawned)
		return;

	//PspAssert(false&&"Group unspawned");

	list<CSquareShip*>::const_iterator it = data->m_listShips.begin();
	const list<CSquareShip*>::const_iterator itEnd = data->m_listShips.end();
	while(it != itEnd)
	{
		CSquareShip *ship = *it;
		PspAssert(ship);
		ship->Unspawn();// on le marque comme unspawned pour le supprimer de la liste des objets

		++it;
	}

	data->m_listShips.clear();
	data->mIsSpawned = false;
}

// Serialization/deserialization

bool CSpawnManager::ReadSectorTxt(unsigned int num)
{
	if(!mListPlanets.empty())
		return false;

	char name[30];
	sprintf(name, "Res/sector%03d.txt", num);
	ifstream fichier(name, ios::in);  // on ouvre en lecture

	if(fichier)  // si l'ouverture a fonctionné
	{
		string line;
		string key;

		// planetes
		getline(fichier, line);
		key = SplitString(line, "=", true);
		if(key != "NbPlanets")
			return false;
		int nbPlanets = (int)atoi(line.c_str());

		for(int i=0; i<nbPlanets; i++)
		{
			getline(fichier, line);
			if(line == "")
			{
				--i;
				continue;
			}
			key = SplitString(line, "=", true);
			if(key != "Name")
				return false;
			string name = line;

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "Size")
				return false;
			float size = (float)atof(line.c_str());

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "IdTexturePlanet")
				return false;
			int idTexPlanet = (int)atoi(line.c_str());

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "IdTextureClouds")
				return false;
			int idTexClouds = (int)atoi(line.c_str());

			CPlanet* planet = new CPlanet(name, size, idTexPlanet, idTexClouds);

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "Alignment")
				return false;
			planet->SetAlignment((int)atoi(line.c_str()));

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "PositionX")
				return false;
			float X = (float)atof(line.c_str());
			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "PositionY")
				return false;
			float Y = (float)atof(line.c_str());
			planet->SetOriginPosition(Vector2D(X,Y));

			PIXEL_TYPE c;

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "ColorTexturePlanet")
				return false;
			c = DeserializeColor(strtoul(line.c_str(), NULL, 16));
			planet->SetPlanetColor(c);

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "ColorTextureClouds")
				return false;
			c = DeserializeColor(strtoul(line.c_str(), NULL, 16));
			planet->SetCloudsColor(c);

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "ColorTextureShadows")
				return false;
			c = DeserializeColor(strtoul(line.c_str(), NULL, 16));
			planet->SetShadowsColor(c);

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "ColorTextureLights")
				return false;
			c = DeserializeColor(strtoul(line.c_str(), NULL, 16));
			planet->SetLightsColor(c);

			AddPlanet(planet);

		}

		// speedways
		getline(fichier, line);
		while(line == "")
			getline(fichier, line);
		key = SplitString(line, "=", true);
		if(key != "NbSpeedWays")
			return false;
		int nbSpeedWays = (int)atoi(line.c_str());

		for(int i=0; i<nbSpeedWays; i++)
		{
			getline(fichier, line);
			while(line == "")
			{
				getline(fichier, line);
			}
			key = SplitString(line, "=", true);
			if(key != "IdPlanet1")
				return false;
			unsigned int idPlanet1 = (unsigned int)atoi(line.c_str());

			getline(fichier, line);
			key = SplitString(line, "=", true);
			if(key != "IdPlanet2")
				return false;
			unsigned int idPlanet2 = (unsigned int)atoi(line.c_str());

			if(idPlanet1 < mListPlanets.size() && idPlanet2 < mListPlanets.size())
			{
				CSpeedWay* sw = new CSpeedWay(mListPlanets[idPlanet1], mListPlanets[idPlanet2]);
				AddSpeedWay(sw);
			}
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

bool CSpawnManager::WriteSectorRes(unsigned int num)
{
	char name[30];
	sprintf(name, "Res/sector%03d.res", num);
	fstream fichier(name, ios::out | ios::binary);  // on ouvre ecriture binaire

	if(fichier)  // si l'ouverture a fonctionné
	{
		// planetes
		vector<CPlanet*>::const_iterator it = mListPlanets.begin();
		const vector<CPlanet*>::const_iterator itEnd = mListPlanets.end();

		char nbPlanets = mListPlanets.size();
		fichier.write((char*)&nbPlanets, sizeof(char));

		while(it != itEnd)  // tant que l'on peut mettre la ligne dans "contenu"
		{
			if(!(*it))
				continue;

			string name = (*it)->GetName();
			int lenName = name.size();
			fichier.write((char*)&lenName, sizeof(int));
			fichier.write((char*)(name.c_str()), lenName*sizeof(char));
			
			float size = (*it)->GetSize();
			fichier.write((char*)&size, sizeof(float));

			int idTexPlanet = (*it)->GetIdTexPlanet();
			fichier.write((char*)&idTexPlanet, sizeof(char));

			int idTexClouds = (*it)->GetIdTexClouds();
			fichier.write((char*)&idTexClouds, sizeof(char));

			int alignment = (*it)->GetAlignment();
			fichier.write((char*)&alignment, sizeof(char));
			
			Vector2D pos = (*it)->GetOriginPosition();
			fichier.write((char*)&(pos.x), sizeof(float));
			fichier.write((char*)&(pos.y), sizeof(float));
			
			PIXEL_TYPE color;

			color = SerializeColor((*it)->GetPlanetColor());
			fichier.write((char*)&color, sizeof(PIXEL_TYPE));
			
			color = SerializeColor((*it)->GetCloudsColor());
			fichier.write((char*)&color, sizeof(PIXEL_TYPE));
			
			color = SerializeColor((*it)->GetShadowsColor());
			fichier.write((char*)&color, sizeof(PIXEL_TYPE));
			
			color = SerializeColor((*it)->GetLightsColor());
			fichier.write((char*)&color, sizeof(PIXEL_TYPE));
			
			++it;
		}

		// speedways
		vector<CSpeedWay*>::const_iterator itSW = mListSpeedWays.begin();
		const vector<CSpeedWay*>::const_iterator itSWEnd = mListSpeedWays.end();

		char nbSpeedWays = mListSpeedWays.size();
		fichier.write((char*)&nbSpeedWays, sizeof(char));

		while(itSW != itSWEnd)  // tant que l'on peut mettre la ligne dans "contenu"
		{
			if(!(*itSW))
				continue;

			CPlanet* planet1 = (*itSW)->GetPlanet(0);
			CPlanet* planet2 = (*itSW)->GetPlanet(1);
			unsigned char id1 = 0xff, id2 = 0xff;
			it = mListPlanets.begin();
			unsigned char i = 0;
			while(it != itEnd)  // tant que l'on peut mettre la ligne dans "contenu"
			{
				if(*it == planet1)
					id1 = i;
				if(*it == planet2)
					id2 = i;
				++it;
				++i;
			}
			fichier.write((char*)&id1, sizeof(char));
			fichier.write((char*)&id2, sizeof(char));

			++itSW;
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

bool CSpawnManager::ReadSectorRes(unsigned int num)
{
	char name[30];
	sprintf(name, "Res/sector%03d.res", num);
	fstream fichier(name, ios::in | ios::binary);  // on ouvre en lecture binaire

	if(fichier)  // si l'ouverture a fonctionné
	{
		if(!mListPlanets.empty())
			return false;
		
		// planetes
		int nbPlanets = 0;
		fichier.read((char*)&nbPlanets, sizeof(char));
		for(int i=0; i<nbPlanets; i++)
		{
			int lenName = 0;
			fichier.read((char*)&lenName, sizeof(int));
			char *cName = new char[lenName+1];
			cName[lenName] = '\0';
			fichier.read((char*)cName, lenName*sizeof(char));
			string name = cName;
			SAFE_DELETE(cName);

			float size = 0.0f;
			fichier.read((char*)&size, sizeof(float));

			int idTexPlanet = 0;
			fichier.read((char*)&idTexPlanet, sizeof(char));

			int idTexClouds = 0;
			fichier.read((char*)&idTexClouds, sizeof(char));

			CPlanet* planet = new CPlanet(name, size, idTexPlanet, idTexClouds);

			int alignment = 0;
			fichier.read((char*)&alignment, sizeof(char));
			planet->SetAlignment(alignment);

			float X = 0.0f;
			fichier.read((char*)&X, sizeof(float));
			float Y = 0.0f;
			fichier.read((char*)&Y, sizeof(float));
			planet->SetOriginPosition(Vector2D(X,Y));

			PIXEL_TYPE color;

			fichier.read((char*)&color, sizeof(PIXEL_TYPE));
			planet->SetPlanetColor(DeserializeColor(color));

			fichier.read((char*)&color, sizeof(PIXEL_TYPE));
			planet->SetCloudsColor(DeserializeColor(color));

			fichier.read((char*)&color, sizeof(PIXEL_TYPE));
			planet->SetShadowsColor(DeserializeColor(color));

			fichier.read((char*)&color, sizeof(PIXEL_TYPE));
			planet->SetLightsColor(DeserializeColor(color));

			AddPlanet(planet);
		}

		// speedways
		int nbSpeedWays = 0;
		fichier.read((char*)&nbSpeedWays, sizeof(char));
		for(int i=0; i<nbSpeedWays; i++)
		{
			unsigned char id1 = 0xff;
			fichier.read((char*)&id1, sizeof(char));
			unsigned char id2 = 0xff;
			fichier.read((char*)&id2, sizeof(char));

			if((unsigned int)id1 < mListPlanets.size() && (unsigned int)id2 < mListPlanets.size())
			{
				CSpeedWay* sw = new CSpeedWay(mListPlanets[id1], mListPlanets[id2]);
				AddSpeedWay(sw);
			}
			else
				return false;
		}
		fichier.close();
	}
	else
		return false;

	return true;
}

