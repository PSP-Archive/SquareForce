#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CMissile.h"
#include "CObject.h"


CMissile::CMissile(float lifeTime, JQuad* quad, CSquareShip* owner, float damages)
:mQuad(quad), mLifeTime(lifeTime), mOwner(owner), mDamages(damages)
{
	mPS = NULL;

	mPosition = mLastPosition = Vector2D(0, 0);
	mVelocity = Vector2D(0, 0);

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
	mParticle->vecLocation = Vector2D(0, 0);
	mParticle->vecVelocity = Vector2D(0, 0);
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

void CMissile::SetPosition(const Vector2D &position)
{
	mPosition = mLastPosition = position;
	mParticle->vecLocation.x = position.x;
	mParticle->vecLocation.y = position.y;
}

void CMissile::SetVelocity(const Vector2D &velocity)
{
	mVelocity = velocity;
	mParticle->vecVelocity.x = velocity.x;
	mParticle->vecVelocity.y = velocity.y;
}

void CMissile::Update(float deltaTime)
{
	mLastPosition = mPosition;
	mPosition += deltaTime * mVelocity;
	mParticle->vecLocation = Vector2D(mPosition.x, mPosition.y);

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


void CMissile::Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat)
{
	if( (camPos - mPosition).Length2() > 90000.0f ) 
		return;

	JRenderer* renderer = JRenderer::GetInstance();

	float size = mParticle->fSize;

	Vector2D position = camMat / (mPosition-camPos);
	float rotation = mParticle->fSpin-camRot;

	if(mPS)
	{
		mPS->Transpose(mPosition.x, mPosition.y);
		mPS->RenderLocal(Vector2D(camPos.x, camPos.y), camRot, camMat);
	}

	mQuad->SetColor(mParticle->colColor.GetHWColor());
	renderer->RenderQuad(mQuad, SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, -rotation, size, size);

}
