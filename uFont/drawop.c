
//  Copyright (c) Michael McElligott
// 
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU LIBRARY GENERAL PUBLIC LICENSE
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU LIBRARY GENERAL PUBLIC LICENSE for details.



#include "common.h"
#include "draw_op.h"


#define DRAW_FILLED		1
#define DRAW_INVERT		1
#define DRAW_DOTTED		1
#define DRAW_CLOSEPLY	1
#define DRAW_CENTERED	1


#define COLi_BLACK		0
#define COLi_WHITE		1
#define COLi_RED		2
#define COLi_GREEN		3
#define COLi_BLUE		4
#define COLi_CYAN		5
#define COLi_YELLOW		6
#define COLi_BROWN		7
#define COLi_MAGENTA	8

#define COLi_TABLESIZE	256

static uint16_t colourTable[COLi_TABLESIZE];




static inline void *uf_malloc (size_t size/*, const uint8_t *func*/)
{
	return malloc(size);
}

static inline void *uf_calloc (size_t nelem, size_t elsize/*, const uint8_t *func*/)
{
	return calloc(nelem, elsize);
}

static inline void *uf_realloc (void *ptr, size_t elsize/*, const uint8_t *func*/)
{
	return realloc(ptr, elsize);
}

static inline void uf_free (void *ptr/*, const uint8_t *func*/)
{
	free(ptr);
}

uint8_t coli_add (const uint8_t colIdx, const uint16_t colour)
{
	colourTable[colIdx] = colour;
	return colIdx;
}

uint16_t coli_lookup (const uint8_t colIdx)
{
	return colourTable[colIdx];
}

void opDrawImage (_ufont_surface_t *surface, _ufont_surface_t *image, const int isCentered, const int cx, const int cy)
{
	surfaceDrawImage(surface, image, isCentered, cx, cy);
}

void opDrawPoint (_ufont_surface_t *surface, const int x, const int y, const uint8_t colIdx)
{
	setPixel16_bc(surface, x, y, coli_lookup(colIdx));
}

void opDrawTriangle (_ufont_surface_t *surface, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3, const uint8_t isFilled, const uint8_t colIdx)
{
	if (!isFilled)
		surfaceDrawTriangle(surface, x1, y1, x2, y2, x3, y3, coli_lookup(colIdx));
	else
		surfaceDrawTriangleFilled(surface, x1, y1, x2, y2, x3, y3, coli_lookup(colIdx));
}

void opDrawArc (_ufont_surface_t *surface, const int x, const int y, const int r1, const int r2, const float a1, const float a2, const uint8_t colIdx)
{
	surfaceDrawArc(surface, x, y, r1, r2, a1, a2, coli_lookup(colIdx));
}

void opDrawCircle (_ufont_surface_t *surface, const int xc, const int yc, const int radius, const uint8_t isFilled, const uint8_t colIdx)
{
	if (!isFilled)
		surfaceDrawCircle(surface, xc, yc, radius, coli_lookup(colIdx));
	else
		surfaceDrawCircleFilled(surface, xc, yc, radius, coli_lookup(colIdx));
}

void opDrawCopy (_ufont_surface_t *surface, const int x1, const int y1, const int x2, const int y2, const int des_x, const int des_y)
{
	surfaceDrawCopy(surface, x1, y1, x2, y2, des_x, des_y);
}

void opDrawEllipse (_ufont_surface_t *surface, const int x, const int y, const int r1, const int r2, const uint8_t colIdx)
{
	surfaceDrawEllipse(surface, x, y, r1, r2, coli_lookup(colIdx));
}

void opDrawPoly (_ufont_surface_t *surface, pointFlt_t *pts, const uint8_t total, const uint8_t close, const uint8_t colIdx)
{
	const uint16_t col = coli_lookup(colIdx);
	const float width = DWIDTH;
	const float height = DHEIGHT;
	
	for (int i = 0; i < total-1; i++)
		surfaceDrawLine(surface, pts[i].x*width, pts[i].y*height, pts[i+1].x*width, pts[i+1].y*height, col);

	if (close)
		surfaceDrawLine(surface, pts[total-1].x*DWIDTH, pts[total-1].y*DHEIGHT, pts[0].x*DWIDTH, pts[0].y*DHEIGHT, col);
}

void opDrawLine (_ufont_surface_t *surface, const int x1, const int y1, const int x2, const int y2, const uint8_t isDotted, const uint8_t colIdx)
{
	if (!isDotted)
		surfaceDrawLine(surface, x1, y1, x2, y2, coli_lookup(colIdx));
	else
		surfaceDrawLineDotted(surface, x1, y1, x2, y2, coli_lookup(colIdx));
	
}

void opDrawRectangle (_ufont_surface_t *surface, const int x1, const int y1, const int x2, const int y2, const uint8_t isFilled, const uint8_t colIdx)
{
	if (!isFilled)
		surfaceDrawRectangle(surface, x1, y1, x2, y2, coli_lookup(colIdx));
	else
		surfaceDrawRectangleFilled(surface, x1, y1, x2, y2, coli_lookup(colIdx));
}

void *op_new (const uint8_t drawOp)
{
	static uint8_t opsize[DRAW_OPS_TOTAL];
	
	
	if (!drawOp || drawOp >=  DRAW_OPS_TOTAL)
		return NULL;

	if (!opsize[DRAW_OP_POINT]){
		opsize[DRAW_OP_POINT] = sizeof(op_point_t);
		opsize[DRAW_OP_LINE] = sizeof(op_line_t);
		opsize[DRAW_OP_RECT] = sizeof(op_rect_t);
		opsize[DRAW_OP_CIRCLE] = sizeof(op_circle_t);
		opsize[DRAW_OP_ELLIPSE] = sizeof(op_ellipse_t);
		opsize[DRAW_OP_ARC] = sizeof(op_arc_t);
		opsize[DRAW_OP_TRIANGLE] = sizeof(op_triangle_t);
		opsize[DRAW_OP_POLYLINE] = sizeof(op_poly_t);
		opsize[DRAW_OP_COPY] = sizeof(op_copy_t);
		opsize[DRAW_OP_IMAGE] = sizeof(op_image_t);
	}
	
	draw_op_t *op = uf_calloc(1, opsize[drawOp]);
	if (op)
		op->type = drawOp;
	
	return op;	
}

void op_release (void *op)
{
	uf_free(op);
}

draw_ops_t *ops_create (const uint32_t total)
{
	draw_ops_t *ops = uf_calloc(1, sizeof(draw_ops_t));
	if (ops){
		ops->list = uf_calloc(total, sizeof(op_base_t*));
		if (ops->list){
			ops->total = total;
			ops->ct = 0;
			return ops;
		}
		uf_free(ops);
	}
	return NULL;
}

void ops_free (draw_ops_t *ops)
{
	for (int i = 0; i < ops->ct; i++)
		op_release(ops->list[i].op);

	uf_free(ops->list);
	uf_free(ops);
}

int op_add (draw_ops_t *ops, void *op)
{
	if (ops->ct+1 >= ops->total){
		int total = ops->total + 32;
		ops->list = uf_realloc(ops->list, (total*sizeof(op_base_t*)));
		if (!ops->list)
			return 0;
		else
			ops->total = total;
	}
	
	ops->list[ops->ct++].op = op;

	return ops->ct;
}

void op_addPoint (draw_ops_t *ops, const float x, const float y, const uint8_t colIdx)
{
	op_point_t *pt = op_new(DRAW_OP_POINT);
	if (pt){
		pt->pt1.x = x;
		pt->pt1.y = y;
		pt->op.colourIdx = colIdx;	
		pt->op.flags = 0;	

		/*int total =*/ op_add(ops, pt);
		//printf("op_addPoint(): %i %i\n", total, pt->op.type);
	}
}

void op_addLine (draw_ops_t *ops, const float x1, const float y1, const float x2, const float y2, const uint8_t isDotted, const uint8_t colIdx)
{
	op_line_t *line = op_new(DRAW_OP_LINE);
	if (line){
		line->rt.x1 = x1;
		line->rt.y1 = y1;
		line->rt.x2 = x2;
		line->rt.y2 = y2;
		line->op.colourIdx = colIdx;	
		line->op.flags |= (DRAW_OP_FLAG_DOTTED*(isDotted&0x01));
	
		/*int total =*/ op_add(ops, line);
		//printf("op_addLine(): %i %i\n", total, line->op.type);
	}
}

void op_addRectangle (draw_ops_t *ops, const float x1, const float y1, const float x2, const float y2, const uint8_t isFilled, const uint8_t colIdx)
{
	op_rect_t *rect = op_new(DRAW_OP_RECT);
	if (rect){
		rect->rt.x1 = x1;
		rect->rt.y1 = y1;
		rect->rt.x2 = x2;
		rect->rt.y2 = y2;
		rect->op.colourIdx = colIdx;	
		rect->op.flags |= (DRAW_OP_FLAG_FILLED*(isFilled&0x01));
	
		/*int total =*/ op_add(ops, rect);
		//printf("op_addRectangle(): %i %i\n", total, rect->op.type);
	}
}

void op_addCircle (draw_ops_t *ops, const float xc, const float yc, const float radius, const uint8_t isFilled, const uint8_t colIdx)
{
	op_circle_t *circle = op_new(DRAW_OP_CIRCLE);
	if (circle){
		circle->pt1.x = xc;
		circle->pt1.y = yc;
		circle->radius = radius;
		circle->op.colourIdx = colIdx;	
		circle->op.flags |= (DRAW_OP_FLAG_FILLED*(isFilled&0x01));

		/*int total =*/ op_add(ops, circle);
		//printf("op_addCircle(): %i %i\n", total, circle->op.type);
	}
}

void op_addEllipse (draw_ops_t *ops, const float xc, const float yc, const float r1, const float r2, const uint8_t colIdx)
{
	op_ellipse_t *ellipse = op_new(DRAW_OP_ELLIPSE);
	if (ellipse){
		ellipse->pt1.x = xc;
		ellipse->pt1.y = yc;
		ellipse->pt2.x = r1;
		ellipse->pt2.y = r2;
		ellipse->op.colourIdx = colIdx;	
		ellipse->op.flags = 0;	

		/*int total =*/ op_add(ops, ellipse);
		//printf("op_addEllipse(): %i %i\n", total, ellipse->op.type);
	}
}

void op_addPoly (draw_ops_t *ops, pointFlt_t *pts, const uint8_t total, const uint8_t close, const uint8_t colIdx)
{
	op_poly_t *poly = op_new(DRAW_OP_POLYLINE);
	if (poly){
		poly->pts = pts;
		poly->op.var1 = total;
		poly->op.colourIdx = colIdx;	
		poly->op.flags |= (DRAW_OP_FLAG_CLOSEPLY*(close&0x01));

		/*int total =*/ op_add(ops, poly);
		//printf("op_addPoly(): %i %i\n", total, poly->op.type);
	}
}

void op_addArc (draw_ops_t *ops, const float x, const float y, const float r1, const float r2, const float a1, const float a2, const uint8_t colIdx)
{
	op_arc_t *arc = op_new(DRAW_OP_ARC);
	if (arc){
		arc->pt1.x = x;
		arc->pt1.y = y;
		arc->pt2.x = r1;
		arc->pt2.y = r2;
		arc->pt3.x = a1;
		arc->pt3.y = a2;
		arc->op.colourIdx = colIdx;	
		arc->op.flags = 0;	

		/*int total =*/ op_add(ops, arc);
		//printf("op_addArc(): %i %i\n", total, arc->op.type);
	}
}

void op_addTriangle (draw_ops_t *ops, const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const uint8_t isFilled, const uint8_t colIdx)
{
	op_triangle_t *triangle = op_new(DRAW_OP_TRIANGLE);
	if (triangle){
		triangle->pt1.x = x1;
		triangle->pt1.y = y1;
		triangle->pt2.x = x2;
		triangle->pt2.y = y2;
		triangle->pt3.x = x3;
		triangle->pt3.y = y3;
		triangle->op.colourIdx = colIdx;	
		triangle->op.flags |= (DRAW_OP_FLAG_FILLED*(isFilled&0x01));

		/*int total =*/ op_add(ops, triangle);
		//printf("op_addTriangle(): %i %i\n", total, triangle->op.type);
	}
}

void op_addCopy (draw_ops_t *ops, const float x1, const float y1, const float x2, const float y2, const float des_x, const float des_y)
{
	op_copy_t *copy = op_new(DRAW_OP_COPY);
	if (copy){
		copy->rt.x1 = x1;
		copy->rt.y1 = y1;
		copy->rt.x2 = x2;
		copy->rt.y2 = y2;
		copy->pt1.x = des_x;
		copy->pt1.y = des_y;
		copy->op.colourIdx = 0;
		copy->op.flags = 0;	

		/*int total =*/ op_add(ops, copy);
		//printf("op_addCopy(): %i %i\n", total, copy->op.type);
	}
}

void op_addImage (draw_ops_t *ops, _ufont_surface_t *surface, const int isCentered, const float des_x, const float des_y)
{
	op_image_t *img = op_new(DRAW_OP_IMAGE);
	if (img){
		img->surface = surface;
		img->pt1.x = des_x;
		img->pt1.y = des_y;
		img->op.colourIdx = 0;
		img->op.flags |= (DRAW_OP_FLAG_CENTERED*(isCentered&0x01));

		/*int total =*/ op_add(ops, img);
		//printf("op_addImage(): %i %i\n", total, copy->op.type);
	}
}

int op_render (op_base_t *cmd, _ufont_surface_t *surface)
{
	//printf("%i\n", cmd->op->type);
	
	const float width = DWIDTH;
	const float height = DHEIGHT;
	
	switch(cmd->op->type){
	  case DRAW_OP_LINE:
	  	opDrawLine(surface, (cmd->line->rt.x1*width), (cmd->line->rt.y1*height), (cmd->line->rt.x2*width), (cmd->line->rt.y2*height), cmd->op->flags&DRAW_OP_FLAG_DOTTED, cmd->op->colourIdx);
		break;
	  case DRAW_OP_RECT:
		opDrawRectangle(surface, (cmd->line->rt.x1*width), (cmd->line->rt.y1*height), (cmd->line->rt.x2*width), (cmd->line->rt.y2*height), cmd->op->flags&DRAW_OP_FLAG_FILLED, cmd->op->colourIdx);
	  	break;
	  case DRAW_OP_POINT:
	    opDrawPoint(surface, (cmd->pt->pt1.x*width), (cmd->pt->pt1.y*height), cmd->op->colourIdx);
	  	break;
	  case DRAW_OP_CIRCLE:
	    opDrawCircle(surface, (cmd->cir->pt1.x*width), (cmd->cir->pt1.y*height), cmd->cir->radius*height, cmd->op->flags&DRAW_OP_FLAG_FILLED, cmd->op->colourIdx);
	  	break;
	  case DRAW_OP_ELLIPSE:
	  	opDrawEllipse(surface, (cmd->elp->pt1.x*width), (cmd->elp->pt1.y*height), (cmd->elp->pt2.x*width), (cmd->elp->pt2.y*height), cmd->op->colourIdx);
	  	break;
	  case DRAW_OP_ARC:
		opDrawArc(surface, (cmd->arc->pt1.x*width), (cmd->arc->pt1.y*height), (cmd->arc->pt2.x*width), (cmd->arc->pt2.y*height), (cmd->arc->pt3.x*width), (cmd->arc->pt3.y*height), cmd->op->colourIdx);
		break;
	  case DRAW_OP_TRIANGLE:
		opDrawTriangle(surface, (cmd->tri->pt1.x*width), (cmd->tri->pt1.y*height), (cmd->tri->pt2.x*width), (cmd->tri->pt2.y*height), (cmd->tri->pt3.x*width), (cmd->tri->pt3.y*height), cmd->op->flags&DRAW_OP_FLAG_FILLED, cmd->op->colourIdx);
		break;
	  case DRAW_OP_POLYLINE:
	  	opDrawPoly(surface, cmd->poly->pts, cmd->op->var1, cmd->op->flags&DRAW_OP_FLAG_CLOSEPLY, cmd->op->colourIdx);
		break;
	  case DRAW_OP_COPY:
	    opDrawCopy(surface, (cmd->copy->rt.x1*width), (cmd->copy->rt.y1*height), (cmd->copy->rt.x2*width), (cmd->copy->rt.y2*height), (cmd->copy->pt1.x*width), (cmd->copy->pt1.y*height));
	  	break;
	  case DRAW_OP_IMAGE:
	    opDrawImage(surface, cmd->image->surface, cmd->op->flags&DRAW_OP_FLAG_CENTERED, (cmd->image->pt1.x*width), (cmd->image->pt1.y*height));
	    break;
	}
	
	return 1;	
}

int ops_execute (draw_ops_t *ops, _ufont_surface_t *surface)
{
	int ret = 0;
	for (int i = 0; i < ops->ct; i++)
		ret += op_render(&ops->list[i], surface);
	return ret;
}

int main (int argc, char **argv)
{

	if (!initDemoConfig("config.cfg"))
		return 0;

	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_G4TO16(4), NULL);

	_ufont_t font;
	if (!fontOpen(&font, NULL))
		return 0;

	fontSetDisplayBuffer(&font, frame->pixels, DWIDTH, DHEIGHT);
	fontSetRenderSurface(&font, surface);
	fontSetRenderColour(surface, COLOUR_24TO16(0xFFBF33));
	


	draw_ops_t *ops = ops_create(20);

	coli_add(COLi_BLACK,  COLOUR_BLACK);
	coli_add(COLi_WHITE,  COLOUR_WHITE);
	coli_add(COLi_RED,    COLOUR_RED);
	coli_add(COLi_GREEN,  COLOUR_GREEN);
	coli_add(COLi_BLUE,   COLOUR_BLUE);
	coli_add(COLi_CYAN,   COLOUR_CYAN);
	coli_add(COLi_YELLOW, COLOUR_YELLOW);
	coli_add(COLi_BROWN,  COLOUR_BROWN);
	coli_add(COLi_MAGENTA,COLOUR_MAGENTA);


#if 1
	_ufont_surface_t *image = fontCreateTexture("images/tex1_80.png");
	if (image){
		op_addImage(ops, image, DRAW_CENTERED, 0.0f, 0.0f);
		op_addImage(ops, image, 0, 0.5f, 0.7f);
	}


	op_addLine(ops, 0.001f, 0.002f, 0.513f, 0.54f, 0, COLi_RED);
	op_addLine(ops, 0.351f, 0.052f, 0.953f, 0.84f, DRAW_DOTTED, COLi_MAGENTA);
	op_addPoint(ops, 0.309f, 0.410f, COLi_RED);
	op_addCircle(ops, 0.809f, 0.500f, 0.1f, DRAW_FILLED, COLi_GREEN);
	op_addCircle(ops, 0.809f, 0.550f, 0.1f, DRAW_FILLED, COLi_WHITE);
	op_addEllipse(ops, 0.105f, 0.206f, 0.07f, 0.08f, COLi_BLUE);
	op_addRectangle(ops, 0.105f, 0.116f, 0.807f, 0.308f, DRAW_FILLED, COLi_MAGENTA);
	op_addRectangle(ops, 0.005f, 0.016f, 0.857f, 0.358f, 0, COLi_BROWN);

	pointFlt_t pts[6] = {{0.0f, 0.0f}, {0.1f, 0.0f}, {0.1f, 0.25f}, {0.5f, 0.5f}, {0.5f, 0.4f}, {0.9f, 0.9f}};
	op_addPoly(ops, pts, 6, 1, COLi_BLUE);
	
	op_addArc(ops, 0.105f, 0.106f, 0.07f, 0.08f, 0.09f, 0.01f, COLi_BLACK);
	op_addTriangle(ops, 0.05f, 0.06f, 0.07f, 0.08f, 0.09f, 0.01f, 0, COLi_CYAN);
	op_addCopy(ops, 0.00f, 0.00f, 0.7f, 0.2f, 0.101f, 0.301f);
	op_addTriangle(ops, 0.05f, 0.96f, 0.17f, 0.58f, 0.09f, 0.31f, DRAW_FILLED, COLi_YELLOW);
#endif	



	float x = 0.5f;
	float y = 0.5f;
	float w = 0.02f;
	float h = 0.1f;
	op_addRectangle(ops, x, y, x+w, y+h, DRAW_FILLED, COLi_BROWN);



	



	ops_execute(ops, fontGetDisplayBuffer(&font));
	
	
	//surfaceDrawLineDotted(fontGetDisplayBuffer(&font), 0, 0, 799, 479, COLOUR_BLUE);
	
	fontDrawLineDotted(&font, 0, 0, 799, 479, 1);
	
	ops_free(ops);


	fontApplySurface(&font, 0, 0);
	fontSurfaceFree(surface);
	fontClose(&font);

	lRefresh(frame);
	Sleep(5000);
	demoCleanup();

	return 1;
}


