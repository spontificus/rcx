SOURCE = main.c shared.c graphics.c physics.c events.c loaders.c
SDL = `sdl-config --cflags --libs`
ODE = `ode-config --cflags --libs`
OGL = `pkg-config --cflags --libs glu`
LIBS   = $(SDL) $(ODE) $(OGL) -lm
ARGS   = -g -O2 -pipe -Wall -march=native -mtune=native -pg

normal: $(SOURCE)
	gcc -o ../rcx main.c $(LIBS) $(ARGS)

#might cause ("nVidious") problems... :-P
static: $(SOURCE)
	gcc -o ../rcx main.c `pkg-config --static --cflags --libs $(LIBS)` -static $(ARGS)

win32-static: $(SOURCE)
	gcc -o ../rcx.exe main.c `sdl-config --static-libs --cflags --libs` `ode-config --cflags --libs` -lm -lopengl32 -lglu32 -static -lstdc++ -lwinmm -lgdi32 -Dwindows $(ARGS)

#only use with a suitable port of gcc (MinGW, CygWin, etc)
win32-hack: $(SOURCE)
	/usr/bin/i686-pc-mingw32-gcc -o ../rcx.exe main.c -I/home/rfranks/ode-0.11/include /home/rfranks/ode-0.11/ode/src/.libs/libode.a /usr/i686-pc-mingw32/sys-root/mingw/lib/libSDL.dll.a -lmingw32 -mwindows -I/usr/i686-pc-mingw32/sys-root/mingw/include/SDL  -D_GNU_SOURCE=1 -Dmain=SDL_main -lmingw32 -lSDLmain  -mwindows -DdSINGLE  -lm -lopengl32 -lglu32 -static -lstdc++ -lwinmm -lgdi32 -Dwindows -L/usr/i686-pc-mingw32/sys-root/mingw/lib/ -L/usr/i686-pc-mingw32/sys-root/mingw/bin/ -lSDL.dll

clean:
	-rm -f ../rcx ../rcx.static ../rcx.exe
