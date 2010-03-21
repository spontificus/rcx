#ifndef _RCX_TRACK_H
#define _RCX_TRACK_H
#include "../loaders/conf.hpp"
#include "file_3d.hpp"
#include "object.hpp"
#include <GL/gl.h>
#include <ode/ode.h>
//track: the main "world", contains simulation and rendering data for one
//large 3D file for the rigid environment, and more simulation data (like
//gravity) - crappy solution for now...

extern dWorldID world;
extern dSpaceID space;
extern dJointGroupID contactgroup;

//Allocated at start
//(in contrary to the other structs, this is actually not allocated on runtime!)
extern struct track_struct {
	//placeholder for stuff like if it's raining/snowing and lightsources
	GLfloat sky[3]; //RGB, alpha is always 1.0f

	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat position[4]; //light position
	
	dReal gravity;
	dReal mu; //friction (normal)
	dReal slip; //for wheel friction
	dReal erp;
	dReal cfm;

	dReal density; //for air drag (friction)
	dReal wind[3];

	dReal start[3];
	dReal cam_start[3];
	dReal target_start[3];

	file_3d_struct *file_3d;
	//NOTE/TODO: currently coded to store 5 planes (components) - only temporary!
	Object *object;
	Space *space;
} track;
//index:

const struct Conf_Index track_index[] = {
	{"sky",		'f',3,	offsetof(struct track_struct, sky[0])},
	{"ambient",	'f',3,	offsetof(struct track_struct, ambient[0])},
	{"diffuse",	'f',3,	offsetof(struct track_struct, diffuse[0])},
	{"specular",	'f',3,	offsetof(struct track_struct, specular[0])},
	{"position",	'f',3,	offsetof(struct track_struct, position[0])},
	{"gravity",	'f',1,	offsetof(struct track_struct, gravity)},
	{"mu",		'f',1,	offsetof(struct track_struct, mu)},
	{"slip",		'f',1,	offsetof(struct track_struct, slip)},
	{"erp",		'f',1,	offsetof(struct track_struct, erp)},
	{"cfm",		'f',1,	offsetof(struct track_struct, cfm)},
	{"density",	'f',1,	offsetof(struct track_struct, density)},
	{"wind",	'f',3,	offsetof(struct track_struct, wind)},
	{"start",	'f',3,	offsetof(struct track_struct, start)},
	{"cam_start",	'f',3,	offsetof(struct track_struct, cam_start)},
	{"target_start",'f',3,	offsetof(struct track_struct, target_start)},
	{"",0,0}};//end

int load_track (char *path);

#endif
