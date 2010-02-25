#ifndef _RCX_JOINT_H
#define _RCX_JOINT_H
#include <ode/ode.h>
#include "object.hpp"
#include "script.hpp"
#include "printlog.hpp"

//joint_data: data for joint (connects bodies), is used for:
//currently only for triggering event script (force threshold)
//
//>Dynamically allocated
typedef struct joint_data_struct {
	//keep track of the "owning" object
	object_struct * object_parent;
	//geom data bellongs to
	dJointID joint_id;

	dJointFeedback feedback; //used if checking forces
	dReal threshold; //if force on body exceeds threshold, eat buffer
	dReal buffer; //if buffer reaches zero, trigger event
	bool event; //if event triggered, run script
	script_struct *script; //the script to run

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct joint_data_struct *prev;
	struct joint_data_struct *next;
} joint_data;

extern joint_data *joint_data_head;
joint_data *allocate_joint_data (dJointID joint, object_struct *obj, bool feedback);
void free_joint_data (joint_data *target);
void joint_physics_step (void);

#endif
