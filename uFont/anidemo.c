
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
#include "images/image130x130.h"
#include "images/abstract128x128.h"


#define QUICKFONT	UFDIR"riskbold38.uf"
#define DIGITFONT	UFDIR"11s0blt.uf"
#define UNIFONT		"../fonts/uf/unifont32.uf"	// for the CJK glyphs



static uint64_t freq;
static uint64_t tStart;
static double resolution;





static void setRes ()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&tStart);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	resolution = 1.0 / (double)freq;
}


static double getTime ()
{
	uint64_t t1 = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&t1);
	return ((double)((uint64_t)(t1 - tStart) * resolution) * 1000.0);
}



void _clearFrame (TFRAME *frame)
{
	lClearFrameClr(frame, COLOUR_CREAM);
}


int main (int argc, char **argv)
{

#if !BUILD_PNG
	printf("BUILD_PNG not compiled in.");
	return 0;
#endif

	if (!initDemoConfig("config.cfg"))
		return 0;

	setRes();

	_ufont_t font;
	_ufont_t font2;

	
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_G4TO16(4), NULL);

	//_ufont_surface_t *texture = fontCreateTexture("images/tex6_64.png");
	//_ufont_surface_t *texture = fontGenerateTexture(130, 130, image130x130);
	_ufont_surface_t *texture = fontGenerateTexture(128, 128, abstract128x128);
	fontTextureSetClamp(texture, SURFACE_TEXCLAMP_H|SURFACE_TEXCLAMP_V);

	if (!fontOpen(&font, DIGITFONT)){
		printf("fontOpen() failed for '%s'\n", DIGITFONT);
		return 0;
	}

	fontSetDisplayBuffer(&font, frame->pixels, DWIDTH, DHEIGHT);
	fontSetRenderSurface(&font, surface);	// composition target
	fontCleanSurface(&font, NULL);
	fontSetRenderColour(surface, COLOUR_24TO16(0x777777));

	int x = 0;
	int y = DHEIGHT/2;

	
	fontDrawRectangleFilled(&font, 200, 50, 500, 200, 1);
	fontDrawCircleFilled(&font, 200, 150, 36, 0);
	
	fontSetRenderSurface(&font, surface);
	fontSetRenderColour(surface, COLOUR_24TO16(0x777777));
	fontSetGlyphPadding(&font, fontGetGlyphPadding(&font)-6);
	fontPrint(&font, &x, &y, "0123456789");

	
	x = 5; y = 5;
	
	if (fontOpen(&font2, QUICKFONT)){
		fontSetRenderFlags(&font2, BFONT_RENDER_NEWLINE | BFONT_RENDER_RETURN);
		fontSetRenderSurface(&font2, surface);
		fontSetGlyphPadding(&font2, fontGetGlyphPadding(&font2)+1);
		fontPrint(&font2, &x, &y, "The\r\nQuick\r\nbrown fox\r\njumps\r\nover the lazy\r\ndog.");
		fontClose(&font2);
	}

	if (fontOpen(&font2, UNIFONT)){
		fontSetRenderFlags(&font2, BFONT_RENDER_ADVANCE_X);
		fontSetRenderSurface(&font2, surface);
		fontSetGlyphPadding(&font2, fontGetGlyphPadding(&font2)+2);
		
		x = 5; y = DHEIGHT-80;
		const wchar_t *wideHex = L"\u4E00\u266a\u266b\u266C\u266d\u266f \u20AC\u2126\u2122\u307E\uB7C7\u9f98\u7360";
		const char *utf8Hex = " \xe9\xbe\x98";
		const uint16_t alist[] = {32, 9664, 9654, 12540, 51217, 54784, 50556, 54620, 45796, 45800, 48177, 51656, 51060, 46976, 0};
		const char *refs8 = "&#19968;&#36215;&#25169;&#26388;&#36855;&#31163;&#30340;&#32843;&#26657;&#23398;&#29983;&#36830;&#32493;&#22833;&#36394;&#20107;&#20214; ";
		const char *utf8Oct = " \346\227\245\346\234\254\350\252\236";
		
		fontPrintW(&font2, &x, &y, wideHex);
		fontPrint8(&font2, &x, &y, utf8Hex);
		fontPrintList(&font2, &x, &y, alist);
		x = 5; y = DHEIGHT-40;
		fontPrint(&font2, &x, &y, refs8);
		fontPrint8(&font2, &x, &y, utf8Oct);
		
		fontClose(&font2);
	}

	fontApplySurfaceOutline(&font, 0, 0);
		
	for (int e = 0; e < 12; e++){
		const double t0 = getTime();

		for (int i = 0; i < 100; i++){
			fontApplySurfaceTextureEx(&font, texture, 0, 0, e, getTime()-t0);
			lRefresh(frame);
			//lSleep(15);
		}

		double t1 = getTime();
		printf("time: %i\n", (int32_t)(t1-t0));
	}
	
	fontClose(&font);
	fontSurfaceFree(surface);
	fontTextureFree(texture);
	
	Sleep(2000);

	demoCleanup();
	return 1;
}


