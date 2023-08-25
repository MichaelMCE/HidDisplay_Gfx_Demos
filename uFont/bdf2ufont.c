
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


#define NO_FIO	1
 
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include <mylcd.h>
#include "../src/ufont.h"


static THWD *hw;
static TFRAME *surface;

// load all glyphs to memory, returns total glyphs built (available)
#define maxGlyphs		65536
#define buildStepSize	1000




static inline void fillGlyphHeader (_glyph_t *glyph, TWCHAR *wc)
{
	glyph->w = wc->w;
	glyph->h = wc->h;
	glyph->xOffset = wc->xoffset;
	glyph->yOffset = wc->yoffset;
	glyph->dwidth = wc->dwidth;
	glyph->length = CALC_PITCH_1(wc->w) * wc->h;

}

static inline void setPixel_NB (const TFRAME *frm, const int x, const int y)
{
	*(frm->pixels+((y*frm->pitch)+(x>>3))) |= (1<<(x&7));
}

static inline TFRAME *glyphToFrame (TWCHAR *g)
{
	int ok = lResizeFrame(surface, g->w, g->h, 0);
	if (!ok) return NULL;	// unlikely
	
	memset(surface->pixels, 0, surface->frameSize);
	
	TGLYPHPOINTS *gp = g->gp;
	for (int i = 0; i < gp->pointsTotal; i++)
		setPixel_NB(surface, gp->points[i].x, gp->points[i].y);
	return surface;
}

static inline int glyphGetLeftMostPixel (TWCHAR *chr)
{
	if (chr->box.left != -1)
		return chr->box.left;

	if (!chr->gp->pointsTotal){
		chr->box.left = -1;
		return chr->box.left;
	}
	
	chr->box.left = 9999;
	for (int i = 0; i < chr->gp->pointsTotal; i++){
		if (chr->box.left > chr->gp->points[i].x)
			chr->box.left = chr->gp->points[i].x;
	}

	return chr->box.left;
}

static inline int glyphGetRightMostPixel (TWCHAR *chr)
{
	if (chr->box.right != -1)
		return chr->box.right;

	
	chr->box.right = -1;
	for (int i = 0; i < chr->gp->pointsTotal; i++){
		if (chr->box.right < chr->gp->points[i].x)
			chr->box.right = chr->gp->points[i].x;
	}

	return chr->box.right;
}


static inline int writeGlyph (FILE *fp, TWCHAR *wc)
{
	// ignore empty glyphs
	//if (wc->encoding != L' ' && ((wc->w * wc->h) == 0))
	//	return 0;

	TFRAME *glyphFrame = glyphToFrame(wc);
	if (!glyphFrame) return 0;

	_glyph_t glyph;
	fillGlyphHeader(&glyph, wc);

#if 0
	if (glyph.dwidth > glyph.w)
		printf("warning: glyph %i: dwidth is greater than width - %i > %i\n", wc->encoding, glyph.dwidth, glyph.w);
#endif
	if (glyph.length < 1){
		glyph.length = 1;
		glyphFrame->pixels[0] = 0;
		
	}

	if (!glyph.length){
		printf("invalid glyph @ %i\n", wc->encoding);
		return 0;
	}

	int pixelBufferSize = glyph.length;
		
#if 0
	//printf("encoding: %i\n", glyph.encoding);
	printf("WxH: %i %i, %i %i\n", glyph.w, glyph.h, glyph.xoffset, glyph.yoffset);
	printf("WxH: %i %i, %i %i\n", wc->w, wc->h, wc->xoffset, wc->yoffset);
	printf("Length: %i %i %i %i\n", glyph.length, sizeof(glyph), glyphFrame->frameSize, pixelBufferSize);
#endif

	fwrite(&glyph, 1, sizeof(glyph), fp);
	fwrite(glyphFrame->pixels, 1, pixelBufferSize, fp);

	return 1;
}

static inline void fillFontHeader (_ufont_header_t *header, TWFONT *font)
{
	header->ident = BFONT_IDENT;					// version ident
	header->verticalPitch = font->PixelSize;		// BDF 'PIXEL_SIZE' field describing maximum rows, ie height
	header->fontAscent = font->fontAscent;			// BDF 'FONT_ASCENT' field
	header->fontDescent = font->fontDescent;		// BDF 'FONT_DESCENT' field
	header->flags = BFONT_FLAGS_HASLOOKUP;

	if (font->spacing == 'P')
		header->flags |= BFONT_FLAGS_PROPSPACE;
	else /*if (font->spacing == 'M')*/
		header->flags |= BFONT_FLAGS_MONOSPACE;

	header->fbb.width = font->QuadWidth;			// FBB, quad width = max width of all char's
	header->fbb.height = font->QuadHeight;			// FBB, quad height = max height of all char's
	header->fbb.xOffset = font->QuadXOffset;	
	header->fbb.yOffset = font->QuadYOffset;
}

int writeHeader (FILE *fp, _ufont_header_t *header, TWFONT *font, const uint16_t totalGlyphs)
{
	fillFontHeader(header, font);
	header->defaultGlyph = 0x0001;					// replacement glyph when requested is not found/unavailable
	header->totalGlyphs = totalGlyphs;
	
	rewind(fp);
	return fwrite(header, 1, sizeof(*header), fp);
}

size_t writeTable (FILE *fp, _lookup_t *table, const uint16_t tglyphs)
{
	size_t tableLength = tglyphs * sizeof(_lookup_t);
	fwrite(table, 1, tableLength, fp);
	return tableLength;
}

static inline void registerFont (const wchar_t *file, int fontid, int charSpace)
{ 
	TWFONT regwfont;
	regwfont.FontID = fontid;
	regwfont.CharSpace = charSpace;
	regwfont.LineSpace = 2;
	lRegisterFontW(hw, file, &regwfont);
}

void createFilename (wchar_t *in, wchar_t *out)
{
	int slen = wcslen(in);
	int o = 0;
	out[0] = 0;

	// remove otf, tff, etc.. from filename
	for (int i = 0; i < slen; i++){
		if (in[i] == L' ') continue;
		
		if (in[i] == L'.'){
			if (towlower(in[i+1]) == L't' && towlower(in[i+2]) == L't' && towlower(in[i+3]) == L'f'){
				i+= 3;
				continue;
			}
			if (towlower(in[i+1]) == L'o' && towlower(in[i+2]) == L't' && towlower(in[i+3]) == L'f'){
				i+= 3;
				continue;
			}
			if (towlower(in[i+1]) == L'b' && towlower(in[i+2]) == L'd' && towlower(in[i+3]) == L'f'){
				i+= 3;
				continue;
			}
			if (towlower(in[i+1]) == L't' && towlower(in[i+2]) == L't' && towlower(in[i+3]) == L'c'){
				i+= 3;
				continue;
			}
		}
		out[o++] = towlower(in[i]);
	}

	wcscpy(&out[o], BFONT_EXT);
}

static inline TWCHAR *getGlyph (const uint32_t enc, const int fontid)
{
	TWCHAR *wc = lGetGlyph(hw, NULL, enc, fontid);
	if (wc){
		if (wc->w > 255 || wc->h > 255){
			printf("glyph not written. %i/%.4X is oversized: %ix%i\n", enc, enc, wc->w, wc->h);
			return NULL;
		}
	}
	return wc;
}

static inline TWFONT *getFont (const int fontid)
{
	return lFontIDToFont(hw, fontid);;
}

static inline void unloadBDFCache (const int fontid)
{
	// free memory
	lFlushFont(hw, fontid);
}

static inline int buildGlyphs (const int fontid, const int from, const int amount)
{
	unloadBDFCache(fontid);
	return lCacheCharacterRange(hw, from, from+amount, fontid);		// build glyphs
}

static inline int getFontMetrics (int fontid, int *width, int *height, int *ascent, int *descent, int *tchars)
{
	return lGetFontMetrics(hw, fontid, width, height, ascent, descent, tchars);
}

int createFont (wchar_t *filename, const int fontid, int8_t *encList)
{
	_ufont_header_t header;
	memset(&header, 0, sizeof(header));

	TWFONT *font = getFont(fontid);
	if (!font) return 0;
	

	FILE *fp = _wfopen(filename, L"wb");
	if (!fp) return 0;
	
	//writeHeader(fp, &header, font);
	fseek(fp, sizeof(_ufont_header_t), 0);


	int width, height, ascent, descent, tchars;
	getFontMetrics(fontid, &width, &height, &ascent, &descent, &tchars);
#if 0
	printf("bbox: width %i\n", width);
	printf("bbox: height %i\n", height);
	printf("font: ascent %i\n", ascent);
	printf("font: descent %i\n", descent);
#endif
	printf("glyphs in font: %i\n", tchars);
	printf("default glyph: %i\n\n", font->DefaultChar);	
	_lookup_t table[maxGlyphs];


	//if (!font->DefaultChar) font->DefaultChar = 1;

	int ct = 0;
	// special case for default char. ufont expects default glyph @ encoding 1 and beginning of first block
	// -1 is an invalid codepoint, default glyph will be returned
	TWCHAR *wc = getGlyph(font->DefaultChar, fontid);
	if (!wc)
		wc = getGlyph((int)-1, fontid);
	if (!wc)
		wc = getGlyph(' ', fontid);

	if (wc){
		table[ct].encoding = 1;
		table[ct].offset = ftell(fp);
		ct += writeGlyph(fp, wc);
		
		if (!ct)
			printf("default not written: %i\n", font->DefaultChar);
	}else{
		printf("default not set\n");
	}
	
	for (int32_t c = 0; c < maxGlyphs; c+=buildStepSize){
		for (int32_t j = c; j < c+buildStepSize; j++){
			if (!encList[j]) continue;

			// load/build glyph from the .bdf
			TWCHAR *wc = getGlyph(j, fontid);
			if (wc){
				if (wc->encoding == font->DefaultChar && font->DefaultChar != ' ')
					continue;
	
				table[ct].encoding = wc->encoding;
				table[ct].offset = ftell(fp);

				int ret = writeGlyph(fp, wc);
				if (!ret)
					printf(" skipped: %i\n", table[ct].encoding);
				ct += ret;
			}
		}
		// free memory
		// without this processed glyphs will remain resident (accumulating memory)
		unloadBDFCache(fontid);
	}

	header.lookup.glyphs = ct;
	header.lookup.offset = ftell(fp);

	int tableLength = writeTable(fp, table, header.lookup.glyphs);
	int fileLength = (int)ftell(fp);
	writeHeader(fp, &header, font, header.lookup.glyphs);
	fclose(fp);

	printf("glyphs written: %i\n", ct);
	printf("table size: %i\n", tableLength);
	//printf("table position: %i\n", header.lookup.offset);
	printf("file size: %i\n", fileLength);

	return ct;
}

	
static inline int buildEncTable (char *opt, int8_t *list)
{
	int ct = 0;
	char *fields[256] = {0};
	fields[ct++] = opt;
	
	for (int i = 0; opt[i+1]; i++){
		if (opt[i] == ','){
			opt[i] = 0;
			fields[ct] = &opt[i+1];
			ct++;
		}
	}
	
	for (int i = 0; i < ct; i++){
		if (fields[i][0] != '+' && fields[i][0] != '-')
			continue;
		if (!isdigit(fields[i][1]))
			continue;
		
		if (strchr(fields[i], '_')){
			int from = 0, to = 0;
			char c;
			if (sscanf(fields[i], "%c%d_%d", &c, &from, &to) != 3)
				return 0;
			from &= 0xFFFF; to &= 0xFFFF;

			if (c == '+'){
				for (int j = from; j <= to; j++)
					list[j] = 0xFF;

			}else if (c == '-'){
				for (int j = from; j <= to; j++)
					list[j] = 0;
			}
		}else if (fields[i][0] == '+'){
			int a = 0;
			char c;
			if (sscanf(fields[i], "%c%d", &c, &a) != 2)
				return 0;
			a &= 0xFFFF;
			
			list[a] = 0xFF;			
		}else if (fields[i][0] == '-'){
			int a = 0;
			char c;
			if (sscanf(fields[i], "%c%d", &c, &a) != 2)
				return 0;
			a &= 0xFFFF;
			
			list[a] = 0;
		}
	}

	ct = 0;
	for (int i = 0; i < maxGlyphs; i++)
		ct += (list[i]&0x01);

	return ct;
}


int main (int argc, char *argv[])
{
	wchar_t **argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (!argvw || argc < 2) return 0;
	
	hw = lOpen(NULL, NULL);
	if (!hw) return 0;

	surface = lNewFrame(hw, 16, 16, LFRM_BPP_1);
		
	int fileIdx = 1;
	int8_t encList[maxGlyphs] = {0};		

	int ct = 0;
	if (argc > 3){
		if (!strcasecmp(argv[1], "-e")){
			ct = buildEncTable(argv[2], encList);
			//printf("Encoding space: %i\n", ct);
			fileIdx = 3;
		}
	}	

	if (!ct)
		memset(encList, 0xFF, sizeof(encList));
	
	wchar_t outfile[1024];
	
	for (int i = fileIdx; i < argc; i++){
		wchar_t *file = argvw[i];
		const int id = 5000+i;

		registerFont(file, id, 2);
		createFilename(file, outfile);
		wprintf(L"  %s -> %s\n", file, outfile);
		createFont(outfile, id, encList);
		printf("\n");
	}


	lDeleteFrame(surface);
	lClose(hw);
	return 1;
}
