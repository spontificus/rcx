/* RollCageX v0.04 (fanmade clone of the original RollCage games)
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


//Required stuff:
#include <SDL.h>
#include <SDL_opengl.h>
#include <ode/ode.h>

#include <stdbool.h>

//local stuff:
#include "shared.h" //custom data definitions

script *debug_box; //keep track of our loaded debug box
//the cars we'll load
car *car_one;
car *car_two;
car *car_three;
car *car_four;

car *car_active = NULL;

#include "graphics.c"
#include "physics.c"
#include "shared.c" //functions for handling custom data
#include "loaders.c" //loading functions for confs, tracks, cars, etc...
#include "events.c"  //responds to events both OS- and game simulation related

//if something goes wrong (after initing physics and graphics)
void emergency_quit (void)
{
	printf("can't continue, must stop\n");
	free_all();
	graphics_quit();
	physics_quit();

	exit -1;
}

//simple demo:
int main (int argc, char *argv[])
{
	if (argc != 1)
		printf("(Passing arguments - not supported)\n");

	printf("\n    -=[ Hello, and welcome to RollCageX version 0.04 ]=-\n\n");
	printf("...as its version number indicates, do not expect too much out of it ;-)\n\n");
	printf(" Copyright (C) 2009, This program comes with ABSOLUTELY NO WARRANTY; see\n \"license.txt\" for details\n\n");
	printf(" This is free software, and you are welcome to redistribute it and/or modify it\n under the GNU General Public License as published by the Free Software\n Foundation, version 3 (or at your option) later\n\n");

	printf("= Credits (all nicknames refers to usernames on the gorcx.net forum):\n");
	printf("    \"MaAkaJon\"\t\tproject creator\n");
	printf("    \"Soul Slinger\"\tcoder (created this and the earlier versions)\n\n");

	printf("* Projects that made RCX possible:\n");
	printf("    GNU\t\t\t\tdefines computer freedom itself... :-)\n");
	printf("    Simple DirectMedia Layer\twindow handling, input/outputs\n");
	printf("    Open Dynamics Engine\tphysics\n");
	printf("= End of credits\n\n");

	printf(" Controlls:\n");
	printf("	>	Arrow keys:	Steering and throttling\n");
	printf("	>	Spacebar:	Drifting break\n");
	printf("	>	Ctrl and Alt:	Soft breaks\n");
	printf("	>	F1 to F4:	switch between cars\n");
	printf("	>	F5:		spawn box\n");
	printf("	>	F6:		spawn box (10s above ground)\n\n");
	printf(" (The cars got wheel diameters ranging from 1m to 4m)\n\n");

	if (graphics_init())
		return -1;

	//<insert menu here>
	
	//menu done, race selected, starting race...
	if (physics_init())
	{
		graphics_quit();
		return -1;
	}

	printf("\n");

	if (load_world_debug())
		emergency_quit();

	debug_box = load_object_debug();
	if (!debug_box)
		emergency_quit();

	car_one = load_car_debug(1); //car with 1m wheel diameter
	car_two = load_car_debug(2); //car with 2m wheel diameter
	car_three = load_car_debug(3); //car with 3m wheel diameter
	car_four = load_car_debug(4); //car with 4m wheel diameter

	if (!(car_one&&car_two&&car_three&&car_four))
	{
		printf("one or more of the cars did not load correctly\n");
		emergency_quit();
	}

	spawn_car_debug (car_one, 20,-15,1.5);
	spawn_car_debug (car_two, 12,-15,2);
	spawn_car_debug (car_three, -2,-15,2.5);
	spawn_car_debug (car_four, -20,-15,3);

	//spawn "humans" (two 1m boxes on each others)
	spawn_object_debug(debug_box, 17,-15,0.5);
	spawn_object_debug(debug_box, 17,-15,1.5);
	spawn_object_debug(debug_box, 6,-15,0.5);
	spawn_object_debug(debug_box, 6,-15,1.5);
	spawn_object_debug(debug_box, -11,-15,0.5);
	spawn_object_debug(debug_box, -11,-15,1.5);
	//spawn the only loaded object (the box) once (over the ground)

	//single-thread function
	//WARNING: Don't run the game constantly for more than around 49 days!
	//(or the realtime will wrap, and the timing solution will go crazy)

	Uint32 simtime = SDL_GetTicks(); //set simulated time to realtime
	Uint32 realtime; //real time (with possible delay since last update)

	//TODO: graphic_step treshold
	runlevel = running;
	while (runlevel == running)
	{
		event_step(); //always check for events

		physics_step();
		simtime += stepsize*1000;

		if (SDL_GetTicks() > simtime) //if realtime is larger than simtime
			printf("\nWarning: simtime less than realtime (to low stepsize), dropping frame..\n\n");
		else //we got time left to draw frame on
		{
			graphics_step();

			//times are unsigned, in order to prevent realtime to become larger than
			//simtime just between the test and sleep (and cause a gigantic sleep)
			realtime = SDL_GetTicks();
			if (simtime > realtime)
			{
				SDL_Delay (simtime - realtime);
			}
			else
				printf("\nWarning: (not sleeping, realtime became to high (to low treshold))\n");
		}
	}

	free_all();
	physics_quit();
	//race done
	
	//<insert menu here>
	
	printf("Race done, quit graphics\n");
	//menu done, quit selected, ending graphics and terminating program
	graphics_quit();
	printf("Bye!\n\n");
	return 0;
}

