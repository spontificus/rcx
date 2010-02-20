#ifndef _RCX_INTERNAL_H
#define _RCX_INTERNAL_H
#include "../loaders/conf.hpp"
#include <stddef.h>
#include <ode/ode.h>

//important system configuration variables
extern struct internal_struct {
	int verbosity;

	//for multithreading
	bool multithread;
	bool limit_fps;
	int events_sleep;

	//physics
	dReal stepsize;
	int iterations;
	int contact_points;
	//bool finite_rotation;
	dReal scale; //TODO
	dReal max_wheel_rotation;
	dReal rim_angle;
	dReal mu,erp,cfm,slip;
	dReal linear_drag, angular_drag;

	dReal dis_linear, dis_angular, dis_time;
	int dis_steps;


	//graphics
	int threshold;
	int res[2]; //resolution
	int dist;
	bool force;
	float angle;
	bool fullscreen;
} internal;

const struct data_index internal_index[] = {
	{"verbosity",		'i',1, offsetof(struct internal_struct, verbosity)},
	{"multithread",		'b',1, offsetof(struct internal_struct, multithread)},
	{"limit_fps",		'b',1, offsetof(struct internal_struct, limit_fps)},
	{"events_sleep",	'i',1, offsetof(struct internal_struct, events_sleep)},
	{"stepsize",		'f',1, offsetof(struct internal_struct, stepsize)},
	{"iterations",		'i',1, offsetof(struct internal_struct, iterations)},
	{"contact_points",	'i',1, offsetof(struct internal_struct, contact_points)},
	//{"finite_rotation",	'b',1, offsetof(struct internal_struct, finite_rotation)},
	//TODO: SCALE
	{"max_wheel_rotation",	'f',1, offsetof(struct internal_struct, max_wheel_rotation)},
	{"rim_angle",		'f',1, offsetof(struct internal_struct, rim_angle)},
	{"default_mu",		'f',1, offsetof(struct internal_struct, mu)},
	{"default_erp",		'f',1, offsetof(struct internal_struct, erp)},
	{"default_cfm",		'f',1, offsetof(struct internal_struct, cfm)},
	{"default_slip",	'f',1, offsetof(struct internal_struct, slip)},
	{"default_linear_drag",	'f',1, offsetof(struct internal_struct, linear_drag)},
	{"default_angular_drag",'f',1, offsetof(struct internal_struct, angular_drag)},
	{"auto_disable_linear",	'f',1, offsetof(struct internal_struct, dis_linear)},
	{"auto_disable_angular",'f',1, offsetof(struct internal_struct, dis_angular)},
	{"auto_disable_time",	'f',1, offsetof(struct internal_struct, dis_time)},
	{"auto_disable_steps",	'i',1, offsetof(struct internal_struct, dis_steps)},
	//graphics
	{"graphics_threshold",	'i',1, offsetof(struct internal_struct, threshold)},
	{"resolution",		'i',2, offsetof(struct internal_struct, res)},
	{"eye_distance",		'i',1, offsetof(struct internal_struct, dist)},
	{"force_angle",		'b',1, offsetof(struct internal_struct, force)},
	{"view_angle",		'f',1, offsetof(struct internal_struct, angle)},
	{"fullscreen",		'b',1, offsetof(struct internal_struct, fullscreen)},
	{"",0,0}};


#endif
