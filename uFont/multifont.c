
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

int main (int argc, char **argv)
{

	if (!initDemoConfig("config.cfg"))
		return 0;

	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_G4TO16(4), NULL);
	
	const int total = sizeof(fonts) / sizeof(*fonts);
	_ufont_t font[total];
	memset(font, 0, sizeof(font));
	
	int x = 0, y = 0;
	uint16_t colours[] = {COLOUR_24TO16(0xBB77BB), COLOUR_24TO16(0xBB7777),
						  COLOUR_24TO16(0x77BB77), COLOUR_24TO16(0x7777BB)};

	int start = utilFontLookup("8x16");	
	for (int i = start; i < start+4; i++){
		if (!fontOpen(&font[i], fonts[i])){
			printf("fontOpen() failed for %i:'%s'\n", i, fonts[i]);
			continue;
		}
		printf("font %i: '%s'\n", i, fonts[i]);

		fontSetDisplayBuffer(&font[i], frame->pixels, DWIDTH, DHEIGHT);
		fontSetRenderSurface(&font[i], surface);
		fontSetRenderColour(surface, colours[i-start]);
		fontSetRenderFlags(&font[i], BFONT_RENDER_DEFAULT);
		
		fontPrint(&font[i], &x, &y, "The quick brown fox jumps over the lazy dog");
		fontApplySurface(&font[i], 0, 0);
		fontCleanSurface(&font[i], NULL);
		fontClose(&font[i]);
	}

	start = utilFontLookup("unifont32");	
	for (int i = start; i < start+3; i++){
		if (!fontOpen(&font[i], fonts[i]))
			continue;

		printf("font %i: '%s'\n", i, fonts[i]);
		
		char buffer[32];
		_snprintf(buffer, sizeof(buffer), "%i:", i+1);
		fontSetDisplayBuffer(&font[i], frame->pixels, DWIDTH, DHEIGHT);
		fontSetRenderSurface(&font[i], surface);
		fontSetRenderColour(surface, COLOUR_24TO16(0x777777));
		x = 2;
		fontSetRenderFlags(&font[i], BFONT_RENDER_ADVANCE_X /*| BFONT_RENDER_BOUNDRECT*/ |BFONT_RENDER_GLYPHRECT);
		fontPrint(&font[i], &x, &y, buffer);
		fontSetRenderFlags(&font[i], BFONT_RENDER_ADVANCE_Y | BFONT_RENDER_BOUNDRECT| BFONT_RENDER_GLYPHRECT | BFONT_RENDER_NEWLINE | BFONT_RENDER_RETURN);
		fontPrint(&font[i], &x, &y, " The\r\nQuick\nbrown fox jumps over the lazy\rdog");
	}

	fontApplySurfaceOutline(&font[start], 0, 0);
	fontSurfaceFree(surface);

	//for (int i = 15; i < 18; i++)
		//fontClose(&font[i]);

	lRefresh(frame);
	Sleep(50000);

	demoCleanup();

	return 1;
}


