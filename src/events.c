//handle events, both real events like window resizing, termination and
//keyboard, but also respond to simulations (usually when objects collides
//and they are meant to respond to collisions - like building destructions...)
//
//See main.c for licensing

SDL_Event event;
Uint8 *keys;

void event_step(Uint32 step)
{
	//loop geoms to see if any event
	geom_data *geom = geom_data_head;
	struct turd_struct *t;
	
	while (geom)
	{
		if (geom->flipper_geom)
		{
			if (geom->event)
			{
				const dReal *pos;
				pos = dGeomGetPosition(geom->flipper_geom);
				dGeomSetPosition(geom->flipper_geom, pos[0], pos[1],
						pos[2]+step*0.02);
				if (++geom->flipper_counter > 10)
					geom->event=false;
			}
			else if (geom->flipper_counter)
			{
				const dReal *pos;
				pos = dGeomGetPosition(geom->flipper_geom);
				dGeomSetPosition(geom->flipper_geom, pos[0], pos[1],
						pos[2]-step*0.02);
				--geom->flipper_counter;
			}
		}
		geom=geom->next;
	}

	//loop joints
	joint_data *joint = joint_data_head;
	while (joint)
	{
		if (joint->event)
		{
			//assume the joint should be destroyed
			dJointDestroy (joint->joint_id);
			free_joint_data (joint);
		}
		joint=joint->next;
	}

	//get SDL events
	while (SDL_PollEvent (&event))
	{
		switch (event.type)
		{
			case SDL_VIDEORESIZE:
				screen = SDL_SetVideoMode (event.resize.w, event.resize.h, 0, flags);
				graphics_resize (screen->w, screen->h);
			break;

			case SDL_QUIT:
				runlevel = done;
			break;

			case SDL_ACTIVEEVENT:
				if (event.active.gain == 0)
					printlog(1, "(FIXME: pause when losing focus (or being iconified)!)\n");
			break;
			//check for special key presses
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						runlevel = done;
					break;

					//box spawning
					case SDLK_F5:
						spawn_object (box, 0,0,10);
					break;
					case SDLK_F6:
						printlog(1, "(10sec height)\n");
						spawn_object (box ,0,0,489.5);
					break;
					
					case SDLK_TAB:
						editing = 1 - editing;
						if ( editing == 0 ) {
							// leaving editing mode, update ODE
							calcTrimesh( edit_h );
						}
						
						break;
						
					// switch between editing angles and position
					case SDLK_KP_MULTIPLY:
						if ( edit_m == 0 ) {
							edit_m = 1;
							printf("Editing Coords\n");
						} else {
							edit_m = 0;
							printf("Editing Angles\n");
						}
						break;
					
					// x movement and rotation
					case SDLK_KP6:
						if (edit_m) {
							edit_t->x++;
						} else {
							edit_t->a += 15;
						}
						print_matrix( edit_t->m );
						recalcTurd( edit_h );
						break;
						
					case SDLK_KP4:
						if (edit_m) {
							edit_t->x--;
						} else {
							edit_t->a -= 15;
						}
						print_matrix( edit_t->m );
						recalcTurd( edit_h );
						break;
					
					// y movement and rotation
					case SDLK_KP8:
						if (edit_m) {
							edit_t->y++;
						} else {
							edit_t->b += 15;
						}
						recalcTurd( edit_h );
						break;
						
					case SDLK_KP2:
						if (edit_m) {
							edit_t->y--;
						} else {
							edit_t->b -= 15;
						}
						recalcTurd( edit_h );
						break;
					
					// z movement and rotation
					case SDLK_KP7:
						if (edit_m) {
							edit_t->z++;
						} else {
							edit_t->c += 15;
						}
						recalcTurd( edit_h );
						break;
						
					case SDLK_KP1:
						if (edit_m) {
							edit_t->z--;
						} else {
							edit_t->c -= 15;
						}
						recalcTurd( edit_h );
						break;
						
					// backup option
					case SDLK_KP5:
						t_restore( edit_t );
						recalcTurd( edit_h );
						break;
						
					// go between different nodes
					case SDLK_PAGEUP:
						if (edit_t->nxt != NULL) {
							edit_t = edit_t->nxt;
							t_backup( edit_t );
						}
						break;
						
					case SDLK_PAGEDOWN:
						if (edit_t->pre != NULL) {
							edit_t = edit_t->pre;
							t_backup( edit_t );
						}
						break;

					case SDLK_HOME:
						if (edit_t->l != NULL) {
							edit_t = edit_t->l;
							t_backup( edit_t );
						}
						break;
						
					case SDLK_END:
						if (edit_t->r != NULL) {
							edit_t = edit_t->r;
							t_backup( edit_t );
						}
						break;
					
					// insert and delete
					case SDLK_INSERT:
						break;
						
					case SDLK_DELETE:
						break;
					
					// printout current file
					case SDLK_KP_ENTER:
						t = edit_h;
						while (t) {
							printf("%f %f %f %f %f %f\n", t->x/15, t->y/15, t->z/15, t->a, t->b, t->c);
							t = t->nxt;
						}
						break;

					default:
						break;
				}
			break;

			default:
				break;
		}
	}

	profile *prof;
	for (prof=profile_head; prof; prof=prof->next)
	{
		//get keys pressed
		keys = SDL_GetKeyState(NULL);

		float step_m = 0.1;

		//move camera
		if (keys[prof->cam_x_pos])
			cpos[0]+=step*step_m;
		else if (keys[prof->cam_x_neg])
			cpos[0]-=step*step_m;
		if (keys[prof->cam_y_pos])
			cpos[1]+=step*step_m;
		else if (keys[prof->cam_y_neg])
			cpos[1]-=step*step_m;
		if (keys[prof->cam_z_pos])
			cpos[2]+=step*step_m;
		else if (keys[prof->cam_z_neg])
			cpos[2]-=step*step_m;

		//if selected car, read input
		if (prof->car)
		{
			car *carp = prof->car;
			if (keys[prof->drift_break])
			{
				carp->drift_breaks = true;
				carp->breaks = false;
			}
			//Alt and Ctrl activates "softer" breaks...
			else if (keys[prof->soft_break])
			{
				carp->breaks = true;
				carp->drift_breaks = false;
			}
			else
			{
				carp->drift_breaks = false;
				carp->breaks = false;
			}

			dReal t_speed = prof->throttle_speed*step;
			if (keys[prof->up])
			{
				carp->throttle += t_speed;
				if (carp->throttle > 1.0)
					carp->throttle = 1.0;
			}
			else if (keys[SDLK_DOWN])
			{
				carp->throttle -= t_speed;
				if (carp->throttle < -1.0)
					carp->throttle = -1.0;
			}
			else
			{
				if (carp->throttle <= t_speed &&
					carp->throttle >= -(t_speed))
					carp->throttle = 0.0;

				else if (carp->throttle > 0.0)
				    carp->throttle -= t_speed;

				else
				    carp->throttle += t_speed;
			}

			t_speed = prof->steer_speed*step;
			dReal max = prof->steer_max;
			if (keys[prof->left]&&!keys[prof->right])
			{
				carp->steering -= t_speed;
				if (carp->steering < -max)
					carp->steering = -max;
			}
			else if (!keys[prof->left]&&keys[prof->right])
			{
				carp->steering += t_speed;
				if (carp->steering > max)
					carp->steering = max;
			}
			else //center
			{
				if (carp->steering <= t_speed &&
				    carp->steering >= -(t_speed))
					carp->steering = 0.0;

				else if (carp->steering > 0.0)
				    carp->steering -= t_speed;

				else
				    carp->steering += t_speed;
			}
		}
	}
}

void event_loop (void)
{
	printlog(1, "Starting even loop\n");
	Uint32 time, time_old;
	time_old = SDL_GetTicks();
	while (runlevel == running)
	{
		time = SDL_GetTicks();
		event_step(time-time_old);
		time_old = time;
	}
}

