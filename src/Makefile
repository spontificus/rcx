SOURCE = main.c shared.c graphics.c physics.c events.c loaders.c
SDL = `sdl-config --cflags --libs`
ODE = `ode-config --cflags --libs`
OGL = `pkg-config --cflags --libs glu`
LIBS   = $(SDL) $(ODE) $(OGL) -lm
ARGS   = -pipe -Wall -march=native -mtune=native -g

normal: $(SOURCE)
	gcc -o ../rcx main.c $(LIBS) $(ARGS)

#might cause ("nVidious") problems... :-P
static: $(SOURCE)
	gcc -o ../rcx main.c `pkg-config --static --cflags --libs $(LIBS)` -static $(ARGS)

#only use with a suitable port of gcc (MinGW, CygWin, etc)
win32-static: $(SOURCE)
	gcc -o ../rcx.exe main.c `sdl-config --static-libs --cflags --libs` `ode-config --cflags --libs` -lm -lopengl32 -lglu32 -static -lstdc++ -lwinmm -lgdi32 -Dwindows $(ARGS)

clean:
	-rm -f ../rcx ../rcx.static ../rcx.exe
