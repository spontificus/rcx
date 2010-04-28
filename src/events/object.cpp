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

#include <ode/ode.h>
#include "../shared/object.hpp"
#include "event_lists.hpp"

//temporary geom event processing
void Object::Events_Step()
{
	Object *obj;
	while (Object_Event_List::Get_Event(&obj))
		delete obj;
}
