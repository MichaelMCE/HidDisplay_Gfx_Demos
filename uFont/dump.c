
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


#define FONTFILE	UFDIR"unicode36.uf"


#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif

#define WM_MM					(WM_USER+4000)
#define WM_DD_PAINT				(WM_MM+1001)
#define WM_DD_MOUSEACTIVATE  	(WM_MM+WM_MOUSEACTIVATE)
#define WM_DD_MOUSEMOVE	    	(WM_MM+WM_MOUSEMOVE)
#define WM_DD_LBUTTONDOWN	   	(WM_MM+WM_LBUTTONDOWN)
#define WM_DD_LBUTTONUP	    	(WM_MM+WM_LBUTTONUP)
#define WM_DD_LBUTTONDBLCLK   	(WM_MM+WM_LBUTTONDBLCLK)
#define WM_DD_RBUTTONDOWN	   	(WM_MM+WM_RBUTTONDOWN)
#define WM_DD_RBUTTONUP	    	(WM_MM+WM_RBUTTONUP)
#define WM_DD_RBUTTONDBLCLK   	(WM_MM+WM_RBUTTONDBLCLK)
#define WM_DD_MBUTTONDOWN	   	(WM_MM+WM_MBUTTONDOWN)
#define WM_DD_MBUTTONUP	    	(WM_MM+WM_MBUTTONUP)
#define WM_DD_MBUTTONDBLCLK   	(WM_MM+WM_MBUTTONDBLCLK)
#define WM_DD_MOUSEWHEEL	   	(WM_MM+WM_MOUSEWHEEL)
#define WM_DD_MOUSEHWHEEL	   	(WM_MM+WM_MOUSEHWHEEL)
#define WM_DD_MOUSEHOVER	   	(WM_MM+WM_MOUSEHOVER)
#define WM_DD_MOUSELEAVE	   	(WM_MM+WM_MOUSELEAVE)
#define WM_DD_NCMOUSEHOVER		(WM_MM+WM_NCMOUSEHOVER)
#define WM_DD_NCMOUSELEAVE		(WM_MM+WM_NCMOUSELEAVE)
#define WM_DD_CHARDOWN			(WM_MM+WM_CHAR)
#define WM_DD_KEYDOWN			(WM_MM+WM_KEYDOWN)
#define WM_DD_CLOSE				(WM_MM+WM_CLOSE)


enum _inevents {
	EV_KEY_DOWN = 1,
	EV_KEY_UP,
	EV_MOUSE_LDOWN,
	EV_MOUSE_LUP,
	EV_MOUSE_MDOWN,
	EV_MOUSE_MUP,
	EV_MOUSE_RDOWN,
	EV_MOUSE_RUP,
	EV_WHEEL_FORWARD,
	EV_WHEEL_BACK,
	EV_WHEEL_LEFT,
	EV_WHEEL_RIGHT,
	EV_PASSTHROUGH
};


static volatile int windowState = 1;
static volatile int update = EV_PASSTHROUGH;
static uint8_t glyphRect = 0;
static int keyPressed = 0;
static uint8_t drawLabel = 0xFF;
static uint32_t hWinMsgThread;
static HANDLE hMsgWin;


static inline LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//printf("%p %i/%X %i %X, %X %X, %X %X\n", hwnd, (int)message, (int)message, (int)wParam, (int)lParam, (int)wParam&0xFFFF, (int)lParam&0xFFFF, (int)(wParam>>16)&0xFFFF, (int)(lParam>>16)&0xFFFF);
		
	switch(message){
	  case WM_DD_KEYDOWN:{
	  	keyPressed = (int)wParam;
		update = EV_KEY_DOWN;	  	
		return 1;
	  }
	  case WM_DD_LBUTTONUP:{
		update = EV_MOUSE_LUP;
		return 1;
	  }
	  case WM_DD_RBUTTONUP:{
	  	update = EV_MOUSE_RUP;
		return 1;
	  }
	  case WM_DD_MOUSEWHEEL:{
		if (((int)wParam>>16) < 0)
			update = EV_WHEEL_FORWARD;
		else if (((int)wParam>>16) > 0)
			update = EV_WHEEL_BACK;

		return 1;
	  }
	  case WM_QUIT: 
		DestroyWindow(hwnd);
	  case WM_CLOSE: 
		return 0;
 	  case WM_DESTROY:
		PostQuitMessage(0);
	    return 0;
	  case WM_DD_CLOSE:
	  	windowState = 0;
		PostMessage(hwnd, WM_QUIT, 0, 0);
		return 0;
	}
	
	return DefWindowProc(hwnd, message, wParam, lParam);
}


static inline int processWindowMessages ()
{
	MSG msg = {0};
	int ret = 0;
	
	if ((ret=GetMessage(&msg, NULL/*hMsgWin*/, 0, 0)) > 0){
		//HWND hwnd = GetActiveWindow();
		//if (!IsWindow(hwnd) || !IsDialogMessageA(hwnd, &msg)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		//}
	}
	
	return ret;
}
static inline HANDLE initGUI (void *opaque)
{
	const char *szClassName = "ufdump";
    WNDCLASSEX wincl;
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hInstance = GetModuleHandle(0);
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.hIcon = NULL;
    wincl.hIconSm = NULL;
    wincl.hCursor = NULL;
    wincl.hbrBackground = NULL;
    wincl.style = CS_DBLCLKS;
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    if (!RegisterClassEx (&wincl))
        return NULL;

    HWND hMsgWin = CreateWindow(szClassName, szClassName, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,\
	  0, 0, HWND_DESKTOP, NULL, wincl.hInstance, NULL);

	if (hMsgWin){
    	SetWindowLongPtr(hMsgWin, GWLP_USERDATA, (LONG_PTR)opaque);
    	ShowWindow(hMsgWin, SW_HIDE);
    }

    return hMsgWin;
}

static unsigned int __stdcall winMessageThread (void *ptr)
{
	//SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);		// be responsive

	HANDLE win = initGUI(ptr);

	lSetDisplayOption(hw, did, lOPT_DDRAW_HWNDTARGET, (intptr_t*)win);
	while(windowState)
		processWindowMessages();
	lSetDisplayOption(hw, did, lOPT_DDRAW_HWNDTARGET, NULL);

	_endthreadex(1);
	return 1;
}

static inline int getCharsHeight (_ufont_t *font, uint16_t ch)
{
	int height = 0;
	uint16_t text[] = {ch, L'!', L':', L'\\', L'/', L'{', L'(', 0};
	fontGetMetricsList(font, text, NULL, &height);
	return height;
}

static inline void clearFrame (void *buffer, const uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;
	
	int tPixels = DWIDTH * DHEIGHT;
	while (tPixels--) pixels[tPixels] = colour;		
}

static inline void startEventListener (void *opaque)
{
	uint32_t winMsgThreadID = 0;
	hWinMsgThread = _beginthreadex(NULL, 0, winMessageThread, opaque, 0, &winMsgThreadID);
	Sleep(10);
}

static inline void stopEventListener ()
{
	PostMessage(hMsgWin, WM_QUIT, 0, 0);
}

static inline void waitforEventListener ()
{
	WaitForSingleObject((HANDLE)hWinMsgThread, INFINITE);
	CloseHandle((HANDLE)hWinMsgThread);
}

static inline _lookup_t *loadGlyphLUT (_ufont_t *font, int *gtotal)
{
	_ufont_header_t *header = &font->header;
	size_t tableSize = header->lookup.glyphs * sizeof(_lookup_t);
	
	_lookup_t *table = calloc(tableSize, 1);
	if (!table) return NULL;
	
	fio_seek(font->fio, header->lookup.offset);
	if (fio_read(font->fio, table, tableSize) != tableSize){
		free(table);
		table = NULL;
	}

	*gtotal = header->lookup.glyphs;
	return table;
}


int main (int argc, char **argv)
{

	if (!initDemoConfig("config.cfg"))
		return 0;


	char *fontFile = FONTFILE;
	if (argc == 2) fontFile = argv[1];
	printf("File: '%s'\n", fontFile);
	
	printf("Use scroll and/or arrow keys\n");

	_ufont_t font;
	if (!fontOpen(&font, fontFile))
		return 0;

	int gtotal = 0;
	_lookup_t *table = loadGlyphLUT(&font, &gtotal);
	if (!table || !gtotal){
		fontClose(&font);
		return 0;
	}	

	startEventListener(&font);

	uint8_t buffer[CALC_PITCH_16(DWIDTH)*DHEIGHT];
	memset(buffer, 0, sizeof(buffer));

	fontSetDisplayBuffer(&font, buffer, DWIDTH, DHEIGHT);
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_24TO16(0x777777), NULL);
	fontSetRenderSurface(&font, surface);
	fontSetRenderFlags(&font, BFONT_RENDER_DEFAULT|BFONT_RENDER_NODEFAULT);
	//fontSetGlyphPadding(&font, 8);


	int ch = 0;
	int total = 0;
	wchar_t line[64] = {0};
	int fheight = getCharsHeight(&font, L'|')-1;

	while (windowState){
		if (update){
			int x = 0, y = 0;
			ch -= total;

			if (update == EV_WHEEL_BACK){			// scroll up
				ch -= total;
				
			}else if (update == EV_WHEEL_FORWARD){	// scroll down
				ch += total;
				
			}else if (update == EV_KEY_DOWN){		// key press
				if (keyPressed == VK_HOME) ch = 0;
				else if (keyPressed == VK_END) ch = 0xFFFF-16;
				else if (keyPressed == VK_UP) ch -= 16;
				else if (keyPressed == VK_DOWN) ch += 16;
				//else if (keyPressed == VK_RIGHT)
				//else if (keyPressed == VK_LEFT)
				
			}else if (update == EV_MOUSE_RUP){		// right mouse button
				drawLabel ^= 0xFF;

			}else if (update == EV_MOUSE_LUP){		// left mouse button
				glyphRect ^= 0xFF;

			}else if (update == EV_PASSTHROUGH){
				
			}
		
			if (ch < 0)
				ch = 0;
			else if (ch >= gtotal-1)
				ch = gtotal-1;
			int first = ch;

			clearFrame(buffer, COLOUR_CREAM);
			fontCleanSurface(&font, NULL);
				
			if (!drawLabel){
				fontSetRenderFlags(&font, BFONT_RENDER_GLYPHCLIP|BFONT_RENDER_ADVANCE_X|BFONT_RENDER_NODEFAULT|BFONT_RENDER_NEWLINE);
				
				if (glyphRect)
					fontSetRenderFlags(&font, BFONT_RENDER_GLYPHRECT|fontGetRenderFlags(&font));

				line[0] = 0;
				line[1] = 0;
				
				while(ch < gtotal && y < DHEIGHT-fheight){
					line[0] = table[ch].encoding;

					int ret = fontPrintList(&font, &x, &y, line);
					if (!ret){
						x = 0;
						fontPrintList(&font, &x, &y, L" \n");
					}else{
						x += 2;
						ch++;
					}
				}
			
			}else{
				if (glyphRect)
					fontSetRenderFlags(&font, BFONT_RENDER_GLYPHRECT|BFONT_RENDER_DEFAULT|BFONT_RENDER_NODEFAULT);
				else
					fontSetRenderFlags(&font, BFONT_RENDER_DEFAULT|BFONT_RENDER_NODEFAULT);
					
				int lineIdx = 6;				// 6 because '%.4X| ' ==  'FFFF| '
				
				while(ch < gtotal && y < DHEIGHT-fheight){
					_swprintf(line, L"%.4X| ", table[ch].encoding);
					int _ch = ch;

					// 16 glyphs per line + 1 space per glyph (16*2)
					for (int i = 0; i < 16*2 && ch < gtotal; i+=2, ch++){
						if (table[ch].encoding > table[_ch].encoding+15)
							break;

						line[lineIdx+i] = table[ch].encoding;
						line[lineIdx+i+1] = L' ';
						line[lineIdx+i+2] = 0;
					}
				
					int ret = fontGetMetricsList(&font, &line[lineIdx], NULL, NULL);
					if (ret > 0)
						fontPrintList(&font, &x, &y, line);
					else
						break;
				}
			}

			total = (ch - first);
			update = 0;
			
			fontApplySurface(&font, 0, 0);
			lUpdate(hw, buffer, sizeof(buffer));			

		}else if (windowState){
			Sleep(40);
		}
	}

	free(table);
	fontSurfaceFree(surface);
	fontClose(&font);
	waitforEventListener();
	demoCleanup();

	return 1;
}


