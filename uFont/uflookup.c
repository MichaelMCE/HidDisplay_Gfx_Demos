
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


typedef struct {
	_glyph_t glyph;
	uint8_t *pixels;
	uint16_t encoding;
}_codepoint_t;



static _ufont_t font;
static char *fontFile;




static inline uint16_t strToInt16 (const char *str)
{
	uint16_t enc;

	if (tolower(str[0]) == 'x' || tolower(str[1]) == 'x'){
		int32_t enc32 = enc;
		sscanf(str, "x%X", &enc32);
		enc = enc32&0xFFFF;			
	}else{
		enc = atoi(str)&0xFFFF;
	}
	return enc;
}

static inline _ufont_header_t *getHeader ()
{
	return &font.header;
}

static inline uint16_t getGlyphTotal ()
{
	return getHeader()->lookup.glyphs;
}

static inline size_t getTablePosition ()
{
	return getHeader()->lookup.offset;
}

static inline int reopenFontWithWrite ()
{
	if (font.fio) fio_close(font.fio);
	font.fio = fio_open((uint8_t*)fontFile, FIO_READ|FIO_WRITE);
	return (font.fio != NULL);
}

static inline int readHeader (_ufont_t *font, _ufont_header_t *header)
{
	fio_rewind(font->fio);
	return fio_read(font->fio, header, sizeof(*header));
}

static inline int writeHeader (_ufont_t *font, _ufont_header_t *header)
{
	fio_rewind(font->fio);
	
	if (&font->header != header)
		memcpy(&font->header, header, sizeof(*header));
		
	return fio_write(font->fio, header, sizeof(*header));
}

static inline uint16_t readTable (_lookup_t *table)
{
	size_t len = sizeof(_lookup_t) * getGlyphTotal();
	fio_seek(font.fio, getTablePosition());
	
	if (fio_read(font.fio, table, len) == len)
		return getGlyphTotal();
	else
		return 0;
}

static inline size_t writeTable (_lookup_t *table, const uint16_t tglyphs)
{
	// given new table is smaller, overwriting existing table should be fine
	fio_seek(font.fio, getTablePosition());
	size_t tableLength = tglyphs * sizeof(_lookup_t);
	return (fio_write(font.fio, table, tableLength) == tableLength);
}

static inline int getGlyph (const uint16_t codepoint, _glyph_t *glyph, uint32_t *glyphOffset)
{
	uint32_t gOffset = fontGetGlyphHeader(&font, codepoint, glyph);
	if (glyphOffset) *glyphOffset = gOffset;
	return (gOffset != 0);
}

static inline uint8_t *getGlyphPixels (const uint16_t codepoint)
{
	return fontGetGlyphPixels(&font, codepoint);
}

static inline void dumpHeader (_ufont_header_t *header)
{
	printf("header size: %i\n", sizeof(*header));
	printf("version: 0x%.4X\n", header->ident);
	printf("table glyphs: %i\n", header->lookup.glyphs);
	printf("table offset: %i\n", header->lookup.offset);
	printf("table size: %i\n", header->lookup.glyphs*sizeof(_lookup_t));
	printf("flags: 0x%X\n", header->flags);
	printf("verticalPitch: %i\n", header->verticalPitch);
	printf("fontAscent: %i\n", header->fontAscent);
	printf("fontDescent: %i\n", header->fontDescent);
	printf("defaultGlyph: %i\n", header->defaultGlyph);
	printf("totalGlyphs: %i\n", header->totalGlyphs);
	printf("fbb width: %i\n", header->fbb.width);
	printf("fbb height: %i\n", header->fbb.height);
	printf("fbb xOffset: %i\n", header->fbb.xOffset);
	printf("fbb yOffset: %i\n", header->fbb.yOffset);	
	
	if (header->flags&BFONT_FLAGS_HASLOOKUP)
		printf("font HASLOOKUP table\n");
	if (header->flags&BFONT_FLAGS_MONOSPACE)
		printf("font is MONOSPACE\n");
	if (header->flags&BFONT_FLAGS_PROPSPACE)
		printf("font is PROPSPACE\n");
	printf("\n");
}

static inline void dumpAvailable (_lookup_t *table, uint16_t tglyphs, const int hex)
{
	printf("\n");
	
	if (hex){
		for (int i = 0; i < tglyphs; i+=16){
			for (int c = i; c < i+16 && c < tglyphs; c++){
				if (table[c].encoding&0xFF00){
					printf("\n");
					for (int i = c; i < tglyphs; i+=16){
						for (int c = i; c < i+16 && c < tglyphs; c++)
							printf("%.4X ", table[c].encoding);
						printf("\n");
					}
					return;
				}
				printf("%.2X ", table[c].encoding);
			}
			printf("\n");
		}
	}else{
		for (int i = 0; i < tglyphs; i+=16){
			for (int c = i; c < i+16 && c < tglyphs; c++){
				//printf("&#%u; ", table[c].encoding);
				printf("%i ", table[c].encoding);
			}
			printf("\n");
		}
	}

	printf("\n");
}

static inline void dumpHtml (_lookup_t *table, uint16_t tglyphs, const int large)
{
	printf("<HTML>\n<font size = ");
	if (!large)
		printf("5>\n");
	else
		printf("8>\n");
		
	for (int i = 0; i < tglyphs; i+=16){
		printf("%.4X:  ", table[i].encoding);

		for (int c = i; c < i+16 && c < tglyphs; c++)
			printf("&#%u; ", table[c].encoding);
		printf("<BR>\n");
	}

	printf("</HTML>\n");
}

static inline void dumpLookup (_lookup_t *table, uint16_t tglyphs)
{
	printf("index: encoding, offset\n");
	for (int i = 0; i < tglyphs; i++)
		printf("%i: %i %u\n", i, table[i].encoding, table[i].offset);
}

static inline void dumpPixels (uint8_t *pixels, uint16_t length)
{
	for (int i = 0; i < length; i++)
		printf("0x%.2X ", pixels[i]);
	printf("\n");
}

static inline void dumpPixelsComma (uint8_t *pixels, uint16_t length, const int hex)
{
	if (hex){
		printf("%.2X", pixels[0]);
		for (int i = 1; i < length; i++)
			printf(",%.2X", pixels[i]);
	}else{
		printf("%d", pixels[0]);
		for (int i = 1; i < length; i++)
			printf(",%d", pixels[i]);
	}
	printf("\n");
}

static inline void cmdDumpEncodings (const int var1, const int var2)
{
	_lookup_t table[getGlyphTotal()];
	memset(table, 0, sizeof(table));

	uint16_t tglyphs = readTable(table);
	dumpAvailable(table, tglyphs, var1);
}

// send/redirect binary data to console
static inline void writeToConsole (uint8_t *data, const size_t dsize)
{
	// dump raw pixel data to console
	uint16_t len = dsize;
	fwrite((char*)&len, sizeof(len), 1, stdout);
	fwrite((char*)data, len, 1, stdout);
}

static inline void cmdDumpPixels (const int var1, const int var2)
{
	uint16_t wc = var2&0xFFFF;
	_glyph_t glyph = {0};

	if (!getGlyph(wc, &glyph, NULL)){
		printf("encoding %i not found\n", wc);
		return;
	}

	uint8_t *pixels = getGlyphPixels(wc);
	if (!pixels)
		printf("pixels read failed\n");
	else
		dumpPixelsComma(pixels, glyph.length, var1);
}

static inline void cmdSetDefaultChar (const int var1, const int var2)
{
	uint16_t wc = var2&0xFFFF;
	if (!wc) return;

	int success = 0;

	if (reopenFontWithWrite()){
		_ufont_header_t *header = getHeader();
		if (readHeader(&font, header) == sizeof(*header)){
			header->defaultGlyph = wc;
			success = (writeHeader(&font, header) == sizeof(*header));
		}
	}

	if (success)
		printf("default glyph set to: %i\n", wc);
	else
		printf("read/write error. glyph not set\n");
}

static inline void cmdDumpHeader (const int var1, const int var2)
{
	_ufont_header_t *header = getHeader();
	printf("file size: %u\n", fio_length(font.fio));
	dumpHeader(header);
}

static inline void cmdDumpGlyphOffsetTable (const int var1, const int var2)
{
	_lookup_t table[getGlyphTotal()];
	memset(table, 0, sizeof(table));

	uint16_t tglyphs = readTable(table);
	dumpLookup(table, tglyphs);
}

static inline void cmdOffsetTable2HTML (const int var1, const int var2)
{
	_lookup_t table[getGlyphTotal()];
	memset(table, 0, sizeof(table));

	uint16_t tglyphs = readTable(table);
	dumpHtml(table, tglyphs, var1);
}


static inline int readGlyphHeader (_glyph_t *glyph, const uint32_t offset)
{
	fio_seek(font.fio, offset);
	return (fio_read(font.fio, glyph, sizeof(_glyph_t)) == sizeof(_glyph_t));
}

static inline uint8_t *readGlyphPixels (_glyph_t *glyph, const uint32_t offset, void *pixels)
{

	fio_seek(font.fio, offset+sizeof(_glyph_t));
	if (!fio_read(font.fio, pixels, glyph->length))
		return NULL;

	return (uint8_t*)pixels;
}

static inline int removeCodepoint (_lookup_t *table, const int gtotal, uint16_t enc)
{
	int c = 0;
	for (int i = 0; i < gtotal; i++){
		if (table[i].encoding == enc) continue;
		table[c].encoding = table[i].encoding;
		table[c++].offset = table[i].offset;
	}
	
	if (gtotal == c)	// nothing changed - nothing removed
		return 0;
	else
		table[c].encoding = 0;

	return c;
}


static inline int removeCodepoints (_lookup_t *table, int tglyphs, const int from, const int to)
{
	int gtotal = tglyphs;
		
	if (from < to){
		printf("Removing codepoints from %i to %i ...\n", (int)from, (int)to);

		for (uint16_t enc = from; enc <= to; enc++){
			int total = removeCodepoint(table, gtotal, enc);
			if (total) gtotal = total;
		}

		if (gtotal == tglyphs){
			printf("Codepoints not removed/not found\n");
			return 0;
		}
	}else if (!to || from == to){
		printf("Removing codepoint %i ...\n", (int)from);
		
		int total = removeCodepoint(table, gtotal, from);
		if (total) gtotal = total;
		if (!total){
			printf("Codepoint %i not found\n", (int)from);
			return 0;
		}
	}
	
	int success = 0;

	if (reopenFontWithWrite()){
		_ufont_header_t *header = getHeader();
		header->lookup.glyphs = gtotal;
		//header->totalGlyphs = gtotal;	
		success = (writeHeader(&font, header) == sizeof(_ufont_header_t));
		if (success)
			success = writeTable(table, gtotal);
		else
			printf("Write failed\n");
	}else{
		printf("Could not open file; Write protected?\n");
	}
	
	if (success)
		printf("%i codepoint(s) removed from table\n", (int)(tglyphs - gtotal));
	else
		printf("Codepoint(s) not removed\n");
		
	return success;
}

static inline int parseFindStr (char *str, int *width, int *height, uint8_t *pixels)
{
	int tbytes = 0;
	
	if (width && height){
		if (sscanf(str, "%i:%i:", width, height) != 2)
			return 0;
	}

	char *c = strtok(str, ":");
	if (!c) return 0;
	c = strtok(NULL, ":");
	if (!c) return 0;

	while((c = strtok(NULL, ","))){
		int val = 0;
		if (sscanf(c, "%x", &val) != 1)
			return 0;
		if (val < 0 || val > 0xFF)
			return 0;
		if (pixels)
			pixels[tbytes] = val;
		tbytes++;
	};
	
	return tbytes;
}


static inline void cmdFindGlyphs (const int remove, const char *var2)
{
	char *findStr = strdup(var2);
	if (!findStr) return;
	
	int findWidth = 0, findHeight = 0;
	int findLen = parseFindStr(findStr, &findWidth, &findHeight, NULL);
	free(findStr);
	if (!findLen) return;

	uint8_t findPixels[findLen];
	findStr = strdup(var2);
	findLen = parseFindStr(findStr, NULL, NULL, findPixels);

	uint8_t readPixels[findLen];
	_glyph_t glyph = {0};

	const uint16_t gtotal = getGlyphTotal();
	_lookup_t table[gtotal];
	memset(table, 0, sizeof(table));
	uint16_t tglyphs = readTable(table);


	int ct = 0;
	for (int i = 0; i < tglyphs; i++){
		if (!readGlyphHeader(&glyph, table[i].offset))
			continue;
			
		if (glyph.w == findWidth && glyph.h == findHeight && glyph.length == findLen){
			if (readGlyphPixels(&glyph, table[i].offset, readPixels)){
				if (!memcmp(findPixels, readPixels, findLen)){
					ct++;
					if (!remove)
						printf("%i: %.4X - %i\n", ct, table[i].encoding, table[i].encoding);
					else
						table[i].encoding = 0;
				}
			}
		}
	}
	
	printf("Found %i match(s)\n", ct);

	if (ct && remove)
		removeCodepoints(table, gtotal, 0, 0);

	free(findStr);
}

static inline int findDuplcates (_codepoint_t *cp)
{

	const uint16_t gtotal = getGlyphTotal();
	_lookup_t table[gtotal];

	const uint16_t tglyphs = readTable(table);
	if (tglyphs != gtotal) return 0;
	
	int ct = 0;
	_glyph_t glyph;
	uint8_t readPixels[cp->glyph.length];
	
	for (int i = 0; i < tglyphs; i++){
		if (!readGlyphHeader(&glyph, table[i].offset))
			continue;

		if (glyph.w == cp->glyph.w && glyph.h == cp->glyph.h && glyph.length == cp->glyph.length){
			if (readGlyphPixels(&glyph, table[i].offset, readPixels)){
				if (!memcmp(cp->pixels, readPixels, cp->glyph.length)){
					ct++;
					printf("%i: %.4X - %i\n", ct, table[i].encoding, table[i].encoding);
				}
			}
		}
	}

	if (ct)
		return --ct;
	else
		return ct;
}

static inline void cmdFindDup (const int var1, const int var2)
{
	_codepoint_t cp = {0};
	cp.encoding = var2&0xFFFF;

	if (!getGlyph(cp.encoding, &cp.glyph, NULL)){
		printf("encoding 0x%.4X not found\n", cp.encoding);
		return;
	}

	cp.pixels = getGlyphPixels(cp.encoding);
	if (!cp.pixels){
		printf("Error reading pixel data for 0x%.4X\n", cp.encoding);
		return;
	}

	int ct = findDuplcates(&cp);
	if (ct)
		printf("Found %i duplicates of 0x%.4X\n", ct, cp.encoding);
	else
		printf("No duplicates found\n");
}

static inline void cmdDumpGlyph (const int var1, const int var2)
{
	uint16_t wc = var2&0xFFFF;
	_glyph_t glyph = {0};
	uint32_t fileOffset;

	if (!getGlyph(wc, &glyph, &fileOffset)){
		printf("encoding %i not found\n", wc);

	}else{
		printf("\nencoding: %i - 0x%.4X\n", wc, wc);
		printf("width: %i\n", glyph.w);
		printf("height: %i\n", glyph.h);
		printf("xOffset: %i\n", glyph.xOffset);
		printf("yOffset: %i\n", glyph.yOffset);
		printf("dwidth: %i\n", glyph.dwidth);
		printf("pixels length: %i\n", glyph.length);
		printf("file offset: %u\n", fileOffset);

		uint8_t *pixels = getGlyphPixels(wc);
		if (pixels)
			printf("pixel data read: %i -\n", glyph.length);

		dumpPixels(pixels, glyph.length);
	}
}

static inline int rebuild (_lookup_t *table, uint16_t total, const char *filename)
{

	fileio_t *fio = fio_open((uint8_t*)filename, FIO_NEW);
	if (!fio){
		printf("File create failed for '%s'. Write protected?\n", filename);
		return 0;
	}

	int ct = 0;
	int error = 0;
	_glyph_t glyph = {0};

	fio_seek(fio, sizeof(_ufont_header_t));

	for (int i = 0; i < total && !error; i++){
		uint16_t enc = table[i].encoding;

		if (getGlyph(enc, &glyph, NULL)){
			uint8_t *pixels = getGlyphPixels(enc);
			if (pixels){
				table[i].offset = fio_getpos(fio);
				if (fio_write(fio, &glyph, sizeof(glyph)) != sizeof(glyph))
					error = i+1;
				if (fio_write(fio, pixels, glyph.length) != glyph.length)
					error = i+1;
				if (!error) ct++;
			}
		}
	}
	
	if (error){
		printf("Error writing glyph #%i\n", error-1);
		fio_close(fio);
		return 0;
	}

	_ufont_header_t *header = getHeader();
	header->totalGlyphs = total;
	header->lookup.glyphs = total;
	header->lookup.offset = fio_getpos(fio);

	size_t tableLength = total * sizeof(_lookup_t);
	if (fio_write(fio, table, tableLength) != tableLength)
		printf("Error writing table\n");
	fio_rewind(fio);
	if (fio_write(fio, header, sizeof(_ufont_header_t)) != sizeof(_ufont_header_t))
		printf("Error writing header\n");
	fio_close(fio);
	
	if (error)
		return 0;
	else
		return ct;
}

static inline void cmdRebuild (const int var1, const char *desFilename)
{
	printf("Rebuilding font '%s' as '%s'\n", fontFile, desFilename);
	printf("...\n");
	
	_lookup_t table[getGlyphTotal()];
	memset(table, 0, sizeof(table));

	uint16_t tglyphs = readTable(table);
	int ret = rebuild(table, tglyphs, desFilename);
	if (!ret)
		printf("Font rebuild failed\n");
	else
		printf("Font rebuilt with %i glyphs\n", ret);
}

static inline int setVPitch (const int vpitch)
{
	int success = 0;

	if (reopenFontWithWrite()){
		_ufont_header_t *header = getHeader();
		if (readHeader(&font, header) == sizeof(_ufont_header_t)){
			header->verticalPitch = vpitch&0xFF;
			success = (writeHeader(&font, header) == sizeof(*header));
		}
	}

	if (success)
		printf("verticalPitch set to: %i\n", getHeader()->verticalPitch);
	else
		printf("read/write error. verticalPitch not set\n");

	return success;
}

static inline void cmdAutoVPitch (const int var1, const int var2)
{
	int maxHeight = getHeader()->verticalPitch;

	if (var2 > 4){	// something sensible
		maxHeight = var2;
		
	}else{
		printf("Calculating verticalPitch ...\n");
		
		fontSetRenderFlags(&font, BFONT_RENDER_DEFAULT|BFONT_RENDER_NODEFAULT);
		_glyph_t glyph = {0};
		maxHeight = 0;

		for (int i = 0; i <= 0xFFFF; i++){
			if (!metricsGetGlyphHeader(&font, i, &glyph))
				continue;
			if (glyph.h > maxHeight){
				maxHeight = glyph.h;
				//printf("enc %i - %i\n",i, maxHeight);
			}
		}
		//printf(" %i\n", maxHeight);
	}
	
	if (getHeader()->verticalPitch != maxHeight){
		printf("Adjusting verticalPitch from %i to %i..\n", getHeader()->verticalPitch, maxHeight);
		setVPitch(maxHeight);
	}else{
		printf("verticalPitch is already %i\n", maxHeight);
	}
}



// from
//		97				remove character 97 only

// from+amount
//		65+1			remove character 65 only
//		65+10			remove 10 characters: 65 through to 74

// from_to
//		10000_11000 	remove all characters from 10000 through to 11000 (1001 characters)
// from-to
//		as with _
// "from to"
//		as with _
static inline int getRange (const char *str, int *from, int *to)
{
	*from = 0;
	*to = 0;

	sscanf(str, "%i+%i", from, to);
	*from = abs(*from);
	*to = abs(*to);

	if (*to == 1)
		*to = *from;
	else if (*to > 1)
		*to = (*from + *to)-1;
	
	if (!*to){
		sscanf(str, "%i-%i", from, to);
		if (!*to)
			sscanf(str, "%i_%i", from, to);
		if (!*to)
			sscanf(str, "%i %i", from, to);

		*from = abs(*from);
		*to = abs(*to);
	}

	if (*from > 0xFFFF || *to > 0xFFFF) return 0;
	if (*from < 1) return 0;

	if (*to > 0 && *to < *from){
		printf("Upper band must be higher than lower band (from:%i to:%i).\n", *from, *to);
		return 0;
	}
	return (*to - *from)+1;
}

static inline void cmdRemoveEnc (const int var1, const char *str)
{
	
	int from, to;
	if (!getRange(str, &from, &to))
		return;
	
	printf("range: %i %i\n", from, to);
	
	_lookup_t table[getGlyphTotal()];
	memset(table, 0, sizeof(table));

	uint16_t tglyphs = readTable(table);
	removeCodepoints(table, tglyphs, from, to);
}


static inline void cmdDefault (const int var1, const int var2)
{
	printf("uflookup.exe font.uf -a/c/d/h/l/m/v/r/0-65535\n");
	printf(" -a: Dump encodings (a for dec, A for hex)\n");
	printf(" -c encoding: Dump pixel data for this encoding (c for dec, C for hex)\n");
	printf(" -d encoding: Set default glyph. (0-65535)\n");
	printf(" -h: Dump font header\n");
	printf(" -l: Display glyph table\n");
	printf(" -m: Print lookup table as HTML\n");
	printf(" -p vpitch: Set verticalPitch. '-p' without args for auto calc and set. (0-255)\n");
	printf(" -r from-to: Dereference these codepoints from glyph table\n");
	printf(" -r from+n: Dereference 'n' codepoints beginning at 'from'\n");
	printf(" -b: Rebuild font according to glyph table. Use this to remove unreferenced glyphs following -r\n");
	printf(" -f w:h:pixelbyte1,pixelbyte2,pixelbyte3,etc..: find/remove glyph(s) matching this spec\n");
	printf(" -z encoding: Find duplicates\n");
	printf(" <num>: eg; 65, Display glyph metrics and pixel data for this codepoint\n");	
}

int main (int argc, char **argv)
{

	if (argc < 3){
		cmdDefault(0, 0);
		return 0;
	}

	fontFile = argv[1];
	if (!fontOpen(&font, fontFile)){
		printf("fontOpen() failed for '%s'\n", fontFile);
		return 0;
	}
	
	const int var1 = isupper(argv[2][1]);
	const char *opt = argv[2];
	
	if (!strcasecmp("-a", opt)){
		cmdDumpEncodings(var1, 0);
		
	}else if (!strcasecmp("-c", opt)){
		if (argc > 3)
			cmdDumpPixels(var1, strToInt16(argv[3]));

	}else if (!strcasecmp("-d", opt)){
		if (argc > 3)
			cmdSetDefaultChar(var1, atoi(argv[3]));
		
	}else if (!strcasecmp("-h", opt)){
		cmdDumpHeader(var1, 0);
		
	}else if (!strcasecmp("-l", opt)){
		cmdDumpGlyphOffsetTable(var1, 0);
		
	}else if (!strcasecmp("-m", opt)){
		cmdOffsetTable2HTML(var1, 0);
		
	}else if (!strcasecmp("-p", opt)){
		if (argc > 3)
			cmdAutoVPitch(var1, atoi(argv[3]));
		else if (argc == 3)
			cmdAutoVPitch(var1, 0);

	}else if (!strcasecmp("-r", opt)){
		if (argc > 3)
			cmdRemoveEnc(var1, argv[3]);

	}else if (!strcasecmp("-b", opt)){
		if (argc > 3)
			cmdRebuild(0, argv[3]);

	}else if (!strcasecmp("-f", opt)){
		if (argc > 3)
			cmdFindGlyphs(var1, argv[3]);

	}else if (!strcasecmp("-z", opt)){
		if (argc > 3)
			cmdFindDup(var1, strToInt16(argv[3]));
			
	}else{
		int wc = strToInt16(argv[2]);
		if (wc > 0)
			cmdDumpGlyph(0, wc);
		else
			cmdDefault(0, 0);
	}

	fontClose(&font);
	return 1;
}
