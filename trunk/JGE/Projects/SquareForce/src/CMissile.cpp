#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CMissile.h"
#include "CObject.h"


CMissile::CMissile(float lifeTime, JQuad* quad, CSquareShip* owner, float damages)
:mQuad(quad), mLifeTime(lifeTime), mOwner(owner), mDamages(damages)
{
	mPS = NULL;

	mPosition = mLastPosition = b2Vec2(0, 0);
	mVelocity = b2Vec2(0, 0);

	mParticle = new hgeParticle;

	mParticle->colColor = hgeColor(0, 1, 0, 1);
	mParticle->colColorDelta = hgeColor(0, 0, 0, 0);
	mParticle->fAge = 0;
	mParticle->fGravity = 0;
	mParticle->fRadialAccel = 0;
	mParticle->fSize = 0.2f;
	mParticle->fSizeDelta = 0.0f;
	mParticle->fSpin = 0;
	mParticle->fSpinDelta = 0;
	mParticle->fTangentialAccel = 0;
	mParticle->fTerminalAge = lifeTime;
	mParticle->vecLocation = hgeVector(0, 0);
	mParticle->vecVelocity = hgeVector(0, 0);
}

CMissile::~CMissile()
{
	SAFE_DELETE(mParticle);

	//SAFE_DELETE(mPS);
}

void CMissile::CreateParticleSystem(const hgeParticleSystem& ps)
{
	hgeParticleSystem* PS = new hgeParticleSystem(ps);
	AddPS(PS);	
}

void CMissile::AddPS(hgeParticleSystem* ps)
{
	//SAFE_DELETE(mPS);
	mPS = ps;
	//mPS->MoveTo(mPosition.x, mPosition.y, true);
	mPS->Fire();
}

void CMissile::SetPosition(const b2Vec2 &position)
{
	mPosition = mLastPosition = position;
	mParticle->vecLocation.x = position.x;
	mParticle->vecLocation.y = position.y;
}

void CMissile::SetVelocity(const b2Vec2 &velocity)
{
	mVelocity = velocity;
	mParticle->vecVelocity.x = velocity.x;
	mParticle->vecVelocity.y = velocity.y;
}

void CMissile::Update(float deltaTime)
{
	mLastPosition = mPosition;
	mPosition += deltaTime * mVelocity;
	mParticle->vecLocation = hgeVector(mPosition.x, mPosition.y);

	mParticle->fSpin += mParticle->fSpinDelta * deltaTime;
	mParticle->fSize += mParticle->fSizeDelta * deltaTime;
	mParticle->colColor += mParticle->colColorDelta * deltaTime;

	mParticle->fAge += deltaTime;

	if(mPS)
	{
		//mPS->MoveTo(mPosition.x, mPosition.y, true);
		//mPS->Update(deltaTime);
	}
}


void CMissile::Render(const b2Vec2& camPos, const float32& camRot, const b2Mat22& camMat)
{
	JRenderer* renderer = JRenderer::GetInstance();

	float size = mParticle->fSize;

	b2Vec2 position = b2MulT(camMat, mPosition-camPos);
	float32 rotation = mParticle->fSpin-camRot;

	if(mPS)
	{
		mPS->Transpose(mPosition.x, mPosition.y);
		mPS->RenderLocal(hgeVector(camPos.x, camPos.y), camRot);
	}

	mQuad->SetColor(mParticle->colColor.GetHWColor());
	renderer->RenderQuad(mQuad, SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, -rotation, size, size);

}
