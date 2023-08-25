
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



static inline char *stristr (const char *string, const char *pattern)
{
	if (!string || !pattern)
		return NULL;
	
	char *pptr, *sptr, *start;
	
	for (start = (char*)string; *start != 0; start++){
		for ( ; ((*start != 0) && (toupper(*start) != toupper(*pattern))); start++){
		}
		
		if (!*start) return NULL;

		pptr = (char*)pattern;
		sptr = (char*)start;

		while (toupper(*sptr) == toupper(*pptr)){
			sptr++;
			pptr++;
			if (!*pptr)
				return (start);
		}
	}
	return NULL;
}

static inline int utilFontLookup (const char *name)
{
	int i = 0;
	
	do{
		if (stristr(fonts[i], name))
			return i;
	}while(fonts[++i]);

	return 0;
}

void clearFrame (TFRAME *frame)
{
	lClearFrameClr(frame, COLOUR_CREAM);
}

int main (int argc, char* argv[])
{

	if (!initDemoConfig("config.cfg"))
		return 0;

	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_CREAM, NULL);
	if (!surface) return 0; 	// what have we done here..
	
	_ufont_surface_t *sprite[2];
	sprite[1] = fontCreateTexture("images/spot8.png");
	sprite[0] = fontCreateTexture("images/shadow6.png");
	clearFrame(frame);

	int x = 6;
	int y = 4;
	
	_ufont_t font;
	const int total = sizeof(fonts) / sizeof(*fonts);
	
	int start = utilFontLookup("london38");
	
	for (int i = start; i < total && y < DHEIGHT-1; i++){
		if (!fontOpen(&font, fonts[i])){
			printf("font open failed for %i: '%s'\n", i, fonts[i]);
			continue;
		}
			
		printf("font %i: '%s'\n", i, fonts[i]);
		
		fontSetDisplayBuffer(&font, frame->pixels, DWIDTH, DHEIGHT);
		fontSetRenderSurface(&font, surface);
		fontSetLineSpace(&font, 6);
		
		fontPrint(&font, &x, &y, "0123456789:-");

		fontApplySurfaceSprite(&font, sprite[i&0x01], COLOUR_BLACK, 2, 2, 0, 0);
		fontCleanSurface(&font, NULL);
		fontClose(&font);
		
		lRefresh(frame);
		//lSleep(1000);
	}
	
	fontSurfaceFree(surface);
	fontSurfaceFree(sprite[0]);
	fontSurfaceFree(sprite[1]);
	
	lRefresh(frame);
	lSleep(20000);
	demoCleanup();
	
	return 1;
}
