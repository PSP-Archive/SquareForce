//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------
// 14/05/2011 : optimised + Matrix22 structure added by Akabane87
//-------------------------------------------------------------------------------------


#ifndef _VECTOR2D_H
#define _VECTOR2D_H

#ifdef WIN32
#include <math.h>
#else
#include <fastmath.h>
#endif

#ifdef PSP
#include <pspmath.h>
#endif


struct Vector2D
{

	float x, y;
	static const Vector2D& Zero() { static const Vector2D V(0, 0); return V; }

	inline Vector2D(void)	{}

	inline Vector2D(const float& _x, const float& _y) : x(_x), y(_y)	{}

	inline void Set(const float& _x, const float& _y) {x = _x; y = _y;}

	inline Vector2D &operator /=(const float& scalar)	{ x /= scalar; y /= scalar;		return *this; }

	inline Vector2D &operator *=(const float& scalar)	{ x *= scalar; y *= scalar;		return *this; }
	
	inline Vector2D &operator +=(const Vector2D &v) { x += v.x;	y += v.y;	return *this; }

	inline Vector2D &operator -=(const Vector2D &v) { x -= v.x;	y -= v.y;	return *this;	}

	inline bool operator ==(const Vector2D &v) const { return x == v.x && y == v.y; }
	inline bool operator !=(const Vector2D &v) const { return x != v.x || y != v.y; }


	// cross product
	inline float operator ^ (const Vector2D &v)	const	{ return (x * v.y) - (y * v.x); } 

	// dot product
	inline float operator * (const Vector2D &v)	const	{ return (x*v.x) + (y*v.y); } 

	inline float Dot(const Vector2D &v)	const	{ return (x * v.x) + (y * v.y); } 
	inline float Cross(const Vector2D &v) const	{ return (x * v.y) - (y * v.x); } 
	

	inline Vector2D operator * (const float& s)			const	{	return Vector2D(x*s, y*s); }
	inline Vector2D operator / (const float& s)			const	{	return Vector2D(x/s, y/s); }
	inline Vector2D operator + (const Vector2D &v)	const	{	return Vector2D(x+v.x, y+v.y); }
	inline Vector2D operator - (const Vector2D &v)	const	{	return Vector2D(x-v.x, y-v.y); }
	friend Vector2D operator * (const float& k, const Vector2D& v) {	return Vector2D(v.x*k, v.y*k); }
	inline Vector2D operator -(void) const { return Vector2D(-x, -y); }

	float Length(void) const 
	{
#ifdef PSP
		return vfpu_sqrtf(x*x + y*y); 
#else
		return sqrtf(x*x + y*y); 
#endif 
	}

	float Length2(void) const { return x*x+y*y; }

	float Normalize(void) 
	{	
		float fLength = Length();	

		if (fLength == 0.0f) 
			return 0.0f; 

		(*this) *= (1.0f / fLength); 

		return fLength;	
	}


	Vector2D Direction(void) const
	{
		Vector2D temp(*this);

		temp.Normalize();

		return temp;
	}

	float Angle(const Vector2D& xE)// slow
	{
		float dot = (*this) * xE;
		float cross = (*this) ^ xE;

		// angle between segments
#ifdef PSP
		float angle = vfpu_atan2f(cross, dot);
#else
		float angle = atan2f(cross, dot);
#endif 

		return angle;
	}

	float Angle() const // slow
	{
#ifdef PSP
		return vfpu_atan2f(y, x);
#else
		return atan2f(y, x);
#endif
	}


	Vector2D& Rotate(const float& angle)// slow : use this only if you can't rotate from a matrix
	{
#ifdef PSP
		float c, s;
		vfpu_sincos(angle, &s, &c);
#else
		float c, s;
		c = cosf(angle);
		s = sinf(angle);
#endif

		float tx = x;
		x = tx * c - y * s;
		y = tx * s + y * c;

		return *this;
	}



	Vector2D& Rotate(const Vector2D& xCentre, const float& fAngle)
	{
		Vector2D D = *this - xCentre;
		D.Rotate(fAngle);

		*this = xCentre + D;

		return *this;
	}


	void Clamp(const Vector2D& min, const Vector2D& max)
	{
		x = (x < min.x)? min.x : (x > max.x)? max.x : x;
		y = (y < min.y)? min.y : (y > max.y)? max.y : y;
	}

};


struct Matrix22
{
	Matrix22() {}
	Matrix22(const Vector2D& c1, const Vector2D& c2)
	{
		col1 = c1;
		col2 = c2;
	}

	Matrix22(const float& angle)
	{
#ifdef PSP
		float c, s;
		vfpu_sincos(angle, &s, &c);
#else
		float c = cosf(angle), s = sinf(angle);
#endif
		col1.x = c; col2.x = -s;
		col1.y = s; col2.y = c;
	}

	void Set(const Vector2D& c1, const Vector2D& c2)
	{
		col1 = c1;
		col2 = c2;
	}

	void Set(const float& angle)
	{
#ifdef PSP
		float c, s;
		vfpu_sincos(angle, &s, &c);
#else
		float c = cosf(angle), s = sinf(angle);
#endif
		col1.x = c; col2.x = -s;
		col1.y = s; col2.y = c;
	}

	static const Matrix22& Identity()
	{
		static const Matrix22 M(Vector2D (1, 0), Vector2D (0, 1)); 
		return M;
	}

	void SetIdentity()
	{
		col1.x = 1.0f; col2.x = 0.0f;
		col1.y = 0.0f; col2.y = 1.0f;
	}

	static const Matrix22& Zero()
	{
		static const Matrix22 M(Vector2D (0, 0), Vector2D (0, 0)); 
		return M;
	}

	void SetZero()
	{
		col1.x = 0.0f; col2.x = 0.0f;
		col1.y = 0.0f; col2.y = 0.0f;
	}

	Matrix22 Invert() const
	{
		float a = col1.x, b = col2.x, c = col1.y, d = col2.y;
		Matrix22 B;
		float det = a * d - b * c;
		if(det == 0.0f) return Zero();
		det = 1.0f / det;
		B.col1.x =  det * d;	B.col2.x = -det * b;
		B.col1.y = -det * c;	B.col2.y =  det * a;
		return B;
	}

	inline Matrix22 operator -(void) const
	{
		float a = col1.x, b = col2.x, c = col1.y, d = col2.y;
		Matrix22 B;
		float det = a * d - b * c;
		if(det == 0.0f) return Zero();
		det = 1.0f / det;
		B.col1.x =  det * d;	B.col2.x = -det * b;
		B.col1.y = -det * c;	B.col2.y =  det * a;
		return B;
	}

	// Solve A * x = b
	Vector2D Solve(const Vector2D& b) const
	{
		float a11 = col1.x, a12 = col2.x, a21 = col1.y, a22 = col2.y;
		float det = a11 * a22 - a12 * a21;
		if(det == 0.0f) return Vector2D::Zero();
		det = 1.0f / det;
		Vector2D x;
		x.x = det * (a22 * b.x - a12 * b.y);
		x.y = det * (a11 * b.y - a21 * b.x);
		return x;
	}

	inline Vector2D Mul (const Vector2D& v) const
	{
		Vector2D u(col1.x * v.x + col2.x * v.y, col1.y * v.x + col2.y * v.y);
		return u;
	}

	// A * B
	inline Matrix22 Mul(const Matrix22& A)
	{
		Matrix22 C(this->Mul(A.col1), this->Mul(A.col2));
		return C;
	}

	inline Vector2D operator * (const Vector2D& v) const
	{
		Vector2D u(col1.x * v.x + col2.x * v.y, col1.y * v.x + col2.y * v.y);
		return u;
	}

	inline Vector2D MulT (const Vector2D& v) const
	{
		Vector2D u(v*col1, v*col2);
		return u;
	}

	// A^T * B
	inline Matrix22 MulT(const Matrix22& A)
	{
		Vector2D c1(col1*A.col1, col2*A.col1);
		Vector2D c2(col1*A.col2, col2*A.col2);
		Matrix22 C(c1, c2);
		return C;
	}

	inline Vector2D operator / (const Vector2D& v) const
	{
		Vector2D u(v*col1, v*col2);
		return u;
	}

	inline Matrix22 operator + (const Matrix22& A) const
	{
		Matrix22 C(col1 + A.col1, col2 + A.col2);
		return C;
	}

	inline bool operator == (const Matrix22& A) const { return col1 == A.col1 && col2 == A.col2; }
	inline bool operator != (const Matrix22& A) const { return col1 != A.col1 || col2 != A.col2; }


	Vector2D col1, col2;
};

#endif 
