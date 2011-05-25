//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#include "CResourceManager.h"
#include "utils.h"

#include <hge/hgedistort.h>

CResourceManager* CResourceManager::mInstance = NULL;

hgeDistortionMesh* CreatePlanetMesh(int rows, int cols)
{
	float cellW =(float)(PLANET_TEXTURE_WIDTH/(cols-1));
	float cellH=(float)(PLANET_TEXTURE_HEIGHT/(rows-1));

	hgeDistortionMesh* mesh = new hgeDistortionMesh(cols, rows);

	float dx = (float)(cellW*(cols-1))*0.5f;
	float dy = (float)(cellH*(rows-1))*0.5f;

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			float y = i*cellH-dy;
			y = sin(y/dy*M_PI_2)*dy;
			float x = j*cellW -dx;
			x = sin(x/dx*M_PI_2)*dx;
			float a = abs(x);
			float b = dy;
			if(x < 0.0f)
				x = -a*sqrt(1.0f-y*y/(b*b));
			else if (x > 0.0f)
				x = a*sqrt(1.0f-y*y/(b*b));
			else
				x = 0.0f;

			mesh->SetDisplacement(j,i,x,y,HGEDISP_CENTER);
		}
	}

	return mesh;
}

CResourceManager::CResourceManager()
{
	JRenderer* renderer = JRenderer::GetInstance();

	mSquareTilesTex = renderer->LoadTexture("SquareTiles.png", TEX_TYPE_USE_VRAM);
	mParticlesTex = renderer->LoadTexture("Particles.png", TEX_TYPE_USE_VRAM);
	mLightningTex = renderer->LoadTexture("nrj10anims18x16.png", TEX_TYPE_USE_VRAM);

	int tileSize = SQUARETILE_SIZE*3;
	mSquareTilesQuads = new JQuad*[2*NB_SQUARETILES_QUADS];
	for(int i=0; i<NB_SQUARETILES_QUADS; i++)
	{
		mSquareTilesQuads[i] = new JQuad(mSquareTilesTex, i*tileSize+1.0f, 1.0f, tileSize-2.0f, tileSize-2.0f);
		mSquareTilesQuads[i]->SetHotSpot((float)(tileSize-2)*0.5f,(float)(tileSize-2)*0.5f);
		mSquareTilesQuads[i]->SetColor(ARGB(255, 255, 255, 255));
	}
	for(int i=0; i<NB_SQUARETILES_QUADS; i++)
	{
		mSquareTilesQuads[NB_SQUARETILES_QUADS+i] = new JQuad(mSquareTilesTex, i*tileSize+1.0f, tileSize+1.0f, tileSize-2.0f, tileSize-2.0f);
		mSquareTilesQuads[NB_SQUARETILES_QUADS+i]->SetHotSpot((float)(tileSize-2)*0.5f,(float)(tileSize-2)*0.5f);
		mSquareTilesQuads[NB_SQUARETILES_QUADS+i]->SetColor(ARGB(255, 255, 255, 255));
	}

	mLightningQuads = new JQuad*[NB_LIGHTNING_FRAMES];
	for(int i=0; i<NB_LIGHTNING_FRAMES; i++)
	{
		mLightningQuads[i] = new JQuad(mLightningTex, 18*i+0.5f, 0.5f, 18-1, 16-1);
		mLightningQuads[i]->SetHotSpot((18-1)>>1, (16-1)>>1);
	}

	mParticlesQuads = new JQuad*[NB_PARTICLE_TEXTURES];
	for(int i=0; i<NB_PARTICLE_TEXTURES; i++)
		mParticlesQuads[i] = NULL;


	mPlanetMesh12 = CreatePlanetMesh(12, 12);
	mPlanetMesh16 = CreatePlanetMesh(16, 16);
	mPlanetMesh20 = CreatePlanetMesh(20, 20);

	mPlanetTex = renderer->LoadTexture("planet.png");
	mCloudsTex = renderer->LoadTexture("clouds.png");
	mShadowsTex = renderer->LoadTexture("shadows.png");
	mLightsTex = renderer->LoadTexture("lights.png");


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

CResourceManager::~CResourceManager()
{
	SAFE_DELETE(mLightningTex);
	SAFE_DELETE(mParticlesTex);
	SAFE_DELETE(mSquareTilesTex);

	for(int i=0; i<2*NB_SQUARETILES_QUADS; i++)
		SAFE_DELETE(mSquareTilesQuads[i]);
	SAFE_DELETE_ARRAY(mSquareTilesQuads);

	for(int i=0; i<NB_LIGHTNING_FRAMES; i++)
		SAFE_DELETE(mLightningQuads[i]);
	SAFE_DELETE_ARRAY(mLightningQuads);

	for(int i=0; i<NB_PARTICLE_TEXTURES; i++)
		SAFE_DELETE(mParticlesQuads[i]);
	SAFE_DELETE_ARRAY(mParticlesQuads);

	SAFE_DELETE(mPlanetMesh20);
	SAFE_DELETE(mPlanetMesh16);
	SAFE_DELETE(mPlanetMesh12);

	SAFE_DELETE(mPlanetTex);
	SAFE_DELETE(mCloudsTex);
	SAFE_DELETE(mShadowsTex);
	SAFE_DELETE(mLightsTex);

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


JQuad* CResourceManager::GetParticlesQuad(int num)
{
	if(num<0 || num>=NB_PARTICLE_TEXTURES)
		return NULL;

	if(!mParticlesQuads[num])
	{
		int texSizePcl = 32;
		mParticlesQuads[num] = new JQuad(mParticlesTex, (num%4)*texSizePcl+0.5f, (int(num/4))*texSizePcl+0.5f, texSizePcl-1.0f, texSizePcl-1.0f);
		mParticlesQuads[num]->SetHotSpot((float)((texSizePcl-1)/2),(float)((texSizePcl-1)/2));
		mParticlesQuads[num]->SetColor(ARGB(255, 255, 255, 255));
	}
	
	return mParticlesQuads[num];
}


// ressources internes

bool CResourceManager::ReadTilesTxt()
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
					CSquareTileHull *t = new CSquareTileHull(Vector2D(0,0));
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
					CSquareTileEngine *t = new CSquareTileEngine(Vector2D(0,0));
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
					CSquareTileGun *t = new CSquareTileGun(Vector2D(0,0));
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
					CSquareTileMissile *t = new CSquareTileMissile(Vector2D(0,0));
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
					CSquareTileMine *t = new CSquareTileMine(Vector2D(0,0));
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

bool CResourceManager::WriteTilesRes()
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

bool CResourceManager::ReadTilesRes()
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
					CSquareTileHull *t = new CSquareTileHull(Vector2D(0,0));
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
					CSquareTileEngine *t = new CSquareTileEngine(Vector2D(0,0));
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
					CSquareTileGun *t = new CSquareTileGun(Vector2D(0,0));
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
					CSquareTileMissile *t = new CSquareTileMissile(Vector2D(0,0));
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
					CSquareTileMine *t = new CSquareTileMine(Vector2D(0,0));
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

bool CResourceManager::ReadTilesDesc()
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

bool CResourceManager::ReadShipsTxt()
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

bool CResourceManager::WriteShipsRes()
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

bool CResourceManager::ReadShipsRes()
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

bool CResourceManager::ReadShipsDesc()
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