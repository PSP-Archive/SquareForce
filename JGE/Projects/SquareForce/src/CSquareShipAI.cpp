#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CSquareShipAI.h"
#include "CSquareShip.h"
#include "CSpawnManager.h"


CSquareShipAI::CSquareShipAI(CSpawnManager *spawnMgr):
mSpawnMgr(spawnMgr)
{
	mSpawnPoint = Vector2D(0,0);
	mSpawnPointRadius = 0;

	mPatrolPoint = Vector2D(0,0);
	mPatrolPointRadius = 1000.0f;

	mCurrentDest = Vector2D(0,0);
	mCurrentTarget = NULL;

	mRatioErrorFiring = 0.1f;
}


CSquareShipAI::~CSquareShipAI()
{

}


void CSquareShipAI::Update(float dt, CSquareShip *owner)
{
	float rangeMax = 500.0f;
	Vector2D myPos = owner->GetOriginPosition();
	bool leavingPatrolArea = false;

	Vector2D shipRear = owner->GetRotationMatrix() * Vector2D(1.0f, -1.0f);
	shipRear.Normalize();
	Vector2D shipRight = Vector2D(-shipRear.y, shipRear.x);

	// perte de la cible courante
	if((myPos-mPatrolPoint).Length()>=mPatrolPointRadius+rangeMax)
	{
		mCurrentTarget = NULL;
		leavingPatrolArea = true;
	}
	if((mCurrentTarget && (mCurrentTarget->GetOriginPosition()-myPos).Length2()>=rangeMax*rangeMax)
		|| (mCurrentTarget && mCurrentTarget->IsDestroyed()))
		mCurrentTarget = NULL;

	float distMin = rangeMax;
	CSquareShip *currentTarget = NULL;
	CSpawnManager* mgr = mSpawnMgr;
	int i = 0;
	CObject* obj = NULL;
	while((obj = mgr->GetActiveObject(i++)))
	{
		CSquareShip *ship = (CSquareShip*)obj;
		if(ship != owner)
		{
			Vector2D dir = myPos - ship->GetOriginPosition();
			float dist = dir.Length();
			dir.Normalize();
			float power = 1.0f - dist/((owner->mSize+ship->mSize)*SQUARETILE_SIZE*2.0f);
			// on majore les rayons des vaisseaux et on double
			if(power > 0.0f)
			{
				power *= (dir * shipRight >= 0)?1.0f:-1.0f;
				owner->Straff(power);
			}
			if(dist < distMin && !ship->IsDestroyed() && !ship->IsLanded())
			{
				distMin = dist;
				currentTarget = ship;
			}
		}
	}
	// si on a pas de cible on en cherche une (la plus proche)
	if(!leavingPatrolArea && !mCurrentTarget)
	{
		mCurrentTarget = currentTarget;
	}

	Vector2D dir;
	// si on a pas de cible en vue on continue la patrouille
	if(!mCurrentTarget)
	{
		if((mCurrentDest-myPos).Length2()<100.0f*100.0f)// destination atteinte : on en choisit une nouvelle
		{
			mCurrentDest = Vector2D(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f));
			mCurrentDest.Normalize();
			mCurrentDest = mPatrolPointRadius * mCurrentDest + mPatrolPoint;
		}

		dir = (mCurrentDest-myPos);
		if(dir.Normalize()==0.0f)
			dir = -shipRear;
	}
	else// si on a une cible
	{
		dir = (mCurrentTarget->GetOriginPosition()-myPos);
		Vector2D trans = dir ;//+ Vector2D(-dir.y, dir.x);
		trans.Normalize();
		dir += 150.0f*trans;
		dir.Normalize();
	}

	// on gère la puissance angulaire de manière à tourner jusqu'à être dans la direction désirée
	float powerA = dir * shipRear;
	float powerL = owner->mEnginePower;
	if(powerA>=0.0f)// si on tourne le dos : on met la puissance angulaire à fond
	{
		powerA = 1.0f;

		powerL -= dt;
		if(powerL<0.0f)
			powerL = 0.0f;
	}
	else// si on est de face on ajuste la puissance selon l'alignement
	{
		if(powerA < -0.8f)
			powerL += dt;
		else
			powerL -= dt;
		powerL = (powerL<0.0f)?0.0f:((powerL>1.0f)?1.0f:powerL);
		powerA = 1.0f-powerA*powerA;// on rescale sur [0;1] avec une courbe logarithmique

		if(mCurrentTarget)
		{
			owner->FireAt(
				owner->GetShootPoint(
					mCurrentTarget->GetOriginPosition(), 
					mCurrentTarget->GetLinearVelocity()), mRatioErrorFiring);
		}
	}
	if(dir * shipRight > 0.0f)// définition du sens
		powerA = -powerA;
	owner->mAngularPower = powerA;
	owner->mEnginePower = powerL;
}

void CSquareShipAI::LightUpdate(float dt, CSquareShip *owner)
{
	static float patrolSpdMax = 200.0f;

	Vector2D myPos = owner->GetCenterPosition();

	Vector2D dir;
	if((mCurrentDest-myPos).Length2()<100.0f*100.0f)// destination atteinte : on en choisit une nouvelle
	{
		mCurrentDest = Vector2D(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f));
		mCurrentDest.Normalize();
		mCurrentDest = mPatrolPointRadius * mCurrentDest + mPatrolPoint;
	}

	dir = (mCurrentDest-myPos);
	dir.Normalize();

	owner->SetLinearVelocity(patrolSpdMax*dir);
}
