#include <SDL/SDL.h>
#include <ode/ode.h>
#include "../shared/body.hpp"
#include "event_lists.hpp"
#include "../shared/printlog.hpp"

void Body::TMP_Events_Step(Uint32 step)
{
	Body *body;
	Geom *geom;

	while (Buffer_Event_List::Get_Event(&body))
	{
		printlog(0, "TODO: \"buffer event\" processing for Body");

		//ok, this is _really_ uggly...
		//ode lacks a "dBodyGetGeom" routine (why?! it's easy to implement!)...

		delete body;
	}
}
