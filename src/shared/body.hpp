#ifndef _RCX_BODY_H
#define _RCX_BODY_H
#include <ode/ode.h>
#include "script.hpp"
#include "object.hpp"

//body_data: data for body (describes mass and mass positioning), used for:
//currently only for triggering event script (force threshold and event variables)
//as well as simple air/liquid drag simulations
//
//>Dynamically allocated
typedef struct body_data_struct {
	//keep track of the "owning" object
	object_struct *object_parent;
	//geom data bellongs to
	dBodyID body_id;

	//data for drag (air+water friction)
	//values for enabled/disabled drag
	bool use_linear_drag, use_advanced_linear_drag;
	bool use_angular_drag;
	//drag values (must be adjusted to the body mass)
	dReal linear_drag, advanced_linear_drag[3];
	dReal angular_drag;

	dReal threshold; //if allocated forces exceeds, eat buffer
	dReal buffer; //if buffer reaches zero, trigger event
	bool event; //set after each buffer empty
	script_struct *script; //execute on event

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct body_data_struct *prev;
	struct body_data_struct *next;
} body_data;

extern body_data *body_data_head;
body_data *allocate_body_data (dBodyID body, object_struct *obj);
void free_body_data (body_data *target);

//drag
void Body_Data_Set_Linear_Drag (body_data *body, dReal drag);
void Body_Data_Set_Advanced_Linear_Drag (body_data *body, dReal drag_x, dReal drag_y, dReal drag_z);
void Body_Data_Set_Angular_Drag (body_data *body, dReal drag);
void Body_Data_Linear_Drag (body_data *body);
void Body_Data_Advanced_Linear_Drag (body_data *body);
void Body_Data_Angular_Drag (body_data *body);
void body_physics_step (void);

#endif
