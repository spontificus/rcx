#ifndef _RCX_SCRIPT_H
#define _RCX_SCRIPT_H

#include "racetime_data.hpp"

//script: human readable (read: not _programming_) langue which will
//describe what should be done when spawning an object (components, joints...),
//and when an component is colliding ("sensor triggering", destroying and so on)
//function arguments can point at 3d files and other scripts and so on...
//
//(currently not used)
//
//>Allocated at start
class Script: public Racetime_Data
{
	public:
		Script(const char*);
		~Script();
};

#endif
