/****************************
* Mysticfy
* Written by Hung Ki Chan
* Written by Michael McElligott
* One clear but cold day in April 2006
* Loosely based on the myLCD Ball example
****************************/


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



#define TOTAL_NODES		16

typedef struct{
	int dx;
	int dy;
	int px;
	int py;
}tlink_t;


static tlink_t nodes[TOTAL_NODES];
static _ufont_t font;



void mystifyDrawLineConnects (tlink_t *nodes, const int tnodes)
{
	for (int i = 0; i < tnodes-1; i++){
		fontDrawLine(&font, nodes[i].px, nodes[i].py, nodes[i+1].px, nodes[i+1].py, 1);
		fontDrawCircleFilled(&font, nodes[i].px, nodes[i].py, 3, 1);
	}
	
	fontDrawLine(&font, nodes[tnodes-1].px, nodes[tnodes-1].py, nodes[0].px, nodes[0].py, 1);
	fontDrawCircleFilled(&font, nodes[tnodes-1].px, nodes[tnodes-1].py, 3, 1);
}

//update all the nodes coords. adjust each node's speed
//important, dx and dy not =0 or u get strange things happen
void mystifyUpdateNodes (tlink_t *nodes, const int tnodes)
{
	for (int i = 0; i < tnodes; i++){
		nodes[i].px += nodes[i].dx;
		nodes[i].py += nodes[i].dy;
			  
		if (nodes[i].px < 0){
			nodes[i].px = 0;
			nodes[i].dx = 1+(rand()&1);		   //different speed, very important, so u dont get same shape
			
		}else if (nodes[i].px >= DWIDTH-1){
			nodes[i].px = DWIDTH-1;
			nodes[i].dx = -1*(1+(rand()&1));	   //not so sure but works, yup this should be invert
		}
			  
		if (nodes[i].py < 0){
			nodes[i].py = 0;
			nodes[i].dy = 1+(rand()&1);
			
		}else if (nodes[i].py > DHEIGHT-1){
			nodes[i].py = DHEIGHT-1;
			nodes[i].dy = -1*(1+(rand()&1));
		}
	}
}

void mystifyInit (tlink_t *nodes, const int tnodes)
{
	for (int i = 0; i < tnodes; i++){
		nodes[i].px = rand() % DWIDTH;
		nodes[i].py = rand() % DHEIGHT;
		nodes[i].dx = 1;
		nodes[i].dy = 1;
	}
	
	mystifyUpdateNodes(nodes, tnodes);
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
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_24TO16(0x777777), NULL);
	fontSetRenderSurface(&font, surface);		

	time_t t;
	srand(((time(&t)&77)*GetTickCount())%1000);
	mystifyInit(nodes, TOTAL_NODES);
	
	_ufont_surface_t *sprite = fontCreateTexture("images/bs.png");
	_ufont_palette_t *pal = fontPaletteLoad("images/pal3.png");

	do{
		clearFrame(buffer, COLOUR_WHITE);
		fontCleanSurface(NULL, surface);
		
		mystifyDrawLineConnects(nodes, TOTAL_NODES);
		mystifyUpdateNodes(nodes, TOTAL_NODES);

		//fontApplySurfaceSprite(&font, sprite, COLOUR_BLACK, 1, 1, 0, 0);
		fontApplySurfaceOutlineEx(&font, 0, 0, 2);
		//fontApplySurfacePalette(&font, pal, 0, 0);

		lUpdate(hw, buffer, sizeof(buffer));
		//lSleep(20);
	}while(1);


	fontSurfaceFree(sprite);
	fontSurfaceFree(surface);
	fontPaletteFree(pal);
	fontClose(&font);

	demoCleanup();
	return 1;
}
