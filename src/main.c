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
#include "main.h" //custom data definitions



#include "graphics.h"
#include "physics.h"
#include "shared.h" //functions for handling custom data
#include "loaders.h" //loading functions for confs, tracks, cars, etc...
#include "events.h"  //responds to events both OS- and game simulation related
#include "trimesh.h" //trimesh class for ODE
#include "turd.h"

//keep track of warnings
unsigned int stepsize_warnings = 0;
unsigned int threshold_warnings = 0;

struct internal_struct internal;
struct track_struct track;

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
	printlog(0, "WTF?!\n");
}

void initCol(GLfloat *c, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = a;
}

GLfloat black[4]; // = nothing for lights
GLfloat dgray[4];
GLfloat gray[4];
GLfloat lgray[4];
GLfloat white[4];
GLfloat red[4];
GLfloat green[4];
GLfloat lgreen[4];
GLfloat blue[4];
GLfloat lblue[4];
GLfloat yellow[4];

struct joint_data_struct *joint_data_head;
struct body_data_struct *body_data_head;
struct geom_data_struct *geom_data_head;
struct object_struct *object_head;
struct script_struct *script_head;
struct file_3d_struct *file_3d_head;
struct car_struct *car_head;
int editing;
int cam_mode;

car_struct *venom;
script_struct *box; //keep track of our loaded debug box
car_struct *focused_car;

SDL_Surface *screen;
GLdouble cpos[3];
GLdouble ecpos[3];
Uint32 flags;
turd_struct *turd_head;
turd_struct *edit_t;
turd_struct *edit_h;
turd_struct edit_b;
int edit_m;

dWorldID world;
dSpaceID space;
dJointGroupID contactgroup;
profile *profile_head;
GLuint tex_ch;

//simple demo:
int main (int argc, char *argv[])
{
	joint_data_head = NULL;
	body_data_head = NULL;
	geom_data_head = NULL;
	object_head = NULL;
	script_head = NULL;
	file_3d_head = NULL;
	car_head = NULL;
	focused_car = NULL;
	editing = 0;
	
	initCol(black, 0.0f, 0.0f, 0.0f, 1.0f); // = nothing for lights
	initCol(dgray, 0.2f, 0.2f, 0.2f, 1.0f);
	initCol(gray, 0.5f, 0.5f, 0.5f, 1.0f);
	initCol(lgray, 0.8f, 0.8f, 0.8f, 1.0f);
	initCol(white, 1.0f, 1.0f, 1.0f, 1.0f);
	initCol(red, 1.0f, 0.0f, 0.0f, 1.0f);
	initCol(green, 0.0f, 1.0f, 0.0f, 1.0f);
	initCol(lgreen, 0.4f, 1.0f, 0.4f, 1.0f);
	initCol(blue, 0.0f, 0.0f, 1.0f, 1.0f);
	initCol(lblue, 0.6f, 0.6f, 1.0f, 1.0f);
	initCol(yellow, 1.0f, 1.0f, 0.0f, 1.0f);
	
	cpos[0] = 0;
	cpos[1] = -100;
	cpos[2] = 30;
	
	ecpos[0] = 0;
	ecpos[1] = -100;
	ecpos[2] = 30;

	flags = SDL_OPENGL;
	
	turd_head = NULL;
	edit_t = NULL;
	edit_h = NULL;
	edit_m = 1;
	

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
	printf("	>	Q and E		move camera along Z axis\n");
	printf("	>	A and D		move camera along X axis\n");
	printf("	>	W and S		move camera along Y axis\n");
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
	if (!box)
		emergency_quit();

	spawn_car (venom, track.start[0], track.start[1], track.start[2]);
	focused_car = venom;

	//single-thread function
	//WARNING: Don't run the game constantly for more than around 49 days!
	//(or the realtime will wrap, and the timing solution will go crazy)

	Uint32 simtime = SDL_GetTicks(); //set simulated time to realtime
	Uint32 realtime; //real time (with possible delay since last update)
	Uint32 stepsize_ms = internal.stepsize*1000;

	printlog (0, "\n-> Starting Race\n");
	runlevel = running;
	while (runlevel == running)
	{
		event_step(stepsize_ms); //always check for events

		if ( editing == 0 ) {
			physics_step();
		}

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
	printlog(0, "-> Race done!\n");

	free_all();
	physics_quit();
	//race done
	
	//<insert menu here>
	
	//menu done, quit selected, ending graphics and terminating program
	graphics_quit();

	printlog(0, "\n<-- Some basic info: -->\n");
	printlog(0, "(does not interest most people)\n");
	printlog(0, "Race time (ms):				%i\n", simtime);
	printlog(0, "Stepsize-to-low (framedrop) warnings:	%i\n", stepsize_warnings);
	printlog(0, "Graphics-threshold-to-low warnings:	%i\n", threshold_warnings);

	printlog(0, "\nBye!\n\n");
	return 0;
}

