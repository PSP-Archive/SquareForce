#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CGlobalParticleEmitter.h"
#include "CObject.h"


CGlobalParticleEmitter::CGlobalParticleEmitter(int nbParticles, JQuad* quad, float zoom)
{
	mRadius = 300.0f;

	mNbParticles = nbParticles;
	mParticles = new hgeParticle[nbParticles];

	mPos = hgeVector();

	mStartColor = hgeColor(1, 1, 1, 1);
	mDeltaColor = hgeColor(0, 0, 0, 0);

	hgeParticle* par = mParticles;
	for(int i=0; i<mNbParticles; i++)
	{
		hgeColor deltaColor = hgeColor(
			b2Random(0.0f, mDeltaColor.r), 
			b2Random(0.0f, mDeltaColor.g), 
			b2Random(0.0f, mDeltaColor.b), 
			b2Random(0.0f, mDeltaColor.a));
		par->colColor = mStartColor+deltaColor;

		par->fSize = b2Random(0.2f, 1.0f);
		par->fSpin = b2Random(0.0f, 360.0f);

		float x = b2Random(-mRadius, mRadius);
		float y = b2Random(-mRadius, mRadius);

		par->vecLocation = mPos+hgeVector(x, y);
		par->vecVelocity = hgeVector();

		par++;
	}

	mQuad = quad;

	mZoom = zoom;
}

CGlobalParticleEmitter::~CGlobalParticleEmitter()
{
	SAFE_DELETE_ARRAY(mParticles);
}

void CGlobalParticleEmitter::Update(float deltaTime, const b2Vec2& camPos)
{
	hgeVector cam = hgeVector(camPos.x, camPos.y);
	hgeVector deltaPos = cam - mPos;
	mPos = cam;

	float popSize = mRadius+mZoom*GLOBAL_PARTICLE_SCALE*23.0f;
	float popSize2 = popSize*popSize;
	hgeParticle* par = mParticles;
	for(int i=0; i<mNbParticles; i++)
	{
		// on applique un pourcentage de la vitesse de la caméra proportionnel à la petitesse de la particule (effet 3d)
		par->vecLocation += (0.8f-1.5f*par->fSize)*deltaPos;
		//par->vecLocation += par->vecVelocity*deltaTime*par->fSize;

		if((par->vecLocation-mPos).Length2() > popSize2)// on est hors du radius de l'emetteur
		{
			hgeColor deltaColor = hgeColor(
				b2Random(0.0f, mDeltaColor.r), 
				b2Random(0.0f, mDeltaColor.g), 
				b2Random(0.0f, mDeltaColor.b), 
				b2Random(0.0f, mDeltaColor.a));
			par->colColor = mStartColor+deltaColor;

			par->fSize = b2Random(0.2f, 1.0f);
			par->fSpin = b2Random(0.0f, 360.0f);

			hgeVector localPos = deltaPos;
			localPos.Normalize();
			localPos = b2Random(0.001f, 1.0f)*localPos + b2Random(-1.0f, 1.0f)*hgeVector(-localPos.y, localPos.x);
			localPos.Normalize();
			localPos *= popSize - 1.0f;

			par->vecLocation = mPos+localPos;
			par->vecVelocity = hgeVector();
		}
		par++;
	}
}


void CGlobalParticleEmitter::Render(const b2Vec2& camPos, const float32& camRot)
{
	JRenderer* renderer = JRenderer::GetInstance();

	hgeParticle *par=mParticles;
	for(int i=0; i<mNbParticles; i++)
	{
		float size = par->fSize*GLOBAL_PARTICLE_SCALE * mZoom;

		b2Vec2 position = b2MulT(b2Mat22(camRot), b2Vec2(par->vecLocation.x, par->vecLocation.y)-camPos);
		float32 rotation = par->fSpin-camRot;

		mQuad->SetColor(par->colColor.GetHWColor());
		renderer->RenderQuad(mQuad, SCREEN_SIZE_X2+position.x, SCREEN_SIZE_Y2-position.y, 0, size, size);
		par++;
	}
}
