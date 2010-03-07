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
build/main.o: main.cpp ./shared/info.hpp ./shared/internal.hpp ./shared/threads.hpp ./shared/printlog.hpp ./shared/cleanup.hpp ./shared/runlevel.hpp ./shared/profile.hpp ./shared/track.hpp
build/events/events.o: events/events.cpp events/../shared/threads.hpp events/../shared/internal.hpp events/../shared/runlevel.hpp events/../shared/printlog.hpp events/../shared/geom.hpp events/../shared/profile.hpp events/../shared/joint.hpp events/../shared/camera.hpp
build/events/geom.o: events/geom.cpp events/../shared/geom.hpp
build/graphics/camera.o: graphics/camera.cpp graphics/../shared/camera.hpp
build/graphics/geom.o: graphics/geom.cpp graphics/../shared/geom.hpp
build/graphics/graphics.o: graphics/graphics.cpp graphics/../shared/internal.hpp graphics/../shared/info.hpp graphics/../shared/track.hpp graphics/../shared/runlevel.hpp graphics/../shared/threads.hpp graphics/../shared/printlog.hpp graphics/../shared/camera.hpp
build/loaders/car.o: loaders/car.cpp loaders/../shared/car.hpp loaders/../shared/printlog.hpp loaders/../shared/track.hpp loaders/../shared/body.hpp loaders/colours.hpp loaders/debug_draw.hpp
build/loaders/conf.o: loaders/conf.cpp loaders/conf.hpp loaders/text_file.hpp loaders/../shared/printlog.hpp
build/loaders/debug_draw.o: loaders/debug_draw.cpp loaders/debug_draw.hpp loaders/colours.hpp loaders/../shared/printlog.hpp
build/loaders/object.o: loaders/object.cpp loaders/../shared/object.hpp loaders/../shared/printlog.hpp loaders/colours.hpp loaders/../shared/track.hpp loaders/debug_draw.hpp loaders/../shared/joint.hpp loaders/../shared/geom.hpp loaders/../shared/body.hpp
build/loaders/profile.o: loaders/profile.cpp loaders/../shared/profile.hpp loaders/../shared/printlog.hpp loaders/text_file.hpp
build/loaders/text_file.o: loaders/text_file.cpp loaders/text_file.hpp loaders/../shared/printlog.hpp
build/loaders/track.o: loaders/track.cpp loaders/../shared/track.hpp loaders/../shared/camera.hpp loaders/../shared/geom.hpp loaders/../shared/printlog.hpp loaders/../shared/object.hpp loaders/colours.hpp loaders/text_file.hpp loaders/debug_draw.hpp
build/physics/body.o: physics/body.cpp physics/../shared/body.hpp physics/../shared/printlog.hpp physics/../shared/track.hpp physics/../shared/internal.hpp
build/physics/camera.o: physics/camera.cpp physics/../shared/camera.hpp physics/../shared/internal.hpp physics/../shared/track.hpp
build/physics/car.o: physics/car.cpp physics/../shared/car.hpp physics/../shared/internal.hpp
build/physics/joint.o: physics/joint.cpp physics/../shared/joint.hpp physics/../shared/internal.hpp
build/physics/physics.o: physics/physics.cpp physics/../shared/threads.hpp physics/../shared/internal.hpp physics/../shared/runlevel.hpp physics/../shared/track.hpp physics/../shared/printlog.hpp physics/../shared/body.hpp physics/../shared/geom.hpp physics/../shared/camera.hpp physics/../shared/car.hpp physics/../shared/joint.hpp physics/../shared/body.hpp
build/shared/body.o: shared/body.cpp shared/body.hpp shared/internal.hpp shared/body.hpp shared/printlog.hpp
build/shared/camera.o: shared/camera.cpp shared/camera.hpp
build/shared/car.o: shared/car.cpp shared/car.hpp shared/printlog.hpp
build/shared/cleanup.o: shared/cleanup.cpp shared/cleanup.hpp shared/car.hpp shared/profile.hpp shared/geom.hpp shared/body.hpp shared/joint.hpp shared/object.hpp shared/script.hpp shared/file_3d.hpp
build/shared/component.o: shared/component.cpp shared/component.hpp shared/object.hpp shared/printlog.hpp
build/shared/file_3d.o: shared/file_3d.cpp shared/file_3d.hpp shared/printlog.hpp
build/shared/geom.o: shared/geom.cpp shared/geom.hpp shared/printlog.hpp shared/track.hpp shared/internal.hpp shared/../loaders/conf.hpp
build/shared/internal.o: shared/internal.cpp shared/internal.hpp
build/shared/joint.o: shared/joint.cpp shared/joint.hpp
build/shared/object.o: shared/object.cpp shared/object.hpp shared/printlog.hpp shared/track.hpp
build/shared/printlog.o: shared/printlog.cpp shared/internal.hpp shared/printlog.hpp
build/shared/profile.o: shared/profile.cpp shared/profile.hpp
build/shared/runlevel.o: shared/runlevel.cpp shared/runlevel.hpp
build/shared/script.o: shared/script.cpp shared/script.hpp shared/printlog.hpp
build/shared/threads.o: shared/threads.cpp shared/threads.hpp
build/shared/track.o: shared/track.cpp shared/track.hpp
#


#universal build rule
$(OBJS): build/%.o:%.cpp
	g++ -c -o $@ $< $(ARGS)

.PHONY: builddirs clean

builddirs:
	-mkdir -p build/{shared,events,loaders,graphics,physics}

clean:
	-rm -f ../rcx $(OBJS)
