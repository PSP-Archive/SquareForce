#ifndef _COBJECT_H_
#define _COBJECT_H_
#include "utils.h"

#include "Box2D.h"

class CObject
{
public:
	CObject(b2World* world);
	virtual ~CObject();

	virtual void Render(const b2Vec2& camPos, const float32& camRot) = 0;
	virtual void Update(float dt, bool updatePhysic = true) = 0;

	void RenderWire(const b2Vec2& camPos, const float& camRot, const b2Vec2& pos, PIXEL_TYPE color);

	inline const b2Vec2& GetOriginPosition() const {return mOriginPosition;}
	inline const b2Vec2& GetCenterPosition() const {return mCenterPosition;}
	inline const b2Vec2& GetLinearVelocity() const {return mLinearVelocity;}

	inline const b2Mat22& GetRotationMatrix() const {return mRotationMatrix;}
	inline const float& GetRotation() const {return mRotation;}
	inline const float& GetAngularVelocity() const {return mAngularVelocity;}

	virtual bool IsDestroyed() = 0;

	virtual void LoadPhysic() = 0;
	virtual void UnloadPhysic() = 0;

protected:
	b2World* mWorld;// pointeur sur le world
	b2Body* mBody;

	b2Vec2 mOriginPosition;
	b2Vec2 mCenterPosition;
	b2Vec2 mLinearVelocity;
	b2Mat22 mRotationMatrix;
	float mRotation;
	float mAngularVelocity;
};

#endif

