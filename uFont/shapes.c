
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



void drawShapes (_ufont_t *font)
{
#if ENABLE_PRIMITIVES

	_ufont_surface_t *front = fontGetDisplayBuffer(font);
	surfaceDrawRectangleFilled(front, front->width*0.4f, 0, front->width-1, front->height/2, COLOUR_CREAM);

	fontInvertRectangle(font, 1, 2, 400, 150);
	
	fontDrawRectangleFilled(font, 50, 50, 200, 200, 0);
	fontDrawRectangle(font, 50, 250, 300, 400, 1);
	fontDrawRectangle(font, 51, 251, 301, 401, 1);
	fontDrawRectangle(font, 52, 252, 302, 402, 1);

	fontDrawLine(font, 52, 25, 400, 400, 1);
	fontDrawCircleFilled(font, 290, 270, 66, 1);
	fontDrawCircleFilled(font, 290, 270, 55, 0);
	fontDrawCircle(font, 300, 200, 66, 1);
	fontDrawCircle(font, 320, 240, 66, 0);			
						
	fontDrawArc(font, 350, 250, -166.6f, 83.0f, 130.3f, 33.0f, 1);
	fontDrawEllipse(font, 500, 150, 156.6f, 33.0f, 1);
	fontInvertRectangle(font, 175, 100, 300, 250);
#endif
}


int main (int argc, char **argv)
{

	if (!initDemoConfig("config.cfg"))
		return 0;

	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_G4TO16(4), NULL);

	_ufont_t font;
	if (!fontOpen(&font, NULL))
		return 0;

	fontSetDisplayBuffer(&font, frame->pixels, DWIDTH, DHEIGHT);
	fontSetRenderSurface(&font, surface);
	fontSetRenderColour(surface, COLOUR_24TO16(0xFFBF33));
	
	drawShapes(&font);
	
	fontApplySurface(&font, 0, 0);
	fontSetRenderColour(surface, COLOUR_24TO16(0x779777));
	fontApplySurfaceOutline(&font, 0, 0);
	fontSurfaceFree(surface);
	fontClose(&font);

	lRefresh(frame);
	Sleep(50000);

	demoCleanup();

	return 1;
}


