#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CSpeedWay.h"
#include "CResourceManager.h"

#include <hge/hgedistort.h>

#define SPEEDGATES_DISTANCE		250.0f
#define SPEEDGATE_WIDTH			100.0f
#define SPEEDWAY_TRACTOR_SPEED	50.0f
#define SPEEDWAY_SPEED			999.99f


CSpeedGate::CSpeedGate(const Vector2D& originPosition, const float& rotation, const Matrix22& matrixRot)
: mOriginPosition(originPosition), mRotation(rotation), mMatrixRot(matrixRot)
{
	CResourceManager* resMgr = CResourceManager::GetInstance();
	mGate[0] = new CSquareTileHull((CSquareTileHull*)resMgr->mListTiles[1]);
	mGate[0]->SetPosition(Vector2D(-SPEEDGATE_WIDTH/2, 0.0f));
	mGate[1] = new CSquareTileHull((CSquareTileHull*)resMgr->mListTiles[1]);
	mGate[1]->SetPosition(Vector2D(SPEEDGATE_WIDTH/2, 0.0f));

	mLightningQuads = resMgr->GetLightningQuads();

	mQuad = resMgr->GetSquareTilesQuads();
}

CSpeedGate::~CSpeedGate()
{
	SAFE_DELETE(mGate[0]);
	SAFE_DELETE(mGate[1]);

}

void CSpeedGate::Update(float dt)
{

}

void CSpeedGate::Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat)
{
	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();	

	Vector2D position = camMat / (mOriginPosition - camPos);
	float rotation = mRotation-camRot;// a inverser pour le rendu
	Matrix22 mat = mMatrixRot/camMat;// deja inversé pour le rendu

	CSquareTile** tile = mGate;
	int i = 3;
	while(--i)
	{
		Vector2D trans = mMatrixRot * (*tile)->GetPosition();
		Vector2D pos = camMat / (mOriginPosition + trans - camPos);

		// tile
		JQuad* quad = mQuad[(int)(*tile)->GetType()];
#ifdef WIN32
		quad->SetColor(ARGB(255, 255, 255, 255));
#else
		quad->mBlend=GU_TFX_ADD;
		quad->SetColor(ARGB(255, 0, 0, 0));
#endif
		renderer->RenderQuad(quad, SCREEN_SIZE_X2+pos.x, SCREEN_SIZE_Y2-pos.y, -rotation - M_PI_4);
		int num = (int)(Random(0, 1)*NB_LIGHTNING_FRAMES*20.0f);
		if(num<NB_LIGHTNING_FRAMES)
		{
			mLightningQuads[num]->SetColor(ARGB(255, 255, 255, 255));
			renderer->RenderQuad(mLightningQuads[num], 
				SCREEN_SIZE_X2+pos.x, SCREEN_SIZE_Y2-pos.y, -rotation, 1.0f, 1.0f);
		}

		++tile;
	}

	// éclairs
	static float size = SPEEDGATE_WIDTH / 18.0f;
	int num = (int)(Random(0, 1)*NB_LIGHTNING_FRAMES);
	if(num<NB_LIGHTNING_FRAMES)
	{	
		mLightningQuads[num]->SetColor(ARGB(255, 0, 255, 128));
		renderer->RenderQuad(mLightningQuads[num], 
		SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, -rotation, size, 1.0f);
	}

}


CSpeedWay::CSpeedWay(CPlanet* planet1, CPlanet* planet2): mPlanet1(planet1), mPlanet2(planet2)
{
	mDir = mPlanet2->GetOriginPosition() - mPlanet1->GetOriginPosition();
	float dist = mDir.Normalize();
	dist -= (mPlanet1->GetSize() + mPlanet2->GetSize()) * PLANET_TEXTURE_SIZE/2;
	float fNbGates = dist/SPEEDGATES_DISTANCE;
	int nbGates = (int)fNbGates - 1;

	mRotation = mDir.Angle() - M_PI_2;
	mMatrixRot.Set(mRotation);

	Vector2D pos = mPlanet1->GetOriginPosition() + (mPlanet1->GetSize() * PLANET_TEXTURE_SIZE/2 + SPEEDGATES_DISTANCE + (fNbGates - (float)(nbGates+1))/2*SPEEDGATES_DISTANCE)*mDir;
	pos += (SPEEDGATE_WIDTH/2+10.0f)*Vector2D(mDir.y, -mDir.x);// right shift
	mStart = pos;
	for(int i = 0; i<nbGates; i++)
	{
		CSpeedGate* gate = new CSpeedGate(pos, mRotation, mMatrixRot);
		mListGates.push_back(gate);
		pos +=  SPEEDGATES_DISTANCE*mDir;
	}
	mEnd = pos - SPEEDGATES_DISTANCE*mDir;

	CResourceManager* resMgr = CResourceManager::GetInstance();
	mPlasmaSizeX = (SPEEDGATE_WIDTH - 16.0f) / resMgr->GetPlasmaTex()->mWidth;
	mPlasmaSizeY = SPEEDGATES_DISTANCE / resMgr->GetPlasmaTex()->mHeight;

}

CSpeedWay::~CSpeedWay()
{
	vector<CSpeedGate*>::iterator it = mListGates.begin();
	const vector<CSpeedGate*>::const_iterator itEnd = mListGates.end();
	while(it != itEnd)
	{
		SAFE_DELETE(*it);
		++it;
	}
	mListGates.clear();
}

void CSpeedWay::Update(float dt)
{
	list<CObject*>::iterator it = mDockedObjects.begin();
	const list<CObject*>::const_iterator itEnd = mDockedObjects.end();
	while(it != itEnd)
	{
		CObject* obj = *it;
		Vector2D dir = DirectionToSegment(obj->GetCenterPosition(), mStart, mEnd);
		float dist = dir.Normalize();
		float dot = dir*mDir;
		if(dot < -0.001f && dist > 50.0f)// on est sorti du speedway
		{
			obj->Dock(false);
			obj->RequestDock(false);
			it = mDockedObjects.erase(it);
			continue;
		}
	
		if(dist > 1.0f && dot >= -0.001f)// on tracte l'objet vers le centre du speedway
		{
			float t = dist/SPEEDWAY_TRACTOR_SPEED;// temps restant (!=0)
			obj->SetLinearVelocity(SPEEDWAY_TRACTOR_SPEED*dir);
			float a = mDir.Angle(obj->GetRotationMatrix() * Vector2D(-1.0f, 1.0f));
			obj->SetAngularVelocity(-a/t);
		}
		else// on speed tout droit
		{
			obj->SetLinearVelocity(SPEEDWAY_SPEED*mDir);
			obj->SetAngularVelocity(0);
		}

		++it;
	}
}

void CSpeedWay::Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat)
{
	if(mListGates.empty())
		return;

	bool activated = !mDockedObjects.empty();

	CResourceManager* resMgr = CResourceManager::GetInstance();
	hgeDistortionMesh* mesh = resMgr->GetPlasmaMesh();

	vector<CSpeedGate*>::iterator it = mListGates.begin();
	vector<CSpeedGate*>::iterator itNext = it;
	++itNext;
	const vector<CSpeedGate*>::const_iterator itEnd = mListGates.end();
	while(itNext != itEnd)
	{
		Vector2D itPos = (*it)->GetOriginPosition();
		Vector2D itNextPos = (*itNext)->GetOriginPosition();
		bool itVisible = ((camPos-itPos).Length2() < 90000.0f);
		if(itVisible || (camPos-itNextPos).Length2() < 90000.0f)
		{
			if(activated)// only render the vortex when activated
			{
				Vector2D center = 0.5f*(itPos+itNextPos);
				Vector2D position = camMat / (center - camPos);
				//float rotation = mRotation - camRot;// a inverser pour le rendu
				Matrix22 mat = mMatrixRot/camMat;// deja inversé pour le rendu

				mesh->Render(SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, mat, -mPlasmaSizeX, -mPlasmaSizeY);
				mesh->Render(SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, mat, mPlasmaSizeX, mPlasmaSizeY);
			}
			if(itVisible)// render the gate if visible
				(*it)->Render(camPos, camRot, camMat);
		}

		++it;
		++itNext;
	}
	if((camPos-(*it)->GetOriginPosition()).Length2() < 90000.0f)
		(*it)->Render(camPos, camRot, camMat);
}
