/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleSystem helper class header
*/


#ifndef HGEPARTICLE_H
#define HGEPARTICLE_H


//#include "hge.h"
//#include "hgesprite.h"
#include "../Vector2D.h"
#include "hgecolor.h"
#include "hgerect.h"

class JQuad;

#define MAX_PARTICLES	500
#define MAX_PSYSTEMS	500

struct hgeParticle
{
	Vector2D	vecLocation;
	Vector2D	vecVelocity;

	float		fGravity;
	float		fRadialAccel;
	float		fTangentialAccel;

	float		fSpin;
	float		fSpinDelta;

	float		fSize;
	float		fSizeDelta;

	hgeColor	colColor;		// + alpha
	hgeColor	colColorDelta;

	float		fAge;
	float		fTerminalAge;
};

struct hgeParticleSystemInfo
{
	hgeParticleSystemInfo() {memset(this, 0, sizeof(this));}

	JQuad*		sprite;    // texture + blend mode
	int			nEmission; // particles per sec
	float		fLifetime;

	float		fParticleLifeMin;
	float		fParticleLifeMax;

	float		fDirection;
	float		fSpread;
	bool		bRelative;
	bool		bAutoSpin;

	float		fSpeedMin;
	float		fSpeedMax;

	float		fGravityMin;
	float		fGravityMax;

	float		fRadialAccelMin;
	float		fRadialAccelMax;

	float		fTangentialAccelMin;
	float		fTangentialAccelMax;

	float		fSizeStart;
	float		fSizeEnd;
	float		fSizeVar;

	float		fSpinStart;
	float		fSpinEnd;
	float		fSpinVar;

	hgeColor	colColorStart; // + alpha
	hgeColor	colColorEnd;
	float		fColorVar;
	float		fAlphaVar;
};

class hgeParticleSystem
{
public:
	hgeParticleSystemInfo info;

	hgeParticleSystem(const char *filename, JQuad *sprite);
	hgeParticleSystem(hgeParticleSystemInfo *psi);
	hgeParticleSystem(const hgeParticleSystem &ps);
	~hgeParticleSystem() { delete[] particles;}

	hgeParticleSystem&	operator= (const hgeParticleSystem &ps);


	void				Render();
	void				RenderLocal(const Vector2D& localPos, const float& localAngle, const Matrix22& localMat);
	void				FireAt(float x, float y);
	void				Fire();
	void				Stop(bool bKillParticles=false);
	void				Update(float fDeltaTime);
	void				MoveTo(float x, float y, bool bMoveParticles=false);
	void				Transpose(float x, float y) { fTx=x; fTy=y; }
	void				TrackBoundingBox(bool bTrack) { bUpdateBoundingBox=bTrack; }

	int					GetParticlesAlive() const { return nParticlesAlive; }
	float				GetAge() const { return fAge; }
	void				GetPosition(float *x, float *y) const { *x=vecLocation.x; *y=vecLocation.y; }
	void				GetTransposition(float *x, float *y) const { *x=fTx; *y=fTy; }
	hgeRect*			GetBoundingBox(hgeRect *rect) const { memcpy(rect, &rectBoundingBox, sizeof(hgeRect)); return rect; }

	// NEW : vector direction
	Vector2D			vDirection;
	float				fSpreadCoeff;// NEW spread coeff conversion : value from -1(360�) to 1(0�)

protected:
	hgeParticleSystem();

	//static HGE			*hge;

	float				fAge;
	float				fEmissionResidue;

	Vector2D			vecPrevLocation;
	Vector2D			vecLocation;
	float				fTx, fTy;

	int					nMaxParticles;
	int					nParticlesAlive;
	hgeRect				rectBoundingBox;
	bool				bUpdateBoundingBox;

	hgeParticle*		particles;

	float				mTimer;

};

class hgeParticleManager
{
public:
	hgeParticleManager();
	~hgeParticleManager();

	void				Update(float dt);
	void				Render();
	void				RenderLocal(const Vector2D& localPos, const float& localAngle, const Matrix22& localMat);

	hgeParticleSystem*	SpawnPS(hgeParticleSystemInfo *psi, float x, float y);
	bool				IsPSAlive(hgeParticleSystem *ps) const;
	void				Transpose(float x, float y);
	void				GetTransposition(float *dx, float *dy) const {*dx=tX; *dy=tY;}
	void				KillPS(hgeParticleSystem *ps);
	void				KillAll();

	inline int			GetNbParticleSystems() {return nPS;}

private:
	hgeParticleManager(const hgeParticleManager &);
	hgeParticleManager&	operator= (const hgeParticleManager &);

	int					nPS;
	float				tX;
	float				tY;
	hgeParticleSystem*	psList[MAX_PSYSTEMS];
};


#endif
