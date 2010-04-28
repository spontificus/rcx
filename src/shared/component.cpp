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

#include "component.hpp"
#include "object.hpp"
#include "printlog.hpp"

Component::Component(Object *obj)
{
	printlog(2, "creating Component class");

	//rather simple: just add it to the top of obj list of components
	next = obj->components;
	prev = NULL;
	obj->components = this;

	if (next)
	{
		next->prev = this;
	}
	else
	{
		printlog(2, "(first for object)");
	}

	//keep track of owning object
	object_parent = obj;
}

Component::~Component()
{
	//just unlink...
	printlog(2, "freeing Component");
	if (prev)
	{
		prev->next = next;
	}
	else
	{
		printlog(2, "(is head)");
		object_parent->components = next;
	}

	if (next)
	{
		next->prev = prev;
	}
	else
	{
		printlog(2, "(is tail)");
	}
}

