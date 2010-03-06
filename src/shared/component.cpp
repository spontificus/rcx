#include "component.hpp"
#include "object.hpp"
#include "printlog.hpp"

Component::Component(object_struct *obj)
{
	printlog(2, " > creating Component class");

	//rather simple: just add it to the top of obj list of components
	next = obj->components;
	prev = NULL;

	if (next)
	{
		printlog(2, " (first for object)");
		next->prev = this;
	}

	printlog(2, "\n");
}

Component::~Component()
{
	//just unlink...
	printlog(2, " > freeing component");
	if (prev)
	{
		prev->next = next;
	}
	else
	{
		printlog(2, " (is head)");
		object_parent->components = next;
	}

	if (next)
	{
		next->prev = prev;
	}
	else
	{
		printlog(2, " (is tail)");
	}

	printlog(2, "\n");
}

