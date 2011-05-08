#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CSquareShip.h"
#include "CResourceManager.h"

#include <hge\hgeparticle.h>

CSquareShip::CSquareShip(b2World* world, list<CMissile*> &missilesPt):
	CObject(world), mMissilesPt(missilesPt)
{
	mQuadPcl = NULL;
	mLightningQuads = NULL;

	mQuad = NULL;

	mEnginePS = NULL;
	mSquareTiles = NULL;

	mAI = NULL;

	mDatas = NULL;
}


CSquareShip::~CSquareShip()
{
	SAFE_DELETE(mEnginePS);

	if(mSquareTiles)
	{
		for(int i=0; i<mSize*mSize; i++)
			SAFE_DELETE(mSquareTiles[i]);
		SAFE_DELETE_ARRAY(mSquareTiles)
	}

	SAFE_DELETE(mAI);
}


void CSquareShip::Create(int size)
{
	mSize = size;
	
	CResourceManager* resMgr = CResourceManager::GetInstance();
	mQuadPcl = resMgr->GetParticlesQuad(0);
	mLightningQuads = resMgr->GetLightningQuads();

	mQuad = resMgr->GetSquareTilesQuads();

	mOriginPosition = b2Vec2(10.0f*b2Random(-200.0f, 200.0f), 10.0f*b2Random(-200.0f, 200.0f));
	mCenterPosition = mOriginPosition;
	mRotation = 0.0f;
	mRotationMatrix.Set(mRotation);
	mLinearVelocity = b2Vec2(0.0f, 0.0f);
	mAngularVelocity = 0.0f;

	mEnginePower = 0.0f;

	mAngularAcceleration = 80000.0f/32.0f/32.0f;
	mAngularPower = 0.0f;

	mStopEngine = false;

	AddEnginePS(hgeVector(0,0));
	SetMissileParticleSystem();

	mDestroyed = false;

	mDashRTimer = 0.0f;
	mDashLTimer = 0.0f;
}

CSquareTile* CSquareShip::SetSquareTile(CSquareTile* squareTile, int slot)
{
	if(slot<0 || slot>=mSize*mSize)
		return NULL;

	CSquareTile* tile = mSquareTiles[slot];
	if(tile)
	{
		switch(tile->GetType())
		{
		case CSquareTile::HULL:
			break;
		case CSquareTile::ENGINE:
			mlistSTEngine.remove((CSquareTileEngine*)tile);
			break;
		case CSquareTile::GUN:;
			mlistSTGun.remove((CSquareTileGun*)tile);
			break;
		case CSquareTile::MISSILE:
			mlistSTMissile.remove((CSquareTileMissile*)tile);
			break;
		case CSquareTile::MINE:
			mlistSTMine.remove((CSquareTileMine*)tile);
			break;
		default:
			break;
		}
		//SAFE_DELETE(tile);
	}
	CSquareTile* lastTile = tile;
	if(lastTile)
		lastTile->SetEquipped(false);

	mSquareTiles[slot] = squareTile;
	tile = mSquareTiles[slot];
	if(!tile)
		return lastTile;

	tile->SetEquipped(true);
	float mid = (float)(mSize>>1);
	b2Vec2 pos = b2Vec2(((slot%mSize)-mid)*SQUARETILE_SIZE, (mid-(slot/mSize))*SQUARETILE_SIZE);
	tile->SetPosition(pos);

	switch(tile->GetType())
	{
	case CSquareTile::HULL:
		break;
	case CSquareTile::ENGINE:
		mlistSTEngine.push_back((CSquareTileEngine*)tile);
		break;
	case CSquareTile::GUN:;
		mlistSTGun.push_back((CSquareTileGun*)tile);
		break;
	case CSquareTile::MISSILE:
		mlistSTMissile.push_back((CSquareTileMissile*)tile);
		break;
	case CSquareTile::MINE:
		mlistSTMine.push_back((CSquareTileMine*)tile);
		break;
	default:
		break;
	}
	AddExplosionPS(tile);

	return lastTile;
}

void CSquareShip::LoadShape(CSquareShipData* datas, const vector<CSquareTile*>& listTiles)
{
	UnloadShape();

	mDatas = datas;
	mSize = mDatas->mSize;
	
	int size2 = mSize*mSize;

	mSquareTiles = new CSquareTile*[size2];

	// on cr�e les listes des positions des moteurs
	float mid = (float)(mSize>>1);
	for(int i=0;i<size2;i++)
	{
		b2Vec2 pos = b2Vec2(((i%mSize)-mid)*SQUARETILE_SIZE, (mid-(i/mSize))*SQUARETILE_SIZE);

		u32 id = mDatas->mTilesId[i];
		CSquareTile* tileRef = listTiles[id];
		if(!tileRef)
		{
			mSquareTiles[i] = NULL;
			continue;
		}
		switch(tileRef->GetType())
		{
		case CSquareTile::HULL:
			{
				CSquareTileHull* hull = new CSquareTileHull((CSquareTileHull*)tileRef);
				mSquareTiles[i] = hull;
				break;
			}
		case CSquareTile::ENGINE:
			{
				CSquareTileEngine* engine = new CSquareTileEngine((CSquareTileEngine*)tileRef);
				mSquareTiles[i] = engine;
				mlistSTEngine.push_back(engine);
				break;
			}
		case CSquareTile::GUN:
			{
				CSquareTileGun* gun = new CSquareTileGun((CSquareTileGun*)tileRef);
				mlistSTGun.push_back(gun);
				mSquareTiles[i] = gun;
				break;
			}
		case CSquareTile::MISSILE:
			{
				CSquareTileMissile* missile = new CSquareTileMissile((CSquareTileMissile*)tileRef);
				mSquareTiles[i] = missile;
				mlistSTMissile.push_back(missile);
				break;
			}
		case CSquareTile::MINE:
			{
				CSquareTileMine* mine = new CSquareTileMine((CSquareTileMine*)tileRef);
				mSquareTiles[i] = mine;
				mlistSTMine.push_back(mine);
				break;
			}
		default:
			mSquareTiles[i] = NULL;
			break;
		}
		if(mSquareTiles[i])
		{
			AddExplosionPS(mSquareTiles[i]);
			mSquareTiles[i]->SetEquipped(true);
			mSquareTiles[i]->SetPosition(pos);
		}
	}

	// Define the dynamic body. We set its position,
	// add the box shape, and call the body factory.
	//LoadPhysic();
}

void CSquareShip::UnloadShape()
{
	int size2 = mSize*mSize;

	// on supprime la physique
	//UnloadPhysic();

	if(mSquareTiles)// s'il y en avait on supprime les anciennes tiles
	{
		for(int i=0;i<size2;i++)
			SAFE_DELETE(mSquareTiles[i]);
	}
	SAFE_DELETE_ARRAY(mSquareTiles);
	mSize = 0;

	mlistSTEngine.clear();
	mlistSTGun.clear();
	mlistSTMissile.clear();
	mlistSTMine.clear();

	mDatas = NULL;
}

void CSquareShip::LoadPhysic()
{
	if(mBody)// si on a deja la physique charg�e on quitte direct (on fera unload avant si on veut la recharger)
		return;

	int size2 = mSize*mSize;

	// Define the dynamic body. We set its position,
	// add the box shape, and call the body factory.
	b2BoxDef *boxDef = new b2BoxDef[size2];
	b2BodyDef bodyDef;
	CSquareTile ** tiles = mSquareTiles;
	for(int i=0; i<size2; i++)
	{
		if(*tiles)
		{
			float32 halfSide = SQUARETILE_SIZE>>1;
			boxDef[i].extents.Set(halfSide, halfSide);
			boxDef[i].density = 0.5f;						// Set the box density to be non-zero, so it will be dynamic.
			boxDef[i].friction = 0.3f;						// Override the default friction.

			const b2Vec2& pos = (*tiles)->GetPosition();
			boxDef[i].localPosition.Set(pos.x, pos.y);
			boxDef[i].localRotation = 0.0f;
			bodyDef.AddShape(&boxDef[i]);
		}
		tiles++;
	}

	bodyDef.position = mOriginPosition;
	bodyDef.rotation = mRotation;
	bodyDef.angularDamping = 0.7f;
	bodyDef.linearDamping = 0.3f;
	bodyDef.linearVelocity = mLinearVelocity;
	bodyDef.angularVelocity = mAngularVelocity;

	mBody = mWorld->CreateBody(&bodyDef);
	//mBody->m_mass = 1.0f;


	SAFE_DELETE_ARRAY(boxDef);

	mOriginPosition = mBody->GetOriginPosition();
	mCenterPosition = mBody->GetCenterPosition();
	mLinearVelocity = mBody->GetLinearVelocity();
	mRotation = mBody->GetRotation();
	mRotationMatrix = mBody->GetRotationMatrix();
	mAngularVelocity = mBody->GetAngularVelocity();
}

void CSquareShip::UnloadPhysic()
{
	// on supprime la physique
	if(mBody)
	{
		mWorld->DestroyBody(mBody);
		mBody = NULL;
	}
}

void CSquareShip::Render(const b2Vec2& camPos, const float32& camRot)
{
	// si ce n'est pas affich� � l'�cran on skip
	if((camPos-mOriginPosition).Length() > 300.0f)
		return;

	JRenderer* renderer = JRenderer::GetInstance();

	mEnginePS->info.fDirection = mRotation+M_PI_4;

	// engines + trails
	list<CSquareTileEngine*>::iterator itEngine = mlistSTEngine.begin();
	list<CSquareTileEngine*>::iterator itEngineEnd = mlistSTEngine.end();
	while(itEngine != itEngineEnd)
	{
		list<b2Vec2>& listPos = (*itEngine)->mListPos;
		if((*itEngine)->GetCurrentLife() > 0 && !listPos.empty())
		{
			if(!mStopEngine)
			{
				b2Vec2 position = listPos.front();//derni�re position en date
				mEnginePS->Transpose(position.x, position.y);
				mEnginePS->RenderLocal(hgeVector(camPos.x, camPos.y), camRot);
			}

			list<b2Vec2>::iterator itPt = listPos.begin();
			list<b2Vec2>::iterator itPtNext = listPos.begin();
			itPtNext++;
			int size = listPos.size();
			int a = 32;
			int da = (size>1)?(a/(size-1)):a;
			b2Mat22 rot(camRot);
			while(itPtNext!=listPos.end())
			{
				b2Vec2 pos1 = b2MulT(rot, *itPt-camPos);
				b2Vec2 pos2 = b2MulT(rot, *itPtNext-camPos);
				renderer->DrawLine(SCREEN_SIZE_X2+pos1.x, SCREEN_SIZE_Y2-pos1.y, 
					SCREEN_SIZE_X2+pos2.x, SCREEN_SIZE_Y2-pos2.y, 2, ARGB(a, 255, 255, 255));
				a -= da;
				itPt++;
				itPtNext++;
			}
		}
		itEngine++;
	}

	
	//renderer->EnableTextureFilter(false);
	for(int i=0; i<mSize*mSize; i++)
	{
		if(!mSquareTiles[i])
			continue;
			
		b2Vec2 trans = b2Mul(mRotationMatrix, mSquareTiles[i]->GetPosition());
		b2Vec2 position = b2MulT(b2Mat22(camRot), 
			mOriginPosition + trans - camPos);
		float32 rotation = mRotation-camRot;

		// tile
		JQuad* quad = NULL;
		if(mSquareTiles[i]->GetCurrentLife() > 0)
		{
			quad = mQuad[(int)mSquareTiles[i]->GetType()];
			float r = mSquareTiles[i]->GetCurrentLife()/mSquareTiles[i]->GetLifePoints();
#ifdef WIN32
			quad->SetColor(ARGB((200+(int)(55*r)), 255, ((int)(255*r)), ((int)(255*r))));
#else
			quad->mBlend=GU_TFX_ADD;
			quad->SetColor(ARGB((200+(int)(55*r)), (200-(int)(200*r)), 0, 0));
#endif
		}
		else
		{
			quad = mQuad[(int)mSquareTiles[i]->GetType()+NB_SQUARETILES_QUADS];
#ifdef WIN32
			quad->SetColor(ARGB(255, 255, 255, 255));
#else
			quad->mBlend=GU_TFX_ADD;
			quad->SetColor(ARGB(0, 0, 0, 0));
#endif
		}
		renderer->RenderQuad(quad, SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, -rotation);

		// �clairs
		int num = (int)(b2Random(0, 1)*20*NB_LIGHTNING_FRAMES);
		if(num<NB_LIGHTNING_FRAMES)
			renderer->RenderQuad(mLightningQuads[num], 
				SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, -rotation);

		// explosion
		hgeParticleSystem *explosionPS = mSquareTiles[i]->GetExplosionPS();
		if(explosionPS)
		{
			b2Vec2 pos = (mOriginPosition + trans);
 			explosionPS->Transpose(pos.x, pos.y);
 			explosionPS->RenderLocal(hgeVector(camPos.x, camPos.y), camRot);
		}
	}	
	//renderer->EnableTextureFilter(true);
}

void CSquareShip::Update(float dt, bool updatePhysic/* = true*/)
{
	if(mAI)
		mAI->Update(dt);

	if(updatePhysic)
	{
		mOriginPosition = mBody->GetOriginPosition();
		mCenterPosition = mBody->GetCenterPosition();
		mLinearVelocity = mBody->GetLinearVelocity();
		mRotation = mBody->GetRotation();
		mRotationMatrix = mBody->GetRotationMatrix();
		mAngularVelocity = mBody->GetAngularVelocity();
	}

	b2Vec2 shipOrigin = mOriginPosition;
	b2Mat22 shipRotation = mRotationMatrix;


	// physique
	if(updatePhysic)
	{
		float mass = mBody->GetMass();

		mBody->WakeUp();

		b2Vec2 p;
		b2Vec2 f;

		// force lin�aire
		if(!mStopEngine)
		{
			mBody->m_linearDamping = 0.7f;
			list<CSquareTileEngine*>::iterator itEngine = mlistSTEngine.begin();
			list<CSquareTileEngine*>::iterator itEngineEnd = mlistSTEngine.end();
			while(itEngine != itEngineEnd)
			{
				if((*itEngine)->GetCurrentLife() > 0)
				{
					// pouss�e moteur
					p = shipOrigin +
						b2Mul(shipRotation, (*itEngine)->GetPosition());
					f = b2Mul(shipRotation, b2Vec2(-1.0f, 1.0f));
					f.Normalize();
					f *= /*(*itEngine)->GetEnginePower()*/mEnginePower * (*itEngine)->GetEngineAcceleration();
					mBody->ApplyForce(f, p);
				}
				itEngine++;
			}

			// frottements
// 			p = mBody->GetCenterPosition();
// 			f = mBody->GetLinearVelocity();
// 			f *= -500.0f;
// 			mBody->ApplyForce(f, p);

			// dash
			if(mDashRTimer < 0.2f)
				Dash(1);
			if(mDashLTimer < 0.2f)
				Dash(-1);
			if(mDashRTimer < 1.0f)
				mDashRTimer += dt;
			if(mDashLTimer < 1.0f)
				mDashLTimer += dt;
		}
		else
			mBody->m_linearDamping = 1.0f;

		// force de rotation
		// pouss�e de rotation
		float32 v;
		v = mass*mass*mAngularPower*mAngularAcceleration;
		mBody->ApplyTorque(v);

		// frottements
// 		v = mBody->GetAngularVelocity();
// 		v *= -500000.0f;
// 		mBody->ApplyTorque(v);

		// missiles collisions
		list<CMissile*>::iterator itMissile = mMissilesPt.begin();
		list<CMissile*>::iterator itMissileEnd = mMissilesPt.end();
		while(itMissile != itMissileEnd)
		{
			ComputeCollision(*itMissile);
			itMissile++;
		}
	}

	mEnginePS->info.fDirection = mRotation + M_PI_4;
	mEnginePS->info.fSpeedMax = 0.0f+mEnginePower*2.0f;
	mEnginePS->info.fSpeedMin = 0.0f+mEnginePower*2.5f;
	mEnginePS->Update(dt);

	mMissilePS->Update(dt);

	// engine trails
	static b2Vec2 delta = b2Vec2((float)SQUARETILE_SIZE*0.5f, -(float)SQUARETILE_SIZE*0.5f);
	list<CSquareTileEngine*>::iterator itEngine = mlistSTEngine.begin();
	list<CSquareTileEngine*>::iterator itEngineEnd = mlistSTEngine.end();
	while(itEngine != itEngineEnd)
	{
		b2Vec2 position = shipOrigin +
			b2Mul(shipRotation, (*itEngine)->GetPosition() + delta);
		list<b2Vec2>& listPos = (*itEngine)->mListPos;
		listPos.push_front(position);
		if(listPos.size() > NB_POINTS_ENGINE_TRAIL)
			listPos.pop_back();
		itEngine++;
	}

	mDestroyed = true;
	// update des squareTiles
	for(int i=0; i<mSize*mSize; i++)
	{
		CSquareTile *tile = mSquareTiles[i];
		if(!tile)
			continue;

		tile->Update(dt);
		if(mDestroyed && tile->GetCurrentLife()>0.0f)// 1ere tile non detruite
			mDestroyed = false;
	}
}

void CSquareShip::FireAt(const b2Vec2& target, float ratioError)
{
	ratioError = max(0.0f, min(1.0f, ratioError));
	static b2Vec2 delta = b2Vec2(-SQUARETILE_SIZE/2, SQUARETILE_SIZE/2);

	b2Vec2 shipFront = b2Mul(mBody->GetRotationMatrix(), b2Vec2(-1.0f, 1.0f));
	shipFront.Normalize();

	list<CSquareTileGun*>::iterator itGun = mlistSTGun.begin();
	list<CSquareTileGun*>::iterator itGunEnd = mlistSTGun.end();
	while(itGun != itGunEnd)
	{
		if((*itGun)->GetCurrentLife() > 0)
		{
			if((*itGun)->Fire())
			{
				b2Vec2 pos = mBody->GetOriginPosition() +
					b2Mul(mBody->GetRotationMatrix(), (*itGun)->GetPosition() + delta);

				b2Vec2 dir = target - pos;
				dir.Normalize();
				float ratio = b2Random(-ratioError, ratioError);
				dir = (1.0f-abs(ratio)) * dir + ratio * b2Vec2(-dir.y, dir.x);
				dir.Normalize();
				// on tire seulement si on vise un objet dans le demi cercle face au canon
				if(b2Dot(dir, shipFront)>=0.0f)
				{
					dir *= (*itGun)->GetMissileSpeed();
					dir += mBody->GetLinearVelocity();

					CMissile *missile = new CMissile(1.0f, mQuadPcl, this, (*itGun)->GetHullDammages());
					missile->SetPosition(pos);
					missile->SetVelocity(dir);

					missile->AddPS(mMissilePS);

					mMissilesPt.push_back(missile);				
				}
			}
		}
		itGun++;
	}
}

void CSquareShip::AddEnginePS(const hgeVector& pos)
{
	SAFE_DELETE(mEnginePS);

	hgeParticleSystemInfo info;
	info.bRelative = false;
	info.colColorEnd = hgeColor(0, 0, 0.8f, 0);
	info.colColorStart = hgeColor(0.5f, 0.5f, 1.0f, 1);
	info.fAlphaVar = 0.0f;
	info.fColorVar = 0.0f;
	info.fDirection = 0.0f;
	info.fGravityMax = 0.0f;
	info.fGravityMin = 0.0f;
	info.fLifetime = -1.0f;
	info.fParticleLifeMax = 0.2f;
	info.fParticleLifeMin = 0.16f;
	info.fRadialAccelMax = 0.0f;
	info.fRadialAccelMin = 0.0f;
	info.fSizeEnd = 0.2f;
	info.fSizeStart = 0.23f;
	info.fSizeVar = -0.03f;
	info.fSpeedMax = 2.5f;
	info.fSpeedMin = 2.0f;
	info.fSpinEnd = 0.0f;
	info.fSpinStart = 0.0f;
	info.fSpinEnd = 0.0f;
	info.fSpinVar = 0.0f;
	info.fSpread = 0.0f;
	info.fTangentialAccelMax = 0.0f;
	info.fTangentialAccelMin = 0.0f;
	info.nEmission = 70;
	info.sprite = mQuadPcl;

	mEnginePS = new hgeParticleSystem(&info);

	mEnginePS->Fire();
}

void CSquareShip::AddExplosionPS(CSquareTile* squareTile)
{
	if(!squareTile)
		return;

	hgeParticleSystemInfo info;
	info.bRelative = false;
	//info.colColorEnd = hgeColor(0, 0, 1, 0);
	//info.colColorStart = hgeColor(0.9f, 0.2f, 0, 0.8f);
	info.colColorEnd = hgeColor(1, 0.8f, 0, 0);
	info.colColorStart = hgeColor(1, 0, 0, 0.8f);
	info.fAlphaVar = 0.0f;
	info.fColorVar = 0.0f;
	info.fDirection = 0.0f;
	info.fGravityMax = 0.0f;
	info.fGravityMin = 0.0f;
	info.fLifetime = 0.3f;
	info.fParticleLifeMax = 1.3f;
	info.fParticleLifeMin = 1.0f;
	info.fRadialAccelMax = -0.6f*2.0f;
	info.fRadialAccelMin = -0.8f*2.0f;
	info.fSizeEnd = 0.16f*1.5f;
	info.fSizeStart = 0.2f*1.5f;
	info.fSizeVar = -0.1f*1.5f;
	info.fSpeedMax = 0.4f*2.0f;
	info.fSpeedMin = 0.3f*2.0f;
	info.fSpinEnd = 0.0f;
	info.fSpinStart = 0.0f;
	info.fSpinEnd = 0.0f;
	info.fSpinVar = 0.0f;
	info.fSpread = 2*M_PI;
	info.fTangentialAccelMax = 0.3f*2.0f;
	info.fTangentialAccelMin = 0.1f*2.0f;
	info.nEmission = 80;
	info.sprite = mQuadPcl;

	hgeParticleSystem *explosionPS = new hgeParticleSystem(&info);
	explosionPS->Fire();
	explosionPS->Stop();
	squareTile->AddExplosionPS(explosionPS);
}

void CSquareShip::SetMissileParticleSystem()
{
	hgeParticleSystemInfo info;
	info.bRelative = false;
	info.colColorEnd = hgeColor(1.0f, 0.5f, 0.2f, 0.5f);
	info.colColorStart = hgeColor(1.0f, 1.0f, 0.2f, 1);
	info.fAlphaVar = 0.0f;
	info.fColorVar = 0.0f;
	info.fDirection = 0.0f;
	info.fGravityMax = 0.0f;
	info.fGravityMin = 0.0f;
	info.fLifetime = -1.0f;
	info.fParticleLifeMax = 0.2f;
	info.fParticleLifeMin = 0.16f;
	info.fRadialAccelMax = 2.5f;
	info.fRadialAccelMin = 2.0f;
	info.fSizeEnd = 0.20f;
	info.fSizeStart = 0.25f;
	info.fSizeVar = -0.05f;
	info.fSpeedMax = 0.5f;
	info.fSpeedMin = 0.0f;
	info.fSpinEnd = 0.0f;
	info.fSpinStart = 0.0f;
	info.fSpinEnd = 0.0f;
	info.fSpinVar = 0.0f;
	info.fSpread = M_PI*2.0f;
	info.fTangentialAccelMax = 5.5f;
	info.fTangentialAccelMin = 5.0f;
	info.nEmission = 20;
	info.sprite = mQuadPcl;

	mMissilePS = new hgeParticleSystem(&info);
}

void CSquareShip::Straff(float power)
{
	power = max(-1.0f, min(1.0f, power));

	int nbEnginesAlive = 0;
	int nbEngines = mlistSTEngine.size();
	if(nbEngines==0)
		return;

	list<CSquareTileEngine*>::iterator itEngine = mlistSTEngine.begin();
	list<CSquareTileEngine*>::iterator itEngineEnd = mlistSTEngine.end();
	while(itEngine != itEngineEnd)
	{
		if((*itEngine)->GetCurrentLife() > 0)
			nbEnginesAlive++;
		itEngine++;
	}
	if(nbEnginesAlive==0)
		return;

	// pouss�e moteur
	b2Vec2 p = mBody->GetCenterPosition();
	b2Vec2 f = b2Mul(mBody->GetRotationMatrix(), b2Vec2(1.0f, 1.0f));
	f.Normalize();
	f *= power * 1000000.0f;
	mBody->ApplyForce(f, p);
}

void CSquareShip::Dash(float power)
{
	power = max(-1.0f, min(1.0f, power));

	int nbEnginesAlive = 0;
	int nbEngines = mlistSTEngine.size();
	if(nbEngines==0)
		return;

	list<CSquareTileEngine*>::iterator itEngine = mlistSTEngine.begin();
	list<CSquareTileEngine*>::iterator itEngineEnd = mlistSTEngine.end();
	while(itEngine != itEngineEnd)
	{
		if((*itEngine)->GetCurrentLife() > 0)
			nbEnginesAlive++;
		itEngine++;
	}
	if(nbEnginesAlive==0)
		return;

	// pouss�e moteur
	b2Vec2 p = mBody->GetCenterPosition();
	b2Vec2 f = b2Mul(mBody->GetRotationMatrix(), b2Vec2(1.0f, 1.0f));
	f.Normalize();
	f *= power * 150.0f * mBody->GetMass();
	mBody->ApplyImpulse(f, p);
}

b2Vec2 CSquareShip::GetShootPoint(const b2Vec2& pos, const b2Vec2& vel)
{
	b2Vec2 P = mBody->GetCenterPosition();//position initiale du projectile (approx)
	float spdP = GUN_MISSILES_SPEED;

	b2Vec2 E = pos;//position de la cible
	b2Vec2 velE = vel;//vitesse de la cible
	b2Vec2 velE0 = vel - mBody->GetLinearVelocity();//vitesse de la cible dans le r�f�rentiel dynamique du tireur

	//si la cible est � l'arret la position recherch�e est E
	if(velE0.Length() == 0.0f) return  E;

	b2Vec2 PE = E - P;//vecteur PE

	float T1, T2;//nos temps � trouver (on prendra le plus petit des 2 (si positifs))

	// �quation du second degr� AT�+BT+C=0
	float A = velE0.Length2() - spdP*spdP;
	float B = 2.0f * b2Dot(PE, velE0);
	float C = PE.Length2();

	if(A > 0.0f)// si A>0 le projectile est moins rapide que l'ennemi
		return E;

	if(A == 0.0f)
		return E - (C/B) * velE0;

	float Delta = B*B - 4.0f*A*C;// discriminant de l'�quation du 2nd degr�

	T1 = (-B - sqrt(Delta)) / (2.0f * A);
	T2 = (-B + sqrt(Delta)) / (2.0f * A);

	//on prend le T le plus petit positif
	float T;
	if(T1 <= 0.0f) T = T2;
	else if(T2 <= 0.0f) T = T1;
	else T = (T1 <= T2)? T1:T2;

	return E + T * velE0;
}

void CSquareShip::ComputeCollision(CMissile* missile)
{
	if(missile->GetOwner() == this)
		return;
	int size2 = mSize*mSize;
	int squareTileRadius2 = (SQUARETILE_SIZE/2)*(SQUARETILE_SIZE/2);// rayon au carr� du cercle passant par les milieux des c�t�s
	int radius2 = size2*squareTileRadius2*2;// rayon au carr� du cercle passant par les coins diagonaux
	if((missile->GetPosition()-mBody->GetOriginPosition()).Length2() > radius2)
		return;

	b2Vec2 localPos = b2MulT(mBody->GetRotationMatrix(), missile->GetLastPosition() - mBody->GetOriginPosition());
	
	b2Vec2 localTraj = b2MulT(mBody->GetRotationMatrix(), missile->GetPosition() - missile->GetLastPosition());
	float minTrajLength = localTraj.Length();
	localTraj.Normalize();

	bool found = false;
	CSquareTile* hitSquareTile = NULL;

	CSquareTile** squareTile = mSquareTiles;
	for(int i=0; i<size2; i++)
	{
		if((*squareTile))
		{
			if((*squareTile)->GetCurrentLife() > 0)// si la squaretile n'est pas d�truite
			{
				b2Vec2 tileDist = (*squareTile)->GetPosition() - localPos;
				float scal = b2Dot(tileDist, localTraj);
				float dist2 = tileDist.Length2() - scal*scal;
				if(dist2 <= squareTileRadius2)// tile travers�e par le projectile
				{
					if(scal < minTrajLength)// tile la plus proche de la position pr�c�dente
					{
						minTrajLength = scal;
						found = true;
						hitSquareTile = *squareTile;
					}
				}
			}
		}
		squareTile++;
	}

	if(found)// on a alors la plus proche squaretile touch�e par le projectile
	{
		missile->Destroy();
		hitSquareTile->ComputeLife(missile->GetDamages());
		if(hitSquareTile->GetCurrentLife()<=0.0f)// tile d�truite
			hitSquareTile->Explode();
	}
}

