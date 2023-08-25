
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


static _ufont_t font;





/* Animation speed (50.0 mimics the original GLUT demo speed) */
#define ANIMATION_SPEED	(80.0f)
#define BALLWIDTH		(7.33f)
#define BOUNCE_WIDTH    (DWIDTH-2)
#define BOUNCE_HEIGHT   (DHEIGHT-2)
#define WALL_L_OFFSET   -BALLWIDTH
#define WALL_R_OFFSET   -BALLWIDTH

static float ball_x = 0.0f;
static float ball_y = 0.0f;
static uint64_t tStart;
static double resolution;


static inline void setRes ()
{
	uint64_t freq;
	QueryPerformanceCounter((LARGE_INTEGER*)&tStart);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	resolution = 1.0 / (double)freq;
}


static inline double getTime ()
{
	uint64_t t1 = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&t1);
	return ((double)((uint64_t)(t1 - tStart) * resolution) * 1000.0);
}

static inline float sin_deg (const float deg)
{
	return sinf(DEG2RAD * deg);
}

static inline void bounceBall (const float dt)
{
	static float ball_x_inc = 1.0f;
	static float ball_y_inc = 2.0f;
	
	
	/* Bounce on walls */
	if (ball_x >  (BOUNCE_WIDTH/2 + WALL_R_OFFSET))
		ball_x_inc = -0.5f - 0.75f * (float)rand() / (float)RAND_MAX;
   
	if (ball_x < -(BOUNCE_WIDTH/2 + WALL_L_OFFSET))
		ball_x_inc =  0.5f + 0.75f * (float)rand() / (float)RAND_MAX;

	/* Bounce on floor / roof */
	if (ball_y > (BOUNCE_HEIGHT/2.0f)-BALLWIDTH)
		ball_y_inc = -0.75f - 1.0f * (float)rand() / (float)RAND_MAX;

	if (ball_y < -(BOUNCE_HEIGHT/2.0f)+BALLWIDTH)
		ball_y_inc =  0.75f + 1.0f * (float)rand() / (float)RAND_MAX;

	/* Update ball position */
	ball_x += ball_x_inc * (dt*ANIMATION_SPEED);
	ball_y += ball_y_inc * (dt*ANIMATION_SPEED);

	
	// Simulate the effects of gravity on Y movement.
	
	float deg = (ball_y + BOUNCE_HEIGHT/2.0f) * 90.0f / BOUNCE_HEIGHT;
	if (deg > 80.0f) deg = 80.0f;
	if (deg < 10.0f) deg = 10.0f;

	float sign = 1.6f;   
	if (ball_y_inc < 0.0f)
		sign = -0.7f;
	//else
	//	sign = 1.0f;

	ball_y_inc = (sign * 4.0f * sin_deg(deg));
}

static inline void clearFrame (TFRAME *frame)
{
	lClearFrameClr(frame, COLOUR_WHITE);
}

static inline void drawFrame ()
{
	fontDrawRectangle(&font, 0, 0, DWIDTH-1, DHEIGHT-1, 1);
	fontDrawRectangle(&font, 1, 1, DWIDTH-2, DHEIGHT-2, 1);
}

static inline void drawBall (int x, int y)
{
	fontDrawCircleFilled(&font, x, y, BALLWIDTH, 1);
}

int main ()
{
	if (!initDemoConfig("config.cfg"))
		return 0;

	if (!fontOpen(&font, NULL))
		return 0;
		
	fontSetDisplayBuffer(&font, frame->pixels, DWIDTH, DHEIGHT);
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_24TO16(0xFF8F33), NULL);
	fontSetRenderSurface(&font, surface);

	setRes();
	//srand(getTime());
	time_t t;
	srand(((time(&t)&77)*GetTickCount())%1000);

	int x = 29000;
	double t0, t_old = 0.0;
	ball_x = (int)(rand()%DWIDTH)-(DWIDTH/2);
	ball_y = (int)(rand()%DHEIGHT)-(DHEIGHT/2);

	_ufont_palette_t *pal = fontPaletteLoad("palettes/pal3.png");

	do{
		//clearFrame(frame);
		//fontCleanSurface(NULL, surface);
		
		drawFrame();
		drawBall(ball_x+(DWIDTH>>1), ball_y+(DHEIGHT>>1));
		fontApplySurfacePalette(&font, pal, 0, 0);
		//fontApplySurfaceOutline(&font, 0, 0);
		
		lUpdate(hw, frame->pixels, frame->frameSize);

		t0 = getTime()/1000;
		bounceBall(t0 - t_old);
		t_old = t0;
		
	//	Sleep(10);
	}while(x--);


	fontPaletteFree(pal);
	fontSurfaceFree(surface);
	fontClose(&font);

	demoCleanup();
	return 0;
}
