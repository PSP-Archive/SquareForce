#ifndef _CGLOBALPARTICLEEMITTER_H_
#define _CGLOBALPARTICLEEMITTER_H_
#include "utils.h"

#include "Box2D.h"

#include <hge\hgeparticle.h>

class CGlobalParticleEmitter
{
public:
	CGlobalParticleEmitter(int nbParticles, JQuad* quad, float zoom = 1.0f, hgeColor startColor = hgeColor(1, 1, 1, 1), hgeColor deltaColor = hgeColor(0, 0, 0, 0));
	~CGlobalParticleEmitter();

	void Update(float deltaTime, const b2Vec2& camPos);
	void Render(const b2Vec2& camPos, const float32& camRot, float minSize = 0.0f, float maxSize = 1.0f);

	void ChangeColors(hgeColor startColor = hgeColor(1, 1, 1, 1), hgeColor deltaColor = hgeColor(0, 0, 0, 0));
	void SpawnAt(float x, float y);

private:
	int mNbParticles;
	hgeParticle* mParticles;
	hgeVector mPos;

	float mRadius;

	JQuad* mQuad;

	float mZoom;

	hgeColor mStartColor;
	hgeColor mDeltaColor;
};

#endif
