#include "space.hpp"
#include "printlog.hpp"
#include "track.hpp"
#include <ode/ode.h>

Space::Space(Object *obj): Component(obj)
{
	printlog(2, "configuring Space class");

	space_id = dSimpleSpaceCreate(space);

	printlog(2, "(autoselecting this as default space for object)");
	obj->selected_space=space_id;
}

Space::~Space()
{
	printlog(2, "clearing Geom class");

	printlog(0, "WARNING: not removing from simulation...\n");
	//TODO: for (i=0; i<space_id.size; ++i); delete Geom; done
	//TODO: dSpaceDestroy(space_id);
}
