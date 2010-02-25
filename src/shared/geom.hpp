#ifndef _RCX_GEOM_H
#define _RCX_GEOM_H
#include "object.hpp"
#include "file_3d.hpp"
#include "script.hpp"

//geom_data: data for geometrical shape (for collision detection), for: 
//contactpoint generation (friction and saftness/hardness). also contains
//rendering data for geom
//
//(contains boolean variable indicating collision - for triggering event script)
//
//>Dynamically allocated
typedef struct geom_data_struct {
	//keep track of the "owning" object
	object_struct * object_parent;
	//geom data bellongs to
	dGeomID geom_id;

	file_3d_struct *file_3d; //points to 3d list, or NULL if invisible

	//Physics data:
	//placeholder for more physics data
	dReal mu, mu_rim, erp, cfm, slip, bounce;

	bool wheel; //true if wheel side slip and connected to hinge2
	dJointID hinge2;

	//End of physics data
	
	bool collide; //create physical collision when touching other components

	bool colliding; //set after each collision
	script_struct *script; //script to execute when colliding (NULL if not used)

	//debug variables
	dGeomID flipper_geom;
	int flipper_counter;

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct geom_data_struct *prev;
	struct geom_data_struct *next;
} geom_data;

extern geom_data *geom_data_head; //points at the first component in chain
geom_data *allocate_geom_data (dGeomID geom, object_struct *obj);
void free_geom_data(geom_data *target);
void geom_graphics_step();

#endif
