OBJS	= main.o shared.o events.o loaders.o graphics.o physics.o
LIBLST	= sdl gl glu ode

LIBS	= `pkg-config --libs $(LIBLST)`
FLAGS	= `pkg-config --cflags $(LIBLST)`
ARGS	= -O2 -pipe -Wall -march=native -mtune=native -lm $(FLAGS)

rcx: $(OBJS)
	g++ -o ../rcx $(OBJS) $(LIBS) $(ARGS)

main.o: main.c shared/shared.hpp loaders/conf.cpp
	g++ -c -o main.o main.c $(ARGS)

shared.o: shared/shared.c shared/shared.hpp shared/camera.c
	g++ -c -o shared.o shared/shared.c $(ARGS)

events.o: events/events.c events/events.h shared/shared.hpp
	g++ -c -o events.o events/events.c $(ARGS)

loaders.o: loaders/loaders.cpp loaders/loaders.h loaders/text_file.hpp loaders/text_file.cpp loaders/profile.cpp loaders/track.cpp shared/shared.hpp
	g++ -c -o loaders.o loaders/loaders.cpp $(ARGS)

graphics.o: graphics/graphics.c graphics/graphics.h graphics/camera.c shared/shared.hpp
	g++ -c -o graphics.o graphics/graphics.c $(ARGS)

physics.o: physics/physics.c physics/physics.h physics/camera.c physics/drag.c shared/shared.hpp
	g++ -c -o physics.o physics/physics.c $(ARGS)


.PHONY: clean
clean:
	-rm -f ../rcx $(OBJS)
