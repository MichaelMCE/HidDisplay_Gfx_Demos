
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


#define FONTFILE		"../fonts/CJK/wqy-zenhei160.uf"		// default font
#define DETAILFONT		"../fonts/uf_Latin/helvr24.uf"		// metrics display font




static inline void clearFrame (void *buffer, const uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;
	
	int tPixels = DWIDTH * DHEIGHT;
	while (tPixels--) pixels[tPixels] = colour;		
}

static inline uint16_t hexToInt16 (const char *str)
{
	uint16_t enc = 0;

	if (tolower(str[0]) == 'x' || tolower(str[1]) == 'x'){
		int32_t enc32 = enc;
		sscanf(str, "x%X", &enc32);
		enc = enc32&0xFFFF;			
	}else{
		enc = atoi(str)&0xFFFF;
	}
	return enc;
}

int main (int argc, char **argv)
{
	if (!initDemoConfig("config.cfg"))
		return 0;

	char *fontFile = FONTFILE;	
	uint16_t enc = 0;
	
	if (argc == 2){
		enc = hexToInt16(argv[1]);
	}else if (argc == 3){
		fontFile = argv[1];
		enc = hexToInt16(argv[2]);		
	}else if (argc == 1){
		printf("Usage: glyph.exe font.uf <encoding>\n");
		printf("Usage: glyph.exe <encoding>\n\n");
		enc = 'A';
	}

	if (enc == 0){
		printf(" Invalid codepoint\n");
		demoCleanup();
		return 0;
	}

	printf("Font: '%s'\n", fontFile);
	printf("Glyph: 0x%.4X %i\n", enc, enc);

	_ufont_t font[2];
	if (!fontOpen(&font[0], fontFile)){
		printf(" Could not open font: '%s'\n", fontFile);
		return 0;
	}
		
	
	_glyph_t glyph;
	if (!metricsGetGlyphHeader(&font[0], enc, &glyph)){
		fontClose(&font[0]);
		printf(" Glyph not found\n");
		return 0;
	}
	
	if (!fontOpen(&font[1], DETAILFONT)){
		fontClose(&font[0]);
		printf(" Could not open font: '%s'\n", DETAILFONT);
		return 0;
	}	
	

	// 16bpp (565) distination [frame] buffer
	// this is what we're sending to the display
	uint8_t buffer[CALC_PITCH_16(DWIDTH)*DHEIGHT];
	fontSetDisplayBuffer(&font[0], buffer, DWIDTH, DHEIGHT);
	fontSetDisplayBuffer(&font[1], buffer, DWIDTH, DHEIGHT);
	fontSetRenderFlags(&font[1], BFONT_RENDER_NEWLINE|BFONT_RENDER_RETURN|BFONT_RENDER_ADVANCE_Y|BFONT_RENDER_ADVANCE_X);
	//fontSetRenderFlags(&font[0], BFONT_RENDER_BOUNDRECT| BFONT_RENDER_GLYPHRECT);
	clearFrame(buffer, COLOUR_CREAM);
	
	uint16_t text[2] = {enc, 0};
	int width = 0, height = 0;
	fontGetMetricsW(&font[0], text, &width, &height);

	_ufont_surface_t *surface[2];
	surface[0] = fontCreateSurface(width+32, height+32, COLOUR_24TO16(0x666666), NULL);
	surface[1] = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_24TO16(0x666666), NULL);
	fontSetRenderSurface(&font[0], surface[0]);
	fontSetRenderSurface(&font[1], surface[1]);
	fontSetLineSpace (&font[1], fontGetLineSpace(&font[1])+2);
	

	int x = 10, y = 10;
	uint8_t str[128];
	_snprintf((char*)str, sizeof(str), "Width:\r\nHeight:\r\nxOffset:\r\nyOffset:\r\nAdvance:\r\nBytes:");
	fontPrint(&font[1], &x, &y, str);

	x += 10; y = 10;
	_snprintf((char*)str, sizeof(str), "%i\r\n%i\r\n%i\r\n%i\r\n%i\r\n%i", glyph.w, glyph.h, glyph.xOffset, glyph.yOffset, glyph.dwidth, glyph.length);
	fontPrint(&font[1], &x, &y, str);

#if 0
	fontSetGlyphPadding(&font[1], -1);
	x = 0; y += 10;
	uint16_t dvb[] = {0xFFF0, 0xFFF1, 0xFFF2, 0};
	fontPrintW(&font[1], &x, &y, dvb);
#endif

	x = abs(surface[0]->width - width)/2;
	y = abs(surface[0]->height - height)/2;
	fontPrintW(&font[0], &x, &y, text);

	x = abs(DWIDTH - surface[0]->width)/2;
	y = abs(DHEIGHT - surface[0]->height)/2;
	fontSetRenderColour(surface[0], COLOUR_GREY);
	fontApplySurfaceOutlineEx(&font[0], x, y, 4);
	fontSetRenderColour(surface[0], COLOUR_BLACK);
	fontApplySurface(&font[0], x, y);

	fontApplySurface(&font[1], 0, 0);

	lUpdate(hw, buffer, sizeof(buffer));

	
	Sleep(50000);
	fontSurfaceFree(surface[0]);
	fontSurfaceFree(surface[1]);
	fontClose(&font[1]);
	fontClose(&font[0]);

	demoCleanup();

	return 1;
}

