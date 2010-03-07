#include <stdlib.h>
#include "script.hpp"
#include "printlog.hpp"

script_struct *script_head = NULL;

//allocate new script storage, and add it to list
//(not used yet, only for storing 3d list pointers...)
script_struct *allocate_script(void)
{
	printlog(2, "allocating script");
	script_struct *tmp = (script_struct *)malloc(sizeof(script_struct));
	tmp->next = script_head;
	script_head = tmp;

	if (!tmp->next)
		printlog(2, "(first registered script)");

	script_head->name = NULL;

	//debug identification bools set to false
	script_head->box = false;
	script_head->flipper = false;
	script_head->NH4 = false;
	script_head->building = false;
	script_head->sphere = false;


	return script_head;
}

