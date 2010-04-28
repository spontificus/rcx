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

#ifndef _RCX_CONF_H
#define _RCX_CONF_H
#include "unistd.h"
#include "stddef.h"

//to make the conf loader able to find variable names in structs, use indexes
struct Conf_Index {
	const char *name;
	char type; //f for float, b for bool, i for int, 0 for end of list
	int length; //normaly 1 (or else more)
	size_t offset;
};

int load_conf (const char *name, char *memory, const struct Conf_Index index[]);
#endif
