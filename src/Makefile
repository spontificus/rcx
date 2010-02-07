OBJS	= build/main.o build/shared/shared.o build/events/events.o build/loaders/loaders.o build/graphics/graphics.o build/physics/physics.o
LIBLST	= sdl gl glu ode

LIBS	= `pkg-config --libs $(LIBLST)`
FLAGS	= `pkg-config --cflags $(LIBLST)`
ARGS	= -O2 -pipe -Wall -march=native -mtune=native -lm $(FLAGS)

rcx: builddirs $(OBJS)
	g++ -o ../rcx $(OBJS) $(LIBS) $(ARGS)

build/main.o: main.c shared/shared.hpp loaders/conf.cpp
	g++ -c -o $@ main.c $(ARGS)

build/shared/shared.o: shared/shared.c shared/shared.hpp shared/camera.c
	g++ -c -o $@ shared/shared.c $(ARGS)

build/events/events.o: events/events.c events/events.h shared/shared.hpp
	g++ -c -o $@ events/events.c $(ARGS)

build/loaders/loaders.o: loaders/loaders.cpp loaders/loaders.h loaders/text_file.hpp loaders/text_file.cpp loaders/profile.cpp loaders/track.cpp shared/shared.hpp
	g++ -c -o $@ loaders/loaders.cpp $(ARGS)

build/graphics/graphics.o: graphics/graphics.c graphics/graphics.h graphics/camera.c shared/shared.hpp
	g++ -c -o $@ graphics/graphics.c $(ARGS)

build/physics/physics.o: physics/physics.c physics/physics.h physics/camera.c physics/drag.c shared/shared.hpp
	g++ -c -o $@ physics/physics.c $(ARGS)


.PHONY: builddirs clean

builddirs:
	-mkdir -p build/{shared,events,loaders,graphics,physics}

clean:
	-rm -f ../rcx $(OBJS)
