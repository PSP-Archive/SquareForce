#ifndef _CSQUARETILE_H_
#define _CSQUARETILE_H_
#include "utils.h"
#include "CLocalization.h"

#include <list>

#include <hge\hgeparticle.h>

using namespace std;

class CSquareTile
{
public:

	enum ESquareTileType{HULL, ENGINE, GUN, MISSILE, MINE};

	CSquareTile(ESquareTileType type, const b2Vec2& pos): mType(type), mPos(pos)
	{
		mId = 0;

		mLifePoints = 100.0f;
		mCurrentLife = mLifePoints;

		mExplosionsPS = NULL;

		mRegenSpeed = 5.0f;

		mRegenTimer = 0.0f;
		mTimeBeforeRecovery = 10.0f;

		mEquipped = false;

		mName = "";
		mDesc = "";
		mStatDesc = "";
	}

	CSquareTile(CSquareTile* tile)
	{
		if(tile)
		{
			mId = tile->GetId();

			mType = tile->GetType();
			mPos = tile->GetPosition();

			mLifePoints = tile->GetLifePoints();
			mCurrentLife = tile->GetCurrentLife();

			mExplosionsPS = NULL;

			mRegenSpeed = tile->GetRegenSpeed();

			mRegenTimer = 0.0f;
			mTimeBeforeRecovery = tile->GetTimeBeforeRecovery();

			mEquipped = false;

			mName = tile->GetName();
			mDesc = tile->GetDesc();
			mStatDesc = tile->GetStatDesc();
		}
	}
	
	virtual ~CSquareTile() {SAFE_DELETE(mExplosionsPS);}

	inline const ESquareTileType& GetType() const {return mType;}

	inline const float& GetLifePoints() const {return mLifePoints;}
	inline void SetLifePoints(const float& lifePoints) {mLifePoints = mCurrentLife = lifePoints;}
	inline const float& GetCurrentLife() const {return mCurrentLife;}
	inline void ComputeLife(const float& damages) 
	{
		mCurrentLife -= damages;
		if(mCurrentLife > mLifePoints)
			mCurrentLife = mLifePoints;
		else if(mCurrentLife < 0.0f)
			mCurrentLife = 0.0f;
	}
	inline const float& GetTimeBeforeRecovery() const {return mTimeBeforeRecovery;}
	inline void SetTimeBeforeRecovery(const float& timeBeforeRecovery) {mTimeBeforeRecovery = timeBeforeRecovery;}

	inline const b2Vec2& GetPosition() const {return mPos;}
	inline void SetPosition(const b2Vec2& pos) {mPos = pos;}

	inline void DeleteExplosionPS() {SAFE_DELETE(mExplosionsPS);}
	inline void AddExplosionPS(hgeParticleSystem* explosionsPS) {SAFE_DELETE(mExplosionsPS); mExplosionsPS = explosionsPS;}
	inline void Explode() {if(mExplosionsPS) mExplosionsPS->Fire();}

	inline hgeParticleSystem* GetExplosionPS() {return mExplosionsPS;}

	inline virtual void Update(float dt)
	{
		if(mCurrentLife <= 0.0f)
			mRegenTimer += dt;
		if(mRegenTimer >= mTimeBeforeRecovery)
			mRegenTimer = 0.0f;
		if(mRegenTimer == 0.0f)
			ComputeLife(-mRegenSpeed*dt);

		if(mExplosionsPS)
			mExplosionsPS->Update(dt);
	}

	inline const float& GetRegenSpeed() const {return mRegenSpeed;}
	inline void SetRegenSpeed(const float& regenSpeed) {mRegenSpeed = regenSpeed;}

	inline const bool IsEquipped() const {return mEquipped;}
	inline void SetEquipped(bool enable) 
	{
		mEquipped = enable; 
		if(!enable)
			DeleteExplosionPS();
	}

	inline const string& GetStatDesc() const {return mStatDesc;}
	virtual void ComputeStatDesc() = 0;
	inline const string& GetName() const {return mName;}
	inline void SetName(const string& name) {mName = name;}
	inline const string& GetDesc() const {return mDesc;}
	inline void SetDesc(const string& desc) {mDesc = desc;}

	inline const u32& GetId() {return mId;}
	inline void SetId(const u32& id) {mId = id;}

protected:
	u32 mId;

	ESquareTileType mType;

	float mLifePoints;
	float mCurrentLife;

	b2Vec2 mPos;// position relative par rapport au centre d'inertie du squareship

	hgeParticleSystem* mExplosionsPS;

	float mRegenSpeed;

	float mRegenTimer;
	float mTimeBeforeRecovery;

	bool mEquipped;

	string mStatDesc;
	string mName;
	string mDesc;
};


class CSquareTileEngine : public CSquareTile
{
public:
	CSquareTileEngine(const b2Vec2& pos):CSquareTile(ENGINE, pos)
	{
		mEngineAcceleration = 200000.0f;
		
		mName = "Réacteur standard classe E\n";
		mDesc = "Matériel d'entraînement de la SquareForce.\n";

		ComputeStatDesc();
	}
	CSquareTileEngine(CSquareTileEngine* tile):CSquareTile(tile)
	{
		mEngineAcceleration = tile->GetEngineAcceleration();
	}
	virtual ~CSquareTileEngine() {}

	inline const float& GetEngineAcceleration() const {return mEngineAcceleration;}
	inline void SetEngineAcceleration(const float& engineAcceleration) {mEngineAcceleration = engineAcceleration;}

	inline virtual void Update(float dt) {CSquareTile::Update(dt);}

	inline void ComputeStatDesc()
	{
		CLocalization* instance = CLocalization::GetInstance();
		mStatDesc = "";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_HULL) + " : ";
		mStatDesc += FloatToString(mLifePoints);
		mStatDesc += " \n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_REGEN_SPEED) + " : ";
		mStatDesc += FloatToString(mRegenSpeed);
		mStatDesc += " cel/s\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_RECOVERY_TIME) + " : ";
		mStatDesc += FloatToString(mTimeBeforeRecovery);
		mStatDesc += " s\n\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_LINEAR_ACCELERATION) + " : ";
		mStatDesc += FloatToString(mEngineAcceleration);
		mStatDesc += " m²/s\n";
		//mStatDesc += "Acceleration angulaire : " + mEngineAcceleration + " rad²/s\n"
	}

	list<b2Vec2> mListPos;

protected:
	float mEngineAcceleration;// accélération fournie par les moteurs
};


class CSquareTileHull : public CSquareTile
{
public:
	CSquareTileHull(const b2Vec2& pos):CSquareTile(HULL, pos) 
	{
		mName = "Coque standard classe E\n";
		mDesc = "Matériel d'entraînement de la SquareForce.\n";

		ComputeStatDesc();
	}
	CSquareTileHull(CSquareTileHull *tile):CSquareTile(tile) 
	{
	}
	virtual ~CSquareTileHull() {}

	inline virtual void Update(float dt) {CSquareTile::Update(dt);}

	inline void ComputeStatDesc()
	{
		CLocalization* instance = CLocalization::GetInstance();
		mStatDesc = "";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_HULL) + " : ";
		mStatDesc += FloatToString(mLifePoints);
		mStatDesc += " \n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_REGEN_SPEED) + " : ";
		mStatDesc += FloatToString(mRegenSpeed);
		mStatDesc += " cel/s\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_RECOVERY_TIME) + " : ";
		mStatDesc += FloatToString(mTimeBeforeRecovery);
		mStatDesc += " s\n\n";
	}
protected:

};


class CSquareTileGun : public CSquareTile
{
public:
	CSquareTileGun(const b2Vec2& pos):CSquareTile(GUN, pos) 
	{
		mMissileSpeed = GUN_MISSILES_SPEED;
		mMissileFrequency = 10.0f;
		mShotPeriode = 1.0f/mMissileFrequency;

		mTimer = 0.0f;

		mHullDammages = 4.0f;

		mName = "Canon à photons pulsés classe E\n";
		mDesc = "Matériel d'entraînement de la SquareForce.\n";

		ComputeStatDesc();
	}
	CSquareTileGun(CSquareTileGun *tile):CSquareTile(tile) 
	{
		mMissileSpeed = tile->GetMissileSpeed();
		mMissileFrequency = tile->GetMissileFrequency();
		mShotPeriode = 1.0f/mMissileFrequency;

		mTimer = 0.0f;

		mHullDammages = tile->GetHullDammages();
	}
	virtual ~CSquareTileGun() {}

	inline const float& GetMissileSpeed() const {return mMissileSpeed;}
	inline void SetMissileSpeed(const float& missileSpeed) {mMissileSpeed = missileSpeed;}

	inline const float& GetMissileFrequency() const {return mMissileFrequency;}
	inline void SetMissileFrequency(const float& missileFrequency) {mMissileFrequency = missileFrequency; mShotPeriode = 1.0f/mMissileFrequency;}

	inline void Update(float dt) 
	{
		CSquareTile::Update(dt);

		mTimer += dt; 
		if(mTimer > mShotPeriode) 
			mTimer = mShotPeriode;
	}
	inline bool Fire() 
	{
		bool canFire = (mTimer >= mShotPeriode); 
		if(canFire) 
			mTimer = 0.0f; 
		return canFire;
	}

	inline const float& GetHullDammages() const {return mHullDammages;}
	inline void SetHullDamages(const float& hullDammages) {mHullDammages = hullDammages;}

	inline void ComputeStatDesc()
	{
		CLocalization* instance = CLocalization::GetInstance();
		mStatDesc = "";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_HULL) + " : ";
		mStatDesc += FloatToString(mLifePoints);
		mStatDesc += " \n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_REGEN_SPEED) + " : ";
		mStatDesc += FloatToString(mRegenSpeed);
		mStatDesc += " cel/s\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_RECOVERY_TIME) + " : ";
		mStatDesc += FloatToString(mTimeBeforeRecovery);
		mStatDesc += " s\n\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_MISSILE_SPEED) + " : ";
		mStatDesc += FloatToString(mMissileSpeed);
		mStatDesc += " m/s\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_MISSILE_CADENCY) + " : ";
		mStatDesc += FloatToString(mMissileFrequency);
		mStatDesc += " Hz\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_MISSILE_DAMAGES) + " : ";
		mStatDesc += FloatToString(mHullDammages);
		mStatDesc += " \n";
	}

protected:
	float mMissileSpeed;// vitesse des projectiles
	float mMissileFrequency;// fréquence de tir
	float mShotPeriode;// période entre 2 tirs (inverse de la fréquence)

	float mTimer;// timer pr déterminer l'instant du tir

	float mHullDammages;// dégats coque par coups

};

class CSquareTileMissile : public CSquareTile
{
public:
	CSquareTileMissile(const b2Vec2& pos):CSquareTile(MISSILE, pos)
	{
		mMissileSpeed = 200.0f;
		mMissileFrequency = 0.5f;

		mHullDammages = 50.0f;

		mName = "Lance-missiles standard classe E\n";
		mDesc = "Matériel d'entraînement de la SquareForce.\n";

		ComputeStatDesc();
	}
	CSquareTileMissile(CSquareTileMissile *tile):CSquareTile(tile) 
	{
		mMissileSpeed = tile->GetMissileSpeed();
		mMissileFrequency = tile->GetMissileFrequency();
		
		mHullDammages = tile->GetHullDammages();
	}
	virtual ~CSquareTileMissile() {}

	inline const float& GetMissileSpeed() const {return mMissileSpeed;}
	inline void SetMissileSpeed(const float& missileSpeed) {mMissileSpeed = missileSpeed;}

	inline const float& GetMissileFrequency() const {return mMissileFrequency;}
	inline void SetMissileFrequency(const float& missileFrequency) {mMissileFrequency = missileFrequency;}

	inline const float& GetHullDammages() const {return mHullDammages;}
	inline void SetHullDammages(const float& hullDammages) {mHullDammages = hullDammages;}

	inline virtual void Update(float dt) {CSquareTile::Update(dt);}

	inline void ComputeStatDesc()
	{
		CLocalization* instance = CLocalization::GetInstance();
		mStatDesc = "";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_HULL) + " : ";
		mStatDesc += FloatToString(mLifePoints);
		mStatDesc += " \n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_REGEN_SPEED) + " : ";
		mStatDesc += FloatToString(mRegenSpeed);
		mStatDesc += " cel/s\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_RECOVERY_TIME) + " : ";
		mStatDesc += FloatToString(mTimeBeforeRecovery);
		mStatDesc += " s\n\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_MISSILE_SPEED) + " : ";
		mStatDesc += FloatToString(mMissileSpeed);
		mStatDesc += " m/s\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_MISSILE_CADENCY) + " : ";
		mStatDesc += FloatToString(mMissileFrequency);
		mStatDesc += " Hz\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_MISSILE_DAMAGES) + " : ";
		mStatDesc += FloatToString(mHullDammages);
		mStatDesc += " \n";
	}

protected:
	float mMissileSpeed;// vitesse des projectiles
	float mMissileFrequency;// fréquence de tir

	float mHullDammages;// dégats coque par coups

};


class CSquareTileMine : public CSquareTile
{
public:
	CSquareTileMine(const b2Vec2& pos):CSquareTile(MINE, pos) 
	{
		mMissileSpeed = 0.0f;
		mMissileFrequency = 2.0f;

		mHullDammages = 20.0f;

		mName = "Lanceur de mines standard classe E\n";
		mDesc = "Matériel d'entraînement de la SquareForce.\n";

		ComputeStatDesc();
	}
	CSquareTileMine(CSquareTileMine *tile):CSquareTile(tile) 
	{
		mMissileSpeed = tile->GetMissileSpeed();
		mMissileFrequency = tile->GetMissileFrequency();

		mHullDammages = tile->GetHullDammages();
	}
	virtual ~CSquareTileMine() {}

	inline const float& GetMissileSpeed() const {return mMissileSpeed;}
	inline void SetMissileSpeed(const float& missileSpeed) {mMissileSpeed = missileSpeed;}

	inline const float& GetMissileFrequency() const {return mMissileFrequency;}
	inline void SetMissileFrequency(const float& missileFrequency) {mMissileFrequency = missileFrequency;}

	inline const float& GetHullDammages() const {return mHullDammages;}
	inline void SetHullDammages(const float& hullDammages) {mHullDammages = hullDammages;}

	inline virtual void Update(float dt) {CSquareTile::Update(dt);}

	inline void ComputeStatDesc()
	{
		CLocalization* instance = CLocalization::GetInstance();
		mStatDesc = "";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_HULL) + " : ";
		mStatDesc += FloatToString(mLifePoints);
		mStatDesc += " \n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_REGEN_SPEED) + " : ";
		mStatDesc += FloatToString(mRegenSpeed);
		mStatDesc += " cel/s\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_RECOVERY_TIME) + " : ";
		mStatDesc += FloatToString(mTimeBeforeRecovery);
		mStatDesc += " s\n\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_MISSILE_SPEED) + " : ";
		mStatDesc += FloatToString(mMissileSpeed);
		mStatDesc += " m/s\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_MISSILE_CADENCY) + " : ";
		mStatDesc += FloatToString(mMissileFrequency);
		mStatDesc += " Hz\n";
		mStatDesc += instance->GetSquareTilesToken(CLocalization::TILE_TOKEN_MISSILE_DAMAGES) + " : ";
		mStatDesc += FloatToString(mHullDammages);
		mStatDesc += " \n";
	}

protected:
	float mMissileSpeed;// vitesse des projectiles
	float mMissileFrequency;// fréquence de tir

	float mHullDammages;// dégats coque par coups

};

#endif

