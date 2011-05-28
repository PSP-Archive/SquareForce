#ifndef _CSQUARESHIP_H_
#define _CSQUARESHIP_H_
#include "utils.h"

#include <list>

#include "Box2D.h"
#include "CObject.h"
#include "CSquareTile.h"
#include "CMissile.h"

#include "CSquareShipAI.h"

using namespace std;

class hgeParticleSystem;

class CSquareShipData
{
public:
	CSquareShipData():mSize(0), mTilesId(NULL) {}
	~CSquareShipData() {SAFE_DELETE_ARRAY(mTilesId);}

	u32 mSize;
	u32 *mTilesId;
};

class CSquareShip: public CObject
{
public:

	CSquareShip(b2World* world, list<CMissile*> &missilesPt);
	~CSquareShip();

	inline const CSquareTile* GetSquareTile(int slot)
	{
		if(slot<0 || slot>=mSize*mSize)
			return NULL;
		return mSquareTiles[slot];
	}
	CSquareTile* SetSquareTile(CSquareTile* squareTile, int slot);
	void LoadShape(CSquareShipData* datas, const vector<CSquareTile*>& listTiles);
	void UnloadShape();

	virtual void LoadPhysic();
	virtual void UnloadPhysic();

	void Create(int size);
	void Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat);
	void Update(float dt, bool updatePhysic = true);

	void LightUpdate(float dt, bool fullUpdate = false);// update without physics (for non rendered displacements)

	void FireAt(const Vector2D& target, float ratioError);

	void AddEnginePS(const Vector2D& pos);
	void AddExplosionPS(CSquareTile* squareTile);
	void SetMissileParticleSystem();

	void Straff(float power);
	inline void DashR() {if(mDashRTimer>0.5f) mDashRTimer = 0.0f;}
	inline void DashL() {if(mDashLTimer>0.5f) mDashLTimer = 0.0f;}

	Vector2D GetShootPoint(const Vector2D& pos, const Vector2D& vel);

	void ComputeCollision(CMissile* missile);

	inline void ToggleStopEngine() {mStopEngine = !mStopEngine;}

	inline CSquareShipAI *SetAI(CSquareShipAI *AI) {CSquareShipAI *lastAI = mAI; mAI = AI; return lastAI;}

	inline bool IsDestroyed() {return mDestroyed;}

	inline bool IsDocked() {return mIsDocked;}
	inline bool WantToDock() {return mWantToDock;}// true quand on veut  se docker/undocker
	inline void Dock(bool state) {mIsDocked = state;}
	inline void RequestDock(bool state) {mWantToDock = state;}// mettre à true pour se docker/undocker
	

	int mSize;// taille du squareship (carré de mSize*mSize)

	float mEnginePower;// puissance actuelle du moteur (de 0 à 1)

	float mAngularAcceleration;// accélération angulaire max
	float mAngularPower;// puissance actuelle de rotation (de -1 à 1)

	bool mStopEngine;// moteurs stoppés ?

	// Ne pas faire n'importe quoi avec ces datas. Hormis le hero tous les autres ont 
	// des datas originales donc à ne surtout pas supprimer.
	CSquareShipData* mDatas;

protected:
	void Dash(float power);

	JQuad** mQuad;
	JQuad* mQuadPcl;
	JQuad** mLightningQuads;

	hgeParticleSystem* mEnginePS;

	hgeParticleSystem* mMissilePS;

	CSquareTile **mSquareTiles;
	list<CSquareTileEngine*> mlistSTEngine;
	list<CSquareTileGun*> mlistSTGun;
	list<CSquareTileMissile*> mlistSTMissile;
	list<CSquareTileMine*> mlistSTMine;

	list<CMissile*> &mMissilesPt;// référence sur la liste de projectiles

	CSquareShipAI* mAI;

	bool mDestroyed;

	float mDashRTimer;
	float mDashLTimer;

	bool mWantToDock;
	bool mIsDocked;
};

#endif

