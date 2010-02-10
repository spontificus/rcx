#ifndef _RCX_OBJECT_H
#define _RCX_OBJECT_H
#include "ode/common.h"

//object: one "thing" on the track, from a complex building to a tree, spawning
//will be controlled by a custom scripting langue in future versions, the most
//important role of "object" is to store the ode space for the spawned object
//(keeps track of the geoms in ode that describes the components) and joint
//group (for cleaning up object)
//
//>Dynamically allocated
typedef struct object_struct {
	//things to keep track of when cleaning out object
	//all geoms can be queried from space, all bodies from geoms and all
	//joints from bodies
	dJointGroupID jointgroup; //store all joints (if needed)
	dSpaceID space; //store all geoms
	bool collide_space; //if the internal geoms should collide with themselves

	//keep track of owned things (to get fast way of auto-removing empty objs)
	unsigned int geom_count;
	unsigned int body_count;
//	unsigned int joint_count;
	//placeholder for more data
	
	//used to find next/prev object in dynamically allocated chain
	//set next to null in last object in chain
	struct object_struct *prev;
	struct object_struct *next;
} object_struct;

extern object_struct *object_head;


void free_object(object_struct *target);
object_struct *allocate_object (bool adspace, bool adjointgroup);
#endif
