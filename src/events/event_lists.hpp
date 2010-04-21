#ifndef _RCX_EVENT_LISTS
#define _RCX_EVENT_LISTS
//definitions of components that causes/uses events
#include "../shared/component.hpp"
#include "../shared/geom.hpp"
#include "../shared/body.hpp"
#include "../shared/joint.hpp"

//for components having "health" buffers
class Buffer_Event_List
{
	public:
		//for adding events
		Buffer_Event_List(Geom*);
		Buffer_Event_List(Body*);
		Buffer_Event_List(Joint*);

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
		static Buffer_Event_List *geom_head;
		static Buffer_Event_List *body_head;
		static Buffer_Event_List *joint_head;

		//next in list
		Buffer_Event_List *next;
};

//geoms functioning as sensors (detects other geoms that gets in/leaves range)
class Sensor_Event_List
{
	public:
		//only geoms:
		Sensor_Event_List(Geom*);
		static bool Get_Event(Geom**);
		static void Remove(Geom*);

	private:
		Geom *geom;

		static Sensor_Event_List *head;
		Sensor_Event_List *next;
};

//objects that have become unactive, and should be removed
class Object_Event_List
{
	public:
		Object_Event_List(Object*);

		static bool Get_Event(Object**);

		static void Remove(Object*);

	private:
		Object *object;

		static Object_Event_List *head;
		Object_Event_List *next;
};

#endif
