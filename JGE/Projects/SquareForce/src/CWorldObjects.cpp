#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CResourceManager.h"

#include "CWorldObjects.h"
#include "CSquareShipAI.h"

#include "utils.h"


CWorldObjects::CWorldObjects(CSpawnManager* spawnMgr, float scale/* = 1.0f*/)
{	
	mWorld = NULL;

	mTimer = 0.0f;

	mScale = scale;

	mSpawnMgr = spawnMgr;

	mObjects = NULL;
	mPlanets = NULL;
	mHero = NULL;

	mGPE1 = NULL;
	mGPE2 = NULL;
}


CWorldObjects::~CWorldObjects()
{
	// ne pas supprimer les tiles sur le hero (elles sont dans l'inventaire)
	for(int i=0; i<mHero->mSize*mHero->mSize; i++)
		mHero->SetSquareTile(NULL, i);
	SAFE_DELETE(mHero->mDatas);
	SAFE_DELETE(mHero);

	for(int i=1; i<mNbObjects; i++)
	{
		SAFE_DELETE(mObjects[i]);
	}
	SAFE_DELETE_ARRAY(mObjects);

	for(int i=1; i<mNbPlanets; i++)
	{
		SAFE_DELETE(mPlanets[i]);
	}
	SAFE_DELETE_ARRAY(mPlanets);

	SAFE_DELETE(mWorld);

	while(!mListMissiles.empty())
	{
		SAFE_DELETE(mListMissiles.front());
		mListMissiles.pop_front();
	}


	SAFE_DELETE(mGPE1);
	SAFE_DELETE(mGPE2);
}


void CWorldObjects::Create()
{
	b2AABB worldAABB;

	worldAABB.minVertex.Set(-1000000.0f, -1000000.0f);
	worldAABB.maxVertex.Set(1000000.0f, 1000000.0f);
	
	b2Vec2 gravity(0.0f, 0.0f);		// Define the gravity vector.
	bool doSleep = true;				// Do we want to let bodies sleep?

	// Construct a world object, which will hold and simulate the rigid bodies.
	mWorld = new b2World(worldAABB, gravity, doSleep);
	
	mNbObjects = NB_OBJECTS;
	mObjects = new CObject*[mNbObjects];

	mHero = new CSquareShip(mWorld, mListMissiles);
	mHero->Create(3);
	mHero->LoadShape(mSpawnMgr->GetEmptyShipDatas(3), mSpawnMgr->mListTiles);// vaisseau sans tile
	mHero->LoadPhysic();
	mObjects[0] = mHero;
	mSpawnMgr->SetHero(mHero);

	for(int i=1; i<mNbObjects; i++)
	{
		CSquareShip *ship = new CSquareShip(mWorld, mListMissiles);
		ship->Create(3);
		ship->LoadShape(mSpawnMgr->mListShipsDatas[(b2Random(0, 1)>0.5f)?0:1], mSpawnMgr->mListTiles);
		ship->SetAI(new CSquareShipAI(this, ship));
		mObjects[i] = ship;
		mSpawnMgr->AddObject(ship);
	}
	
	mCamPos = mHero->GetCenterPosition();
	mCamRot = M_PI_4;

	CResourceManager* resMgr = CResourceManager::GetInstance();
	int num = 10;
	mGPE1 = new CGlobalParticleEmitter(150, resMgr->GetParticlesQuad(10), 1.0f);
	//mGPE->mStartColor = hgeColor(0.3f, 1.0f, 1.0f, 0.5f);
	//mGPE->mDeltaColor = hgeColor(-0.3f, -0.7f, -0.2f, 0);
	mGPE1->mStartColor = hgeColor(0.3f, 0.7f, 1.0f, 1.0f);
	mGPE1->mDeltaColor = hgeColor(-0.3f, -0.7f, -0.7f, -0.2f);

	num = 1;
	mGPE2 = new CGlobalParticleEmitter(50, resMgr->GetParticlesQuad(1), 60.0f);
	mGPE2->mStartColor = hgeColor(0.6f, 0.0f, 0.9f, 0.2f);
	mGPE2->mDeltaColor = hgeColor(-0.6f, 0.3f, -0.3f, 0.0f);

	mNbPlanets = NB_PLANETS;
	mPlanets = new CPlanet*[mNbPlanets];
	for(int i=0; i<mNbPlanets; i++)
	{
		mPlanets[i] = new CPlanet;
		mPlanets[i]->SetOriginPosition(b2Vec2(10.0f*b2Random(-200.0f, 200.0f), 10.0f*b2Random(-200.0f, 200.0f)));
	}
}


void CWorldObjects::Update(float dt)
{
	mSpawnMgr->Update();

	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and 10 iterations. This provides a high quality simulation
	// in most game scenarios.
	static float32 timeStep = 1.0f / 30.0f;
	static int32 iterations = 1;

	mTimer += dt;
  	if (mTimer < timeStep)
  		return;
	mTimer -= timeStep;
	// on update le world
	{		
		// Instruct the world to perform a single step of simulation. It is
		// generally best to keep the time step and iterations fixed.
		mWorld->Step(timeStep, iterations);
	}

	// on update les projectiles
	list<CMissile*>::iterator it = mListMissiles.begin();
	list<CMissile*>::iterator itEnd = mListMissiles.end();
	while(it != itEnd)
	{
		(*it)->Update(timeStep);
		if(!(*it)->IsAlive())
		{
			SAFE_DELETE((*it));
			it = mListMissiles.erase(it);
			continue;
		}
		it++;
	}

	// on update les vaisseaux
	{
// 		for(int i=0; i<mNbObjects; i++)
// 			mObjects[i]->Update(timeStep);
		int i = 0;
		CObject* obj = NULL;
		while((obj = mSpawnMgr->GetObject(i++)))
		{
			obj->Update(timeStep);
		}
	}

	{
		for(int i=0; i<mNbPlanets; i++)
			mPlanets[i]->Update(timeStep);
	}

	mGPE1->Update(timeStep, mCamPos);
	mGPE2->Update(timeStep, mCamPos);


	//const b2Vec2 force = mHero->m_force;

	// on update la cam�ra
	{
		float ratio = SMOOTH_CAM_COEFF;// facteur smooth

		b2Vec2 v = b2Mul(mHero->GetRotationMatrix(), b2Vec2(-1, 1));
		//v.Rotate(mCamRot);
		v.Normalize();
		b2Vec2 newCamPos = mHero->GetCenterPosition() + 100.0f*v;// + deltaPos;
		b2Vec2 deltaPos = 0.2f*(newCamPos-mCamPos);
// 		if(deltaPos.Length()>1000.0f*mTimer)
// 		{
// 			deltaPos.Normalize();
// 			deltaPos *= 1000.0f*mTimer;
// 		}
		mCamPos += deltaPos;

		float deltaAngle = mHero->GetRotation() - (mCamRot-M_PI_4);
// 		if(deltaAngle>M_PI)
// 			printf("");
		
		// FIXME : (lors du reset sur la psp la rotation du hero 
		// peut passer de 0 � une valeur tr�s elev�e)
		if(abs(deltaAngle) > 0.001f)
		{
			if(abs(deltaAngle) < M_PI)
				deltaAngle *= ratio * 0.15f;
			mCamRot += deltaAngle;
		}
	}

	mTimer = 0.0f;
}

void CWorldObjects::Render()
{
	JRenderer* renderer = JRenderer::GetInstance();	

	// on dessine le fond
	//renderer->RenderQuad(mBGQuad, 0, 0);

	mGPE2->Render(mCamPos, mCamRot);

	// on dessine l'�metteur de particules global
	mGPE1->Render(mCamPos, mCamRot);

	for(int i=0; i<mNbPlanets; i++)
		mPlanets[i]->Render(mCamPos, mCamRot);

	// on dessine les projectiles
	list<CMissile*>::iterator it = mListMissiles.begin();
	list<CMissile*>::iterator itEnd = mListMissiles.end();
	while(it != itEnd)
	{
		(*it)->Render(mCamPos, mCamRot);
		it++;
	}
	
	// on dessine les objets
// 	for(int i=0; i<mNbObjects; i++)
// 	{
// 		mObjects[i]->Render(mCamPos, mCamRot);
// 	}
	int i = 0;
	CObject* obj = NULL;
	while((obj = mSpawnMgr->GetObject(i++)))
	{
		obj->Render(mCamPos, mCamRot);
	}
}


void CWorldObjects::RenderShape()
{

	JRenderer* renderer = JRenderer::GetInstance();	

	float xx[32];
	float yy[32];

	for (b2Body* b = mWorld->m_bodyList; b; b = b->m_next)
	{

		for (b2Shape* shape = b->m_shapeList; shape; shape = shape->m_next)
		{


			switch (shape->m_type)
			{
			case e_circleShape:
				{
					const b2CircleShape* circle = (const b2CircleShape*)shape;
					b2Vec2 pos(SCREEN_SIZE_X2+(circle->m_position.x-mCamPos.x), 
						SCREEN_SIZE_Y2+(circle->m_position.y-mCamPos.y));
					float32 r = circle->m_radius-mCamRot;

					renderer->DrawCircle(pos.x, pos.y, r, ARGB(255,255,255,255));
				}
				break;

			case e_polyShape:
				{
					const b2PolyShape* poly = (const b2PolyShape*)shape;

					for (int32 i = 0; i < poly->m_vertexCount; ++i)
					{
						b2Mat22 camRot(mCamRot);
						b2Vec2 v = b2MulT(camRot, poly->m_position - mCamPos + b2Mul(poly->m_R, poly->m_vertices[i]));
						xx[i] = SCREEN_SIZE_X2+v.x;
						yy[i] = SCREEN_SIZE_Y2-v.y;
					}

					renderer->DrawPolygon(xx, yy, poly->m_vertexCount, ARGB(255,255,255,255));

				}
				break;
			}
		}

	}

}

CObject* CWorldObjects::GetNearestObject(const b2Vec2& worldPos, CObject* skippedObj /*= NULL*/)
{
	CObject* nearestObj = NULL;
	CObject** it = mObjects;
	float minDist = 500.0f*500.0f;
	for(int i=0; i<mNbObjects; i++)
	{
		if(*it != skippedObj && !(*it)->IsDestroyed())
		{
			float dist = (worldPos - (*it)->GetOriginPosition()).Length2();
			if(dist < minDist)
			{
				minDist = dist;
				nearestObj = *it;
			}
		}
		it++;
	}
	return nearestObj;
}
