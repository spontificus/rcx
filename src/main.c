/* RollCageX (fanmade clone of the original RollCage games)
 * Copyright (C) 2009  "Soul Slinger" (on gorcx.net forum)
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


#define VERSION "0.05" //supports alphanumeric versioning

//Required stuff:
#include <SDL.h>
#include <SDL_opengl.h>
#include <ode/ode.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

//local stuff:
#include "shared.h" //custom data definitions

//
car_struct *venom;
script_struct *box; //keep track of our loaded debug box
script_struct *sphere;
car_struct *focused_car = NULL;

void printlog (int, const char*, ...); //prototype (for included functions

//keep track of warnings
unsigned int stepsize_warnings = 0;
unsigned int threshold_warnings = 0;

//when multithreading, use semaphores
SDL_sem *ode_lock = NULL; //only one thread for ode
SDL_sem *sdl_lock = NULL; //only one thread for sdl
//


#include "graphics.c"
#include "physics.c"
#include "shared.c" //functions for handling custom data
#include "loaders.c" //loading functions for confs, tracks, cars, etc...
#include "events.c"  //responds to events both OS- and game simulation related

//print log message - if it's below or equal to the current verbosity level
void printlog (int level, const char *text, ...)
{
	if (level <= internal.verbosity)
	{
		va_list list;
		va_start (list, text);
		vprintf (text, list);
		va_end (list);
	}
}

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
		ode_lock = SDL_CreateSemaphore(1); //create semaphore for ode locking (1 thread)
		sdl_lock = SDL_CreateSemaphore(1); //only use sdl in 1 thread
		runlevel = running;

		//launch threads
		SDL_Thread *physics = SDL_CreateThread (physics_loop, NULL);
		SDL_Thread *events = SDL_CreateThread (events_loop, NULL);
		graphics_loop(); //we already got opengl context in main thread

		//wait for threads
		SDL_WaitThread (events, NULL);
		SDL_WaitThread (physics, NULL);

		SDL_DestroySemaphore(ode_lock);
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
	printf("\n    -=[ Hello, and welcome to RollCageX version %s ]=-\n\n", VERSION);
	printf(" Copyright (C) 2009, This program comes with ABSOLUTELY NO WARRANTY; see\n \"license.txt\" for details\n\n");
	printf(" This is free software, and you are welcome to redistribute it and/or modify\n it under the GNU General Public License as published by the Free Software\n Foundation, version 3 or (at your option) later\n\n");

	printf("= Credits (nicknames refers to usernames on the gorcx.net forum):\n");
	printf("    \"MaAkaJon\"\t\tproject creator\n");
	printf("    \"Soul Slinger\"\tcoder (created this and the earlier versions)\n");
	printf("    All of you on the planetrollcage.com and gorcx.net forums!\n\n");

	printf("* Projects that made RCX possible:\n");
	printf("    GNU\t\t\t\tdefines computer freedom itself... :-)\n");
	printf("    Simple DirectMedia Layer\twindow handling, input/outputs\n");
	printf("    Open Dynamics Engine\trigid body dynamics\n");
	printf("= End of credits\n\n");

	printf(" Default controls (can be changed in profile):\n");
	printf("	>	Arrow keys:	Steering and throttling\n");
	printf("	>	Spacebar:	Drifting break\n");
	printf("	>	Left Alt:	Soft breaks\n");
	printf("	>	Q and E:	change camera distance along Z axis\n");
	printf("	>	A and D:	change camera distance along X axis\n");
	printf("	>	W and S:	change camera distance along Y axis\n");
	printf("	>	F1 to F4:	change camera settings\n");
	printf("	>	F5:		spawn box\n");
	printf("	>	F6:		spawn box (10s above ground)\n\n");
	//end of issue

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

	venom = load_car((char *)"data/teams/Nemesis/cars/Venom");
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
	focused_car = venom;

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

