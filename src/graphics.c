//handle drawing of 3d/2d accelerated graphics
//
//See main.c for licensing

//Just in case it's not defined...
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#include "graphics/camera.c"

SDL_Surface *screen;
Uint32 flags = SDL_OPENGL | SDL_RESIZABLE;

//count frames
Uint32 frame_count = 0;

//if multithreading, event thread will alert graphics thread about resizing events (to avoid stealing the context)
bool graphics_event_resize = false;
int graphics_event_resize_w, graphics_event_resize_h;
//

void graphics_resize (int w, int h)
{
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
		printlog(1, "(perspective: %f degrees, based on (your) eye distance: %i pixels\n", angle, internal.dist);
	}
	else //bad...
	{
		angle = internal.angle;
		printlog(1, "Angle forced to: %f degrees. And you are an evil person...\n", angle);
	}

	gluPerspective (angle, (GLdouble) w/h, 1, 1000);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	printlog(1, "\n");
}

int graphics_init(void)
{
	printlog(0, "=> Initiating graphics\n");

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode (internal.res[0], internal.res[1], 0, flags);

	if (!screen)
	{
		printlog(0, "Error: couldn't set video mode\n");
		return -1;
	}

	//hide cursor
	SDL_ShowCursor (SDL_DISABLE);

	//toggle fullscreen (if requested)
	if (internal.fullscreen)
		if (!SDL_WM_ToggleFullScreen(screen))
			printlog(0, "Error: unable to toggle fullscreen\n");

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
	return 0;
}


//render lists, position "camera" (time step not used for now)
void graphics_step (Uint32 step)
{
	//keep track of how many rendered frames
	++frame_count;

	//see if we need to resize
	if (graphics_event_resize)
	{
		//if in a thread, make sure sdl request doesn't collide with other thread
		if (internal.multithread)
		{
			SDL_mutexP(sdl_lock);
			screen = SDL_SetVideoMode (graphics_event_resize_w, graphics_event_resize_h, 0, flags);
			SDL_mutexV(sdl_lock);
		}
		else
			screen = SDL_SetVideoMode (graphics_event_resize_w, graphics_event_resize_h, 0, flags);

		if (screen)
		{
			graphics_resize (screen->w, screen->h);
			graphics_event_resize = false;
		}
		else
			printlog(0, "Warning: resizing failed, will retry\n");
	}

	//start rendering
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glLoadIdentity();

	glPushMatrix();

	//move camera
	camera_graphics_step();

	//place sun
	glLightfv (GL_LIGHT0, GL_POSITION, track.position);

	//render world
	glPushMatrix();
		glCallList (track.file_3d->list);
	glPopMatrix();

	//loop through all geoms, see if they need rendering
	geom_data *geom;
	const dReal *pos, *rot; //store rendering position
	for (geom = geom_data_head; geom; geom = geom->next)
	{
		if (!geom->file_3d) //invisible
			continue;

		glPushMatrix();
			pos = dGeomGetPosition (geom->geom_id);
			rot = dGeomGetRotation (geom->geom_id);


			//create transformation matrix to render correct position
			//and rotation (float)
			GLfloat matrix[16];
			matrix[0]=rot[0];
			matrix[1]=rot[4];
			matrix[2]=rot[8];
			matrix[3]=0;
			matrix[4]=rot[1];
			matrix[5]=rot[5];
			matrix[6]=rot[9];
			matrix[7]=0;
			matrix[8]=rot[2];
			matrix[9]=rot[6];
			matrix[10]=rot[10];
			matrix[11]=0;
			matrix[12]=pos[0];
			matrix[13]=pos[1];
			matrix[14]=pos[2];
			matrix[15]=1;

			glMultMatrixf (matrix);

			//render
			glCallList (geom->file_3d->list);
		glPopMatrix();
	}

	glPopMatrix();

	SDL_GL_SwapBuffers();
}

int graphics_loop ()
{
	printlog(1, "Starting graphics loop\n");
	Uint32 time, time_old;
	time_old = SDL_GetTicks();

	while (runlevel == running)
	{
		time = SDL_GetTicks();

		//SDL_SemWait(ode_lock); //make sure physics is completely simulated before rendering
		graphics_step(time-time_old);
		//SDL_SemPost(ode_lock);

		time_old = time;

		if (internal.graphics_sleep)
			SDL_Delay (internal.graphics_sleep);

		//in case event thread can't pump SDL events (limit of some OSes)
		SDL_mutexP(sdl_lock);
		SDL_PumpEvents();
		SDL_mutexV(sdl_lock);
	}

	return 0;
}

void graphics_quit(void)
{
	printlog(1, "=> Quit graphics\n");
	SDL_Quit();
}

