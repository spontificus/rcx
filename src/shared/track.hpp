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
extern struct Track_Struct {
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

	file_3d *f_3d;
	//NOTE/TODO: currently coded to store 5 planes (components) - only temporary!
	Object *object;
	Space *space;
} track;
//index:

const struct Track_Struct track_defaults = {
	{0.5,0.8,0.8},
	{0.0,0.0,0.0},
	{1.0,1.0,1.0},
	{1.0,1.0,1.0},
	{-1.0,0.5,1.0},
	9.82,
	1.0,
	1.0,
	0.8,
	0.01,
	1.29,
	{0.5,1.0,0.0},
	{0,-20,1.5},
	{15,8,15},
	{0,0,4}};

const struct Conf_Index track_index[] = {
	{"sky",		'f',3,	offsetof(Track_Struct, sky[0])},
	{"ambient",	'f',3,	offsetof(Track_Struct, ambient[0])},
	{"diffuse",	'f',3,	offsetof(Track_Struct, diffuse[0])},
	{"specular",	'f',3,	offsetof(Track_Struct, specular[0])},
	{"position",	'f',3,	offsetof(Track_Struct, position[0])},
	{"gravity",	'f',1,	offsetof(Track_Struct, gravity)},
	{"mu",		'f',1,	offsetof(Track_Struct, mu)},
	{"slip",	'f',1,	offsetof(Track_Struct, slip)},
	{"erp",		'f',1,	offsetof(Track_Struct, erp)},
	{"cfm",		'f',1,	offsetof(Track_Struct, cfm)},
	{"density",	'f',1,	offsetof(Track_Struct, density)},
	{"wind",	'f',3,	offsetof(Track_Struct, wind)},
	{"start",	'f',3,	offsetof(Track_Struct, start)},
	{"cam_start",	'f',3,	offsetof(Track_Struct, cam_start)},
	{"target_start",'f',3,	offsetof(Track_Struct, target_start)},
	{"",0,0}};//end

bool load_track (const char *path);

#endif
