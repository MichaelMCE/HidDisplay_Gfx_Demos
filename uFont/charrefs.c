
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
#define fontFile	UFDIR"10x20.uf"
//#define fontFile	UFDIR"unifont32.uf"
//#define fontFile	UFDIR"freesans32.uf"




static const char *entities[] = {
	"&lt; ", "&gt; ", "&le; ", "&ge; ", "&ne; ", "&or; ", "&ni; ", "&Mu; ", 
	"&Nu; ", "&Xi; ", "&Pi; ", "&mu; ", "&nu; ", "&xi; ", "&pi; ", "&amp; ", 
	"&yen; ", "&rho; ", "&tau; ", "&phi; ", "&chi; ", "&psi; ", "&eta; ", 
	"&piv; ", "&Rho; ", "&uml; ", "&not; ", "&shy; ", "&reg; ", "&deg; ", 
	"&Eta; ", "&Tau; ", "&Phi; ", "&Chi; ", "&Psi; ", "&and; ",
	/*"&zwj; ", 
	"&lrm; ", 		space glyphs
	"&rlm; ", */
	"&ang; ", "&cap; ", "&cup; ", "&int; ", "&sim; ", "&sub; ", "&sup; ", 
	"&sum; ", "&loz; ", "&ETH; ", "&eth; ", "&quot; ", "&euro; ", "&nbsp; ", 
	"&bull; ", "&apos; ", "&circ; ", 
	/*"&ensp; ", 
	"&emsp; ", 		 space glyphs
	"&zwnj; ",*/
	"&cent; ", "&real; ", "&larr; ", "&uarr; ", "&rarr; ", "&darr; ", 
	"&harr; ", "&lArr; ", "&uArr; ", "&rArr; ", "&dArr; ", "&hArr; ", 
	"&isin; ", "&part; ", "&prod; ", "&prop; ", "&cong; ", "&nsub; ", 
	"&sube; ", "&supe; ", "&perp; ", "&sdot; ", "&lang; ", "&rang; ", 
	"&sect; ", "&copy; ", "&ordf; ", "&macr; ", "&para; ", "&ordm; ",
	"&sup1; ", "&sup2; ", "&sup3; ", "&fnof; ", "&Beta; ", "&Zeta; ", 
	"&Iota; ", "&beta; ", "&zeta; ", "&iota; ", "&Yuml; ", "&Auml; ", 
	"&Ouml; ", "&Euml; ", "&Iuml; ", "&Uuml; ", "&auml; ", "&euml; ", 
	"&iuml; ", "&ouml; ", "&uuml; ", "&yuml; ", "&ucirc; ", "&thorn; ", 
	"&sigma; ", "&theta; ", "&kappa; ", "&omega; ", "&upsih; ", "&OElig; ", 
	"&oelig; ", "&Acirc; ", "&Aring; ", "&AElig; ", "&Alpha; ", "&Gamma; ", 
	"&Delta; ", "&Theta; ", "&Kappa; ", "&Sigma; ", "&Omega; ", "&alpha; ", 
	"&gamma; ", "&delta; ", "&iexcl; ", "&diams; ", "&clubs; ", "&lceil; ", 
	"&rceil; ", "&oplus; ", "&minus; ", "&radic; ", "&infin; ", "&asymp; ", 
	"&equiv; ", "&exist; ", "&empty; ", "&nabla; ", "&notin; ", "&pound; ", 
	"&tilde; ", "&ndash; ", "&mdash; ", "&lsquo; ", "&rsquo; ", "&sbquo; ", 
	"&ldquo; ", "&rdquo; ", "&bdquo; ", "&prime; ", "&Prime; ", "&oline; ", 
	"&frasl; ", "&image; ", "&trade; ", "&crarr; ", "&laquo; ", "&times; ", 
	"&acute; ", "&micro; ", "&cedil; ", "&raquo; ", "&Ucirc; ", "&Ecirc; ", 
	"&Icirc; ", "&Ocirc; ", "&THORN; ", "&szlig; ", "&acirc; ", "&aring; ", 
	"&aelig; ", "&ecirc; ", "&icirc; ", "&ocirc; ", "&ccedil; ", "&egrave; ", 
	"&eacute; ", "&igrave; ", "&iacute; ", "&ntilde; ", "&ograve; ", "&oacute; ", 
	"&otilde; ", "&oslash; ", "&ugrave; ", "&uacute; ", "&Igrave; ", "&Iacute; ", 
	"&Ntilde; ", "&Ograve; ", "&Oacute; ", "&Otilde; ", "&Oslash; ", "&Ugrave; ", 
	"&Uacute; ", "&Yacute; ", "&agrave; ", "&aacute; ", "&atilde; ", "&plusmn; ", 
	"&divide; ", "&middot; ", "&frac14; ", "&frac12; ", "&frac34; ", "&iquest; ", 
	"&spades; ", "&hearts; ", "&brvbar; ", "&curren; ", "&thinsp; ", "&dagger; ", 
	"&Dagger; ", "&permil; ", "&lsaquo; ", "&rsaquo; ", "&hellip; ", "&weierp; ", 
	"&forall; ", "&there4; ", "&lowast; ", "&otimes; ", "&vellip; ", "&lfloor; ", 
	"&rfloor; ", "&Lambda; ", "&sigmaf; ", "&lambda; ", "&Scaron; ", "&scaron; ", 
	"&Agrave; ", "&Aacute; ", "&Atilde; ", "&Ccedil; ", "&Egrave; ", "&Eacute; ", 
	"&yacute; ", "&Epsilon; ", "&Omicron; ", "&Upsilon; ", "&epsilon; ", 
	"&omicron; ", "&upsilon; ", "&alefsym; ", "&thetasym; ", ""
};


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

	uint8_t *displayOutputBuffer = frame->pixels;	// 16bpp
	fontSetDisplayBuffer(&font, displayOutputBuffer, DWIDTH, DHEIGHT);
	fontSetRenderSurface(&font, surface);	// composition target
	//fontSetGlyphPadding(&font, fontGetGlyphPadding(&font)+0);
	fontSetLineSpace(&font, fontGetLineSpace(&font)-1);
	fontSetRenderFlags(&font, BFONT_RENDER_NEWLINE|BFONT_RENDER_NODEFAULT|BFONT_RENDER_ADVANCE_Y/* | BFONT_RENDER_GLYPHRECT*/);

	char buffer[256] = {0};
	int total = sizeof(entities) / sizeof(*entities);
	
#if 0
	int y = 5;
	int x = 0;
	fontPrint(&font, &x, &y, "Hello\nWorld!");
#else
	int x = 0;
	int y = 1;
	
	for (int i = 0; i < total; ){
		buffer[0] = 0;
		for (int c = 0; c < 23 && i < total; c++)
			strcat(buffer, entities[i++]);
		fontPrint(&font, &x, &y, (uint8_t*)buffer);
		//fontDrawLine(&font, 0, y, DWIDTH-1, y, 1);

	}
#endif

	fontApplySurface(&font, 0, 0);
	
	fontSurfaceFree(surface);
	fontClose(&font);

	lRefresh(frame);
	Sleep(200000);

	demoCleanup();
	return 1;
}


