/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeDistortionMesh helper class implementation
*/

#include "..\..\include\JGE.h"
#include "..\..\include\JTypes.h"
#include "..\..\include\JRenderer.h"
#include "..\..\include\JFileSystem.h"

#include "..\..\include\Vector2D.h"
#include "..\..\include\hge\hgedistort.h"


//HGE *hgeDistortionMesh::hge=0;


hgeDistortionMesh::hgeDistortionMesh(int cols, int rows)
{
	int i;
	
	//hge=hgeCreate(HGE_VERSION);

	nRows=rows;
	nCols=cols;
	cellw=cellh=0;
	//quad.tex=0;
	//quad.blend=BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE;

	quad = NULL;

	int size = rows*cols;
	disp_array=new Vertex[size];

	i = size+1;
	Vertex* vert = disp_array;
	while(--i)
	{
		vert->x=0.0f;
		vert->y=0.0f;
		vert->u=0.0f;
		vert->v=0.0f;
		
		vert->z=0.5f;
		vert->color=ARGB(0xFF,0xFF,0xFF,0xFF);

		++vert;
	}
}

hgeDistortionMesh::hgeDistortionMesh(const hgeDistortionMesh &dm)
{
//	hge=hgeCreate(HGE_VERSION);

	nRows=dm.nRows;
	nCols=dm.nCols;
	cellw=dm.cellw;
	cellh=dm.cellh;
	tx=dm.tx;
	ty=dm.ty;
	width=dm.width;
	height=dm.height;
	quad=dm.quad;

	disp_array=new Vertex[nRows*nCols];
	memcpy(disp_array, dm.disp_array, sizeof(Vertex)*nRows*nCols);
}

hgeDistortionMesh::~hgeDistortionMesh()
{
	delete[] disp_array;
	//hge->Release();
}

hgeDistortionMesh& hgeDistortionMesh::operator= (const hgeDistortionMesh &dm)
{
	if(this!=&dm)
	{
		nRows=dm.nRows;
		nCols=dm.nCols;
		cellw=dm.cellw;
		cellh=dm.cellh;
		tx=dm.tx;
		ty=dm.ty;
		width=dm.width;
		height=dm.height;
		quad=dm.quad;

		delete[] disp_array;
		int size = nRows*nCols;
		disp_array=new Vertex[size];
		memcpy(disp_array, dm.disp_array, sizeof(Vertex)*size);
	}

	return *this;
	
}

void hgeDistortionMesh::SetTexture(JTexture* tex)
{
	if (quad)
		delete quad;

	quad = new JQuad(tex, 0, 0, 16, 16);
	//quad.tex=tex;
}

void hgeDistortionMesh::SetTextureRect(float x, float y, float w, float h)
{
	int i,j;
	
	tx=x; ty=y; width=w; height=h;

	cellw=w/(nCols-1);
	cellh=h/(nRows-1);

	Vertex* vert = disp_array;
	for(j=0; j<nRows; j++)
	{
		for(i=0; i<nCols; i++)
		{
			vert->u=(x+i*cellw);
			vert->v=(y+j*cellh);

			//disp_array[j*nCols+i].x=i*cellw;
			//disp_array[j*nCols+i].y=j*cellh;

			++vert;
		}
	}
}

void hgeDistortionMesh::SetBlendMode(int blend)
{
//	quad.blend=blend;
}

void hgeDistortionMesh::Clear(PIXEL_TYPE col)
{
	int i = nRows*nCols+1;
	Vertex* vert = disp_array;
	while(--i)
	{
		vert->color=col;
		++vert;
	}
}

void hgeDistortionMesh::Render(float x, float y, float scaleX, float scaleY, float angle)
{
	int i,j;

	VertexColor points[4];
	JRenderer* renderer = JRenderer::GetInstance();

	Vertex* vert = disp_array;
	for(j=0; j<nRows-1; j++)
	{
		for(i=0; i<nCols-1; i++)
		{
			quad->SetTextureRect(vert->u, vert->v, cellw, cellh);

			VertexColor* quadPt = points;

			Vector2D pt;
			Vertex* vert2;

			vert2 = vert + nCols;
			pt = Vector2D(vert2->x, vert2->y);
			if(angle != 0.0f)
				pt.Rotate(angle);
			quadPt->x = x+pt.x*scaleX;
			quadPt->y = y+pt.y*scaleY;
			quadPt->z = vert2->z;
			quadPt->color = vert2->color;
			++quadPt;

			vert2 = vert + nCols + 1;
			pt = Vector2D(vert2->x, vert2->y);
			if(angle != 0.0f)
				pt.Rotate(angle);
			quadPt->x = x+pt.x*scaleX;
			quadPt->y = y+pt.y*scaleY;
			quadPt->z = vert2->z;
			quadPt->color = vert2->color;
			++quadPt;

			vert2 = vert + 1;
			pt = Vector2D(vert2->x, vert2->y);
			if(angle != 0.0f)
				pt.Rotate(angle);
			quadPt->x = x+pt.x*scaleX;
			quadPt->y = y+pt.y*scaleY;
			quadPt->z = vert2->z;
			quadPt->color = vert2->color;
			++quadPt;

			vert2 = vert;
			pt = Vector2D(vert2->x, vert2->y);
			if(angle != 0.0f)
				pt.Rotate(angle);
			quadPt->x = x+pt.x*scaleX;
			quadPt->y = y+pt.y*scaleY;
			quadPt->z = vert2->z;
			quadPt->color = vert2->color;


			renderer->RenderQuad(quad, points);

			++vert;
		}
		++vert;
	}
}

void hgeDistortionMesh::Render(float x, float y, const Matrix22& rotMat, float scaleX, float scaleY)
{
	int i,j;

	VertexColor points[4];
	JRenderer* renderer = JRenderer::GetInstance();

	Vertex* vert = disp_array;
	for(j=0; j<nRows-1; j++)
	{
		for(i=0; i<nCols-1; i++)
		{
			quad->SetTextureRect(vert->u, vert->v, cellw, cellh);

			VertexColor* quadPt = points;

			Vector2D pt;
			Vertex* vert2;

			vert2 = vert + nCols;
			pt = rotMat * Vector2D(vert2->x, vert2->y);
			quadPt->x = x+pt.x*scaleX;
			quadPt->y = y+pt.y*scaleY;
			quadPt->z = vert2->z;
			quadPt->color = vert2->color;
			++quadPt;

			vert2 = vert + nCols + 1;
			pt = Vector2D(vert2->x, vert2->y);
			pt = rotMat * Vector2D(vert2->x, vert2->y);
			quadPt->x = x+pt.x*scaleX;
			quadPt->y = y+pt.y*scaleY;
			quadPt->z = vert2->z;
			quadPt->color = vert2->color;
			++quadPt;

			vert2 = vert + 1;
			pt = Vector2D(vert2->x, vert2->y);
			pt = rotMat * Vector2D(vert2->x, vert2->y);
			quadPt->x = x+pt.x*scaleX;
			quadPt->y = y+pt.y*scaleY;
			quadPt->z = vert2->z;
			quadPt->color = vert2->color;
			++quadPt;

			vert2 = vert;
			pt = Vector2D(vert2->x, vert2->y);
			pt = rotMat * Vector2D(vert2->x, vert2->y);
			quadPt->x = x+pt.x*scaleX;
			quadPt->y = y+pt.y*scaleY;
			quadPt->z = vert2->z;
			quadPt->color = vert2->color;


			renderer->RenderQuad(quad, points);

			++vert;
		}
		++vert;
	}
}

void hgeDistortionMesh::SetZ(int col, int row, float z)
{
	if(row<nRows && col<nCols) disp_array[row*nCols+col].z=z;
}

void hgeDistortionMesh::SetColor(int col, int row, PIXEL_TYPE color)
{
	if(row<nRows && col<nCols) disp_array[row*nCols+col].color=color;
}

void hgeDistortionMesh::SetDisplacement(int col, int row, float dx, float dy, int ref)
{
	if(row<nRows && col<nCols)
	{
		switch(ref)
		{
			case HGEDISP_NODE:		dx+=col*cellw; dy+=row*cellh; break;
			case HGEDISP_CENTER:	dx+=cellw*(nCols-1)/2;dy+=cellh*(nRows-1)/2; break;
			case HGEDISP_TOPLEFT:	break;
		}

		Vertex* vert = disp_array+row*nCols+col;
		vert->x=dx;
		vert->y=dy;
	}
}

float hgeDistortionMesh::GetZ(int col, int row) const
{
	if(row<nRows && col<nCols) return disp_array[row*nCols+col].z;
	else return 0.0f;
}

PIXEL_TYPE hgeDistortionMesh::GetColor(int col, int row) const
{
	if(row<nRows && col<nCols) return disp_array[row*nCols+col].color;
	else return 0;
}

void hgeDistortionMesh::GetDisplacement(int col, int row, float *dx, float *dy, int ref) const
{
	if(row<nRows && col<nCols)
	{
		Vertex* vert = disp_array+row*nCols+col;
		switch(ref)
		{
			case HGEDISP_NODE:		*dx=vert->x-col*cellw;
									*dy=vert->y-row*cellh;
									break;

			case HGEDISP_CENTER:	*dx=vert->x-cellw*(nCols-1)/2;
									*dy=vert->y-cellh*(nRows-1)/2;
									break;

			case HGEDISP_TOPLEFT:	*dx=vert->x;
									*dy=vert->y;
									break;
		}
	}
}

