#include "joint.hpp"
#include "component.hpp"
#include "../events/event_lists.hpp"

Joint *Joint::head = NULL;

Joint::Joint (dJointID joint, Object *obj): Component(obj)
{
	printlog(2, "configuring Joint class");

	//add it to the list
	next = head;
	head = this;
	prev = NULL;

	if (next)
		next->prev = this;
	else
		printlog(2, "(first registered)");

	//add it to the joint
	dJointSetData (joint, (void*)(this));
	joint_id = joint;

	//default values (currently only event triggering)
	buffer_event = false; //disables event testing
	//TODO: send_to_body option?
	feedback = NULL;
}

//destroys a joint, and removes it from the list
Joint::~Joint ()
{
	//lets just hope the given pointer is ok...
	printlog(2, "clearing Joint class");

	//remove all events
	Buffer_Event_List::Remove(this);

	//1: remove it from the list
	if (!prev) //head in list, change head pointer
	{
		printlog(2, "(is head)");
		head = next;
	}
	else //not head in list, got a previous link to update
		prev->next = next;

	if (next) //not last link in list
		next->prev = prev;
	else
		printlog(2, "(is last)");

	//2: remove it from memory
	if (feedback)
		delete feedback;

	dJointDestroy(joint_id);
}

