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
	void Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat);

	inline Vector2D GetPosition() const {return mPosition;}
	inline Vector2D GetLastPosition() const {return mLastPosition;}
	void SetPosition(const Vector2D &position);
	inline Vector2D GetVelocity() const {return mVelocity;}
	void SetVelocity(const Vector2D &velocity);
	inline const bool IsAlive() const {return (mParticle->fAge<mParticle->fTerminalAge);}
	inline void Destroy() {mParticle->fAge=mParticle->fTerminalAge;}

	inline const float& GetDamages() const {return mDamages;}

	inline const CSquareShip* GetOwner() const {return mOwner;} 

private:
	Vector2D mPosition;
	Vector2D mLastPosition;
	Vector2D mVelocity;

	float mLifeTime;
	hgeParticle* mParticle;

	hgeParticleSystem *mPS;

	JQuad* mQuad;// quad de la particule

	CSquareShip* mOwner;

	float mDamages;
};

#endif
