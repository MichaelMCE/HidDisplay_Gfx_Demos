
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


	if (!initDemoConfig("config.cfg"))
		return 0;


	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_G4TO16(4), NULL);
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
	fontSetLineSpace(&font, 1);
	fontSetRenderFlags(&font, fontGetRenderFlags(&font) | BFONT_RENDER_FIXEDWIDTH);

	int x = 2;
	int y = 2;
	
#if 1
	fontPrint8(&font, &x, &y, "⡍⠜⠇⠑⠹ ⠺⠁⠎ ⠙⠑⠁⠙⠒ ⠞⠕ ⠃⠑⠛⠔ ⠺⠊⠹⠲ ⡹⠻⠑ ⠊⠎ ⠝⠕ ⠙⠳⠃⠞");
#endif

#if 1
 	fontPrint8(&font, &x, &y, " ╔════════════════╗");
	fontPrint8(&font, &x, &y, "8║░♜▓♞░♝▓♛░♚▓♝░♞▓♜║");
	fontPrint8(&font, &x, &y, "7║▓♟░♟▓♟░♟▓♟░♟▓♟░♟║");
	fontPrint8(&font, &x, &y, "6║░░▓▓░░▓▓░░▓▓░░▓▓║");
	fontPrint8(&font, &x, &y, "5║▓▓░░▓▓░░▓▓░░▓▓░░║");
	fontPrint8(&font, &x, &y, "4║░░▓▓░░▓▓░░▓▓░░▓▓║");
	fontPrint8(&font, &x, &y, "3║▓▓░░▓▓░░▓▓░░▓▓░░║");
	fontPrint8(&font, &x, &y, "2║░♙▓♙░♙▓♙░♙▓♙░♙▓♙║");
	fontPrint8(&font, &x, &y, "1║▓♖░♘▓♗░♕▓♔░♗▓♘░♖║");
	fontPrint8(&font, &x, &y, " ╚════════════════╝");
	fontPrint8(&font, &x, &y, "   a b c d e f g h");
#endif


#if 1

	x = 350;
	y = 30;
	fontPrint8(&font, &x, &y, "⎧⎡⎛┌─────┐⎞⎤⎫");
	fontPrint8(&font, &x, &y, "⎪⎢⎜│a²+b³ ⎟⎥⎪");
	fontPrint8(&font, &x, &y, "⎪⎢⎜│───── ⎟⎥⎪");
	fontPrint8(&font, &x, &y, "⎪⎢⎜⎷ c₈   ⎟⎥⎪");
	fontPrint8(&font, &x, &y, "⎨⎢⎜       ⎟⎥⎬");
	fontPrint8(&font, &x, &y, "⎪⎢⎜ ∞     ⎟⎥⎪");
	fontPrint8(&font, &x, &y, "⎪⎢⎜ ⎲     ⎟⎥⎪");
	fontPrint8(&font, &x, &y, "⎪⎢⎜ ⎳aⁱ-bⁱ⎟⎥⎪");
	fontPrint8(&font, &x, &y, "⎩⎣⎝i=1    ⎠⎦⎭");
	
#endif


#if 0
	fontPrint8(&font, &x, &y, "╔══════════════════════════════════════════╗");
	fontPrint8(&font, &x, &y, "║                                          ║");
	fontPrint8(&font, &x, &y, "║   • ‘single’ and “double” quotes         ║");
	fontPrint8(&font, &x, &y, "║                                          ║");
	fontPrint8(&font, &x, &y, "║   • Curly apostrophes: “We’ve been here” ║");
	fontPrint8(&font, &x, &y, "║                                          ║");
	fontPrint8(&font, &x, &y, "║   • Latin-1 apostrophe and accents: '´`  ║");
	fontPrint8(&font, &x, &y, "║                                          ║");
	fontPrint8(&font, &x, &y, "║   • ‚deutsche‘ „Anführungszeichen“       ║");
	fontPrint8(&font, &x, &y, "║                                          ║");
	fontPrint8(&font, &x, &y, "║   • †, ‡, ‰, •, 3–4, —, −5/+5, ™, …      ║");
	fontPrint8(&font, &x, &y, "║                                          ║");
	fontPrint8(&font, &x, &y, "║   • ASCII safety test: 1lI|, 0OD, 8B     ║");
	fontPrint8(&font, &x, &y, "║                      ╭─────────╮         ║");
	fontPrint8(&font, &x, &y, "║   • the euro symbol: │ 14.95 € │         ║");
	fontPrint8(&font, &x, &y, "║                      ╰─────────╯         ║");
	fontPrint8(&font, &x, &y, "╚══════════════════════════════════════════╝");
#endif

#if 1

	x = 2;
	y += 40;
	fontPrint8(&font, &x, &y, "╔══╦══╗  ┌──┬──┐  ╭──┬──╮  ╭──┬──╮  ┏━━┳━━┓  ┎┒┏┑   ╷  ╻ ┏┯┓ ┌┰┐    ▊ ╱╲╱╲╳╳╳");
	fontPrint8(&font, &x, &y, "║┌─╨─┐║  │╔═╧═╗│  │╒═╪═╕│  │╓─╁─╖│  ┃┌─╂─┐┃  ┗╃╄┙  ╶┼╴╺╋╸┠┼┨ ┝╋┥    ▋ ╲╱╲╱╳╳╳");
	fontPrint8(&font, &x, &y, "║│╲ ╱│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╿ │┃  ┍╅╆┓   ╵  ╹ ┗┷┛ └┸┘    ▌ ╱╲╱╲╳╳╳");
	fontPrint8(&font, &x, &y, "╠╡ ╳ ╞╣  ├╢   ╟┤  ├┼─┼─┼┤  ├╫─╂─╫┤  ┣┿╾┼╼┿┫  ┕┛┖┚     ┌┄┄┐ ╎ ┏┅┅┓ ┋ ▍ ╲╱╲╱╳╳╳");
	fontPrint8(&font, &x, &y, "║│╱ ╲│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╽ │┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▎");
	fontPrint8(&font, &x, &y, "║└─╥─┘║  │╚═╤═╝│  │╘═╪═╛│  │╙─╀─╜│  ┃└─╂─┘┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▏");
	fontPrint8(&font, &x, &y, "╚══╩══╝  └──┴──┘  ╰──┴──╯  ╰──┴──╯  ┗━━┻━━┛  ▗▄▖▛▀▜   └╌╌┘ ╎ ┗╍╍┛ ┋  ▁▂▃▄▅▆▇█");
	fontPrint8(&font, &x, &y, "                                             ▝▀▘▙▄▟");
#endif

#if 0
	// runes
	fontPrint8(&font, &x, &y, "ᚻᛖ ᚳᚹᚫᚦ ᚦᚫᛏ ᚻᛖ ᛒᚢᛞᛖ ᚩᚾ ᚦᚫᛗ ᛚᚪᚾᛞᛖ ᚾᚩᚱᚦᚹᛖᚪᚱᛞᚢᛗ ᚹᛁᚦ ᚦᚪ ᚹᛖᛥᚫ");
	fontPrint8(&font, &x, &y, "ሰማይ አይታረስ ንጉሥ አይከሰስ።");
	fontPrint8(&font, &x, &y, "ብላ ካለኝ እንደአባቴ በቆመጠኝ።");
	fontPrint8(&font, &x, &y, "ጌጥ ያለቤቱ ቁምጥና ነው።");
	fontPrint8(&font, &x, &y, "ደሀ በሕልሙ ቅቤ ባይጠጣ ንጣት በገደለው።");
	fontPrint8(&font, &x, &y, "የአፍ ወለምታ በቅቤ አይታሽም።");
	fontPrint8(&font, &x, &y, "አይጥ በበላ ዳዋ ተመታ።");
	fontPrint8(&font, &x, &y, "ሲተረጉሙ ይደረግሙ።");
#endif


#if 0
	fontPrint8(&font, &x, &y, "    &lt;──C0──&gt; &lt;─────────GL──────────&gt;&lt;──C1──&gt; &lt;─────────GR──────────&gt;");
	fontPrint8(&font, &x, &y, "      00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15");
	fontPrint8(&font, &x, &y, "    ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐");
	fontPrint8(&font, &x, &y, " 00 │       │SP │                   │       │   │                   │");
	fontPrint8(&font, &x, &y, " 01 │       ├───┘                   │       ├───┘                   │");
	fontPrint8(&font, &x, &y, " 02 │       │                       │       │                       │");
	fontPrint8(&font, &x, &y, " 03 │   C   │       ASCII           │   C   │        Special        │");
	fontPrint8(&font, &x, &y, " 04 │   o   │       graphics        │   o   │        Graphics       │");
	fontPrint8(&font, &x, &y, " 05 │   n   │                       │   n   │                       │");
	fontPrint8(&font, &x, &y, " 06 │   t   │                       │   t   │                       │");
	fontPrint8(&font, &x, &y, " 07 │   r   │                       │   r   │                       │");
	fontPrint8(&font, &x, &y, " 08 │   o   │                       │   o   │                       │");
	fontPrint8(&font, &x, &y, " 09 │   l   │                       │   l   │                       │");
	fontPrint8(&font, &x, &y, " 10 │   s   │                       │   s   │                       │");
	fontPrint8(&font, &x, &y, " 11 │       │                       │       │                       │");
	fontPrint8(&font, &x, &y, " 12 │       │                       │       │                       │");
	fontPrint8(&font, &x, &y, " 13 │       │                       │       │                       │");
	fontPrint8(&font, &x, &y, " 14 │       │                   ┌───┤       │                   ┌───┤");
	fontPrint8(&font, &x, &y, " 15 │       │                   │DEL│       │                   │   │");
	fontPrint8(&font, &x, &y, "    └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘");
	fontPrint8(&font, &x, &y, "    &lt;──C0──&gt; &lt;─────────GL──────────&gt;&lt;──C1──&gt; &lt;─────────GR──────────&gt;");
#endif


#if 1
	fontPrint8(&font, &x, &y, "⎛ n ⎞         n!");
	fontPrint8(&font, &x, &y, "⎜   ⎟ =  ───────────  =  n  ×  (n - 1)");
	fontPrint8(&font, &x, &y, "⎝ 2 ⎠    2! × (n-2)!");
#endif



	fontApplySurface(&font, 0, 0);
	fontSurfaceFree(surface);
	fontClose(&font);

	lRefresh(frame);
	Sleep(20000);

	demoCleanup();
	return 1;
}


