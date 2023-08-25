
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

#define fontFile	UFDIR"10x20.uf"



int main (int argc, char **argv)
{

#if !ENABLE_COMBINED
	printf("ENABLE_COMBINED not compiled in.");
	return 0;
#endif

	if (!initDemoConfig("config.cfg"))
		return 0;


	uint8_t c = 4;
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, (c<<11)|(c<<6)|c, NULL);
	_ufont_t font;
	

	if (!fontOpen(&font, fontFile)){
		printf("fontOpen() failed for '%s'\n", fontFile);
		return 0;
	}
			
	printf("font '%s'\n", fontFile);

	fontSetDisplayBuffer(&font, frame->pixels, DWIDTH, DHEIGHT);
	fontSetRenderSurface(&font, surface);	// composition target
	fontCleanSurface(&font, NULL);
	fontSetRenderColour(surface, COLOUR_24TO16(0x777777));

	fontSetGlyphPadding(&font, 0);
	fontSetLineSpace(&font, 2);
	uint32_t flags = BFONT_RENDER_ADVANCE_Y | BFONT_RENDER_FIXEDWIDTH | BFONT_RENDER_COMBINED | BFONT_RENDER_RETURN | BFONT_RENDER_NEWLINE;
	fontSetRenderFlags(&font, flags);

	int x = 10; int y = 2;

	fontPrint8(&font, &x, &y, 
		"2380 20E3 ⎀⃣   < Keyboard Symbols with combining enclosing keycap\r\n"
		"2381 20E3 ⎁⃣ \r\n"
		"2382 20E3 ⎂⃣ \r\n"
		"2383 20E3 ⎃⃣ \r\n"
		"2384 20E3 ⎄⃣ \r\n"
		"2385 20E3 ⎅⃣ \r\n"
		"2386 20E3 ⎆⃣ \r\n"
		"2387 20E3 ⎇⃣ \r\n"
		"2388 20E3 ⎈⃣ \r\n"
		"2389 20E3 ⎉⃣ \r\n"
		"238A 20E3 ⎊⃣ \r\n"
		"238B 20E3 ⎋⃣ \r\n"
		"238C 20E3 ⎌⃣ \r\n"
		"2396 20E3 ⎖⃣ \r\n"
		"2397 20E3 ⎗⃣ \r\n"
		"2398 20E3 ⎘⃣ \r\n"
		"2399 20E3 ⎙⃣ \r\n"
		"239A 20E3 ⎚⃣ \r\n"
		"\r\n"
		"2139 20DD ℹ⃝   < Information source with combining enclosing circle"
	);


	y += 6;
	fontPrint8(&font, &x, &y, "b̸̛̠͕͉̘͇̻̪̺̰̾̽̓̃͒̊̍͂ͯ̊̐ͬ͟͡͞ͅͅͅe̵̶̸̗͕͈͚̹̻̝͇̲͎̗͑ͦ̈́̋̎̀̐̋̎̎͐͋͂̔͐͋̂ͅḩ̴̨̛̫̻̖̙͚̻̣͉̺̻̭͚͎̻̺̮͙͚ͨͩ͐̾ͫ̿̔̉̏̃̾̊ͬ͐̍ͩ̑̓ͮi̧̼̝̫̭͉͗͑ͫ̈̿̽̍̓͑͛͋ͣͦ̋̑͆̈́ͅn̷̛͓͙͉̪͖̯͈͙̹̏̈̅̓ͮ̀ͅd̴̷͍̼̹͙̥̙̬͚̫̙̥̭̱̜̗̀̔̆͂̓ͥͫ̈͜ ̴̨̪̰̠̮̼͉͕͓̗͑̌ͨ̈́̿ͤ̆͋ͯ͗͆̕ť̵̷̡̛͇̜̜̎͑̌ͬͮ̄̊ͪ͆ͨ̀ͅh̨͔̠͇͖͕͚̣̫̄ͬ̄̾ͤ̑̑͌ͪͣ̍͌̌ͦ̅̆͌̚͟͢͝e̊̈́ͪ͐̌̀͝҉̢̣͎̞̱͙̪̗ ̔̂̊͌̔͗ͫͪ̋̊̇̍҉͏͇̝̭̝͎̞͚̞̳͕͔̯̪͎̫̲̗̯ͅw̷̡̢̠̘̠̙̻̳̞͐ͥͬ̉̿ͤ̇͌͋̿̚a̶͛̉̆́҉̖͇̱͚͙̭̟͓͙̫̘̟͈̫͟l̛̙̳̤̔̓̏͋͆ͮͩ̀ͥ̏ͮ̋͂ͩ̔̌́͘͞l͋ͧͮ̅ͯͭ̔ͯ͆ͩͦ̍͏̷̶̡͇̤̯̖̪̹͓̖́");
	y += 2;
	fontPrint8(&font, &x, &y, "The Greek word 'kosme': \"κόσμε\"");
	fontPrint8(&font, &x, &y, "STARGΛ̊TE SG-1, a = v̇ = r̈, a⃑ ⊥ b⃑");

	x = 150; y = 50;
	fontPrint8(&font, &x, &y, 
		"  [----------------------------|------------------------]\r\n"
		"    ๏ แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช  พระปกเกศกองบู๊กู้ขึ้นใหม่\r\n"
		"  สิบสองกษัตริย์ก่อนหน้าแลถัดไป       สององค์ไซร้โง่เขลาเบาปัญญา\r\n"
		"    ทรงนับถือขันทีเป็นที่พึ่ง           บ้านเมืองจึงวิปริตเป็นนักหนา\r\n"
		"  โฮจิ๋นเรียกทัพทั่วหัวเมืองมา         หมายจะฆ่ามดชั่วตัวสำคัญ\r\n"
		"    เหมือนขับไสไล่เสือจากเคหา      รับหมาป่าเข้ามาเลยอาสัญ\r\n"
		"  ฝ่ายอ้องอุ้นยุแยกให้แตกกัน          ใช้สาวนั้นเป็นชนวนชื่นชวนใจ\r\n"
		"    พลันลิฉุยกุยกีกลับก่อเหตุ          ช่างอาเพศจริงหนาฟ้าร้องไห้\r\n"
		"  ต้องรบราฆ่าฟันจนบรรลัย           ฤๅหาใครค้ำชูกู้บรรลังก์ ฯ"
		);

	y += 8;
	fontPrint8(&font, &x, &y, 
		"(The above is a two-column text. If combining characters are\r\n"
		" handled correctly, the lines of the second column should be\r\n"
		" aligned with the | character above.)");




	fontApplySurface(&font, 0, 0);
	fontSurfaceFree(surface);
	fontClose(&font);

	lRefresh(frame);
	Sleep(20000);

	demoCleanup();
	return 1;
}


