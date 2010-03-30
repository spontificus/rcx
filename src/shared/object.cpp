#include "object.hpp"
#include "printlog.hpp"
#include "track.hpp"
#include "printlog.hpp"

#include <stdlib.h>

//allocate new script storage, and add it to list
//(not used yet, only for storing 3d list pointers...)
Object_Template::Object_Template(const char *name): Racetime_Data(name)
{
	//debug identification bools set to false
	box = false;
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
	printlog(2, "creating Object class");

	prev=NULL;
	next=head;
	head=this;

	if (next)
		next->prev = this;
	else
		printlog(2, "(first registered object)");

	//default values
	components = NULL;
	selected_space = NULL;
}

//destroys an object
Object::~Object()
{
	//lets just hope the given pointer is ok...
	printlog(2, "freeing object");

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
}

//destroys all objects
void Object::Destroy_All()
{
	while (head)
		delete (head);
}
