//handle events, both real events like window resizing, termination and
//keyboard, but also respond to simulations (usually when objects collides
//and they are meant to respond to collisions - like building destructions...)

//debug
bool spawning = false;
//end


SDL_Event event;
Uint8 *keys;

void event_step(void)
{
	//get SDL events
	while (SDL_PollEvent (&event))
	{
		switch (event.type)
		{
			case SDL_VIDEORESIZE:
			screen = SDL_SetVideoMode (event.resize.w, event.resize.h, 0, flags); //this is a resize event, assume we can resize (set flag)
			graphics_resize (screen->w, screen->h);
			break;

			case SDL_QUIT:
			runlevel = done;
			break;
		}
	}

	//get keys pressed
	keys = SDL_GetKeyState(NULL);

	if (keys[SDLK_ESCAPE])
		runlevel = done;

	if (keys[SDLK_F5])
	{
		if (spawning == false)
		{
			spawn_object_debug (debug_box ,0,0,10);
			spawning = true;
		}
	}
	else if (keys[SDLK_F6])
	{
		if (spawning == false)
		{
			printf("(10sec height)\n");
			spawn_object_debug (debug_box ,0,0,489.5);
			spawning = true;
		}
	}
	else
		spawning = false;

	//select car
	if (keys[SDLK_F1])
		car_active = car_one;
	else if (keys[SDLK_F2])
		car_active = car_two;
	else if (keys[SDLK_F3])
		car_active = car_three;
	else if (keys[SDLK_F4])
		car_active = car_four;
		
	//if selected car, read input
	if (car_active)
	{
		if (keys[SDLK_SPACE])
		{
			car_active->drift_breaks = true;
			car_active->breaks = false;
		}
		//Alt and Ctrl activates "softer" breaks...
		else if (keys[SDLK_LALT]||keys[SDLK_RALT]||keys[SDLK_LCTRL]||keys[SDLK_RCTRL])
		{
			car_active->breaks = true;
			car_active->drift_breaks = false;
		}
		else
		{
			car_active->drift_breaks = false;
			car_active->breaks = false;
		}

		if (keys[SDLK_UP])
			car_active->throttle = 1;
		else if (keys[SDLK_DOWN])
			car_active->throttle = -1;
		else
			car_active->throttle = 0; //motor off

		if (keys[SDLK_LEFT]&&!keys[SDLK_RIGHT])
			car_active->steering = -1;
		else if (!keys[SDLK_LEFT]&&keys[SDLK_RIGHT])
			car_active->steering = 1;
		else
			car_active->steering = 0;
	}
}

void event_loop (void)
{
	printf("Starting een loop\n");
	while (runlevel == running)
		event_step();
		//TODO: sleep?
}

