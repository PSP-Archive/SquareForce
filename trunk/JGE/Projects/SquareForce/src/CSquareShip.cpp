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

	mOriginPosition = Vector2D(10.0f*b2Random(-200.0f, 200.0f), 10.0f*b2Random(-200.0f, 200.0f));
	mCenterPosition = mOriginPosition;
	mRotation = 0.0f;
	mRotationMatrix.Set(mRotation);
	mLinearVelocity = Vector2D(0.0f, 0.0f);
	mAngularVelocity = 0.0f;

	mEnginePower = 0.0f;

	mAngularAcceleration = 100.0f;
	mAngularPower = 0.0f;

	mStopEngine = false;

	AddEnginePS(Vector2D(0,0));
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
	Vector2D pos = Vector2D(((slot%mSize)-mid)*SQUARETILE_SIZE, (mid-(slot/mSize))*SQUARETILE_SIZE);
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

	// on crée les listes des positions des moteurs
	float mid = (float)(mSize>>1);
	for(int i=0;i<size2;i++)
	{
		Vector2D pos = Vector2D(((i%mSize)-mid)*SQUARETILE_SIZE, (mid-(i/mSize))*SQUARETILE_SIZE);

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
	if(mBody)// si on a deja la physique chargée on quitte direct (on fera unload avant si on veut la recharger)
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
			float halfSide = SQUARETILE_SIZE>>1;
			boxDef[i].extents.Set(halfSide, halfSide);
			boxDef[i].density = 0.5f;						// Set the box density to be non-zero, so it will be dynamic.
			boxDef[i].friction = 0.3f;						// Override the default friction.

			const Vector2D& pos = (*tiles)->GetPosition();
			boxDef[i].localPosition.Set(pos.x, pos.y);
			boxDef[i].localRotation = 0.0f;
			bodyDef.AddShape(&boxDef[i]);
		}
		tiles++;
	}

	bodyDef.position = popCast(b2Vec2, mOriginPosition);
	bodyDef.rotation = mRotation;
	bodyDef.angularDamping = 0.7f;
	bodyDef.linearDamping = 0.3f;
	bodyDef.linearVelocity = popCast(b2Vec2, mLinearVelocity);
	bodyDef.angularVelocity = mAngularVelocity;

	mBody = mWorld->CreateBody(&bodyDef);
	//mBody->m_mass = 1.0f;


	SAFE_DELETE_ARRAY(boxDef);

	mOriginPosition = popCast(Vector2D, mBody->GetOriginPosition());
	mCenterPosition = popCast(Vector2D, mBody->GetCenterPosition());
	mLinearVelocity = popCast(Vector2D, mBody->GetLinearVelocity());
	mRotation = mBody->GetRotation();
	mRotationMatrix = popCast(Matrix22, mBody->GetRotationMatrix());
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

void CSquareShip::Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat)
{
	// si ce n'est pas affiché à l'écran on skip
	if((camPos-mOriginPosition).Length() > 300.0f)
		return;

	JRenderer* renderer = JRenderer::GetInstance();

	mEnginePS->info.fDirection = mRotation+M_PI_4;

	// engines + trails
	list<CSquareTileEngine*>::const_iterator itEngine = mlistSTEngine.begin();
	const list<CSquareTileEngine*>::const_iterator itEngineEnd = mlistSTEngine.end();
	while(itEngine != itEngineEnd)
	{
		list<Vector2D>& listPos = (*itEngine)->mListPos;
		if((*itEngine)->GetCurrentLife() > 0 && !listPos.empty())
		{
			if(!mStopEngine)
			{
				Vector2D position = listPos.front();//dernière position en date
				mEnginePS->Transpose(position.x, position.y);
				mEnginePS->RenderLocal(Vector2D(camPos.x, camPos.y), camRot, camMat);
			}

			list<Vector2D>::const_iterator itPt = listPos.begin();
			list<Vector2D>::const_iterator itPtNext = itPt;
			++itPtNext;
			const list<Vector2D>::const_iterator itPtEnd = listPos.end();
			int size = listPos.size();
			int a = 32;
			int da = (size>1)?(a/(size-1)):a;
			while(itPtNext != itPtEnd)
			{
				Vector2D pos1 = camMat / (*itPt-camPos);
				Vector2D pos2 = camMat / (*itPtNext-camPos);
				renderer->DrawLine(SCREEN_SIZE_X2+pos1.x, SCREEN_SIZE_Y2-pos1.y, 
					SCREEN_SIZE_X2+pos2.x, SCREEN_SIZE_Y2-pos2.y, 2, ARGB(a, 255, 255, 255));
				a -= da;
				++itPt;
				++itPtNext;
			}
		}
		++itEngine;
	}

	
	//renderer->EnableTextureFilter(false);
	CSquareTile** tiles = mSquareTiles;
	int i = mSize*mSize+1;
	while(--i)
	{
		CSquareTile* tile = *tiles;
		if(tile)
		{
			Vector2D trans = mRotationMatrix * tile->GetPosition();
			Vector2D position = camMat / (mOriginPosition + trans - camPos);
			float rotation = mRotation-camRot;

			// tile
			JQuad* quad = NULL;
			float life = tile->GetCurrentLife();
			if(life > 0)
			{
				quad = mQuad[(int)tile->GetType()];
				float r = life/tile->GetLifePoints();
#ifdef WIN32
				quad->SetColor(ARGB((200+(int)(55*r)), 255, ((int)(255*r)), ((int)(255*r))));
#else
				quad->mBlend=GU_TFX_ADD;
				quad->SetColor(ARGB((200+(int)(55*r)), (200-(int)(200*r)), 0, 0));
#endif
			}
			else
			{
				quad = mQuad[(int)tile->GetType()+NB_SQUARETILES_QUADS];
#ifdef WIN32
				quad->SetColor(ARGB(255, 255, 255, 255));
#else
				quad->mBlend=GU_TFX_ADD;
				quad->SetColor(ARGB(255, 0, 0, 0));
#endif
			}
			renderer->RenderQuad(quad, SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, -rotation);

			// éclairs
			int num = (int)(b2Random(0, 1)*20*NB_LIGHTNING_FRAMES);
			if(num<NB_LIGHTNING_FRAMES)
			{
				mLightningQuads[num]->SetColor(ARGB(255, 255, 255, 255));
				renderer->RenderQuad(mLightningQuads[num], 
				SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, -rotation);
			}

			// explosion
			hgeParticleSystem *explosionPS = tile->GetExplosionPS();
			if(explosionPS)
			{
				Vector2D pos = (mOriginPosition + trans);
				explosionPS->Transpose(pos.x, pos.y);
				explosionPS->RenderLocal(Vector2D(camPos.x, camPos.y), camRot, camMat);
			}
		}

		++tiles;
	}	
	//renderer->EnableTextureFilter(true);
}

void CSquareShip::Update(float dt, bool updatePhysic/* = true*/)
{
	if(mAI)
		mAI->Update(dt);

	if(updatePhysic)
	{
		mOriginPosition = popCast(Vector2D, mBody->GetOriginPosition());
		mCenterPosition = popCast(Vector2D, mBody->GetCenterPosition());
		mLinearVelocity = popCast(Vector2D, mBody->GetLinearVelocity());
		mRotation = mBody->GetRotation();
		mRotationMatrix = popCast(Matrix22, mBody->GetRotationMatrix());
		mAngularVelocity = mBody->GetAngularVelocity();
	}

	Vector2D shipOrigin = mOriginPosition;
	Matrix22 shipRotation = mRotationMatrix;


	// physique
	if(updatePhysic)
	{
		float mass = mBody->GetMass();

		mBody->WakeUp();

		Vector2D p;
		Vector2D f;

		// force linéaire
		if(!mStopEngine)
		{
			mBody->m_linearDamping = 0.7f;
			list<CSquareTileEngine*>::const_iterator itEngine = mlistSTEngine.begin();
			const list<CSquareTileEngine*>::const_iterator itEngineEnd = mlistSTEngine.end();
			while(itEngine != itEngineEnd)
			{
				if((*itEngine)->GetCurrentLife() > 0)
				{
					// poussée moteur
					p = shipOrigin + shipRotation * (*itEngine)->GetPosition();
					f = shipRotation * Vector2D(-1.0f, 1.0f);
					f.Normalize();
					f *= /*(*itEngine)->GetEnginePower()*/mEnginePower * (*itEngine)->GetEngineAcceleration();
					mBody->ApplyForce(popCast(b2Vec2, f), popCast(b2Vec2, p));
				}
				++itEngine;
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
		// poussée de rotation
		float v;
		v = mass*mass*mAngularPower*mAngularAcceleration;
		mBody->ApplyTorque(v);

		// frottements
// 		v = mBody->GetAngularVelocity();
// 		v *= -500000.0f;
// 		mBody->ApplyTorque(v);

		// missiles collisions
		list<CMissile*>::const_iterator itMissile = mMissilesPt.begin();
		const list<CMissile*>::const_iterator itMissileEnd = mMissilesPt.end();
		while(itMissile != itMissileEnd)
		{
			ComputeCollision(*itMissile);
			++itMissile;
		}
	}

	mEnginePS->vDirection = mRotationMatrix*Vector2D(1,-1);
	mEnginePS->info.fSpeedMax = 0.0f+mEnginePower*2.5f;
	mEnginePS->info.fSpeedMin = 0.0f+mEnginePower*2.0f;
	mEnginePS->Update(dt);

	mMissilePS->Update(dt);

	// engine trails
	static Vector2D delta = Vector2D((float)SQUARETILE_SIZE*0.5f, -(float)SQUARETILE_SIZE*0.5f);
	list<CSquareTileEngine*>::iterator itEngine = mlistSTEngine.begin();
	list<CSquareTileEngine*>::iterator itEngineEnd = mlistSTEngine.end();
	while(itEngine != itEngineEnd)
	{
		Vector2D position = shipOrigin + shipRotation * ((*itEngine)->GetPosition() + delta);
		list<Vector2D>& listPos = (*itEngine)->mListPos;
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

void CSquareShip::FireAt(const Vector2D& target, float ratioError)
{
	ratioError = max(0.0f, min(1.0f, ratioError));
	static Vector2D delta = Vector2D(-SQUARETILE_SIZE/2, SQUARETILE_SIZE/2);

	Vector2D shipFront = popCast(Matrix22, mBody->GetRotationMatrix()) * Vector2D(-1.0f, 1.0f);
	shipFront.Normalize();

	list<CSquareTileGun*>::iterator itGun = mlistSTGun.begin();
	list<CSquareTileGun*>::iterator itGunEnd = mlistSTGun.end();
	while(itGun != itGunEnd)
	{
		if((*itGun)->GetCurrentLife() > 0)
		{
			if((*itGun)->Fire())
			{
				Vector2D pos = popCast(Vector2D, mBody->GetOriginPosition()) + 
					popCast(Matrix22, mBody->GetRotationMatrix()) * ((*itGun)->GetPosition() + delta);

				Vector2D dir = target - pos;
				dir.Normalize();
				float ratio = b2Random(-ratioError, ratioError);
				dir = (1.0f-abs(ratio)) * dir + ratio * Vector2D(-dir.y, dir.x);
				dir.Normalize();
				// on tire seulement si on vise un objet dans le demi cercle face au canon
				if( dir * shipFront >= 0.0f)
				{
					dir *= (*itGun)->GetMissileSpeed();
					dir += popCast(Vector2D, mBody->GetLinearVelocity());

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

void CSquareShip::AddEnginePS(const Vector2D& pos)
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

	// poussée moteur
	Vector2D p = popCast(Vector2D, mBody->GetCenterPosition());
	Vector2D f = popCast(Matrix22, mBody->GetRotationMatrix()) * Vector2D(1.0f, 1.0f);
	f.Normalize();
	f *= power * 1000000.0f;
	mBody->ApplyForce(popCast(b2Vec2, f), popCast(b2Vec2, p));
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

	// poussée moteur
	Vector2D p = popCast(Vector2D, mBody->GetCenterPosition());
	Vector2D f = popCast(Matrix22, mBody->GetRotationMatrix()) * Vector2D(1.0f, 1.0f);
	f.Normalize();
	f *= power * 150.0f * mBody->GetMass();
	mBody->ApplyImpulse(popCast(b2Vec2, f), popCast(b2Vec2, p));
}

Vector2D CSquareShip::GetShootPoint(const Vector2D& pos, const Vector2D& vel)
{
	Vector2D P = popCast(Vector2D, mBody->GetCenterPosition());//position initiale du projectile (approx)
	float spdP = GUN_MISSILES_SPEED;

	Vector2D E = pos;//position de la cible
	Vector2D velE = vel;//vitesse de la cible
	Vector2D velE0 = vel - popCast(Vector2D, mBody->GetLinearVelocity());//vitesse de la cible dans le référentiel dynamique du tireur

	//si la cible est à l'arret la position recherchée est E
	if(velE0.Length() == 0.0f) return  E;

	Vector2D PE = E - P;//vecteur PE

	float T1, T2;//nos temps à trouver (on prendra le plus petit des 2 (si positifs))

	// équation du second degré AT²+BT+C=0
	float A = velE0.Length2() - spdP*spdP;
	float B = 2.0f * (PE * velE0);
	float C = PE.Length2();

	if(A > 0.0f)// si A>0 le projectile est moins rapide que l'ennemi
		return E;

	if(A == 0.0f)
		return E - (C/B) * velE0;

	float Delta = B*B - 4.0f*A*C;// discriminant de l'équation du 2nd degré

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
	int squareTileRadius2 = (SQUARETILE_SIZE/2)*(SQUARETILE_SIZE/2);// rayon au carré du cercle passant par les milieux des côtés
	int radius2 = size2*squareTileRadius2*2;// rayon au carré du cercle passant par les coins diagonaux
	if((missile->GetPosition()-popCast(Vector2D, mBody->GetOriginPosition())).Length2() > radius2)
		return;

	Vector2D localPos = popCast(Matrix22, mBody->GetRotationMatrix()) / 
		(missile->GetLastPosition() - popCast(Vector2D, mBody->GetOriginPosition()));
	
	Vector2D localTraj = popCast(Matrix22, mBody->GetRotationMatrix()) / 
		(missile->GetPosition() - missile->GetLastPosition());
	float minTrajLength = localTraj.Length();
	localTraj.Normalize();

	bool found = false;
	CSquareTile* hitSquareTile = NULL;

	CSquareTile** squareTile = mSquareTiles;
	for(int i=0; i<size2; i++)
	{
		if((*squareTile))
		{
			if((*squareTile)->GetCurrentLife() > 0)// si la squaretile n'est pas détruite
			{
				Vector2D tileDist = (*squareTile)->GetPosition() - localPos;
				float scal = tileDist * localTraj;
				float dist2 = tileDist.Length2() - scal*scal;
				if(dist2 <= squareTileRadius2)// tile traversée par le projectile
				{
					if(scal < minTrajLength)// tile la plus proche de la position précédente
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

	if(found)// on a alors la plus proche squaretile touchée par le projectile
	{
		missile->Destroy();
		hitSquareTile->ComputeLife(missile->GetDamages());
		if(hitSquareTile->GetCurrentLife()<=0.0f)// tile détruite
			hitSquareTile->Explode();
	}
}

