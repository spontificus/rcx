#ifndef _RCX_OBJECT_H
#define _RCX_OBJECT_H
#include <ode/common.h>

#include "racetime_data.hpp"
#include "file_3d.hpp"
#include "script.hpp"
#include "component.hpp"
#include "space.hpp"

//object: one "thing" on the track, from a complex building to a tree, spawning
//will be controlled by a custom scripting langue in future versions, the most
//important role of "object" is to store the ode space for the spawned object
//(keeps track of the geoms in ode that describes the components) and joint
//group (for cleaning up object)

//template for spawning
class Object_Template:public Racetime_Data
{
	public:
		static Object_Template *Load(const char *path);
		void Spawn(dReal x, dReal y, dReal z);

	private:
		Object_Template(const char*); //just set some default values
		//placeholder for script data, now just variables

		//script to be run when spawning object
		Script *spawn;

		//temporary graphics
		file_3d_struct *graphics_debug1;
		file_3d_struct *graphics_debug2;
		file_3d_struct *graphics_debug3;

		//temporary solution
		bool box;
		bool flipper;
		bool NH4;
		bool building;
		bool sphere;
		bool pillar;
};

//can be added/removed at runtime ("racetime")
class Object
{
	public:
		virtual ~Object(); //(virtual makes sure also inherited classes calls this destructor)
		static void Destroy_All();

		//removes empty objects
		static void Events_Step();

	private:
		Object();
		//the following are either using or inherited from this class
		friend class Object_Template; //needs access to constructor
		friend int load_track (char *);
		friend class Car;

		//things to keep track of when cleaning out object
		Component *components;
		unsigned int activity; //counts geoms,bodies and future stuff (script timers, loops, etc)
		dSpaceID selected_space;
		//to allow acces to the two above pointers
		friend class Component;
		friend class Geom;
		friend class Body;
		friend class Space;

		//placeholder for more data
			
		//used to find next/prev object in dynamically allocated chain
		//set next to null in last object in chain
		static Object *head;
		Object *prev, *next;
};

#endif
