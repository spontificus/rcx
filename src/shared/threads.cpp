#include "threads.hpp"

//when multithreading, use mutexes
SDL_mutex *ode_mutex = NULL; //only one thread for ode
SDL_mutex *sdl_mutex = NULL; //only one thread for sdl

SDL_mutex *sync_mutex = NULL; //for using sync_cond
SDL_cond  *sync_cond  = NULL; //threads can sleep until synced
//


