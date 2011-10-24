/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleManager helper class implementation
*/


#include "..\..\include\hge\hgeparticle.h"


hgeParticleManager::hgeParticleManager()
{
	nPS=0;
	tX=tY=0.0f;
}

hgeParticleManager::~hgeParticleManager()
{
	int i;
	for(i=0;i<nPS;i++) 
		delete psList[i];
}

void hgeParticleManager::Update(float dt)
{
	int i = nPS+1;
	hgeParticleSystem** ps = psList;
	while(--i)
	{
		(*ps)->Update(dt);
		if((*ps)->GetAge()==-2.0f && (*ps)->GetParticlesAlive()==0)
		{
			--nPS;
			delete (*ps);
			if(i>1)
				memcpy(ps, (ps+1), (i-1)*sizeof(hgeParticleSystem*));
			continue;
		}

		++ps;
	}
}

void hgeParticleManager::Render()
{
	int i = nPS+1;
	hgeParticleSystem** ps = psList;
	while(--i)
	{
		(*ps)->Render();
		++ps;
	}
}

void hgeParticleManager::RenderLocal(const Vector2D& localPos, const float& localAngle, const Matrix22& localMat)
{
	int i = nPS+1;
	hgeParticleSystem** ps = psList;
	while(--i)
	{
		(*ps)->RenderLocal(localPos, localAngle, localMat);
		++ps;
	}
}

hgeParticleSystem* hgeParticleManager::SpawnPS(hgeParticleSystemInfo *psi, float x, float y)
{
	if(nPS==MAX_PSYSTEMS) 
		return 0;
	psList[nPS]=new hgeParticleSystem(psi);
	psList[nPS]->FireAt(x,y);
	psList[nPS]->Transpose(tX,tY);
	nPS++;
	return psList[nPS-1];
}

bool hgeParticleManager::IsPSAlive(hgeParticleSystem *ps) const
{
	int i;
	for(i=0;i<nPS;i++) 
		if(psList[i]==ps) 
			return true;
	return false;
}

void hgeParticleManager::Transpose(float x, float y)
{
	int i;
	for(i=0;i<nPS;i++) 
		psList[i]->Transpose(x,y);
	tX=x; tY=y;
}

void hgeParticleManager::KillPS(hgeParticleSystem *ps)
{
	int i;
	for(i=0;i<nPS;i++)
	{
		if(psList[i]==ps)
		{
			delete psList[i];
			psList[i]=psList[nPS-1];
			nPS--;
			return;
		}
	}
}

void hgeParticleManager::KillAll()
{
	int i;
	for(i=0;i<nPS;i++) 
		delete psList[i];
	nPS=0;
}
