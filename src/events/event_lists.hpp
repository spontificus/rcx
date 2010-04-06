#ifndef _RCX_EVENT_LISTS
#define _RCX_EVENT_LISTS
//definitions of components that causes/uses events
#include "../shared/component.hpp"
#include "../shared/geom.hpp"
#include "../shared/body.hpp"
#include "../shared/joint.hpp"

class Event_Lists
{
	public:
		//for adding events
		Event_Lists(Geom*);
		Event_Lists(Body*);
		Event_Lists(Joint*);

		//for processing (read and remove) events
		static bool Get_Event(Geom**);
		static bool Get_Event(Body**);
		static bool Get_Event(Joint**);

		//for directly removing/ignoring events for a component
		static void Remove(Geom*);
		static void Remove(Body*);
		static void Remove(Joint*);
		//TODO: one function for removing from all lists? Maybe for future stuff

	private:
		Component *component; //the one thing to keep track of

		//different lists (by heads) for different components
		static Event_Lists *geom_head;
		static Event_Lists *body_head;
		static Event_Lists *joint_head;

		//next in list
		Event_Lists *next;
};

#endif
