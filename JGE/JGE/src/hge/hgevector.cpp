/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeVector helper class implementation
*/


#include "..\..\include\hge\hgevector.h"
#ifdef PSP
#include <pspmath.h>
#endif

float InvSqrt(float x)
{
		union
        {
          int intPart;
          float floatPart;
        } convertor;

        convertor.floatPart = x;
        convertor.intPart = 0x5f3759df - (convertor.intPart >> 1);
        return convertor.floatPart*(1.5f - 0.4999f*x*convertor.floatPart*convertor.floatPart);
}

/*
hgeVector *hgeVector::Normalize()
{
	float lenRcp;

	lenRcp=sqrtf(Dot(this));

	if(lenRcp)
	{
		lenRcp=1.0f/lenRcp;

		x*=lenRcp;
		y*=lenRcp;
	}

	return this;
}
*/

float hgeVector::Angle(const hgeVector *v) const
{
	if(v)
	{
		hgeVector s=*this, t=*v;

		s.Normalize(); t.Normalize();
#ifdef PSP
		return vfpu_acosf(s.Dot(&t));
	}
	else return vfpu_atan2f(y, x);
#else
		return acosf(s.Dot(&t));
	}
	else return atan2f(y, x);
#endif
}

hgeVector *hgeVector::Rotate(float a)
{
#ifdef PSP
	float c, s;
	vfpu_sincos(a, &s, &c);
#else
	float c, s;
	c = cosf(a);
	s = sinf(a);
#endif

	float _x = x;
	x = _x*c - y*s;
	y = _x*s + y*c;

	return this;
}



