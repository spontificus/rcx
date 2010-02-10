#ifndef _RCX_CONF_H
#define _RCX_CONF_H
#include "unistd.h"

//to make the conf loader able to find variable names in structs, use indexes
struct data_index {
	const char *name;
	char type; //f for float, b for bool, i for int, 0 for end of list
	int length; //normaly 1 (or else more)
	size_t offset;
};

int load_conf (char *name, char *memory, const struct data_index index[]);
#endif
