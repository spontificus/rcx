OBJS	= build/main.o build/shared/shared.o build/events/events.o build/loaders/loaders.o build/graphics/graphics.o build/physics/physics.o
LIBLST	= sdl gl glu ode

LIBS	= `pkg-config --libs $(LIBLST)`
FLAGS	= `pkg-config --cflags $(LIBLST)`
ARGS	= -O2 -pipe -Wall -march=native -mtune=native -lm $(FLAGS)

rcx: builddirs $(OBJS)
	g++ -o ../rcx $(OBJS) $(LIBS) $(ARGS)

build/main.o: main.cpp shared/shared.hpp loaders/conf.cpp
	g++ -c -o $@ main.cpp $(ARGS)

build/shared/shared.o: shared/shared.cpp shared/shared.hpp shared/camera.cpp
	g++ -c -o $@ shared/shared.cpp $(ARGS)

build/events/events.o: events/events.cpp events/events.hpp shared/shared.hpp
	g++ -c -o $@ events/events.cpp $(ARGS)

build/loaders/loaders.o: loaders/loaders.cpp loaders/loaders.hpp loaders/text_file.hpp loaders/text_file.cpp loaders/profile.cpp loaders/track.cpp shared/shared.hpp
	g++ -c -o $@ loaders/loaders.cpp $(ARGS)

build/graphics/graphics.o: graphics/graphics.cpp graphics/graphics.hpp graphics/camera.cpp shared/shared.hpp
	g++ -c -o $@ graphics/graphics.cpp $(ARGS)

build/physics/physics.o: physics/physics.cpp physics/physics.hpp physics/camera.cpp physics/drag.cpp shared/shared.hpp
	g++ -c -o $@ physics/physics.cpp $(ARGS)


.PHONY: builddirs clean

builddirs:
	-mkdir -p build/{shared,events,loaders,graphics,physics}

clean:
	-rm -f ../rcx $(OBJS)
