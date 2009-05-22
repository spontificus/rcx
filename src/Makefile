SOURCE = main.c shared.c graphics.c physics.c events.c loaders.c trimesh.cpp turd.cpp interp.cpp mglui.cpp

#normal args and libs
LIBS    = sdl gl glu
ARGS    = -g -pipe -Wall -march=native -mtune=native -lm  -DdSINGLE -lode -L. -lglui -lglut
CC      = g++

#windows specific args and libs
W32LIBS = `sdl-config --static-libs --cflags --libs` `ode-config --cflags --libs` -lm -lopengl32 -lglu32
W32ARGS = -O2 -pipe -Wall -march=i686   -mtune=i686   -lm


normal: $(SOURCE)
	$(CC) -o ../rcx $(SOURCE) `pkg-config --cflags --libs $(LIBS)` $(ARGS) 

#might cause ("nVidious") problems... :-P
static: $(SOURCE)
	$(CC) -o ../rcx $(SOURCE) `pkg-config --static --cflags --libs $(LIBS)` -static $(ARGS)

#only use with a suitable port of gcc (MinGW, CygWin, etc)
win32-static: $(SOURCE)
	$(CC) -o ../rcx.exe $(SOURCE) $(W32LIBS) -static -lstdc++ -lwinmm -lgdi32 -Dwindows $(W32ARGS)

clean:
	-rm -f ../rcx ../rcx.static ../rcx.exe
