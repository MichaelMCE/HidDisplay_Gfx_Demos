

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



#define MAXSTARS	384
#define LAYERS		7
#define SPEED		0.20f


typedef struct __attribute__((packed)){
	int32_t x;
	int32_t y;
	uint8_t plane;
}TStar;


static inline void clearFrame (void *buffer, const uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;
	
	int tPixels = DWIDTH * DHEIGHT;
	while (tPixels--) pixels[tPixels] = colour;		
}

static inline uint16_t gray8to16 (const uint8_t val)
{
	return COLOUR_24TO16(COLOUR_G8TO24(val));
}


int main (int argc, char* argv[])
{

	if (!initDemoConfig("config.cfg"))
		return 0;
	

	uint16_t colour[7] = {
	   	gray8to16(0x33),
	   	gray8to16(0x44),
	   	gray8to16(0x55),
	   	gray8to16(0x76),
	   	gray8to16(0x97),
	   	gray8to16(0xAA),
	   	gray8to16(0xFF)
	};
	
    TStar *stars = (TStar*)calloc(sizeof(TStar), MAXSTARS);
	srand(GetTickCount()%9997);
	
    for (int i = 0; i < MAXSTARS; i++){
        stars[i].x = rand() % (frame->width<<2);	// no need for the shifts when using float
        stars[i].y = rand() % (frame->height<<2);
        stars[i].plane = rand() % LAYERS;     // star colour [idx] between 0 and LAYERS
    }
    

    _ufont_surface_t *surface[LAYERS];
    for (int i = 0; i < LAYERS; i++){
    	uint16_t col = (int)(((7.0f/(float)LAYERS)*(float)i));
		surface[i] = fontCreateSurface(DWIDTH, DHEIGHT, colour[col%7], NULL);
	}

	
	_ufont_surface_t *sprite[LAYERS];
	memset(sprite, 0, sizeof(sprite));
	sprite[LAYERS-2] = fontCreateTexture("images/spot8a.png");
	sprite[LAYERS-1] = fontCreateTexture("images/spot8b.png");

	_ufont_t font;
	if (!fontOpen(&font, NULL))
		return 0;
	
	uint8_t buffer[CALC_PITCH_16(DWIDTH)*DHEIGHT];
	fontSetDisplayBuffer(&font, buffer, DWIDTH, DHEIGHT);
	const int pitch = CALC_PITCH_1(DWIDTH);	
	
	while (!kbhit()){
		clearFrame(buffer, COLOUR_BLACK);
		
		for (int i = 0; i < MAXSTARS; i++){
			stars[i].x += (((1.0f+(float)stars[i].plane*4.0f))*SPEED)*1.0f;
			if (stars[i].x >= (frame->width<<2)){
				stars[i].x = 0;
				stars[i].y = rand() % (frame->height<<2);
				//stars[i].plane = rand() % LAYERS;
			}
			setPixel1_nb(surface[stars[i].plane], pitch, stars[i].x>>2, stars[i].y>>2);
		}

		for (int i = 0; i < LAYERS; i++){
			fontSetRenderSurface(&font, surface[i]);
			if (i >= LAYERS-2)
				fontApplySurfaceSprite(&font, sprite[i], 0, 1, 1, 0, 0);
			else
				fontApplySurface(&font, 0, 0);

			fontCleanSurface(NULL, surface[i]);
		}
		lUpdate(hw, buffer, sizeof(buffer));
		lSleep(20);
	}

	fontClose(&font);
	fontSurfaceFree(sprite[LAYERS-2]);
	fontSurfaceFree(sprite[LAYERS-1]);
	
	for (int i = 0; i < LAYERS; i++)
		fontSurfaceFree(surface[i]);

	free(stars);
	demoCleanup();
	
	return 1;
}
