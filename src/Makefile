SOURCE = main.c shared.c graphics.c physics.c events.c loaders.c
SDL = `sdl-config --cflags --libs`
ODE = `ode-config --cflags --libs`
OGL = `pkg-config --cflags --libs glu`
LIBS   = $(SDL) $(ODE) $(OGL) -lm
ARGS   = -O2 -pipe -Wall -march=native -mtune=native
W32ARGS= -march=i686 -mtune=i686 -O2 -pipe -Wall

normal: $(SOURCE)
	g++ -o ../rcx main.c $(LIBS) $(ARGS)

#might cause ("nVidious") problems... :-P
static: $(SOURCE)
	g++ -o ../rcx main.c `pkg-config --static --cflags --libs $(LIBS)` -static $(ARGS)

#only use with a suitable port of gcc (MinGW, CygWin, etc)
win32-static: $(SOURCE)
	g++ -o ../rcx.exe main.c `sdl-config --static-libs --cflags --libs` `ode-config --cflags --libs` -lm -lopengl32 -lglu32 -static -lstdc++ -lwinmm -lgdi32 -Dwindows $(W32ARGS)

clean:
	-rm -f ../rcx ../rcx.static ../rcx.exe
