//Just in case...
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

//dGeomID focused_geom = NULL;
SDL_Surface *screen;

//SDL (1.2) can't resize window on some systems (...windowz... OSX...) without destroying OGL context...
#ifdef BADWM
#define flags SDL_OPENGL
#else
#define flags SDL_OPENGL | SDL_RESIZABLE
#endif

void graphics_resize (int w, int h)
{
	glViewport (0,0,w,h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	//lets calculate viewing angle (height) based on the players _real_
	//viewing angle... requires a correct distance from screen instead of
	//this hardcoded one, based on my own head distance (units in pixels)
	//
	//(divide your screens resolution height or width with real height or
	//width, and multiply that with your eyes distance from the screen,
	//then use that value here - instead of 2087)
	//
	//may need some explanation: angle up+down     ratio h/z   rad to angle
	GLdouble angle = (GLdouble)  2*atan((GLdouble) (h/2)/2087) *180/M_PI;
	printf ("(perspective: %f°, assumed (your) eye distance: %i pixels\n", angle, 2087);
	gluPerspective (angle, (GLdouble) w/h, 1, 1000);

//if you lack imagination and originality, use the following instead... :-p
//	gluPerspective (45, (GLdouble) w/h, 1, 100);
	
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

int graphics_init(void)
{
	printf ("-> Initiating graphics\n");
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode (1024, 768, 0, flags);

	#ifdef BADWM
		printf("This build got window resizing disabled, since it might destroy loaded data on some platforms! (windows, osx - SDL 1.2 bug)\n\n");
	#endif

	if (!screen)
	{
		printf ("Error: couldn't set video mode\n");
		return -1;
	}

	//glClearDepth (1.0); pointless to define this?

	glDepthFunc (GL_LESS);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH); //by default, can be changed

	graphics_resize (screen->w, screen->h);

	SDL_WM_SetCaption ("RollCageX v0.04", "rcx");

	//everything ok
	return 0;
}



dReal geom_pos_default[] = {0,-20,5};
//render lists, position "camera"
void graphics_step (void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glLoadIdentity();

	glPushMatrix();

	const dReal *geom_pos;

	if (!car_active)
		geom_pos = geom_pos_default; //not focused, use default
	else
		geom_pos = dGeomGetPosition(car_active->body_geom);

	gluLookAt (40,40,40, geom_pos[0],geom_pos[1],geom_pos[2], 0,0,1);

	//mark bottom
	glBegin (GL_LINES);
	glColor3f (0.0f, 0.0f, 0.0f);
	glVertex3f (-0.4f, 0.4f, 0.1f);
	glVertex3f (0.4f, -0.4f, 0.1f);
	glVertex3f (0.4f, 0.4f, 0.1f);
	glVertex3f (-0.4f, -0.4f, 0.1f);
	glEnd();

	//render world
	glPushMatrix();
		glCallList (track.graphics_list->render_list);
	glPopMatrix();

	//loop through all components, see if they need rendering
	component *cmp = component_head;
	const dReal *pos, *rot; //store rendering position
	while (cmp)
	{
		if (!cmp->graphics_list) //invisible
			break;
                
                


		glPushMatrix();
			pos = dBodyGetPosition (cmp->body_id);
			rot = dBodyGetRotation (cmp->body_id);


			//create transformation matrix to render correct position and rotation (float)
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
                        
                        if (cmp->c != NULL) {
                          render_cube_shadow(cmp->c, matrix);
                        }
                        
                        if (cmp->s != NULL) {
                          render_gen_shadow(cmp->s, matrix);
                        }

			glMultMatrixf (matrix);

			//render (should not require any testing)
			glCallList (cmp->graphics_list->render_list);
                        
                        
		glPopMatrix();
                
                

		//load next component
		cmp = cmp->next;
	}

	glPopMatrix();

	SDL_GL_SwapBuffers();
}

void graphics_loop (void)
{
	printf ("Starting drawing loop\n");
	while (runlevel == running)
		graphics_step();
		//TODO: sleep
}

void graphics_quit(void)
{
	printf ("Quit graphics\n");
	SDL_Quit();
}

