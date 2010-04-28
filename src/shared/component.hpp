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

#ifndef _RCX_COMPONENT_H
#define _RCX_COMPONENT_H
//base class for creating component classes (geoms, joints, bodies, spaces)
//TODO: make polymorph, this claaa should not be used directly...

//prototype for object class
class Object;

class Component
{
	public:
		virtual ~Component(); //useful outside

	private:
		Component *prev, *next;

	protected: //private for this and subclasses
		Component(Object *obj);
		//keep track of the "owning" object
		Object * object_parent;

};
#endif
