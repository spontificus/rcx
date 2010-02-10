//create and destroy objects, geoms and bodies
//(should only be used by other abstraction functions (look in loader.c)
//and not directly, except for free_all (to do quick full free)
//
//See main.c for licensing

#include "shared.hpp"
//#include "camera.hpp"


//initiate global variables defined in shared.h
//variables
script_struct *box = NULL;
script_struct *sphere = NULL;

#include "profile.hpp"


/*#include "script.cpp"
#include "object.cpp"
#include "geom.cpp"
#include "body.cpp"
#include "joint.cpp"
#include "profile.cpp"
#include "car.cpp"
#include "file_3d.cpp"*/

//free all things allocated (to simplify memory leak preventing)
//simple dirty way of removing everything
void free_all (void)
{
	printlog(1, "-> Freeing all data\n");
	printlog(2, ">>> TODO: optimize???\n");
	//first thing to destroy: cars
	printlog(2, ">>> TODO: change from free_car to destroy_car?\n\n");

	while (car_head)
		free_car(car_head);

	while (profile_head)
		free_profile(profile_head);

	//take care of object (without calling ode - not needed)
	while (geom_data_head)
		free_geom_data(geom_data_head);
	while (body_data_head)
		free_body_data(body_data_head);
	while (joint_data_head)
		free_joint_data(joint_data_head);
       	
	while (object_head)
		free_object(object_head);

	//only place where scripts and 3d lists are removed
	script_struct *script_tmp = script_head;
	while (script_head)
	{
		script_tmp = script_head->next;
		free (script_head->name);
		free (script_head);
		script_head = script_tmp;
	}

	//destroy loaded 3d files
	file_3d_struct *file_3d_tmp;
	while (file_3d_head)
	{
		file_3d_tmp = file_3d_head; //copy from from list
		file_3d_head = file_3d_head->next; //remove from list

		glDeleteLists (file_3d_tmp->list, 1);
		free (file_3d_tmp);
	}

	//no need to destroy track, since it's not allocated by program
}
