#ifndef _RCX_SHARED_H
#define _RCX_SHARED_H
//Shared data structs and typedefs.
//
//See licensing info in main.c

//Required stuff:
//#include <SDL.h>
//#include <SDL_opengl.h>
//#include <ode/ode.h>

//C++ libs
//#include <string>
//#include <vector>
//#include <fstream>

//C libs
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stddef.h>
//#include <stdbool.h>
//#include <unistd.h>


#include "info.hpp"
#include "runlevel.hpp"
#include "../loaders/conf.hpp"
#include "../physics/drag.hpp"
#include "internal.hpp"
#include "file_3d.hpp"
#include "script.hpp"
#include "object.hpp"
#include "geom.hpp"
#include "body.hpp"
#include "joint.hpp"
#include "car.hpp"
#include "camera.hpp"
#include "profile.hpp"
#include "track.hpp"

//TODO: weapons



//prototypes specific for shared data
void free_all (void);


extern script_struct *box; //keep track of our loaded debug box
extern script_struct *sphere;

#endif
