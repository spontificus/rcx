#ifndef __CONTROLPOINT_H__
#define __CONTROLPOINT_H__
#ifdef __cplusplus

#include "orientation.h"

class controlpoint {
public:
	controlpoint *p;
	orientation o;
	float x,y,z;
};

#endif
#endif