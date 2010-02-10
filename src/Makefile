#OBJS	= build/main.o build/shared/shared.o build/events/events.o build/loaders/loaders.o build/graphics/graphics.o build/physics/physics.o
OBJS    = $(patsubst %.cpp,build/%.o, $(wildcard */*.cpp) main.cpp)
LIBLST	= sdl gl glu ode

LIBS	= `pkg-config --libs $(LIBLST)`
FLAGS	= `pkg-config --cflags $(LIBLST)`
ARGS	= -O2 -pipe -Wall -march=native -mtune=native -lm $(FLAGS)

#final normal target
rcx: builddirs $(OBJS)
	g++ -o ../rcx $(OBJS) $(LIBS) $(ARGS)

#prerequisites
#basic dependencies (dependency on own header):
build/graphics/camera.o build/physics/camera.o build/shared/camera.o: shared/camera.hpp

build/loaders/conf.o: loaders/conf.hpp
build/loaders/profile.o: shared/profile.hpp
build/loaders/text_file.o: loaders/text_file.hpp
build/loaders/track.o: shared/track.hpp

build/physics/drag.o: physics/drag.hpp

#extra dependencies (dependency on other headers)
build/loaders/conf.o build/loaders/profile.o build/loaders/track.o: build/loaders/text_file.o
build/loaders/profile.o: loaders/conf.hpp

#TMP: only here until divided to smaller files
build/main.o: shared/*.hpp loaders/conf.hpp
build/graphics/graphics.o: graphics/graphics.hpp shared/shared.hpp
build/events/events.o: events/events.hpp shared/shared.hpp graphics/graphics.hpp loaders/loaders.hpp shared/camera.hpp
build/shared/shared.o: shared/shared.hpp
build/loaders/loaders.o: loaders/loaders.hpp loaders/text_file.hpp shared/shared.hpp
build/physics/physics.o: physics/physics.hpp shared/shared.hpp

#universal build rule
$(OBJS): build/%.o:%.cpp
	g++ -c -o $@ $< $(ARGS)

.PHONY: builddirs clean

builddirs:
	-mkdir -p build/{shared,events,loaders,graphics,physics}

clean:
	-rm -f ../rcx $(OBJS)
