/*
 * RCX Copyright (C) Slinger
 *
 * This program comes with ABSOLUTELY NO WARRANTY!
 *
 * This is free software, and you are welcome to
 * redistribute it under certain conditions.
 *
 * See license.txt and README for more info
 */

#ifndef _RCX_TIMERS_H
#define _RCX_TIMERS_H

#include "../shared/object.hpp"
#include "../shared/script.hpp"

#include <ode/ode.h>
#include <SDL/SDL_stdinc.h> //Uint32

class Animation_Timer
{
	public:
		Animation_Timer (Object*, Script*, dReal start, dReal stop, dReal duration);
		~Animation_Timer();
		static void Events_Step(Uint32 step);

	private:
		Object *object;
		Script *script;
		dReal counter;
		dReal goal;
		dReal speed;

		Animation_Timer *next, *prev;
		static Animation_Timer *head;
};

#endif
