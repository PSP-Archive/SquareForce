#include <JGE.h>
#include <JRenderer.h>
#include <JTypes.h>

#include "CObject.h"


CObject::CObject(b2World* world): mWorld(world)
{
	mBody = NULL;
}



CObject::~CObject()
{

}

void CObject::RenderWire(const b2Vec2& camPos, const float& camRot, const b2Vec2& pos, PIXEL_TYPE color)
{
	JRenderer* renderer = JRenderer::GetInstance();	

	float xx[32];
	float yy[32];

	b2Vec2 trans = pos - mBody->GetOriginPosition();

	for (b2Shape* shape = mBody->m_shapeList; shape; shape = shape->m_next)
	{


		switch (shape->m_type)
		{
		case e_circleShape:
			{
				const b2CircleShape* circle = (const b2CircleShape*)shape;
				b2Vec2 pos(SCREEN_SIZE_X2+(trans.x+circle->m_position.x-camPos.x), 
					SCREEN_SIZE_Y2-(trans.y+circle->m_position.y-camPos.y));
				float32 r = circle->m_radius;

				renderer->DrawCircle(pos.x, pos.y, r, color);
			}
			break;

		case e_polyShape:
			{
				const b2PolyShape* poly = (const b2PolyShape*)shape;

				for (int32 i = 0; i < poly->m_vertexCount; ++i)
				{
					b2Mat22 rot(camRot);
					b2Vec2 v = b2MulT(rot, trans + poly->m_position - camPos + b2Mul(poly->m_R, poly->m_vertices[i]));
					xx[i] = SCREEN_SIZE_X2+v.x;
					yy[i] = SCREEN_SIZE_Y2-v.y;
				}

				renderer->DrawPolygon(xx, yy, poly->m_vertexCount, color);

			}
			break;
		}
	}
}

