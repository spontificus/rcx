#ifndef _RCX_COMPONENT_H
#define _RCX_COMPONENT_H
//base class for creating component classes (geoms, joints, bodies, spaces)
//TODO: make polymorph, this claaa should not be used directly...

//prototype for object class
struct object_struct; 

class Component
{
	public:
		Component(object_struct *obj);
		virtual ~Component();
	private:
		Component *prev, *next;

	protected: //private for this and subclasses
		//keep track of the "owning" object
		object_struct * object_parent;

};
#endif
