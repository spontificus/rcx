#include "joint.hpp"
#include "component.hpp"

Joint *Joint::head = NULL;

Joint::Joint (dJointID joint, Object *obj): Component(obj)
{
	printlog(2, "configuring Joint class");

	//parent object
	object_parent = obj;

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
	threshold = 0; //no threshold (disables event testing)
	buffer = 1; //almost empty
	event = false;
	script = NULL;
	feedback = NULL;
}

//destroys a joint, and removes it from the list
Joint::~Joint ()
{
	//lets just hope the given pointer is ok...
	printlog(2, "clearing Joint class");

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

