#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CSquareShipAI.h"
#include "CSquareShip.h"
#include "CWorldObjects.h"


CSquareShipAI::CSquareShipAI(CWorldObjects *worldObjects, CSquareShip *owner):
mWorldObjects(worldObjects), mOwner(owner)
{
	mSpawnPoint = Vector2D(0,0);
	mSpawnPointRadius = 0;

	mPatrolPoint = mOwner->GetOriginPosition();
	mPatrolPointRadius = 1000.0f;

	mCurrentDest = mOwner->GetOriginPosition();
	mCurrentTarget = NULL;

	mRatioErrorFiring = 0.1f;
}


CSquareShipAI::~CSquareShipAI()
{

}


void CSquareShipAI::Update(float dt)
{
	float rangeMax = 500.0f;
	Vector2D myPos = mOwner->GetOriginPosition();
	bool leavingPatrolArea = false;

	Vector2D shipRear = mOwner->GetRotationMatrix() * Vector2D(1.0f, -1.0f);
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
	CSpawnManager* mgr = mWorldObjects->mSpawnMgr;
	int i = 0;
	CObject* obj = NULL;
	while((obj = mgr->GetActiveObject(i++)))
	{
		CSquareShip *ship = (CSquareShip*)obj;
		if(ship != mOwner)
		{
			Vector2D dir = myPos - ship->GetOriginPosition();
			float dist = dir.Length();
			dir.Normalize();
			float power = 1.0f - dist/((mOwner->mSize+ship->mSize)*SQUARETILE_SIZE*2.0f);
			// on majore les rayons des vaisseaux et on double
			if(power > 0.0f)
			{
				power *= (dir * shipRight >= 0)?1.0f:-1.0f;
				mOwner->Straff(power);
			}
			if(dist < distMin && !ship->IsDestroyed())
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

	// on g�re la puissance angulaire de mani�re � tourner jusqu'� �tre dans la direction d�sir�e
	float powerA = dir * shipRear;
	float powerL = mOwner->mEnginePower;
	if(powerA>=0.0f)// si on tourne le dos : on met la puissance � fond
	{
		powerA = 1.0f;

		powerL -= 0.02f;
		if(powerL<0.0f)
			powerL = 0.0f;
	}
	else// si on est de face on ajuste la puissance selon l'alignement
	{
		powerA = 1.0f-powerA*powerA;// on rescale sur [0;1] avec une courbe logarithmique

		powerL -= 0.04f*(powerA-0.5f);
		if(powerL<0.0f)
			powerL = 0.0f;
		else if(powerL>1.0f)
			powerL = 1.0f;

		if(mCurrentTarget)
		{
			mOwner->FireAt(
				mOwner->GetShootPoint(
					mCurrentTarget->GetOriginPosition(), 
					mCurrentTarget->GetLinearVelocity()), mRatioErrorFiring);
		}
	}
	if(dir * shipRight > 0.0f)// d�finition du sens
		powerA = -powerA;
	mOwner->mAngularPower = powerA;
	mOwner->mEnginePower = powerL;
}

void CSquareShipAI::LightUpdate(float dt)
{
	static float patrolSpdMax = 200.0f;

	Vector2D myPos = mOwner->GetCenterPosition();

	Vector2D dir;
	if((mCurrentDest-myPos).Length2()<100.0f*100.0f)// destination atteinte : on en choisit une nouvelle
	{
		mCurrentDest = Vector2D(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f));
		mCurrentDest.Normalize();
		mCurrentDest = mPatrolPointRadius * mCurrentDest + mPatrolPoint;
	}

	dir = (mCurrentDest-myPos);
	dir.Normalize();

	mOwner->SetLinearVelocity(patrolSpdMax*dir);
}