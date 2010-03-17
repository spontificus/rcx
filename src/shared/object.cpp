#include "object.hpp"
#include "printlog.hpp"
#include "track.hpp"


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
