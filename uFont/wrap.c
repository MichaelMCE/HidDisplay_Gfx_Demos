
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

//#define fontFile	UFDIR"unicode36.uf"
//#define fontFile	UFDIR"10x20.uf"
#define fontFile	UFDIR"utopia_b24.uf"
//#define fontFile	UFDIR"lucida18.uf";
//#define fontFile	UFDIR"unifont32.uf"
//#define fontFile	UFDIR"freesans32.uf"



int main (int argc, char **argv)
{

	if (!initDemoConfig("config.cfg"))
		return 0;


	uint8_t pixels[CALC_PITCH_1(DWIDTH) * DHEIGHT];
	memset(pixels, 0, sizeof(pixels));
	// this is where we'll render our text/pixels on to
	// supply a 1BPP buffer or pass NULL, one will then be created (alloc'd).
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_24TO16(0x777777), pixels);
	_ufont_t font;
	
	char *ufont = fontFile;
	if (argc == 2) ufont = argv[1];
	
	if (!fontOpen(&font, ufont)){
		printf("fontOpen() failed for '%s'\n", ufont);
		return 0;
	}
	printf("font '%s'\n", ufont);

	//fontSetGlyphPadding(&font, fontGetGlyphPadding(&font)+1);
	uint8_t *displayOutputBuffer = frame->pixels;
	fontSetDisplayBuffer(&font, displayOutputBuffer, DWIDTH, DHEIGHT);
	fontSetRenderSurface(&font, surface);
	fontSetRenderFlags(&font, BFONT_RENDER_NEWLINE|BFONT_RENDER_RETURN|BFONT_RENDER_NODEFAULT|BFONT_RENDER_ADVANCE_X|BFONT_RENDER_ADVANCE_Y /*|BFONT_RENDER_GLYPHRECT*/|BFONT_RENDER_BOUNDRECT);

	const uint8_t *str = (const uint8_t*)"This program is free software; you can redistribute it and/or "
	 "modify it under the terms of the GNU LIBRARY GENERAL PUBLIC LICENSE "
	 "as published by the Free Software Foundation; either version 2 "
	 "of the License, or (at your option) any later version.";


	int width, height;
	fontGetMetrics(&font, (uint8_t*)str, &width, &height);
	printf("metrics WxH: %i,%i\n", width, height);

	int y = 5;
	int x = 50;
	fontDrawLine(&font, x, 0, x, DHEIGHT-1, 1);
	fontPrint(&font, &x, &y, str);

	int desX = 50;
	x = desX;
	y += 25;
	fontSetRenderFlags(&font,BFONT_RENDER_WORDWRAP|BFONT_RENDER_NEWLINE|BFONT_RENDER_NODEFAULT|
		BFONT_RENDER_ADVANCE_X|BFONT_RENDER_ADVANCE_Y|BFONT_RENDER_BOUNDRECT);
	fontPrint(&font, &x, &y, str);

	x = desX;
	y += 40;
	fontSetRenderFlags(&font,BFONT_RENDER_WORDWRAP|BFONT_RENDER_NEWLINE|BFONT_RENDER_NODEFAULT|
		BFONT_RENDER_ADVANCE_X|BFONT_RENDER_ADVANCE_Y|BFONT_RENDER_GLYPHRECT);
	fontPrint(&font, &x, &y, str);


	fontApplySurface(&font, 0, 0);
	fontSurfaceFree(surface);
	fontClose(&font);

	lRefresh(frame);
	Sleep(200000);

	demoCleanup();
	return 1;
}


