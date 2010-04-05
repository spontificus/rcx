#ifndef _RCX_EVENT_LISTS
#define _RCX_EVENT_LISTS
//definitions of components that causes/uses events
#include "../shared/geom.hpp"
#include "../shared/body.hpp"
#include "../shared/joint.hpp"

template <typename T>
class Event_List
{
	public:
		Event_List(T*); //for adding event
		T *Get_Event(); //for reading event (and remove from list)
		static void Remove(T*); //remove all events for component from this list

	private:
		static Event_List *head;
		Event_List *next;
};

#endif
