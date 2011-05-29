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

	mHero = NULL;

	mGPE1 = NULL;
	mGPE2 = NULL;
}


CWorldObjects::~CWorldObjects()
{
	mWorld->CleanBodyList();
	SAFE_DELETE(mWorld);

	while(!mListMissiles.empty())
	{
		SAFE_DELETE(mListMissiles.front());
		mListMissiles.pop_front();
	}

	SAFE_DELETE(mGPE1);
	SAFE_DELETE(mGPE2);

// #ifdef PSP
// 	f = fopen("debug.txt", "a");
// 	fprintf(f, "exit CWorldObjects delete\n");
// 	fclose(f);
// #endif
}


void CWorldObjects::Create()
{
	b2AABB worldAABB;

	worldAABB.minVertex.Set(-25000.0f, -25000.0f);
	worldAABB.maxVertex.Set(25000.0f, 25000.0f);
	
	b2Vec2 gravity(0.0f, 0.0f);		// Define the gravity vector.
	bool doSleep = true;				// Do we want to let bodies sleep?

	// Construct a world object, which will hold and simulate the rigid bodies.
// #ifdef PSP
// 	int size = ramAvailable();
// #endif
	mWorld = new b2World(worldAABB, gravity, doSleep);
// #ifdef PSP
// 	int size2 = ramAvailable();
// 	FILE *f = fopen("debug.txt", "w");
// 	fprintf(f, "ram free : %d o\nCreating b2World.\nram free : %d o\n", size, size2);
// 	fclose(f);
// #endif

	CResourceManager* resMgr = CResourceManager::GetInstance();

	mHero = new CSquareShip(mWorld, mListMissiles);
	mHero->Create(3);
	mHero->LoadShape(mSpawnMgr->GetEmptyShipDatas(3), resMgr->mListTiles);// vaisseau sans tiles
	mHero->LoadPhysic();
	mSpawnMgr->SetHero(mHero);

	for(int i=1; i<NB_OBJECTS; i++)
	{
		CSquareShip *ship = new CSquareShip(mWorld, mListMissiles);
		ship->Create(3);
		ship->LoadShape(resMgr->mListShipsDatas[(b2Random(0, 1)>0.5f)?0:1], resMgr->mListTiles);
		ship->SetAI(new CSquareShipAI(this, ship));
		mSpawnMgr->AddObject(ship);
	}
// #ifdef PSP
// 	size = ramAvailable();
// 	f = fopen("debug.txt", "a");
// 	fprintf(f, "Creating objects.\nram free : %d o\n", size);
// 	fclose(f);
// #endif
	
	mCamPos = mHero->GetCenterPosition();
	mCamRot = M_PI_4;
	mCamMat.Set(mCamRot);


	int num = 10;
	mGPE1 = new CGlobalParticleEmitter(80, resMgr->GetParticlesQuad(num), 1.0f, 
		hgeColor(0.6f, 0.9f, 0.9f, 0.9f), hgeColor(-0.3f, -0.4f, -0.3f, 0.0f));
	mGPE1->SpawnAt(mCamPos.x, mCamPos.y);
	
	num = 1;
	mGPE2 = new CGlobalParticleEmitter(70, resMgr->GetParticlesQuad(num), 90.0f, 
		hgeColor(0.6f, 0.0f, 0.9f, 0.2f), hgeColor(-0.6f, 0.3f, -0.3f, 0.0f));
	mGPE2->SpawnAt(mCamPos.x, mCamPos.y);
	
// 	for(int i=0; i<NB_PLANETS; ++i)
// 	{
// 		CPlanet* planet = new CPlanet;
// 		planet->SetOriginPosition(Vector2D(10.0f*b2Random(-200.0f, 200.0f), 10.0f*b2Random(-200.0f, 200.0f)));
// 		mSpawnMgr->AddPlanet(planet);
// 	}

// 	mSpawnMgr->AddSpeedWay(new CSpeedWay(mSpawnMgr->GetPlanet(0), mSpawnMgr->GetPlanet(3)));
// 	mSpawnMgr->AddSpeedWay(new CSpeedWay(mSpawnMgr->GetPlanet(3), mSpawnMgr->GetPlanet(0)));
}


void CWorldObjects::Update(float dt)
{
	CResourceManager::GetInstance()->UpdatePlasmaMesh(dt);

	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and 10 iterations. This provides a high quality simulation
	// in most game scenarios.
	static float timeStep = 1.0f / 30.0f;
	static int32 iterations = 1;

	mTimer += dt;
  	if (mTimer < timeStep)
  		return;
	mTimer = 0.0f;
	// on update le world
	{		
		// Instruct the world to perform a single step of simulation. It is
		// generally best to keep the time step and iterations fixed.
		mWorld->Step(timeStep, iterations);
	}

	mSpawnMgr->Update(timeStep, mCamPos);

	// on update les projectiles
	list<CMissile*>::iterator it = mListMissiles.begin();
	const list<CMissile*>::const_iterator itEnd = mListMissiles.end();
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
		int i = 0;
		CObject* obj = NULL;
		while((obj = mSpawnMgr->GetObject(i++)))
		{
			obj->Update(timeStep);
		}
	}

	// on update les planetes
	{
		int i = 0;
		CPlanet* planet = NULL;
		while((planet = mSpawnMgr->GetPlanet(i++)))
		{
			planet->Update(timeStep);
		}
	}

	//on update les speedways
	{
		int i = 0;
		CSpeedWay* speedWay = NULL;
		while((speedWay = mSpawnMgr->GetSpeedWay(i++)))
		{
			speedWay->Update(timeStep);
		}
	}

	mGPE1->Update(timeStep, mCamPos);
	mGPE2->Update(timeStep, mCamPos);


	//const Vector2D force = mHero->m_force;

	// on update la cam�ra
	{
		float ratio = SMOOTH_CAM_COEFF;// facteur smooth

		Vector2D v = mHero->GetRotationMatrix() * Vector2D(-1, 1);
		//v.Rotate(mCamRot);
		v.Normalize();
		Vector2D newCamPos = mHero->GetCenterPosition() + 100.0f*v;// + deltaPos;
		Vector2D deltaPos = 0.2f*(newCamPos-mCamPos);
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

			mCamMat.Set(mCamRot);
		}
	}

	mTimer = 0.0f;
}

void CWorldObjects::Render()
{
	int i = 0;

	JRenderer* renderer = JRenderer::GetInstance();	

	// on dessine le fond
	//renderer->RenderQuad(mBGQuad, 0, 0);

	mGPE2->Render(mCamPos, mCamRot, mCamMat);

	// on dessine l'�metteur de particules global
	mGPE1->Render(mCamPos, mCamRot, mCamMat, 0.0f, 0.7f);

	// on dessine les planetes
	i = 0;
	CPlanet* planet = NULL;
	while((planet = mSpawnMgr->GetPlanet(i++)))
	{
		planet->Render(mCamPos, mCamRot, mCamMat);
	}
	
	// on dessine les projectiles
	list<CMissile*>::iterator it = mListMissiles.begin();
	list<CMissile*>::iterator itEnd = mListMissiles.end();
	while(it != itEnd)
	{
		(*it)->Render(mCamPos, mCamRot, mCamMat);
		it++;
	}
	
	// on dessine les objets
	i = 0;
	CObject* obj = NULL;
	while((obj = mSpawnMgr->GetActiveObject(i++)))
	{
		obj->Render(mCamPos, mCamRot, mCamMat);
	}

	// on dessine les speedways
	i = 0;
	CSpeedWay* speedWay = NULL;
	while((speedWay = mSpawnMgr->GetSpeedWay(i++)))
	{
		speedWay->Render(mCamPos, mCamRot, mCamMat);
	}

	// on dessine l'�metteur de particules global
	mGPE1->Render(mCamPos, mCamRot, mCamMat, 0.7f, 1.0f);
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
					Vector2D pos(SCREEN_SIZE_X2+(circle->m_position.x-mCamPos.x), 
						SCREEN_SIZE_Y2+(circle->m_position.y-mCamPos.y));
					float r = circle->m_radius-mCamRot;

					renderer->DrawCircle(pos.x, pos.y, r, ARGB(255,255,255,255));
				}
				break;

			case e_polyShape:
				{
					const b2PolyShape* poly = (const b2PolyShape*)shape;

					for (int32 i = 0; i < poly->m_vertexCount; ++i)
					{
						Matrix22 camRot(mCamRot);
						Vector2D v = camRot / (popCast(Vector2D, poly->m_position) - mCamPos + 
							popCast(Vector2D, b2Mul(poly->m_R, poly->m_vertices[i])));
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

CObject* CWorldObjects::GetNearestObject(const Vector2D& worldPos, CObject* skippedObj /*= NULL*/)
{
	CObject* nearestObj = NULL;
	float minDist = 500.0f*500.0f;
	CObject* obj = NULL;
	int i = 0;
	while((obj = mSpawnMgr->GetActiveObject(i++)))
	{
		if(obj != skippedObj && !obj->IsDestroyed())
		{
			float dist = (worldPos - obj->GetOriginPosition()).Length2();
			if(dist < minDist)
			{
				minDist = dist;
				nearestObj = obj;
			}
		}
	}
	return nearestObj;
}