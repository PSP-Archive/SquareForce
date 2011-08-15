#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CSquareShipAI.h"
#include "CSquareShip.h"
#include "CSpawnManager.h"


CSquareShipAI::CSquareShipAI(CSpawnManager *spawnMgr, CGroupData* groupData):
mSpawnMgr(spawnMgr), mGroupData(groupData)
{
	mCurrentDest = Vector2D(0,0);
	mCurrentTarget = NULL;

	mRatioErrorFiring = 0.1f;

	mDir = Vector2D(0,0);
}


CSquareShipAI::~CSquareShipAI()
{
	// on enleve le pointeur sur notre IA pour les autres owners (normalement il n'y en a plus aucun)
	for(unsigned int i=0; i<mOwners.size(); ++i)
	{
		mOwners[i]->SetAI(NULL);
	}
	if(mGroupData)
		mGroupData->mIsSpawned = false;
}

bool CSquareShipAI::IsOwner(CSquareShip* ship) 
{
	// 		for(int i=0; i<mOwners.size(); ++i) 
	// 			if(mOwners[i] == ship) 
	// 				return true; 
	// 		return false;
	// faster
	return (this == ship->GetAI());
}

bool CSquareShipAI::IsLeader(CSquareShip* ship)
{
	CSquareShip* leader = NULL;
	vector<CSquareShip*>::const_iterator it = mOwners.begin();
	const vector<CSquareShip*>::const_iterator itEnd = mOwners.end();
	for(;it != itEnd;++it)
	{
		if(!(*it)->IsDestroyed())
		{
			leader = *it;
			break;
		}
	}
	return (ship == leader);
}

bool CSquareShipAI::RemoveOwner(CSquareShip* ship)
{
	vector<CSquareShip*>::iterator it = mOwners.begin();
	const vector<CSquareShip*>::const_iterator itEnd = mOwners.end();
	for(;it != itEnd;++it)
	{
		if((*it) == ship)
		{
			mOwners.erase(it);
			return true;
		}
	}
	return false;
}

void CSquareShipAI::Update(float dt, CSquareShip *owner)
{
	float rangeMax = 500.0f;
	Vector2D myPos = owner->GetOriginPosition();
	bool leavingPatrolArea = false;

	Vector2D shipRear = owner->GetRotationMatrix() * Vector2D(1.0f, -1.0f);
	shipRear.Normalize();
	Vector2D shipRight = Vector2D(-shipRear.y, shipRear.x);

	bool isLeader = IsLeader(owner);

	if(isLeader)
	{
		// perte de la cible courante
		if((myPos-mGroupData->mPatrolPoint).Length()>=mGroupData->mPatrolPointRadius+rangeMax)
		{
			mCurrentTarget = NULL;
			leavingPatrolArea = true;
		}
		if((mCurrentTarget && (mCurrentTarget->GetOriginPosition()-myPos).Length2()>=rangeMax*rangeMax)
			|| (mCurrentTarget && mCurrentTarget->IsDestroyed()))
			mCurrentTarget = NULL;
	}

	bool waitForMates = false;
	float distMin = rangeMax;
	CSquareShip *currentTarget = NULL;
	CSpawnManager* mgr = mSpawnMgr;
	int i = 0;
	CObject* obj = NULL;
	while((obj = mgr->GetObject(i++)))
	{
		CSquareShip *ship = (CSquareShip*)obj;
		if(ship != owner)
		{
			bool isOwner = IsOwner(ship);
			Vector2D dir = myPos - ship->GetOriginPosition();
			float dist = dir.Normalize();
			float power = 1.0f - dist/((owner->mSize+ship->mSize)*SQUARETILE_SIZE * 2.0f);
			if(power > 0.0f)
			{
				power *= (dir * shipRight >= 0)?1.0f:-1.0f;
				owner->Straff(power);
			}
			if(!ship->IsDestroyed() && !ship->IsLanded())
			{
				if(!isOwner)// ce n'est pas un membre du groupe
				{
					if(dist < distMin)// dans le range of sight et le plus proche : on le prend pour cible
					{
						distMin = dist;
						currentTarget = ship;
					}
				}
				else
				{
					if(shipRear*dir < 0.0f && dist > mGroupRadius)// c'est un membre du groupe à la bourre (derrière et a plus de 300pxl)
					{
						waitForMates = true;// je ralentis pr l'attendre
					}
					// aimantation vers le vaisseau du groupe (=>baricentre du groupe)
					{
						power = (dir * Vector2D(mDir.y, -mDir.x) <= 0)?0.05f:-0.05f;
						owner->Straff(power);
					}
				}
			}
		}
	}
	// si on a pas de cible on en cherche une (la plus proche)
	if(isLeader && !leavingPatrolArea && !mCurrentTarget)
	{
		mCurrentTarget = currentTarget;
	}

	Vector2D dir;
	if(isLeader)
	{
		// si on a pas de cible en vue on continue la patrouille
		if(!mCurrentTarget)
		{
			dir = (mCurrentDest-myPos);
			if(dir.Length2() < 100.0f*100.0f)// destination atteinte : on en choisit une nouvelle
			{
				mCurrentDest = Vector2D(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f));
				mCurrentDest.Normalize();
				mCurrentDest = (Random(0.0f, 1.0f)*mGroupData->mPatrolPointRadius) * mCurrentDest + mGroupData->mPatrolPoint;
				dir = (mCurrentDest-myPos);
			}

			if(dir.Normalize() == 0.0f)
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
		mDir = dir;
	}
	else
	{
		dir = mDir;
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
		if(powerA < -0.8f && !waitForMates)
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