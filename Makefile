SOURCE = main.c shared.c graphics.c physics.c events.c loaders.c scene.c
SDL = `sdl-config --cflags --libs`
ODE = `ode-config --cflags --libs`
OGL = -lGL -lGLU
LIBS = $(SDL) $(ODE) $(OGL) -lm

normal: $(SOURCE)
	gcc -o rcx main.c $(LIBS)

#might cause ("nVidious") problems... :-P
static: $(SOURCE)
	gcc -o rcx.static main.c $(LIBS) -static

#only use with a suitable port of gcc (MinGW, CygWin, etc) (look in graphics.c for explanation of BADWM)
win32-static: $(SOURCE)
	gcc -o rcx.exe main.c $(SDL) $(ODE) -lm -lopengl32 -lglu32 -static -lstdc++ -lwinmm -lgdi32 -D BADWM

clean:
	-rm -f rcx rcx.static
