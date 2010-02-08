OBJS	= build/main.o build/shared/shared.o build/events/events.o build/loaders/loaders.o build/graphics/graphics.o build/physics/physics.o
LIBLST	= sdl gl glu ode

LIBS	= `pkg-config --libs $(LIBLST)`
FLAGS	= `pkg-config --cflags $(LIBLST)`
ARGS	= -O2 -pipe -Wall -march=native -mtune=native -lm $(FLAGS)

#final normal target
rcx: builddirs $(OBJS)
	g++ -o ../rcx $(OBJS) $(LIBS) $(ARGS)

#prerequisites
build/main.o: build/shared/shared.o loaders/conf.cpp
build/shared/shared.o: shared/shared.hpp shared/camera.cpp
build/events/events.o: events/events.hpp shared/shared.hpp
build/loaders/loaders.o: loaders/loaders.hpp loaders/text_file.hpp loaders/text_file.cpp loaders/profile.cpp loaders/track.cpp shared/shared.hpp
build/graphics/graphics.o: graphics/graphics.hpp graphics/camera.cpp shared/shared.hpp
build/physocs/physics.o: physics/physics.hpp physics/camera.cpp physics/drag.cpp shared/shared.hpp

#universal build rule
$(OBJS): build/%.o:%.cpp
	g++ -c -o $@ $< $(ARGS)

.PHONY: builddirs clean

builddirs:
	-mkdir -p build/{shared,events,loaders,graphics,physics}

clean:
	-rm -f ../rcx $(OBJS)
