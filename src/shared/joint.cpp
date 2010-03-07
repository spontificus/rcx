#include "joint.hpp"

joint_data *joint_data_head = NULL;
joint_data *allocate_joint_data (dJointID joint, object_struct *obj, bool feedback)
{
	printlog(2, "allocating joint_data");
	bool warn = false;
	joint_data *tmp_joint = (joint_data *)malloc(sizeof(joint_data));

	//parent object
	tmp_joint->object_parent = obj;
	if (obj)
	{
		printlog(2, "(parent object)");
//		obj->joint_count += 1;

		warn = true;
	}
	else
		printlog(2, "WARNING: no parent object!");

	//add it to the list
	tmp_joint->next = joint_data_head;
	joint_data_head = tmp_joint;
	joint_data_head->prev = NULL;

	if (joint_data_head->next)
		joint_data_head->next->prev = joint_data_head;
	else
		printlog(2, "(first registered)");

	//add it to the joint
	dJointSetData (joint, (void*)(joint_data*)(joint_data_head));
	joint_data_head->joint_id = joint;

	//default values (currently only event triggering)
	joint_data_head->threshold = 0; //no threshold (disables event testing)
	joint_data_head->buffer = 1; //almost empty
	joint_data_head->event = false;
	joint_data_head->script = NULL;

	if (feedback)
	{
		printlog(2, "(with feedback)");
		dJointSetFeedback (joint, &(joint_data_head->feedback));
	}


	if (warn)
		printlog(2, "Warning: can't change jointgroup (specify when creating!)");

	return joint_data_head;
}


//destroys a joint, and removes it from the list
void free_joint_data (joint_data *target)
{
	//lets just hope the given pointer is ok...
	printlog(2, "freeing joint");

	//1: remove it from the list
	if (!target->prev) //head in list, change head pointer
	{
		printlog(2, "(is head)");
		joint_data_head = target->next;
	}
	else //not head in list, got a previous link to update
		target->prev->next = target->next;

	if (target->next) //not last link in list
		target->next->prev = target->prev;
	else
		printlog(2, "(is last)");

	//2: remove it from memory

	free(target);

}

