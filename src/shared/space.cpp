/*
 * RCX Copyright (C) Slinger
 *
 * This program comes with ABSOLUTELY NO WARRANTY!
 *
 * This is free software, and you are welcome to
 * redistribute it under certain conditions.
 *
 * See license.txt and README for more info
 */

#include "space.hpp"
#include "geom.hpp"
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

	Geom *g;

	while (dSpaceGetNumGeoms(space_id)) //while contains geoms
	{
		//remove first geom - next time first will be the next geom
		g = (Geom*)dGeomGetData(dSpaceGetGeom(space_id, 0));
		delete g;
	}

	dSpaceDestroy(space_id);
}
