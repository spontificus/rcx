SOURCE = main.c shared/shared.c graphics/graphics.c \
	 physics/physics.c events/events.c loaders/loaders.c

#normal args and libs
LIBS    = sdl gl glu ode
ARGS    = -O2 -pipe -Wall -march=native -mtune=native -lm

#windows specific args and libs
W32LIBS = `sdl-config --static-libs --cflags --libs` `ode-config --cflags --libs` -lm -lopengl32 -lglu32
W32ARGS = -O2 -pipe -Wall -march=i686   -mtune=i686   -lm


normal: $(SOURCE)
	g++ -o ../rcx main.c `pkg-config --cflags --libs $(LIBS)` $(ARGS)

#only use with a suitable port of gcc (MinGW, CygWin, etc)
win32-static: $(SOURCE)
	g++ -o ../rcx.exe main.c $(W32LIBS) -static -lstdc++ -lwinmm -lgdi32 -Dwindows $(W32ARGS)

clean:
	-rm -f ../rcx ../rcx.exe
