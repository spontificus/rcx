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

#include "object.hpp"
#include "printlog.hpp"
#include "track.hpp"
#include "printlog.hpp"
#include "../events/event_lists.hpp"

#include <stdlib.h>

//allocate new script storage, and add it to list
//(not used yet, only for storing 3d list pointers...)
Object_Template::Object_Template(const char *name): Racetime_Data(name)
{
	//debug identification bools set to false
	box = false;
	funbox = false;
	flipper = false;
	NH4 = false;
	building = false;
	sphere = false;
	pillar = false;
}


Object *Object::head = NULL;

//allocate a new object, add it to the list and returns its pointer
Object::Object ()
{
	printlog(1, "creating Object");

	prev=NULL;
	next=head;
	head=this;

	if (next)
		next->prev = this;
	else
		printlog(2, "(first registered object)");

	//default values
	components = NULL;
	activity = 0;
	selected_space = NULL;
}

//destroys an object
Object::~Object()
{
	//lets just hope the given pointer is ok...
	printlog(1, "freeing Object");

	//1: remove it from the list
	if (prev == NULL) //first link
	{
		printlog(2, "(object is head)");
		head = next;
	}
	else
		prev->next = next;

	if (next) //not last link
		next->prev = prev;
	else
		printlog(2, "(object is last)");


	//remove components
	while (components)
		delete components; //just removes the one in top each time

	//make sure no events for this object is left
	Object_Event_List::Remove(this);
}

void Object::Increase_Activity()
{
	++activity;
}

void Object::Decrease_Activity()
{
	if ((--activity) == 0)
	{
		printlog(2, "Object became inactive, generating event");
		new Object_Event_List(this);
	}
}

//destroys all objects
void Object::Destroy_All()
{
	while (head)
		delete (head);
}
