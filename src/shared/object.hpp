#ifndef _RCX_OBJECT_H
#define _RCX_OBJECT_H
#include <ode/common.h>

#include "script.hpp"
#include "component.hpp"
#include "space.hpp"

//object: one "thing" on the track, from a complex building to a tree, spawning
//will be controlled by a custom scripting langue in future versions, the most
//important role of "object" is to store the ode space for the spawned object
//(keeps track of the geoms in ode that describes the components) and joint
//group (for cleaning up object)
//
//>Dynamically allocated
class Object
{
	public:
		Object();
		virtual ~Object(); //(makes sure also inherited classes calls this destructor)

		static void Destroy_All();
		static void Spawn(script_struct *script, dReal x, dReal y, dReal z);
		static script_struct *Load(const char *path); //TODO: move to Object_Template class
	private:
		//things to keep track of when cleaning out object
		Component *components;
		friend class Component; //to allow access to above
		dSpaceID selected_space;
		//both geom and space uses the variable above
		friend class Geom;
		friend class Space;

		//placeholder for more data
			
		//used to find next/prev object in dynamically allocated chain
		//set next to null in last object in chain
		static Object *head;
		Object *prev, *next;
};

#endif
