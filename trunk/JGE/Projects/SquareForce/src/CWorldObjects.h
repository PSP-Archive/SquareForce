#ifndef _CWORLDOBJECTS_H_
#define _CWORLDOBJECTS_H_
#include "utils.h"

#include "Box2D.h"

#include "CSpawnManager.h"
#include "CObject.h"
#include "CSquareShip.h"
#include "CPlanet.h"
#include "CGlobalParticleEmitter.h"
#include "CMissile.h"

#include <list>
using namespace std;


class CWorldObjects
{

public:

	CWorldObjects(CSpawnManager* spawnMgr, float scale = 1.0f);
	~CWorldObjects();

	void Update(float dt);
	void RenderShape();
	void Render();

	void Create();

	CObject* GetNearestObject(const Vector2D& worldPos, CObject* skippedObj = NULL);

	int mNbObjects;
	CObject **mObjects;

	int mNbPlanets;
	CPlanet **mPlanets;

	CSquareShip *mHero;

	Vector2D mCamPos;
	float mCamRot;
	Matrix22 mCamMat;
private:
	CSpawnManager* mSpawnMgr;

	float mTimer;

	JQuad* mEnginePclQuad;

	b2World* mWorld;
	float mScale;

	CGlobalParticleEmitter* mGPE1;
	CGlobalParticleEmitter* mGPE2;

	list<CMissile*> mListMissiles;
};

#endif

