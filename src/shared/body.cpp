#include "body.hpp"
#include "object.hpp"
#include "internal.hpp"
#include "printlog.hpp"
#include "../events/event_lists.hpp"

Body *Body::head = NULL;

Body::Body (dBodyID body, Object *obj): Component(obj)
{
	printlog(2, "configuring Body class");

	//ad it to the list
	next = head;
	head = this;
	prev = NULL;

	if (next)
		next->prev = this;
	else
		printlog(2, "(first registered)");

	//add it to the body
	dBodySetData (body, (void*)(this));
	body_id = body;

	//default values
	Set_Linear_Drag(internal.linear_drag);
	Set_Angular_Drag(internal.angular_drag);

	threshold = 0; //no threshold (disables event testing)
	buffer = 1; //almost empty buffer
	event = false;
	script = NULL;
}

//destroys a body, and removes it from the list
Body::~Body()
{
	//lets just hope the given pointer is ok...
	printlog(2, "clearing Body class");

	//remove all events
	Event_Lists::Remove(this);

	//1: remove it from the list
	if (!prev) //head in list, change head pointer
	{
		printlog(2, "(body is head)");
		head = next;
	}
	else //not head in list, got a previous link to update
		prev->next = next;

	if (next) //not last link in list
		next->prev = prev;
	else
		printlog(2, "(body is last)");

	//2: remove it from memory

	dBodyDestroy(body_id);
}

