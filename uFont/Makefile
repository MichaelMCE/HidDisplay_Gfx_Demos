

CC=gcc


x64 := 0
ifeq ($(x64),1)
MDIR=x64
BITMODE=-m64
else
MDIR=x32
BITMODE=-m32
endif

# Graphite loop optimizations
#GLOP = -ftree-vectorize -floop-interchange -floop-strip-mine -floop-block
GLOP =

# -flto
CFLAGS = $(BITMODE) -I../include -Wall -O2 
#CFLAGS = -Wno-pointer-sign -DNO_DUMMY_DECL $(BITMODE) -I../include -Wall -march=i686 -mtune=i686 -O2 -DSTBI_FAILURE_USERMSG -D_WIN32 -D_WIN32_WINNT=0x0501 -DWINVER=0x0501 -D__WIN32__=1 -std=gnu99 -funroll-loops -finline-functions -fomit-frame-pointer -pipe -s -DUSE_MMX -DHAVE_MMX -DHAVE_MMX1 -DUSE_MMX1 -mmmx -msse -mfpmath=sse,387 -fgcse-las -fgcse-sm -fgcse-lm -fmodulo-sched-allow-regmoves -fmodulo-sched -ftree-vectorizer-verbose=0 $(GLOP) -Wno-strict-aliasing 
#CFLAGS = $(BITMODE) -static-libgcc -I../include  -D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -D__WIN32__=1 -std=gnu99  -Wall -march=i686 -mtune=i686 -O2

# -flto
LIBS = $(BITMODE)  -static-libgcc -L../libs/ -L"lib" -lm -lmylcddll -lole32 -luuid

PNG = ../src/libpng/png.o \
	../src/libpng/pngerror.o \
	../src/libpng/pngget.o \
	../src/libpng/pngmem.o \
	../src/libpng/pngpread.o \
	../src/libpng/pngread.o \
	../src/libpng/pngrio.o \
	../src/libpng/pngrtran.o \
	../src/libpng/pngrutil.o \
	../src/libpng/pngset.o \
	../src/libpng/pngtrans.o \
	../src/libpng/pngwio.o \
	../src/libpng/pngwrite.o \
	../src/libpng/pngwtran.o \
	../src/libpng/pngwutil.o

ZLIB = ../src/libpng/zutil.o \
	../src/libpng/inftrees.o \
	../src/libpng/inflate.o \
	../src/libpng/inffast.o \
	../src/libpng/crc32.o \
	../src/libpng/adler32.o


UFONTOBJECTS = ../src/fio/fileio.o \
	../src/libuf.o \
	../src/libpng.o \
	../src/ufont_primitives.o

LIBOBJECTS = $(UFONTOBJECTS) $(ZLIB) $(PNG) 

EXAMPLES=glyph.exe clock.exe mystify.exe ball.exe cube.exe palettes.exe helloworld.exe uflookup.exe \
		charrefs.exe utf8demo.exe bdf2ufont.exe multifont.exe stars.exe fontfun.exe anidemo.exe shapes.exe \
		dump.exe utf8combining.exe spritetext.exe wrap.exe drawop.exe

all : $(EXAMPLES)


multifont.exe: $(LIBOBJECTS)
ufdemo.exe: $(LIBOBJECTS)
utf8demo.exe: $(LIBOBJECTS)
shapes.exe: $(LIBOBJECTS)
anidemo.exe: $(LIBOBJECTS)
stars.exe: $(LIBOBJECTS)
spritetext.exe: $(LIBOBJECTS)
fontfun.exe: $(LIBOBJECTS)
utf8combining.exe: $(LIBOBJECTS)
charrefs.exe: $(LIBOBJECTS)
uflookup.exe: $(LIBOBJECTS)
helloworld.exe: $(LIBOBJECTS)
palettes.exe: $(LIBOBJECTS)
cube.exe: $(LIBOBJECTS)
ball.exe: $(LIBOBJECTS)
mystify.exe: $(LIBOBJECTS)
clock.exe: $(LIBOBJECTS)
dump.exe: $(LIBOBJECTS)
glyph.exe: $(LIBOBJECTS)
wrap.exe: $(LIBOBJECTS)
drawop.exe: $(LIBOBJECTS)


# -static-libgcc
%.exe: %.o 
	$(CC)  -o $@ $^ $(LIBS) 
#	strip.exe $@

%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.exe *.o *.bak


### PHONY define
.PHONY: all all-before all-after clean clean-custom



