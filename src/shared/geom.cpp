#include <ode/ode.h>
#include "geom.hpp"
#include "printlog.hpp"
#include "track.hpp"
#include "internal.hpp"
#include "../loaders/conf.hpp"

Geom *Geom::head = NULL;

//allocates a new geom data, returns its pointer (and uppdate its object's count),
//ads it to the component list, and ads the data to specified geom (assumed)
Geom::Geom (dGeomID geom, object_struct *obj): Component(obj) //pass object argument to base class constructor
{
	printlog(2, "configuring Geom class");

	//parent object
	if (obj->selected_space)
	{
		printlog(2, "(adding to selected space)");
		dSpaceAdd (obj->selected_space, geom);
	}
	else //add geom to global space
	{
		dSpaceAdd (space, geom);
	}

	//add it to the geom list
	next = Geom::head;
	Geom::head = this;
	prev = NULL;

	//one more geom after this
	if (next)
		next->prev = this;
	else
		printlog(2, "(first registered)");

	//add it to the geom
	dGeomSetData (geom, (void*)(Geom*)(this));
	geom_id = geom;

	//now lets set some default values...
	//event processing (triggering):
	colliding = false; //no collision event yet
	script = NULL; //nothing to run on collision (yet)
	
	file_3d = NULL; //default, isn't rendered
	//collision contactpoint data
	mu = internal.mu;
	mu_rim = internal.mu;
	erp = internal.erp;
	cfm = internal.cfm;
	slip = internal.slip;

	wheel = false; //not a wheel
	bounce = 0.0; //no bouncyness

	collide = true; //on collision, create opposing forces

	//debug variables
	flipper_geom = 0;
	flipper_counter = 0;

}
//destroys a geom, and removes it from the list
Geom::~Geom ()
{
	//lets just hope the given pointer is ok...
	printlog(2, "clearing Geom class");

	//1: remove it from the list
	if (!prev) //head in list, change head pointer
	{
		printlog(2, "(geom is head)");
		Geom::head = next;
	}
	else //not head in list, got a previous link to update
		prev->next = next;

	if (next) //not last link in list
		next->prev = prev;
	else
		printlog(2, "(geom is last)");

	//TODO: dGeomDestroy(geom_id);

}
