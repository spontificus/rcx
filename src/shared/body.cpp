#include "body.hpp"
#include "internal.hpp"
#include "body.hpp"
#include "printlog.hpp"

body_data *body_data_head = NULL;
body_data *allocate_body_data (dBodyID body, object_struct *obj)
{
	printlog(2, "allocating body_data");
	body_data *tmp_body = (body_data *)malloc(sizeof(body_data));

	//parent object
	tmp_body->object_parent = obj;

	//ad it to the list
	tmp_body->next = body_data_head;
	body_data_head = tmp_body;
	body_data_head->prev = NULL;

	if (body_data_head->next)
		body_data_head->next->prev = body_data_head;
	else
		printlog(2, "(first registered)");

	//add it to the body
	dBodySetData (body, (void*)(body_data*)(body_data_head));
	body_data_head->body_id = body;

	//default values
	Body_Data_Set_Linear_Drag(body_data_head, internal.linear_drag);
	Body_Data_Set_Angular_Drag(body_data_head, internal.angular_drag);

	body_data_head->threshold = 0; //no threshold (disables event testing)
	body_data_head->buffer = 1; //almost empty buffer
	body_data_head->event = false;
	body_data_head->script = NULL;

	return body_data_head;
}

//destroys a body, and removes it from the list
void free_body_data (body_data *target)
{
	//lets just hope the given pointer is ok...
	printlog(2, "freeing body");

	//1: remove it from the list
	if (!target->prev) //head in list, change head pointer
	{
		printlog(2, "(body is head)");
		body_data_head = target->next;
	}
	else //not head in list, got a previous link to update
		target->prev->next = target->next;

	if (target->next) //not last link in list
		target->next->prev = target->prev;
	else
		printlog(2, "(body is last)");

	//2: remove it from memory

	free(target);

}

