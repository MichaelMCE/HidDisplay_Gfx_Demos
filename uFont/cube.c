
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


#ifndef DEGTORAD
#define DEGTORAD 0.034906585039f
#endif


typedef struct{
	float x[8];
	float y[8];
	float z[8];
}tcube_t;


static tcube_t cube = {
	{1,  1, -1, -1, -1,  1,  1, -1},
	{1,  1,  1,  1, -1, -1, -1, -1},
	{1, -1, -1,  1,  1,  1, -1, -1}
};

static tcube_t cube2;
static _ufont_t font;

static float flength = 320.0f;
static float camz = -4.5f;
static int destx;
static int desty;




static inline void point3DTo2D (const float x, const float y, const float z, const float flength, const float camz, const int x0, const int y0, int *screenx, int *screeny)
{
	if (z-camz < 0.0f) return;
	const float tmp = flength/(z-camz);
	*screenx = (x*tmp)+x0;
	*screeny = (y*tmp)+y0;
}


static inline void rotateX (const float angle, const float y, const float z, float *yr, float *zr)
{
	*yr = y*cosf(angle) - z*sinf(angle);
	*zr = y*sinf(angle) + z*cosf(angle);
}

static inline void rotateY (const float angle, const float x, const float z, float *xr, float *zr)
{
	*xr =  x*cosf(angle) + z*sinf(angle);
	*zr = -x*sinf(angle) + z*cosf(angle);
}

static inline void rotateZ (const float angle, const float x, const float y, float *xr, float *yr)
{
	*xr = x*cosf(angle) - y*sinf(angle);
	*yr = x*sinf(angle) + y*cosf(angle);
}

static inline void drawCube (tcube_t *cube)
{
	int sx = 0, sy = 0;
	int sx2 = 0, sy2 = 0;

	for (int i = 0; i < 7; i++){
		point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx,&sy);
		point3DTo2D(cube->x[i+1],-cube->y[i+1],cube->z[i+1],flength,camz,destx,desty,&sx2,&sy2);
		fontDrawLine(&font, sx, sy, sx2, sy2,  1);
	}
	
	int i = 7;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx,&sy);
	i = 2;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx2,&sy2);
	fontDrawLine(&font,sx,sy,sx2,sy2,  1);


	i = 0;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx,&sy);
	i = 5;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx2,&sy2);
	fontDrawLine(&font,sx,sy,sx2,sy2, 1);	
	
	i = 1;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx,&sy);
	i = 6;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx2,&sy2);
	fontDrawLine(&font,sx,sy,sx2,sy2, 1);

	i = 0;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx,&sy);
	i = 3;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx2,&sy2);
	fontDrawLine(&font,sx,sy,sx2,sy2, 1);
	
	i = 4;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx,&sy);
	i = 7;
	point3DTo2D(cube->x[i],-cube->y[i],cube->z[i],flength,camz,destx,desty,&sx2,&sy2);
	fontDrawLine(&font, sx, sy, sx2, sy2, 1);
}

static inline void rotateCube (tcube_t *in, tcube_t *out, float anglex, float angley, float anglez)
{
	anglex *= DEGTORAD;
	angley *= DEGTORAD;
	anglez *= DEGTORAD;
	memcpy(out, in, sizeof(tcube_t));
	
	for (int i = 0; i < 8; i++){
		rotateX(anglex, out->y[i], out->z[i], &out->y[i], &out->z[i]);
		rotateY(angley, out->x[i], out->z[i], &out->x[i], &out->z[i]);
		rotateZ(anglez, out->x[i], out->y[i], &out->x[i], &out->y[i]);
	}
}

static inline void clearFrame (void *buffer, const uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;
	
	int tPixels = DWIDTH * DHEIGHT;
	while (tPixels--) pixels[tPixels] = colour;		
}

int main ()
{
	if (!initDemoConfig("config.cfg"))
		return 0;


	if (!fontOpen(&font, NULL))
		return 0;
		
	uint8_t buffer[CALC_PITCH_16(DWIDTH)*DHEIGHT];
	
	fontSetDisplayBuffer(&font, buffer, DWIDTH, DHEIGHT);
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_24TO16(0xFFBF33), NULL);
	fontSetRenderSurface(&font, surface);


	flength = DWIDTH * 0.83f;
	destx = DWIDTH / 2;
	desty = DHEIGHT / 2;

	
	float x0 = 99.0f, y0 = 100.0f, z0 = 99.0f;
	const float step = 0.43f;
 	int tframes = 0;
	int start = GetTickCount();

	for (float a = 0.0f; a < 3600.0f; a+=step){
		clearFrame(buffer, COLOUR_WHITE);
		fontCleanSurface(NULL, surface);

		rotateCube(&cube, &cube2, x0, y0+a, z0);
		drawCube(&cube2);

		fontSetRenderColour(surface, COLOUR_24TO16(0x775535));
		fontApplySurface(&font, 0, 0);
		fontSetRenderColour(surface, COLOUR_RED);
		fontApplySurfaceOutlineEx(&font, 0, 0, 15);

		lUpdate(hw, buffer, sizeof(buffer));

		//lSleep(15);
		tframes++;
	}

	int end = GetTickCount();
	double time = (double)(end-start)*0.001;
	printf("frames:%d\ntime:%.2fs\nfps:%.1f\n\n",tframes,time,tframes/time);

	fontSurfaceFree(surface);
	fontClose(&font);

	demoCleanup();
	return 1;
}
