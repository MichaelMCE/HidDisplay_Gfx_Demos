#ifndef _DRAW_OP_H
#define _DRAW_OP_H

#define DRAW_OP_INVALID			0		// something went wrong - not an op
#define DRAW_OP_POINT			1
#define DRAW_OP_LINE			2
#define DRAW_OP_RECT			3
#define DRAW_OP_CIRCLE			4
#define DRAW_OP_ELLIPSE			5
#define DRAW_OP_ARC				6
#define DRAW_OP_TRIANGLE		7
#define DRAW_OP_POLYLINE		8
#define DRAW_OP_COPY			9
#define DRAW_OP_IMAGE			10
#define DRAW_OPS_TOTAL			11		// number of op commands+1. used for array size calcs'


#define DRAW_OP_FLAG_FILLED		0x01
#define DRAW_OP_FLAG_INVERT		0x02
#define DRAW_OP_FLAG_DOTTED		0x04
#define DRAW_OP_FLAG_CLOSEPLY	0x08	// close polygon (connect last to first point)
#define DRAW_OP_FLAG_CENTERED	0x10	// center image over x/y



typedef struct {
	uint8_t type;		// which draw op			(DRAW_OP_xxxx)
	uint8_t flags;		// flags local to draw op	(DRAW_OP_FLAG_xxxx)
	uint8_t colourIdx;	// index in to colour table
	uint8_t var1;
}draw_op_t;

typedef struct {
	float x;
	float y;
}pointFlt_t;

typedef struct {
	float x1;
	float y1;
	float x2;
	float y2;
}rectFlt_t;

typedef struct {
	draw_op_t op;
	
	pointFlt_t pt1;		// x y
}op_point_t;

typedef struct {
	draw_op_t op;
	
	rectFlt_t rt;		// x1 y1, x2 y2
}op_line_t;

typedef struct {
	draw_op_t op;		// var1:total polys. 2 lines equates to 3 points. Maximum number of points is 255
	
	pointFlt_t *pts;	// x y
}op_poly_t;

typedef struct {
	draw_op_t op;
	
	rectFlt_t rt;		// x1 y1, x2 y2
}op_rect_t;

typedef struct {
	draw_op_t op;
	
	pointFlt_t pt1;		// x y
	float radius;
}op_circle_t;

typedef struct {
	draw_op_t op;
	
	pointFlt_t pt1;		// x  y
	pointFlt_t pt2;		// r1 r2
}op_ellipse_t;

typedef struct {
	draw_op_t op;
	
	pointFlt_t pt1;		// x  y
	pointFlt_t pt2;		// r1 r2
	pointFlt_t pt3;		// a1 a2
}op_arc_t;

typedef struct {
	draw_op_t op;
	
	pointFlt_t pt1;		// x1  y1
	pointFlt_t pt2;		// x2  y2
	pointFlt_t pt3;		// x3  y3
}op_triangle_t;

typedef struct {
	draw_op_t op;

	rectFlt_t rt;		// source x1 y1, x2 y2
	pointFlt_t pt1;		// des x y
}op_copy_t;

typedef struct {
	draw_op_t op;
	
	_ufont_surface_t *surface;
	pointFlt_t pt1;		// des x y
}op_image_t;

typedef union {
	draw_op_t     *op;

	op_point_t    *pt;
	op_line_t     *line;
	op_poly_t     *poly;
	op_rect_t     *rect;
	op_circle_t   *cir;
	op_ellipse_t  *elp;
	op_arc_t      *arc;
	op_triangle_t *tri;
	op_copy_t     *copy;
	op_image_t	  *image;
}op_base_t;


typedef struct {
	uint16_t  total;	// max possible ops
	uint16_t  ct;		// total ops added (size of queue)
	op_base_t *list;
}draw_ops_t;



#endif



