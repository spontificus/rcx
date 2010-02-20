#ifndef _RCX_SCRIPT_H
#define _RCX_SCRIPT_H
#include "file_3d.hpp"

//script: human readable (read: not _programming_) langue which will
//describe what should be done when spawning an object (components, joints...),
//and when an component is colliding ("sensor triggering", destroying and so on)
//function arguments can point at 3d files and other scripts and so on...
//
//(currently not used)
//
//>Allocated at start
typedef struct script_struct {
	char *name; //usefull if to see if the same object is requested more times
	//placeholder for script data, now just a single variable (what to render)
	file_3d_struct *graphics_debug1;
	file_3d_struct *graphics_debug2;
	file_3d_struct *graphics_debug3;

	//temporary solution
	bool box;
	bool flipper;
	bool NH4;
	bool building;
	bool sphere;

	struct script_struct *next;
} script_struct;

extern script_struct *script_head;
script_struct *allocate_script(void);

#endif
