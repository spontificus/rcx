#include "object.hpp"
#include "printlog.hpp"
#include "track.hpp"


object_struct *object_head = NULL;
//allocate a new object, add it to the list and returns its pointer
object_struct *allocate_object ()
{
	printlog(2, " > allocating object");
	object_struct *object_next = object_head;

	object_head = (object_struct *)malloc(sizeof(object_struct));

	object_head->prev = NULL;
	object_head->next = object_next;

	object_head->space = dHashSpaceCreate(space); //inside world

	if (object_next)
		object_next->prev = object_head;
	else
		printlog(2, " (first registered object)");

	//default values


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

	//2: remove it from memory

	free(target);
}


