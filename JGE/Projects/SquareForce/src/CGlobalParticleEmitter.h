#ifndef _CGLOBALPARTICLEEMITTER_H_
#define _CGLOBALPARTICLEEMITTER_H_
#include "utils.h"

#include "Box2D.h"

#include <hge\hgeparticle.h>

class CGlobalParticleEmitter
{
public:
	CGlobalParticleEmitter(int nbParticles, JQuad* quad, float zoom = 1.0f);
	~CGlobalParticleEmitter();

	void Update(float deltaTime, const b2Vec2& camPos);
	void Render(const b2Vec2& camPos, const float32& camRot);

	hgeColor mStartColor;
	hgeColor mDeltaColor;

private:
	int mNbParticles;
	hgeParticle* mParticles;
	hgeVector mPos;

	float mRadius;

	JQuad* mQuad;

	float mZoom;
};

#endif
