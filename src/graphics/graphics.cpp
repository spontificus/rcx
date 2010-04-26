//handle drawing of 3d/2d accelerated graphics
//
//See main.c for licensing

#include <SDL/SDL.h>
#include <GL/glu.h>

#include "../shared/internal.hpp"
#include "../shared/info.hpp"
#include "../shared/track.hpp"
#include "../shared/runlevel.hpp"
#include "../shared/threads.hpp"
#include "../shared/printlog.hpp"

//Just in case it's not defined...
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#include "../shared/camera.hpp"

SDL_Surface *screen;
Uint32 flags = SDL_OPENGL | SDL_RESIZABLE;

//count frames
Uint32 frame_count = 0;

//if multithreading, event thread will alert graphics thread about resizing events (to avoid stealing the context)
bool graphics_event_resize = false;
int graphics_event_resize_w, graphics_event_resize_h;
//


void graphics_resize (int new_w, int new_h)
{
	screen = SDL_SetVideoMode (new_w, new_h, 0, flags);
	int w=screen->w;
	int h=screen->h;

	glViewport (0,0,w,h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	//lets calculate viewing angle (height) based on the players _real_
	//viewing angle... the player should specify an eye_distance in internal
	//conf
	//
	//(divide your screens resolution height or width with real height or
	//width, and multiply that with your eyes distance from the screen,
	//then use that value here - instead of 2087)
	//
	GLdouble angle;
		
	if (!internal.force) //good
	{
		//some explanation: angle up+down      ratio h/z        rad to angle
		angle = (GLdouble) 2*atan((GLdouble) (h/2)/internal.dist) *180/M_PI;
		printlog(1, "(perspective: %f degrees, based on (your) eye distance: %i pixels", angle, internal.dist);
	}
	else //bad...
	{
		angle = internal.angle;
		printlog(1, "Angle forced to: %f degrees. And you are an evil person...", angle);
	}

	gluPerspective (angle, (GLdouble) w/h, 1, 1000);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

bool graphics_init(void)
{
	printlog(0, "Initiating graphics");

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode (internal.res[0], internal.res[1], 0, flags);

	if (!screen)
	{
		printlog(0, "Error: couldn't set video mode");
		return -1;
	}

	//hide cursor
	SDL_ShowCursor (SDL_DISABLE);

	//toggle fullscreen (if requested)
	if (internal.fullscreen)
		if (!SDL_WM_ToggleFullScreen(screen))
			printlog(0, "Error: unable to toggle fullscreen");

	//glClearDepth (1.0); pointless to define this?

	glDepthFunc (GL_LESS);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH); //by default, can be changed

	graphics_resize (screen->w, screen->h);

	char *name = (char *)calloc(10+strlen(VERSION)+1, sizeof(char));
	strcpy (name,"RollCageX ");
	strcat (name,VERSION);

	SDL_WM_SetCaption (name, "RCX");

	free (name);

	//everything ok
	return true;
}



int graphics_loop ()
{
	printlog(1, "Starting graphics loop");

	while (runlevel == running)
	{
		//make sure only render frame after it's been simulated
		//quckly lock mutex in order to listen to physics broadcasts
		if (internal.sync_graphics)
		{
			SDL_mutexP(sync_mutex);
			SDL_CondWaitTimeout (sync_cond, sync_mutex, 500); //if no signal in half a second, stop waiting
			SDL_mutexV(sync_mutex);
		}

		//keep track of how many rendered frames
		++frame_count;

		//see if we need to resize
		if (graphics_event_resize)
		{
			//make sure sdl request doesn't collide with other thread
			SDL_mutexP(sdl_mutex);
			screen = SDL_SetVideoMode (graphics_event_resize_w, graphics_event_resize_h, 0, flags);
			SDL_mutexV(sdl_mutex);

			if (screen)
			{
				graphics_resize (screen->w, screen->h);
				graphics_event_resize = false;
			}
			else
				printlog(0, "Warning: resizing failed, will retry");
		}

		//start rendering
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	glLoadIdentity();

		glPushMatrix();

		//move camera
		camera.Graphics_Step();

		//place sun
		glLightfv (GL_LIGHT0, GL_POSITION, track.position);

		//render world
		glPushMatrix();
			glCallList (track.f_3d->list);
		glPopMatrix();

		Geom::Graphics_Step();


		glPopMatrix();

		SDL_GL_SwapBuffers();

		//in case event thread can't pump SDL events (limit of some OSes)
		SDL_mutexP(sdl_mutex);
		SDL_PumpEvents();
		SDL_mutexV(sdl_mutex);
	}

	return 0;
}

void graphics_quit(void)
{
	printlog(1, "Quit graphics");
	SDL_Quit();
}

