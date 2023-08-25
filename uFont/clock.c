
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
#include <time.h>




//#define FONTFILE		"uf_digits/nanosecondthickbrk100.uf"		// 320x240
//#define FONTFILE		"uf/break84.uf"		// 320x240
#define FONTFILE		UFDIR"ravebold180.uf"



static inline void clearFrame (void *buffer, const uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;
	
	int tPixels = DWIDTH * DHEIGHT;
	while (tPixels--) pixels[tPixels] = colour;		
}


static inline int getCharWidth (_ufont_t *font, uint8_t ch)
{
	int width = 0;
	uint16_t text[] = {ch, 0};
	fontGetMetricsList(font, text, &width, NULL);
	return width;
}


// find the smallest rect that will fit '88:88:88'
static inline void calcClockRect (_ufont_t *font, int *width, int *height)
{
	int maxW = 0;
	int maxH = 0;
	
	for (int i = '0'; i <= '9'; i++){
		uint8_t text[] = {i, i, ':', i, i, ':', i, i, 0};
		fontGetMetrics(font, text, width, height);
		//printf("getClockRect: %i %i\n", *width, *height);
		
		if (*width > maxW) maxW = *width;
		if (*height > maxH) maxH = *height;
	}

	*width = maxW;
	*height = maxH;
}

int main (int argc, char **argv)
{

	if (!initDemoConfig("config.cfg"))
		return 0;

	char *fontFile = FONTFILE;
	if (argc == 2) fontFile = argv[1];
	printf("File: '%s'\n", fontFile);

	//lSleep(5000);

	_ufont_t font;
	if (!fontOpen(&font, fontFile))
		return 0;
		
	// 16bpp (565) distination [frame] buffer
	// this is what we're sending to the display
	uint8_t buffer[CALC_PITCH_16(DWIDTH)*DHEIGHT];
	memset(buffer, 0, sizeof(buffer));

	_ufont_surface_t *texture = fontCreateTexture("images/carbon2.png");
	fontSetDisplayBuffer(&font, buffer, DWIDTH, DHEIGHT);
	fontSetGlyphPadding(&font, 0);

	char timestr[12];
	int width = 0;
	int height = 0;
	calcClockRect(&font, &width, &height);
	//printf("::%i %i\n", width, height);

	_ufont_surface_t *surface = fontCreateSurface(width, height, COLOUR_24TO16(0x777777), NULL);
	fontSetRenderSurface(&font, surface);
	fontSetRenderFlags(&font, BFONT_RENDER_ADVANCE_X/*|BFONT_RENDER_BOUNDRECT| BFONT_RENDER_GLYPHRECT*//*|BFONT_RENDER_CONDENSED*/);

	while (1){
		clearFrame(buffer, COLOUR_CREAM);
		fontCleanSurface(&font, NULL);
		//fontDrawRectangle(&font, 0, 0, surface->width-1, surface->height-1, 1);
	
		time_t t = time(0);
		struct tm *tdate = localtime(&t);
		strftime(timestr, sizeof(timestr), "%H:%M:%S", tdate);
		fontGetMetrics(&font, (uint8_t*)timestr, &width, &height);
		
		int x = abs(surface->width - width)/2;
		int y = abs(surface->height - height)/2;
		//y = -font.header.fontDescent;
		fontPrint(&font, &x, &y, (uint8_t*)timestr);
		
		x = (DWIDTH - surface->width)/2;
		y = (DHEIGHT - surface->height)/2;
		fontApplySurfaceTexture(&font, texture, x, y);
		fontApplySurfaceOutline(&font, x, y);
		
		lUpdate(hw, buffer, sizeof(buffer));
		Sleep(200);
	}
	
	fontTextureFree(texture);
	fontSurfaceFree(surface);
	fontClose(&font);

	demoCleanup();

	return 1;
}

