
#ifndef _COMMONMP_H_
#define _COMMONMP_H_


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdarg.h>
#include <ctype.h>
#include <wtypes.h>
#include <math.h>
#include <time.h>

#include <mylcd.h>
#include <demos.h>


#if FONTS_LLATIN
#define UFDIR		"../fonts/uf_Latin/"
#elif FONTS_DIGIITS
#define UFDIR		"../fonts/uf_Digits/"
#elif FONTS_NOCJK
#define UFDIR		"../fonts/uf_noCJK/"
#else
#define UFDIR		"../fonts/uf/"
#endif



#include "../src/ufont.h"
#include "../src/fonts.h"
#include "../src/ufont_primitives.h"

#endif

