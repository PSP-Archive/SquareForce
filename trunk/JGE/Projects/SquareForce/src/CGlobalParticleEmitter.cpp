#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CGlobalParticleEmitter.h"
#include "CObject.h"


CGlobalParticleEmitter::CGlobalParticleEmitter(int nbParticles, JQuad* quad, float zoom, hgeColor startColor, hgeColor deltaColor)
{
	mRadius = 300.0f;

	mZoom = zoom;

	mNbParticles = nbParticles;
	mParticles = new hgeParticle[nbParticles];
	PspAssert(mParticles);

	mPos = Vector2D(0,0);

	mStartColor = startColor;
	mDeltaColor = deltaColor;

	float popSize = mRadius+mZoom*GLOBAL_PARTICLE_SCALE*22.63f;

	hgeParticle* par = mParticles;
	for(int i=0; i<mNbParticles; i++)
	{
		memset(par, 0, sizeof(hgeParticle));
		hgeColor deltaColor = hgeColor(
			Random(0.0f, mDeltaColor.r), 
			Random(0.0f, mDeltaColor.g), 
			Random(0.0f, mDeltaColor.b), 
			Random(0.0f, mDeltaColor.a));
		par->colColor = mStartColor+deltaColor;

		par->fSize = Random(0.2f, 1.0f);
		par->fSpin = Random(0.0f, 360.0f);

		float x = Random(-popSize, popSize);
		float y = Random(-popSize, popSize);

		par->vecLocation = mPos+Vector2D(x, y);
		par->vecVelocity = Vector2D(0,0);

		par++;
	}

	mQuad = quad;
}

CGlobalParticleEmitter::~CGlobalParticleEmitter()
{
	SAFE_DELETE_ARRAY(mParticles);
}

void CGlobalParticleEmitter::ChangeColors(hgeColor startColor /*= hgeColor(1, 1, 1, 1)*/, hgeColor deltaColor /*= hgeColor(0, 0, 0, 0)*/)
{
	mStartColor = startColor;
	mDeltaColor = deltaColor;

	hgeParticle* par = mParticles;
	int i = mNbParticles+1;
	while(--i)
	{
		hgeColor deltaColor = hgeColor(
			Random(0.0f, mDeltaColor.r), 
			Random(0.0f, mDeltaColor.g), 
			Random(0.0f, mDeltaColor.b), 
			Random(0.0f, mDeltaColor.a));
		par->colColor = mStartColor+deltaColor;

		++par;
	}
}

void CGlobalParticleEmitter::SpawnAt(float x, float y)
{
	mPos = Vector2D(x, y);

	float popSize = mRadius+mZoom*GLOBAL_PARTICLE_SCALE*22.63f;

	hgeParticle* par = mParticles;
	int i = mNbParticles+1;
	while(--i)
	{
		par->fSize = Random(0.2f, 1.0f);
		par->fSpin = Random(0.0f, 360.0f);

		float _x = Random(-popSize, popSize);
		float _y = Random(-popSize, popSize);

		par->vecLocation = mPos+Vector2D(_x, _y);
		par->vecVelocity = Vector2D(0,0);

		++par;
	}
}

void CGlobalParticleEmitter::Update(float deltaTime, const Vector2D& camPos)
{
	Vector2D cam = Vector2D(camPos.x, camPos.y);
	Vector2D deltaPos = cam - mPos;
	mPos = cam;

	float popSize = mRadius+mZoom*GLOBAL_PARTICLE_SCALE*22.63f;
	float popSize2 = popSize*popSize;
	hgeParticle* par = mParticles;
	int i = mNbParticles+1;
	while(--i)
	{
		// on applique un pourcentage de la vitesse de la caméra proportionnel à la petitesse de la particule (effet 3d)
		par->vecLocation += (0.8f-1.5f*par->fSize)*deltaPos;
		//par->vecLocation += par->vecVelocity*deltaTime*par->fSize;

		if((par->vecLocation-mPos).Length2() > popSize2)// on est hors du radius de l'emetteur
		{
			hgeColor deltaColor = hgeColor(
				Random(0.0f, mDeltaColor.r), 
				Random(0.0f, mDeltaColor.g), 
				Random(0.0f, mDeltaColor.b), 
				Random(0.0f, mDeltaColor.a));
			par->colColor = mStartColor+deltaColor;

			par->fSize = Random(0.2f, 1.0f);
			par->fSpin = Random(0.0f, 360.0f);

			Vector2D localPos = deltaPos;
			localPos.Normalize();
			// on pop dans le demi plan devant nous
			localPos = Random(0.001f, 1.0f)*localPos + b2Random(-1.0f, 1.0f)*Vector2D(-localPos.y, localPos.x);
			localPos.Normalize();
			localPos *= popSize - 1.0f;

			par->vecLocation = mPos+localPos;
			par->vecVelocity = Vector2D(0,0);
		}
		++par;
	}
}


void CGlobalParticleEmitter::Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat, float minSize /*= 0.0f*/, float maxSize /*= 1.0f*/)
{
	JRenderer* renderer = JRenderer::GetInstance();

	hgeParticle *par = mParticles;
	int i = mNbParticles+1;
	while(--i)
	{
		float size = par->fSize;
		if(size > minSize && size <= maxSize)
		{
			size *= GLOBAL_PARTICLE_SCALE * mZoom;

			Vector2D position = camMat / (par->vecLocation-camPos);
			float rotation = par->fSpin-camRot;

			mQuad->SetColor(par->colColor.GetHWColor());
			renderer->RenderQuad(mQuad, SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, -rotation, size, size);
		}
		++par;
	}
}
