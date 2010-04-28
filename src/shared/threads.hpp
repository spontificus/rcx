/*
 * RCX Copyright (C) Slinger
 *
 * This program comes with ABSOLUTELY NO WARRANTY!
 *
 * This is free software, and you are welcome to
 * redistribute it under certain conditions.
 *
 * See license.txt and README for more info
 */

#ifndef _RCX_THREADS_H
#define _RCX_THREADS_H

#include <SDL/SDL_mutex.h>

//mutexes
extern SDL_mutex *ode_mutex;
extern SDL_mutex *sdl_mutex;
extern SDL_mutex *sync_mutex;
extern SDL_cond  *sync_cond;
//


//prototypes for communication/statistic variables
extern unsigned int stepsize_warnings;
extern Uint32 frame_count;
extern bool graphics_event_resize;
extern int graphics_event_resize_w, graphics_event_resize_h;

//functions
bool graphics_init(void);
void graphics_quit(void);
bool physics_init(void);
void physics_quit (void);

int graphics_loop (void);
int events_loop (void *d);
int physics_loop (void *d);



//TMP: used by events for keeping track of objects spawning
#include "object.hpp"
#include "car.hpp"
extern Object_Template *box; //keep track of our loaded debug box
extern Object_Template *sphere;
extern Object_Template *funbox;
extern Car *Venom1;
extern Car *Venom2;

#endif
