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
	virtual void LightUpdate(float dt, bool fullUpdate = false) = 0;

	void RenderWire(const Vector2D& camPos, const float& camRot, const Vector2D& pos, PIXEL_TYPE color);

	inline const Vector2D& GetOriginPosition() const {return mOriginPosition;}
	inline const Vector2D& GetCenterPosition() const {return mCenterPosition;}
	inline const Vector2D& GetLinearVelocity() const {return mLinearVelocity;}

	inline const Matrix22& GetRotationMatrix() const {return mRotationMatrix;}
	inline const float& GetRotation() const {return mRotation;}
	inline const float& GetAngularVelocity() const {return mAngularVelocity;}

	// only effective when no physics attached
	inline void SetPosition(const Vector2D& pos) 
	{ 
		if(!mBody) 
		{
			mOriginPosition = pos + mOriginPosition-mCenterPosition; 
			mCenterPosition = pos;
		} 
	}
	inline void SetRotation(const float& rotation) {mRotation = rotation;}// matrix will be updated from rotation when physics is reloaded
	inline void SetLinearVelocity(const Vector2D& velocity) {mLinearVelocity = velocity;} 
	inline void SetAngularVelocity(const float& velocity) {mAngularVelocity = velocity;}


	virtual bool IsDestroyed() = 0;

	virtual void LoadPhysic() = 0;
	virtual void UnloadPhysic() = 0;

	virtual bool IsDocked() = 0;
	virtual bool WantToDock() = 0;// true quand on veut  se docker/undocker
	virtual void Dock(bool state) = 0;
	virtual void RequestDock(bool state) = 0;// mettre � true pour se docker/undocker

	virtual bool IsLanded() = 0;
	virtual void Land(bool state) = 0;

	virtual bool IsUnspawned() = 0;
	virtual void Unspawn() = 0;

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

