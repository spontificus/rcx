/* RollCageX (fanmade clone of the original RollCage games)
 * Copyright (C) 2009-2010  "Slinger" (on gorcx.net forum)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

//Required stuff:
#include <SDL.h>

//local stuff:
#include "shared/info.hpp"
#include "shared/shared.hpp"
#include "events/events.hpp"
#include "graphics/graphics.hpp"
#include "physics/physics.hpp"
#include "loaders/loaders.hpp"


//TMP: used by events for keeping track of objects spawning
extern script_struct *box; //keep track of our loaded debug box
extern script_struct *sphere;


//keep track of warnings
unsigned int stepsize_warnings = 0;
unsigned int threshold_warnings = 0;

//prototype for some variables
extern Uint32 frame_count; //from graphics

//when multithreading, use mutexes
SDL_mutex *ode_mutex = NULL; //only one thread for ode
SDL_mutex *sdl_mutex = NULL; //only one thread for sdl

SDL_mutex *sync_mutex = NULL; //for using sync_cond
SDL_cond  *sync_cond  = NULL; //threads can sleep until synced
//


//if something goes wrong (after initing physics and graphics)
void emergency_quit (void)
{
	printlog(0, "ERROR REQUIRES GAME TO STOP:\n");
	free_all();
	graphics_quit();
	physics_quit();

	exit (-1);
}

void start_race(void)
{
	Uint32 simtime = SDL_GetTicks(); //set simulated time to realtime
	Uint32 start_time = simtime; //how long it took for race to start

	//singlethread or multi?
	if (internal.multithread)
	{
		printlog (0, "\n-> Starting Race (multithreaded)\n");

		ode_mutex = SDL_CreateMutex(); //create mutex for ode locking
		sdl_mutex = SDL_CreateMutex(); //only use sdl in 1 thread

		sync_mutex = SDL_CreateMutex();
		sync_cond = SDL_CreateCond();

		runlevel  = running;

		//launch threads
		SDL_Thread *physics = SDL_CreateThread (physics_loop, NULL);
		SDL_Thread *events = SDL_CreateThread (events_loop, NULL);
		graphics_loop(); //we already got opengl context in main thread

		//wait for threads
		SDL_WaitThread (events, NULL);
		SDL_WaitThread (physics, NULL);

		//cleanup
		SDL_DestroyMutex(ode_mutex);
		SDL_DestroyMutex(sdl_mutex);
		SDL_DestroyMutex(sync_mutex);
		SDL_DestroyCond(sync_cond);
		//done!

		simtime = SDL_GetTicks(); //set time (for info output)
	}
	else
	{
		Uint32 realtime; //real time (with possible delay since last update)
		Uint32 stepsize_ms = internal.stepsize*1000+0.0001; //calculate stepsize from s to ms (append  0.0001 for correct conversion)

		printlog (0, "\n-> Starting Race (single thread)\n");
		runlevel = running;
		while (runlevel == running)
		{
			event_step(stepsize_ms); //always check for events

			physics_step();

			simtime += stepsize_ms;

			//if realtime is larger than simtime (and graphics threshold)
			if (SDL_GetTicks()+internal.threshold > simtime)
			{
				printlog(2, "\nWarning: simtime less than realtime (to low stepsize), dropping frame..\n\n");
				++stepsize_warnings;
			}
			else //we got time left to draw frame on
			{
				graphics_step(stepsize_ms);

				realtime = SDL_GetTicks();
				if (simtime > realtime)
				{
					SDL_Delay (simtime - realtime);
				}
				else
				{
					printlog(2, "\nWarning: (not sleeping, realtime became to high (to low treshold?))\n");
					++threshold_warnings;
				}
			}
		}
	}

	simtime -= start_time;
	printlog(0, "-> Race done!\n");
	printlog(0, "\n<-- Some basic info: -->\n");
	printlog(0, "(does not interest most people)\n");
	printlog(0, "Startup time (ms):			%i\n", start_time);
	printlog(0, "Race time (ms):				%i\n", simtime);
	printlog(0, "Avarage FPS:				%i\n", (1000*frame_count)/simtime);
	printlog(0, "Threading mode:				");
	if (internal.multithread)
	{
		printlog(0, "Multithreaded (3 threads)\n");
		printlog(0, "Stepsize-too-low (slowdown) warnings:	%i\n", stepsize_warnings);
	}
	else
	{
		printlog(0, "Singlethreaded (1 thread)\n");
		printlog(0, "Graphics-threshold-too-low warnings:	%i\n", threshold_warnings);
		printlog(0, "Stepsize-too-low (framedrop) warnings:	%i\n", stepsize_warnings);
	}
}

//simple demo:
int main (int argc, char *argv[])
{
	//issue
	printf("\n     -=[ Hello, and welcome to RollCageX version %s ]=-\n\n%s\n", VERSION, ISSUE);
	//end

	if (argc != 1)
		printf("(Passing arguments - not supported)\n\n");

	//printlog needs internal.verbosity, set it to default value
	printf("(verbosity level is assumed \"1\" until read from internal conf)\n");
	internal.verbosity = 1;
	//check if program was called with another pwd (got '/' in "name")
	int count;
	for (count = strlen(argv[0]); count != -1; --count)
		if (argv[0][count] == '/')
		{
			char pwd[count+2]; //1 for \0 and 1 for [0]
			strncpy (pwd, argv[0], count+1);
			pwd[count+1] = '\0';
			printf ("(changing pwd: %s)\n", pwd);
			chdir (pwd);
			break;
		}

	if (load_conf ((char *)"data/internal.conf", (char *)&internal, internal_index))
		return -1;

	if (graphics_init())
		return -1;

	//<insert menu here>
	
	profile *prof = load_profile ((char *)"data/profiles/default");
	if (!prof)
		return -1;

	car_struct *venom = load_car((char *)"data/teams/Nemesis/cars/Venom");
	if (!venom)
		emergency_quit();
	prof->car = venom;
	camera.car = venom;

	//menu done, race selected, starting race...
	if (physics_init())
	{
		graphics_quit();
		return -1;
	}

	if (load_track((char *)"data/worlds/Sandbox/tracks/Box"))
		emergency_quit();

	//load box for online spawning
	box = load_object((char *)"data/objects/misc/box");
	sphere = load_object((char *)"data/objects/misc/sphere");
	if (!box)
		emergency_quit();

	spawn_car (venom, track.start[0], track.start[1], track.start[2]);

	//start race
	start_race();

	//race done
	free_all();
	physics_quit();
	
	//<insert menu here>
	
	//menu done, quit selected, ending graphics and terminating program
	graphics_quit();
	
	printlog(0, "\nBye!\n\n");
	return 0;
}

