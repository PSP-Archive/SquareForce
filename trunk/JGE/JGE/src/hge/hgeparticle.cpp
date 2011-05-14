/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleSystem helper class implementation
*/

#include "..\..\include\JGE.h"
#include "..\..\include\JTypes.h"
#include "..\..\include\JRenderer.h"
#include "..\..\include\JFileSystem.h"

#include "..\..\include\hge\hgeparticle.h"

#ifdef PSP
#include <pspmath.h>
#endif

//HGE	*hgeParticleSystem::hge=0;

/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: random number generation
*/


unsigned int g_seed=0;

void Random_Seed(int seed)
{
	if(!seed) g_seed=JGE::GetInstance()->GetTime();
	else g_seed=seed;
#ifdef PSP
	vfpu_srand(g_seed);
#endif
}

int Random_Int(int min, int max)
{
#ifdef PSP
	return vfpu_rand_8888(min, max);
#else
	g_seed=214013*g_seed+2531011;
	return min+(g_seed ^ g_seed>>15)%(max-min+1);
#endif
}

float Random_Float(float min, float max)
{
#ifdef PSP
	return vfpu_randf(min, max);
#else
	g_seed=214013*g_seed+2531011;
	return min+(g_seed>>16)*(1.0f/65535.0f)*(max-min);
#endif
}




hgeParticleSystem::hgeParticleSystem(const char *filename, JQuad *sprite)
{
	//void *psi;
	//hgeParticleSystemInfo psi;

	JFileSystem* fileSys = JFileSystem::GetInstance();
	//hge=hgeCreate(HGE_VERSION);

	//psi=hge->Resource_Load(filename);
	if (!fileSys->OpenFile(filename)) return;

	//if(!psi) return;

	//memcpy(&info, psi, sizeof(hgeParticleSystemInfo));
	//hge->Resource_Free(psi);

	fileSys->ReadFile(&info, sizeof(hgeParticleSystemInfo));
	fileSys->CloseFile();

	fSpreadCoeff = 1.0f - fabs(info.fSpread)/M_PI;
	vDirection = Vector2D(1,0).Rotate(info.fDirection);

	info.sprite=sprite;
	//  	info.fGravityMin *= 100;
	//  	info.fGravityMax *= 100;
	// 	info.fSpeedMin *= 100;
	// 	info.fSpeedMax *= 100;

	vecLocation.x=vecPrevLocation.x=0.0f;
	vecLocation.y=vecPrevLocation.y=0.0f;
	fTx=fTy=0;

	fEmissionResidue=0.0f;
	nParticlesAlive=0;
	fAge=-2.0;

	rectBoundingBox.Clear();
	bUpdateBoundingBox=false;

	nMaxParticles = info.fParticleLifeMax*info.nEmission;
	if(nMaxParticles > MAX_PARTICLES)
		nMaxParticles = MAX_PARTICLES;
	particles = new hgeParticle[nMaxParticles];
}

hgeParticleSystem::hgeParticleSystem(hgeParticleSystemInfo *psi)
{
	//hge=hgeCreate(HGE_VERSION);

	memcpy(&info, psi, sizeof(hgeParticleSystemInfo));

	fSpreadCoeff = 1.0f - fabs(info.fSpread)/M_PI;
	vDirection = Vector2D(1,0).Rotate(info.fDirection);

	vecLocation.x=vecPrevLocation.x=0.0f;
	vecLocation.y=vecPrevLocation.y=0.0f;
	fTx=fTy=0;

	fEmissionResidue=0.0f;
	nParticlesAlive=0;
	fAge=-2.0;

	rectBoundingBox.Clear();
	bUpdateBoundingBox=false;

	nMaxParticles = info.fParticleLifeMax*info.nEmission;
	if(nMaxParticles > MAX_PARTICLES)
		nMaxParticles = MAX_PARTICLES;
	particles = new hgeParticle[nMaxParticles];
}

hgeParticleSystem::hgeParticleSystem(const hgeParticleSystem &ps)
{
	memcpy(this, &ps, sizeof(hgeParticleSystem));
	//hge=hgeCreate(HGE_VERSION);
}

void hgeParticleSystem::Update(float fDeltaTime)
{
	int i;
	float ang;
	hgeParticle *par;
	Vector2D vecAccel, vecAccel2;

	if(fAge >= 0)
	{
		fAge += fDeltaTime;
		if(fAge >= info.fLifetime) fAge = -2.0f;
	}

	mTimer += fDeltaTime;
	if (mTimer < 0.01f)
		return;

	fDeltaTime = mTimer;
	mTimer = 0.0f;


	int maxParticles = info.fParticleLifeMax*info.nEmission;
	if(maxParticles > MAX_PARTICLES)
		maxParticles = MAX_PARTICLES;
	if( nMaxParticles != maxParticles)
	{
		hgeParticle* p = particles;
		particles = new hgeParticle[maxParticles];
		memcpy(particles, p, nParticlesAlive*sizeof(hgeParticle));
		delete[] p;
		nMaxParticles = maxParticles;
	}

	// update all alive particles

	if(bUpdateBoundingBox) rectBoundingBox.Clear();
	par=particles;

	i = nParticlesAlive+1;
	while(--i)
	{
		par->fAge += fDeltaTime;
		if(par->fAge >= par->fTerminalAge)
		{
			--nParticlesAlive;
			memcpy(par, &particles[nParticlesAlive], sizeof(hgeParticle));
			continue;
		}

		vecAccel = par->vecLocation-vecLocation;
		vecAccel.Normalize();
		float tgAcc = par->fTangentialAccel;
		vecAccel2 = Vector2D(-vecAccel.y*tgAcc, vecAccel.x*tgAcc);
		vecAccel *= par->fRadialAccel;

		par->vecVelocity += (vecAccel+vecAccel2)*fDeltaTime;
		par->vecVelocity.y += par->fGravity*fDeltaTime;

		//par->vecVelocity.y = 0.1f;
		par->vecLocation += par->vecVelocity;

		par->fSpin += par->fSpinDelta*fDeltaTime;
		par->fSize += par->fSizeDelta*fDeltaTime;
		par->colColor += par->colColorDelta*fDeltaTime;

		if(bUpdateBoundingBox) rectBoundingBox.Encapsulate(par->vecLocation.x, par->vecLocation.y);

		++par;
	}

	// generate new particles

	if(fAge != -2.0f)
	{
		float fParticlesNeeded = info.nEmission*fDeltaTime + fEmissionResidue;
		int nParticlesCreated = (unsigned int)fParticlesNeeded;
		fEmissionResidue=fParticlesNeeded-nParticlesCreated;

		par=particles+nParticlesAlive;

		i = nParticlesCreated+1;
		while(--i)
		{
			if(nParticlesAlive>=nMaxParticles) break;

			par->fAge = 0.0f;
			par->fTerminalAge = Random_Float(info.fParticleLifeMin, info.fParticleLifeMax);

			par->vecLocation = vecPrevLocation+(vecLocation-vecPrevLocation)*Random_Float(0.0f, 1.0f);
			par->vecLocation.x += Random_Float(-2.0f, 2.0f);
			par->vecLocation.y += Random_Float(-2.0f, 2.0f);

			ang = Random_Float(fSpreadCoeff, 1.0f);
			Vector2D dir;
			if(info.bRelative)
				dir = vecPrevLocation - vecLocation;
			else
				dir = vDirection;
			Vector2D dirN(-dir.y, dir.x);
			if(Random_Float(-1,1) > 0)
				par->vecVelocity = ang*dir + (1.0f-fabs(ang))*dirN;// left
			else
				par->vecVelocity = ang*dir - (1.0f-fabs(ang))*dirN;// right
			par->vecVelocity.Normalize();
			par->vecVelocity *= Random_Float(info.fSpeedMin, info.fSpeedMax);

			par->fGravity = Random_Float(info.fGravityMin, info.fGravityMax);
			par->fRadialAccel = Random_Float(info.fRadialAccelMin, info.fRadialAccelMax);
			par->fTangentialAccel = Random_Float(info.fTangentialAccelMin, info.fTangentialAccelMax);

			par->fSize = Random_Float(info.fSizeStart, info.fSizeStart+(info.fSizeEnd-info.fSizeStart)*info.fSizeVar);
			par->fSizeDelta = (info.fSizeEnd-par->fSize) / par->fTerminalAge;

			par->fSpin = Random_Float(info.fSpinStart, info.fSpinStart+(info.fSpinEnd-info.fSpinStart)*info.fSpinVar);
			par->fSpinDelta = (info.fSpinEnd-par->fSpin) / par->fTerminalAge;

			par->colColor.r = Random_Float(info.colColorStart.r, info.colColorStart.r+(info.colColorEnd.r-info.colColorStart.r)*info.fColorVar);
			par->colColor.g = Random_Float(info.colColorStart.g, info.colColorStart.g+(info.colColorEnd.g-info.colColorStart.g)*info.fColorVar);
			par->colColor.b = Random_Float(info.colColorStart.b, info.colColorStart.b+(info.colColorEnd.b-info.colColorStart.b)*info.fColorVar);
			par->colColor.a = Random_Float(info.colColorStart.a, info.colColorStart.a+(info.colColorEnd.a-info.colColorStart.a)*info.fAlphaVar);

			par->colColorDelta = (info.colColorEnd-par->colColor) / par->fTerminalAge;
			
			if(bUpdateBoundingBox) rectBoundingBox.Encapsulate(par->vecLocation.x, par->vecLocation.y);

			++nParticlesAlive;
			++par;
		}
	}

	vecPrevLocation=vecLocation;
}

void hgeParticleSystem::MoveTo(float x, float y, bool bMoveParticles)
{	
	if(bMoveParticles)
	{
		Vector2D d(x-vecLocation.x, y-vecLocation.y);
		hgeParticle* par = particles;

		int i = nParticlesAlive+1;
		while(--i)
			par->vecLocation += d;

		vecPrevLocation += d;
	}
	else
	{
		if(fAge==-2.0)
		{
			vecPrevLocation.x = x; 
			vecPrevLocation.y = y; 
		}
		else 
			vecPrevLocation = vecLocation; 
	}

	vecLocation.x = x;
	vecLocation.y = y;
}

void hgeParticleSystem::FireAt(float x, float y)
{
	Stop();
	MoveTo(x,y);
	Fire();
}

void hgeParticleSystem::Fire()
{
	mTimer = 0.0f;

	if(info.fLifetime==-1.0f) 
		fAge=-1.0f;
	else 
		fAge=0.0f;
}

void hgeParticleSystem::Stop(bool bKillParticles)
{
	fAge=-2.0f;
	if(bKillParticles) 
	{
		nParticlesAlive=0;
		rectBoundingBox.Clear();
	}
}

void hgeParticleSystem::Render()
{
	int i = nParticlesAlive+1;
	//	DWORD col;
	hgeParticle *par=particles;

	//col=info.sprite->GetColor();

	while(--i)
	{
		info.sprite->SetColor(par->colColor.GetHWColor());
		JRenderer::GetInstance()->RenderQuad(info.sprite, par->vecLocation.x+fTx, par->vecLocation.y+fTy, par->fSpin*par->fAge, par->fSize, par->fSize);
		++par;
	}

	//info.sprite->SetColor(col);
}

void hgeParticleSystem::RenderLocal(const Vector2D& localPos, const float& localAngle, const Matrix22& localMat)
{
	int i = nParticlesAlive+1;
	hgeParticle *par=particles;

	while(--i)
	{
		Vector2D pos = localMat / Vector2D(par->vecLocation.x+fTx-localPos.x, par->vecLocation.y+fTy-localPos.y);

		info.sprite->SetColor(par->colColor.GetHWColor());
		JRenderer::GetInstance()->RenderQuad(info.sprite, 240+pos.x, 136-pos.y, -(par->fSpin*par->fAge-localAngle), par->fSize, par->fSize);
		++par;
	}
}
