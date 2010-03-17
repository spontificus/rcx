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
#include "shared/internal.hpp"
#include "shared/threads.hpp"
#include "shared/printlog.hpp"
#include "shared/cleanup.hpp"
#include "shared/runlevel.hpp"
#include "shared/profile.hpp"
#include "shared/track.hpp"


//if something goes wrong (after initing physics and graphics)
void emergency_quit (void)
{
	printlog(0, "ERROR REQUIRES GAME TO STOP:\n");
	free_all();
	graphics_quit();
	physics_quit();

	exit (-1);
}

Uint32 start_time = 0;
void start_race(void)
{
	Uint32 simtime = SDL_GetTicks(); //set simulated time to realtime
	start_time = simtime; //how long it took for race to start

	//start
	printlog (0, "Starting Race (multithreaded)");

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
}

void print_info()
{
	Uint32 uptime = SDL_GetTicks();
	uptime -= start_time;
	printlog(0, "Race Done!");
	printlog(1, "<-- Some basic info: -->");
	printlog(1, "(does not interest most people)");
	printlog(1, "Startup time (ms):				%i", start_time);
	printlog(1, "Race time (ms):				%i", uptime);
	printlog(1, "Threading mode:				%i threads", 3);
	printlog(1, "Avarage FPS:					%i", (1000*frame_count)/uptime);
	printlog(1, "Stepsize-too-low (slowdown) warnings:	%i", stepsize_warnings);
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

	printlog(0, "Loading...\n");

	if (load_conf ("data/internal.conf", (char *)&internal, internal_index))
		return -1;

	if (graphics_init())
		return -1;

	//<insert menu here>
	
	profile *prof = load_profile ("data/profiles/default");
	if (!prof)
		return -1;

	car_struct *venom = load_car("data/teams/Nemesis/cars/Venom");
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
	if (!box || !sphere)
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
	
	//some basic info
	print_info();

	printf("\nBye!\n\n");
	return 0;
}

