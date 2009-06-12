//handle drawing of 3d/2d accelerated graphics
//
//See main.c for licensing

//Just in case it's not defined...
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

SDL_Surface *screen;
GLdouble cpos[3] = {20,-25,20};
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

//new trimesh rendering method, instead of calling rendering list
//TODO: some of this data (vertices) can be moved to video ram, which
//would make the necessary data to send to gpu less (=higher fps)
void render_trimesh (trimesh* target)
{
	//variables for fast looping
	char *inst = target->instructions;
	unsigned int *v_index = target->vector_indices;
	unsigned int *n_index = target->normal_indices;
	unsigned int *mat_index = target->material_indices;
	GLfloat *vertex, *normal; //vectors
	material *mat;
	
	glBegin (GL_TRIANGLES);
	//future instructions might be different, like vertex and normal indices
	//as sepparate (but since they are often specified at the same time,
	//maybe not?)
	while (1)
	{
		if (*inst == 'i') //vertex&normal index
		{
			normal = &target->normals[*n_index];
			glNormal3f (normal[0], normal[1], normal[2]);
			vertex = &target->vertices[*v_index];
			glVertex3f (vertex[0], vertex[1], vertex[2]);

			v_index += 3;
			n_index += 3;

		}
		else if (*inst == 'm') //material "index"
		{
			mat = &target->materials[*mat_index];

			glMaterialfv (GL_FRONT, GL_AMBIENT,mat->ambient);
			glMaterialfv (GL_FRONT, GL_DIFFUSE,mat->diffuse);
			glMaterialfv (GL_FRONT, GL_SPECULAR, mat->specular);
			glMateriali  (GL_FRONT, GL_SHININESS, mat->shininess);

			mat_index += 3;
		}
		else //assumed to be '\0'
			break;

		++inst;
	}
	glEnd();

	//glMaterialfv (GL_FRONT, GL_SPECULAR, black);
}

dReal geom_pos_default[] = {0,-20,5};
//render lists, position "camera" (time step not used for now)
void graphics_step (Uint32 step)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glLoadIdentity();

	glPushMatrix();

	const dReal *gpos;

	if (!focused_car)
		gpos = geom_pos_default; //not focused, use default
	else
		gpos = dBodyGetPosition(focused_car->bodyid);

	gluLookAt (cpos[0],cpos[1],cpos[2], gpos[0],gpos[1],gpos[2], 0,0,1);

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
		if (!geom->file_3d&&!geom->geom_trimesh) //invisible
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
			if (geom->file_3d) //old (obsolete) rendering method
				glCallList (geom->file_3d->list);
			if (geom->geom_trimesh) //new method
				render_trimesh(geom->geom_trimesh);

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

