#include "cleanup.hpp"

#include "car.hpp"
#include "profile.hpp"
#include "geom.hpp"
#include "body.hpp"
#include "joint.hpp"
#include "object.hpp"
#include "script.hpp"
#include "file_3d.hpp"

//free all things allocated (to simplify memory leak preventing)
//simple dirty way of removing everything
void free_all (void)
{
	printlog(1, "Freeing all data");
	printlog(2, "TODO: optimize???");
	//first thing to destroy: cars
	printlog(2, "TODO: change from free_car to destroy_car?");

	while (car_head)
		free_car(car_head);

	while (profile_head)
		free_profile(profile_head);

	//TODO: Object::Destroy_All() (which calls Component->Destroy_All() for included components)
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
