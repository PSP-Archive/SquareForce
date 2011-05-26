#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CSpeedWay.h"
#include "CResourceManager.h"

#include <hge/hgedistort.h>

#define SPEEDGATES_DISTANCE		250.0f
#define SPEEDGATE_WIDTH			100.0f

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
		quad->SetColor(ARGB(0, 0, 0, 0));
#endif
		renderer->RenderQuad(quad, SCREEN_SIZE_X2+pos.x, SCREEN_SIZE_Y2-pos.y, -rotation - M_PI_4);

		++tile;
	}

	// éclairs
	static float size = SPEEDGATE_WIDTH / 18.0f;
	int num = (int)(Random(0, 1)*NB_LIGHTNING_FRAMES);
	if(num<NB_LIGHTNING_FRAMES)
		renderer->RenderQuad(mLightningQuads[num], 
		SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, -rotation, size, 1.0f);

}


CSpeedWay::CSpeedWay(CPlanet* planet1, CPlanet* planet2): mPlanet1(planet1), mPlanet2(planet2)
{
	Vector2D dir = mPlanet2->GetOriginPosition() - mPlanet1->GetOriginPosition();
	float dist = dir.Normalize();
	dist -= (mPlanet1->GetSize() + mPlanet2->GetSize()) * 64.0f;
	int nbGates = (int)(dist/SPEEDGATES_DISTANCE) - 1;

	float rot = dir.Angle() + M_PI_2;
	Matrix22 mat(rot);

	Vector2D pos = mPlanet1->GetOriginPosition() + (mPlanet1->GetSize() * 64.0f + SPEEDGATES_DISTANCE)*dir;
	for(int i = 0; i<nbGates; i++)
	{
		CSpeedGate* gate = new CSpeedGate(pos, rot, mat);
		mListGates.push_back(gate);
		pos +=  SPEEDGATES_DISTANCE*dir;
	}

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
	
}

void CSpeedWay::Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat)
{
	vector<CSpeedGate*>::iterator it = mListGates.begin();
	const vector<CSpeedGate*>::const_iterator itEnd = mListGates.end();
	while(it != itEnd)
	{
		(*it)->Render(camPos, camRot, camMat);
		++it;
	}
}
