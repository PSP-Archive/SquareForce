#ifndef _CMISSILE_H_
#define _CMISSILE_H_
#include "utils.h"

#include "Box2D.h"

#include <hge\hgeparticle.h>
#include <list>

using namespace std;

class CSquareShip;

class CMissile
{
public:
	CMissile(float lifeTime, JQuad* quad, CSquareShip* owner, float damages);
	~CMissile();

	void CreateParticleSystem(const hgeParticleSystem& ps);
	void AddPS(hgeParticleSystem* ps);
	void Update(float deltaTime);
	void Render(const b2Vec2& camPos, const float32& camRot, const b2Mat22& camMat);

	inline b2Vec2 GetPosition() const {return mPosition;}
	inline b2Vec2 GetLastPosition() const {return mLastPosition;}
	void SetPosition(const b2Vec2 &position);
	inline b2Vec2 GetVelocity() const {return mVelocity;}
	void SetVelocity(const b2Vec2 &velocity);
	inline const bool IsAlive() const {return (mParticle->fAge<mParticle->fTerminalAge);}
	inline void Destroy() {mParticle->fAge=mParticle->fTerminalAge;}

	inline const float& GetDamages() const {return mDamages;}

	inline const CSquareShip* GetOwner() const {return mOwner;} 

private:
	b2Vec2 mPosition;
	b2Vec2 mLastPosition;
	b2Vec2 mVelocity;

	float mLifeTime;
	hgeParticle* mParticle;

	hgeParticleSystem *mPS;

	JQuad* mQuad;// quad de la particule

	CSquareShip* mOwner;

	float mDamages;
};

#endif
