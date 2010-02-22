//handle events, both real events like window resizing, termination and
//keyboard, but also respond to simulations (usually when objects collides
//and they are meant to respond to collisions - like building destructions...)
//
//See main.c for licensing

#include "../shared/shared.hpp" //shared (global defined) data
#include <SDL/SDL.h>
#include "../graphics/graphics.hpp" //for window resizing events
#include "../loaders/loaders.hpp" //spawn/destroy events
#include "../shared/runlevel.hpp"
#include "../shared/printlog.hpp"

SDL_Event event;
Uint8 *keys;


//TMP: keep track of demo spawn stuff
script_struct *box = NULL;
script_struct *sphere = NULL;

//prototypes for graphics control
extern bool graphics_event_resize;
extern int graphics_event_resize_w, graphics_event_resize_h;

//mutex
extern SDL_mutex *sdl_mutex;
extern SDL_mutex *ode_mutex;
extern SDL_mutex *sync_mutex;
extern SDL_cond  *sync_cond;

void event_step(Uint32 step)
{
	//loop geoms to see if any event
	geom_data *geom = geom_data_head;
	while (geom)
	{
		if (geom->flipper_geom)
		{
			if (geom->colliding)
			{
				const dReal *pos;
				pos = dGeomGetPosition(geom->flipper_geom);
				dGeomSetPosition(geom->flipper_geom, pos[0], pos[1],
						pos[2]+step*0.02);
				if (++geom->flipper_counter > 10)
					geom->colliding=false;
			}
			else if (geom->flipper_counter)
			{
				const dReal *pos;
				pos = dGeomGetPosition(geom->flipper_geom);
				dGeomSetPosition(geom->flipper_geom, pos[0], pos[1],
						pos[2]-step*0.02);
				--geom->flipper_counter;
			}
		}
		geom=geom->next;
	}

	//loop joints
	joint_data *joint = joint_data_head;
	while (joint)
	{
		if (joint->event)
		{
			//assume the joint should be destroyed
			dJointDestroy (joint->joint_id);
			free_joint_data (joint);
		}
		joint=joint->next;
	}

	//get SDL events
	if (internal.multithread) //make sure not colliding if threading
		SDL_mutexP(sdl_mutex);

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
					printlog(1, "(FIXME: pause when losing focus (or being iconified)!)\n");
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
						spawn_object (box, 0,0,10);
					break;
					case SDLK_F6:
						printlog(1, "(10sec height)\n");
						spawn_object (box ,0,0,489.5);
					break;
					
					case SDLK_F7:
						spawn_object (sphere, 0,0,10);
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
			set_camera_settings (&prof->cam[0]);
		else if (keys[prof->cam2])
			set_camera_settings (&prof->cam[1]);
		else if (keys[prof->cam3])
			set_camera_settings (&prof->cam[2]);
		else if (keys[prof->cam4])
			set_camera_settings (&prof->cam[3]);

		//move camera
		camera_settings *settings = camera.settings;
		if (settings)
		{
			if (keys[prof->cam_x_pos])
				settings->distance[0]+=step*0.03;
			else if (keys[prof->cam_x_neg])
				settings->distance[0]-=step*0.03;
			if (keys[prof->cam_y_pos])
				settings->distance[1]+=step*0.03;
			else if (keys[prof->cam_y_neg])
				settings->distance[1]-=step*0.03;
			if (keys[prof->cam_z_pos])
				settings->distance[2]+=step*0.03;
			else if (keys[prof->cam_z_neg])
				settings->distance[2]-=step*0.03;
		}

		//if selected car, read input
		if (prof->car)
		{
			car_struct *carp = prof->car;
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

	//if we locked, unlock
	if (internal.multithread)
		SDL_mutexV(sdl_mutex);
}

int events_loop (void *d)
{
	printlog(1, "Starting event loop\n");
	//dAllocateODEDataForThread (dAllocateFlagBasicData); //needed for manipulating ode from event thread
	Uint32 time, time_old;
	time_old = SDL_GetTicks();
	while (runlevel == running)
	{
		//if syncing, sleep until physics signals
		if (internal.multithread&&internal.sync_events)
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

