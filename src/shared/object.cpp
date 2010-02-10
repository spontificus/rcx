#include "object.hpp"
#include "printlog.hpp"
#include "track.hpp"


object_struct *object_head = NULL;
//allocate a new object, add it to the list and returns its pointer
object_struct *allocate_object (bool adspace, bool adjointgroup)
{
	printlog(2, " > allocating object");
	object_struct *object_next = object_head;

	object_head = (object_struct *)malloc(sizeof(object_struct));

	object_head->prev = NULL;
	object_head->next = object_next;

	//set space and jointgroup to NULL?
	printlog(2, " (create space:");
	if (adspace)
	{
		printlog(2, "yes)");
		object_head->space = dHashSpaceCreate(space); //inside world
	}
	else
	{
		printlog(2, "no)");
		object_head->space = NULL;
	}

	printlog(2, " (create jointgroup:");
	if (adjointgroup)
	{
		printlog(2, "yes)");
		object_head->jointgroup = dJointGroupCreate(0); //always 0
	}
	else
	{
		printlog(2, "no)");
		object_head->jointgroup = NULL;
	}

	if (object_next)
		object_next->prev = object_head;
	else
		printlog(2, " (first registered object)");

	//default values
	object_head->geom_count = 0;//nothing bellongs to object (yet)
	object_head->body_count = 0;
//	object_head->joint_count = 0;


	object_head->collide_space = false; //objects components doesn't collide with each other

	printlog (2, "\n");
	return object_head;
}
//destroys an object
void free_object(object_struct *target)
{
	//lets just hope the given pointer is ok...
	printlog(2, " > freeing object");

	//1: remove it from the list
	if (target->prev == NULL) //first link
	{
		printlog(2, " (object is head)");
		object_head = target->next;
	}
	else
		target->prev->next = target->next;

	if (target->next) //not last link
		target->next->prev = target->prev;
	else
		printlog(2, " (object is last)");


	printlog (2, "\n");

	//check if has jointgroup
	if (target->jointgroup)
	{
		printlog(2, "FIXME: free_object - detected jointgroup, destroying\n\n");
		dJointGroupDestroy (target->jointgroup);
	}

	//2: remove it from memory

	free(target);
}


