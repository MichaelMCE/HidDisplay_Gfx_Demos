
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



typedef struct{
	int id;
	int width;
	int height;
	char file[128];
}uf_met_t;



#define RENDERFLAGS	(BFONT_RENDER_DEFAULT )
//#define RENDERFLAGS	(BFONT_RENDER_DEFAULT | BFONT_RENDER_FIXEDWIDTH)

static const char *text = "The quick brown fox jumps over the lazy dog.0123456789:";


static inline void swapi32 (int *a, int *b)
{
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

static inline void sortByWidth (uf_met_t *fid, int total)
{
	if (!total) return;
	
	char pathtemp[128];	
	int i = --total;

	while(i--){
		for (int j = 0; j < total; j++){
			if (fid[j].width > fid[j+1].width){
				swapi32(&fid[j].id, &fid[j+1].id);
				swapi32(&fid[j].height, &fid[j+1].height);
				swapi32(&fid[j].width, &fid[j+1].width);
				
				strcpy(pathtemp, fid[j+1].file);
				strcpy(fid[j+1].file, fid[j].file);
				strcpy(fid[j].file, pathtemp);
			}
		}
	}
}

static inline void clearFrame (void *buffer, const uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;
	
	int tPixels = DWIDTH * DHEIGHT;
	while (tPixels--) pixels[tPixels] = colour;		
}

int main (int argc, char **argv)
{

	if (!initDemoConfig("config.cfg"))
		return 0;


	uint8_t buffer[CALC_PITCH_16(DWIDTH)*DHEIGHT];
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_G4TO16(6), NULL);

	const int total = (sizeof(fonts) / sizeof(*fonts))-1;
	uf_met_t ufm[total];

	_ufont_t font;

	for (int i = 0; fonts[i]; i++){
		if (fontOpen(&font, fonts[i])){
			fontSetRenderFlags(&font, RENDERFLAGS);
			
			int width = 0;
			int height = 0;
			fontGetMetrics(&font, text, &width, &height);

			ufm[i].id = i;
			ufm[i].width = width;
			ufm[i].height = height;
			strcpy(ufm[i].file, fonts[i]);
			fontClose(&font);
		}else{
			printf("failed to open %i: '%s'\n", i, fonts[i]);
		}
	}

//	return 0;
	sortByWidth(ufm, total);

	int x = 2, y = 1;
	clearFrame(buffer, COLOUR_CREAM);
	
	for (int i = 0; i < total; i++){
		if (!fontOpen(&font, fonts[ufm[i].id])){
			printf("failed to open %i: '%s'\n", i, fonts[ufm[i].id]);
			continue;
		}

		fontSetDisplayBuffer(&font, buffer, DWIDTH, DHEIGHT);
		fontSetRenderSurface(&font, surface);
		fontSetRenderFlags(&font, RENDERFLAGS);

		if (y+ufm[i].height >= DHEIGHT){
			y = 1;
			fontCleanSurface(&font, NULL);
			clearFrame(buffer, COLOUR_CREAM);
			Sleep(2000);
			printf("\n");
		}

		printf("font %i %i: \t'%s'\n", (font.header.flags&BFONT_FLAGS_MONOSPACE)>0, ufm[i].id, fonts[ufm[i].id]);
		
		if (!fontPrint(&font, &x, &y, text))
			printf(" font render failed\n");

		fontApplySurface(&font, 0, 0);
		lUpdate(hw, buffer, sizeof(buffer));
		fontClose(&font);
	}

	Sleep(5000);
	demoCleanup();

	return 1;
}


