//handle drawing of 3d/2d accelerated graphics
//
//See main.c for licensing

//Just in case it's not defined...
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

SDL_Surface *screen;
Uint32 flags = SDL_OPENGL;

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

	//SDL (1.2) can't resize window on some systems (...windowz... OSX...)
	//without destroying OGL context... only enable resizing if requested
	if (internal.resize)
		flags |= SDL_RESIZABLE;

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

void graphics_camera(Uint32 step)
{
	car_struct *car = camera.car;
	camera_settings *settings = camera.settings;

	if (car && settings) //do some magic ;-)
	{
		//move camera
		camera.pos[0] += camera.vel[0]*step/1000;
		camera.pos[1] += camera.vel[1]*step/1000;
		camera.pos[2] += camera.vel[2]*step/1000;

		//get position of target, simple
		dVector3 target;
		dBodyGetRelPointPos (car->bodyid, settings->target[0], settings->target[1], settings->target[2]*car->dir, target);



		//smart/soft camera movement, fun
		dVector3 tmp;
		dReal accel[3];

		//relative position and velocity
		dBodyGetRelPointPos (car->bodyid, settings->position[0], settings->position[1], settings->position[2]*car->dir, tmp);
		dReal pos[3] = {tmp[0]-camera.pos[0], tmp[1]-camera.pos[1], tmp[2]-camera.pos[2]};
		dBodyGetRelPointVel (car->bodyid, settings->position[0], settings->position[1], settings->position[2]*car->dir, tmp);
		dReal vel[3] = {tmp[0]-camera.vel[0], tmp[1]-camera.vel[1], tmp[2]-camera.vel[2]};

		dReal pos_l = sqrt(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]); //length
		dReal pos_n[3] = {pos[0]/pos_l, pos[1]/pos_l, pos[2]/pos_l}; //normalized

		//find point on length that gives projection by velocity on length
		dReal p = (pos_n[0]*vel[0]+pos_n[1]*vel[1]+pos_n[2]*vel[2]);
		dReal v[3] = {p*pos_n[0], p*pos_n[1], p*pos_n[2]}; //vector

		//remove the movement in the desired axis from total, and add breaking
		accel[0] = (vel[0]-v[0])*settings->accel_tweak;
		accel[1] = (vel[1]-v[1])*settings->accel_tweak;
		accel[2] = (vel[2]-v[2])*settings->accel_tweak;

		//deceleration over distance > current velocity (=keep on accelerating)
		if (sqrt(2*pos_l*settings->accel_max) > -p)
		{
			accel[0] += pos_n[0]*settings->accel_max;
			accel[1] += pos_n[1]*settings->accel_max;
			accel[2] += pos_n[2]*settings->accel_max;
		}
		else //we need to break
		{
			//dReal acceleration= (p*p)/(2*pos_l);
			accel[0] -= pos_n[0]*settings->accel_max;
			accel[1] -= pos_n[1]*settings->accel_max;
			accel[2] -= pos_n[2]*settings->accel_max;
		}

		//see if wanted acceleration is above accepted max
		dReal accel_l = sqrt(accel[0]*accel[0]+accel[1]*accel[1]+accel[2]*accel[2]);

		dReal change = (settings->accel_max/accel_l);
		if (change < 1)
		{
			accel[0] *= change;
			accel[1] *= change;
			accel[2] *= change;
		}

		//now add acceleration
		camera.vel[0] += accel[0];
		camera.vel[1] += accel[1];
		camera.vel[2] += accel[2];


		//set camera
		gluLookAt(camera.pos[0], camera.pos[1], camera.pos[2], target[0], target[1], target[2], 0,0,1);
	}
	else
		gluLookAt (10, -10, 10, 0,0,0, 0,0,1);
}


//render lists, position "camera" (time step not used for now)
void graphics_step (Uint32 step)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glLoadIdentity();

	glPushMatrix();

	//move camera
	graphics_camera(step);

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

void graphics_loop (void)
{
	printlog(1, "Starting drawing loop\n");
	Uint32 time, time_old;
	time_old = SDL_GetTicks();
	while (runlevel == running)
	{
		time = SDL_GetTicks();
		graphics_step(time-time_old);
		//TODO: sleep?
	}
}

void graphics_quit(void)
{
	printlog(1, "=> Quit graphics\n");
	SDL_Quit();
}

