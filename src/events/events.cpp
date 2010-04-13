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

SDL_Event event;
Uint8 *keys;


//TMP: keep track of demo spawn stuff
Object_Template *box = NULL;
Object_Template *sphere = NULL;


void event_step(Uint32 step)
{
	//loop geoms to see if any event
	Geom::TMP_Events_Step(step);
	Joint::TMP_Events_Step(step);
	Body::TMP_Events_Step(step);

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
					case SDLK_F6:
						printlog(1, "(10sec height)");
						box->Spawn (0,0,489.5);
					break;
					
					case SDLK_F7:
						sphere->Spawn (0,0,10);
					break;

					default:
						break;
				}
			break;

			default:
				break;
		}
	}

	profile *prof;
	for (prof=profile_head; prof; prof=prof->next)
	{
		//get keys pressed
		keys = SDL_GetKeyState(NULL);


		//set camera settings
		if (keys[prof->cam1])
			camera.Set_Settings (&prof->cam[0]);
		else if (keys[prof->cam2])
			camera.Set_Settings (&prof->cam[1]);
		else if (keys[prof->cam3])
			camera.Set_Settings (&prof->cam[2]);
		else if (keys[prof->cam4])
			camera.Set_Settings (&prof->cam[3]);

		//move camera
		if (keys[prof->cam_x_pos]) //x
			camera.Move(+(step*0.03), 0, 0);
		if (keys[prof->cam_x_neg])
			camera.Move(-(step*0.03), 0, 0);

		if (keys[prof->cam_y_pos]) //y
			camera.Move(0, +(step*0.03), 0);
		if (keys[prof->cam_y_neg])
			camera.Move(0, -(step*0.03), 0);

		if (keys[prof->cam_z_pos]) //z
			camera.Move(0, 0, +(step*0.03));
		if (keys[prof->cam_z_neg])
			camera.Move(0, 0, -(step*0.03));

		//if selected car, read input
		if (prof->car)
		{
			Car *carp = prof->car;
			if (keys[prof->drift_break])
			{
				carp->drift_breaks = true;
				carp->breaks = false;
			}
			//Alt and Ctrl activates "softer" breaks...
			else if (keys[prof->soft_break])
			{
				carp->breaks = true;
				carp->drift_breaks = false;
			}
			else
			{
				carp->drift_breaks = false;
				carp->breaks = false;
			}

			dReal t_speed = prof->throttle_speed*step;
			if (keys[prof->up])
			{
				carp->throttle += t_speed;
				if (carp->throttle > 1.0)
					carp->throttle = 1.0;
			}
			else if (keys[SDLK_DOWN])
			{
				carp->throttle -= t_speed;
				if (carp->throttle < -1.0)
					carp->throttle = -1.0;
			}
			else
			{
				if (carp->throttle <= t_speed &&
					carp->throttle >= -(t_speed))
					carp->throttle = 0.0;

				else if (carp->throttle > 0.0)
				    carp->throttle -= t_speed;

				else
				    carp->throttle += t_speed;
			}

			t_speed = prof->steer_speed*step;
			dReal max = prof->steer_max;
			if (keys[prof->left]&&!keys[prof->right])
			{
				carp->steering -= t_speed;
				if (carp->steering < -max)
					carp->steering = -max;
			}
			else if (!keys[prof->left]&&keys[prof->right])
			{
				carp->steering += t_speed;
				if (carp->steering > max)
					carp->steering = max;
			}
			else //center
			{
				if (carp->steering <= t_speed &&
				    carp->steering >= -(t_speed))
					carp->steering = 0.0;

				else if (carp->steering > 0.0)
				    carp->steering -= t_speed;

				else
				    carp->steering += t_speed;
			}
		}
	}

	//unlock sdl access
	SDL_mutexV(sdl_mutex);
}

int events_loop (void *d)
{
	printlog(1, "Starting event loop");
	//dAllocateODEDataForThread (dAllocateFlagBasicData); //needed for manipulating ode from event thread
	Uint32 time, time_old;
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
		event_step(time-time_old);

		SDL_mutexV(ode_mutex);

		time_old = time;
	}
	return 0;
}

