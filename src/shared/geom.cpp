#include <ode/ode.h>
#include "geom.hpp"
#include "printlog.hpp"
#include "track.hpp"
#include "internal.hpp"
#include "../loaders/conf.hpp"

geom_data *geom_data_head = NULL;

//allocates a new geom data, returns its pointer (and uppdate its object's count),
//ads it to the component list, and ads the data to specified geom (assumed)
geom_data *allocate_geom_data (dGeomID geom, object_struct *obj)
{
	printlog(2, " > allocating geom_data");
	geom_data *tmp_geom = (geom_data *)malloc(sizeof(geom_data));

	//parent object
	tmp_geom->object_parent = obj;
	printlog(2, " (parent object:");
	if (obj)
	{
		printlog(2, "yes) (space:");
		obj->geom_count += 1;

		if (obj->space)
		{
			printlog(2, "yes)");
			dSpaceAdd (obj->space, geom);
		}
		else
		{
			printlog(2, "no)");
			dSpaceAdd (space, geom);
		}
	}
	else
	{
		printlog(2, "no)");
		dSpaceAdd (space, geom);
	}

	//add it to the list
	tmp_geom->next = geom_data_head;
	geom_data_head = tmp_geom;
	geom_data_head->prev = NULL;

	if (geom_data_head->next)
		geom_data_head->next->prev = geom_data_head;
	else
		printlog(2, " (first registered)");

	//add it to the geom
	dGeomSetData (geom, (void*)(geom_data*)(geom_data_head));
	geom_data_head->geom_id = geom;

	//now lets set some default values...
	//event processing (triggering):
	geom_data_head->colliding = false; //no collision event yet
	geom_data_head->script = NULL; //nothing to run on collision (yet)
	
	geom_data_head->file_3d = NULL; //default, isn't rendered
	//collision contactpoint data
	geom_data_head->mu = internal.mu;
	geom_data_head->mu_rim = internal.mu;
	geom_data_head->erp = internal.erp;
	geom_data_head->cfm = internal.cfm;
	geom_data_head->slip = internal.slip;

	geom_data_head->wheel = false; //not a wheel
	geom_data_head->bounce = 0.0; //no bouncyness

	geom_data_head->collide = true; //on collision, create opposing forces

	//debug variables
	geom_data_head->flipper_geom = 0;
	geom_data_head->flipper_counter = 0;

	printlog (2, "\n");
	return geom_data_head;
}
//destroys a geom, and removes it from the list
void free_geom_data(geom_data *target)
{
	//lets just hope the given pointer is ok...
	printlog(2, " > freeing geom");

	//1: remove it from the list
	if (!target->prev) //head in list, change head pointer
	{
		printlog(2, " (geom is head)");
		geom_data_head = target->next;
	}
	else //not head in list, got a previous link to update
		target->prev->next = target->next;

	if (target->next) //not last link in list
		target->next->prev = target->prev;
	else
		printlog(2, " (geom is last)");

	//2: remove it from memory

	free(target);

	printlog (2, "\n");
}

