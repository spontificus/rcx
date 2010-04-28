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

#ifndef _RCX_SPACE_H
#define _RCX_SPACE_H
#include "component.hpp"
#include "object.hpp"

//Space: (meta)data for spaces (collects geoms that should not collide)
//(in contrary to other components, this can't be rendered or cause events)
//
//Dynamic allocation
class Space: public Component
{
	public:
		//methods
		Space (Object *obj);
		~Space();

		//variables
		dSpaceID space_id;

	private:
		//no need for global list of spaces...
};

#endif
