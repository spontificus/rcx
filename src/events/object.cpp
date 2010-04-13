#include <ode/ode.h>
#include "../shared/object.hpp"
#include "event_lists.hpp"

//temporary geom event processing
void Object::Events_Step()
{
	Object *obj;
	while (Object_Event_List::Get_Event(&obj))
		delete obj;
}
