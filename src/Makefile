#OBJS	= build/main.o build/shared/shared.o build/events/events.o build/loaders/loaders.o build/graphics/graphics.o build/physics/physics.o
OBJS    = $(patsubst %.cpp,build/%.o, $(wildcard */*.cpp) main.cpp)
LIBLST	= sdl gl glu ode

LIBS	= `pkg-config --libs $(LIBLST)`
FLAGS	= `pkg-config --cflags $(LIBLST)`
ARGS	= -O2 -pipe -Wall -march=native -mtune=native -lm $(FLAGS)

all: rcx

#final normal target
rcx: builddirs $(OBJS)
	g++ -o ../rcx $(OBJS) $(LIBS) $(ARGS)

#prerequisites
include prereqs.lst

#universal build rule
$(OBJS): build/%.o:%.cpp
	g++ -c -o $@ $< $(ARGS)

.PHONY: builddirs clean distclean

builddirs:
	-mkdir -p build/{shared,events,loaders,graphics,physics}

clean:
	-rm -f $(OBJS)

distclean: clean
	-rm -f ../rcx

