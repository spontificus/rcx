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
		Space (object_struct *obj);
		~Space();

		//variables
		dSpaceID space_id;

	private:
		//no need for global list of spaces...
};

#endif
