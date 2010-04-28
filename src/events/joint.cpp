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

#include <SDL/SDL.h>
#include <ode/ode.h>
#include "../shared/joint.hpp"
#include "../events/event_lists.hpp"

void Joint::TMP_Events_Step(Uint32 step)
{
	//loop joints
	Joint *joint;
	while (Buffer_Event_List::Get_Event(&joint))
	{
		//assume the joint should be destroyed
		delete joint;
	}
}

