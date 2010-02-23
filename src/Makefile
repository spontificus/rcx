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
build/main.o: ./shared/info.hpp ./shared/shared.hpp ./events/events.hpp ./graphics/graphics.hpp ./physics/physics.hpp ./loaders/loaders.hpp
build/events/events.o: events/../shared/shared.hpp events/../graphics/graphics.hpp events/../loaders/loaders.hpp events/../shared/runlevel.hpp events/../shared/printlog.hpp
build/graphics/camera.o: graphics/../shared/camera.hpp
build/graphics/graphics.o: graphics/../shared/shared.hpp graphics/../shared/printlog.hpp graphics/../shared/camera.hpp
build/loaders/car.o: loaders/../shared/car.hpp loaders/../shared/printlog.hpp loaders/../shared/track.hpp loaders/../physics/drag.hpp loaders/colours.hpp loaders/debug_draw.hpp
build/loaders/conf.o: loaders/../shared/shared.hpp loaders/loaders.hpp loaders/conf.hpp loaders/text_file.hpp loaders/../shared/printlog.hpp
build/loaders/debug_draw.o: loaders/debug_draw.hpp loaders/colours.hpp loaders/../shared/printlog.hpp
build/loaders/object.o: loaders/../shared/object.hpp loaders/../shared/printlog.hpp loaders/colours.hpp loaders/../shared/track.hpp loaders/debug_draw.hpp loaders/../shared/joint.hpp loaders/../shared/geom.hpp loaders/../shared/body.hpp
build/loaders/profile.o: loaders/../shared/shared.hpp loaders/../shared/profile.hpp loaders/../shared/printlog.hpp loaders/text_file.hpp loaders/loaders.hpp
build/loaders/text_file.o: loaders/../shared/shared.hpp loaders/loaders.hpp loaders/text_file.hpp loaders/../shared/printlog.hpp
build/loaders/track.o: loaders/../shared/shared.hpp loaders/../shared/track.hpp loaders/../shared/camera.hpp loaders/../shared/geom.hpp loaders/../shared/printlog.hpp loaders/../shared/object.hpp loaders/colours.hpp loaders/text_file.hpp loaders/debug_draw.hpp loaders/loaders.hpp
build/physics/camera.o: physics/../shared/camera.hpp physics/../shared/shared.hpp
build/physics/drag.o: physics/../shared/shared.hpp
build/physics/physics.o: physics/../shared/shared.hpp physics/../shared/printlog.hpp physics/physics.hpp physics/drag.hpp
build/shared/body.o: shared/internal.hpp shared/body.hpp shared/printlog.hpp shared/../physics/drag.hpp
build/shared/camera.o: shared/shared.hpp
build/shared/car.o: shared/car.hpp shared/printlog.hpp
build/shared/cleanup.o: shared/cleanup.hpp shared/car.hpp shared/profile.hpp shared/geom.hpp shared/body.hpp shared/joint.hpp shared/object.hpp shared/script.hpp shared/file_3d.hpp
build/shared/file_3d.o: shared/file_3d.hpp shared/printlog.hpp
build/shared/geom.o: shared/geom.hpp shared/printlog.hpp shared/track.hpp shared/internal.hpp shared/../loaders/conf.hpp
build/shared/internal.o: shared/internal.hpp
build/shared/joint.o: shared/joint.hpp
build/shared/object.o: shared/object.hpp shared/printlog.hpp shared/track.hpp
build/shared/printlog.o: shared/internal.hpp shared/printlog.hpp
build/shared/profile.o: shared/profile.hpp
build/shared/runlevel.o: shared/runlevel.hpp
build/shared/script.o: shared/script.hpp shared/printlog.hpp
build/shared/track.o: shared/track.hpp
#


#universal build rule
$(OBJS): build/%.o:%.cpp
	g++ -c -o $@ $< $(ARGS)

.PHONY: builddirs clean

builddirs:
	-mkdir -p build/{shared,events,loaders,graphics,physics}

clean:
	-rm -f ../rcx $(OBJS)
