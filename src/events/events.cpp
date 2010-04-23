//handle events, both real events like window resizing, termination and
//keyboard, but also respond to simulations (usually when objects collides
//and they are meant to respond to collisions - like building destructions...)
//
//See main.c for licensing

#include <SDL/SDL.h>
#include <ode/ode.h>
#include "../shared/threads.hpp"
#include "../shared/internal.hpp"
#include "../shared/runlevel.hpp"
#include "../shared/printlog.hpp"
#include "../shared/geom.hpp"
#include "../shared/profile.hpp"
#include "../shared/joint.hpp"
#include "../shared/camera.hpp"
#include "timers.hpp"



//TMP: keep track of demo spawn stuff
Object_Template *box = NULL;
Object_Template *sphere = NULL;
Object_Template *funbox = NULL;
Car *Venom1, *Venom2;


int events_loop (void *d)
{
	printlog(1, "Starting event loop");

	SDL_Event event;
	Uint32 time, time_old, delta;
	time_old = SDL_GetTicks();

	while (runlevel == running)
	{
		//if syncing, sleep until physics signals
		if (internal.sync_events)
		{
			SDL_mutexP(sync_mutex);
			SDL_CondWaitTimeout (sync_cond, sync_mutex, 500); //if no signal in half a second, stop waiting
			SDL_mutexV(sync_mutex);
		}

		//wait for permission for ode (in case some event causes ode manipulation)
		SDL_mutexP(ode_mutex);

		time = SDL_GetTicks();
		delta = time-time_old;

		//process events
		Geom::TMP_Events_Step(delta);
		Joint::TMP_Events_Step(delta);
		Body::TMP_Events_Step(delta);

		Object::Events_Step(); //remove inactive objects

		//get SDL events
		SDL_mutexP(sdl_mutex); //make sure not colliding with other threads

		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_VIDEORESIZE:
					graphics_event_resize_w = event.resize.w;
					graphics_event_resize_h = event.resize.h;
					graphics_event_resize = true;
				break;

				case SDL_QUIT:
					runlevel = done;
				break;

				case SDL_ACTIVEEVENT:
					if (event.active.gain == 0)
						printlog(1, "(FIXME: pause when losing focus (or being iconified)!)");
				break;
				//check for special key presses
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							runlevel = done;
						break;

						//box spawning
						case SDLK_F5:
							box->Spawn (0,0,10);
						break;

						//sphere spawning
						case SDLK_F6:
							sphere->Spawn (0,0,10);
						break;

						//spawn funbox
						case SDLK_F7:
							funbox->Spawn (0,0,10);
						break;

						//tmp: switch cars
						case SDLK_F8:
							if (profile_head->car == Venom1)
							{
								profile_head->car = Venom2;
								camera.car = Venom2;
							}
							else
							{
								profile_head->car = Venom1;
								camera.car = Venom1;
							}
						break;

						default:
							break;
					}
				break;

				default:
					break;
			}
		}

		Profile_Events_Step(delta);


		//unlock sdl access
		SDL_mutexV(sdl_mutex);

		//timers
		Animation_Timer::Events_Step(delta);


		//done
		SDL_mutexV(ode_mutex);

		time_old = time;
	}
	return 0;
}

