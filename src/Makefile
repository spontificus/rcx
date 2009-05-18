SOURCE = main.c shared.c graphics.c physics.c events.c loaders.c trimesh.cpp turd.cpp

#normal args and libs
LIBS    = sdl gl glu
ARGS    = -g -pipe -Wall -march=native -mtune=native -lm  -DdSINGLE -lode
CC      = g++

#windows specific args and libs
W32LIBS = `sdl-config --static-libs --cflags --libs` `ode-config --cflags --libs` -lm -lopengl32 -lglu32
W32ARGS = -O2 -pipe -Wall -march=i686   -mtune=i686   -lm


normal: $(SOURCE)
	$(CC) -o ../rcx $(SOURCE) `pkg-config --cflags --libs $(LIBS)` $(ARGS) 

#might cause ("nVidious") problems... :-P
static: $(SOURCE)
	$(CC) -o ../rcx $(SOURCE) `pkg-config --static --cflags --libs $(LIBS)` -static $(ARGS)

win32-static: $(SOURCE)
	$(CC) -o ../rcx.exe $(SOURCE) $(W32LIBS) -static -lstdc++ -lwinmm -lgdi32 -Dwindows $(W32ARGS)

#only use with a suitable port of gcc (MinGW, CygWin, etc)
win32-hack: $(SOURCE)
	/usr/bin/i686-pc-mingw32-gcc -o ../rcx.exe main.c -I/home/rfranks/ode-0.11/include /home/rfranks/ode-0.11/ode/src/.libs/libode.a /usr/i686-pc-mingw32/sys-root/mingw/lib/libSDL.dll.a -lmingw32 -mwindows -I/usr/i686-pc-mingw32/sys-root/mingw/include/SDL  -D_GNU_SOURCE=1 -Dmain=SDL_main -lmingw32 -lSDLmain  -mwindows -DdSINGLE  -lm -lopengl32 -lglu32 -static -lstdc++ -lwinmm -lgdi32 -Dwindows -L/usr/i686-pc-mingw32/sys-root/mingw/lib/ -L/usr/i686-pc-mingw32/sys-root/mingw/bin/ -lSDL.dll

win32-mac: $(SOURCE)
	gcc -o ../rcx.exe main.c `sdl-config --static-libs --cflags --libs` -DdSINGLE -lode -lm -lopengl32 -lglu32 -static -lstdc++ -lwinmm -lgdi32 -Dwindows -O2 -pipe -Wall

clean:
	-rm -f ../rcx ../rcx.static ../rcx.exe
