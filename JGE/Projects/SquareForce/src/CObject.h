#ifndef _COBJECT_H_
#define _COBJECT_H_
#include "utils.h"

#include "Box2D.h"

class CObject
{
public:
	CObject(b2World* world);
	virtual ~CObject();

	virtual void Render(const Vector2D& camPos, const float& camRot, const Matrix22& camMat) = 0;
	virtual void Update(float dt, bool updatePhysic = true) = 0;

	void RenderWire(const Vector2D& camPos, const float& camRot, const Vector2D& pos, PIXEL_TYPE color);

	inline const Vector2D& GetOriginPosition() const {return mOriginPosition;}
	inline const Vector2D& GetCenterPosition() const {return mCenterPosition;}
	inline const Vector2D& GetLinearVelocity() const {return mLinearVelocity;}

	inline const Matrix22& GetRotationMatrix() const {return mRotationMatrix;}
	inline const float& GetRotation() const {return mRotation;}
	inline const float& GetAngularVelocity() const {return mAngularVelocity;}

	virtual bool IsDestroyed() = 0;

	virtual void LoadPhysic() = 0;
	virtual void UnloadPhysic() = 0;

protected:
	b2World* mWorld;// pointeur sur le world
	b2Body* mBody;

	Vector2D mOriginPosition;
	Vector2D mCenterPosition;
	Vector2D mLinearVelocity;
	Matrix22 mRotationMatrix;
	float mRotation;
	float mAngularVelocity;
};

#endif

