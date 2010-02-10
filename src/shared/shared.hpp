#ifndef _RCX_SHARED_H
#define _RCX_SHARED_H
//Shared data structs and typedefs.
//
//See licensing info in main.c

//Required stuff:
#include <SDL.h>
#include <SDL_opengl.h>
#include <ode/ode.h>

//C++ libs
//#include <string>
//#include <vector>
//#include <fstream>

//C libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>



//general info
#define VERSION "0.06 (NOT DONE)" //supports alphanumeric versioning

const char ISSUE[] =
"    RollCageX  Copyright (C) 2009-2010  \"Slinger\" (on gorcx.net forum)\n\n\
   This program is free software: you can redistribute it and/or modify\n\
   it under the terms of the GNU General Public License as published by\n\
   the Free Software Foundation, either version 3 of the License, or\n\
   (at your option) any later version.\n\n\
   This program comes with ABSOLUTELY NO WARRANTY:\n\n\
   This program is distributed in the hope that it will be useful,\n\
   but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
   GNU General Public License for more details.\n\n\
   You should have received a copy of the GNU General Public License\n\
   along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n\
    = Credits =\n\
      * \"Slinger\"	Creator (coder)\n\
      * \"XEWEASEL\"	3D Models\n\n\
    = Special Thanks =\n\
      * \"K.Mac\"		Extensive testing and new ideas\n\
      * \"MeAkaJon\"	Creator/maintainer of gorcx.net webpage\n\
      * \"Spontificus\"	Testing, hacks, github registion and various fixes\n\n\
    = Other Projects that made RCX possible =\n\
      * \"Simple DirectMedia Layer\"	OS/hardware abstractions\n\
      * \"Open Dynamics Engine\"		Rigid body dynamics and collision detection\n\
      * \"The GNU Project\"		Its fight for computer freedom has changed the world\n\n\
    - See README for more info -\n\n";
 

//use a "runlevel" (enum) variable to make all threads/loops aware of status
//(locked is used when we want to pause the loops, but still try to catch
//up with real time when unlocked, basically: stop simulation, but do not
//reset "simulated time" variables... Use it when building objects)
typedef enum {running, done, paused, locked, error} runlevel_type;
extern runlevel_type runlevel;

#include "../loaders/conf.hpp"
#include "../physics/drag.hpp"

//important system configuration variables
extern struct internal_struct {
	int verbosity;
	bool multithread; //TODO

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
	//TODO: MULTITHREAD
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

#include "file_3d.hpp"
#include "script.hpp"
#include "object.hpp"
#include "geom.hpp"
#include "body.hpp"

//joint_data: data for joint (connects bodies), is used for:
//currently only for triggering event script (force threshold)
//
//>Dynamically allocated
typedef struct joint_data_struct {
	//keep track of the "owning" object
	object_struct * object_parent;
	//geom data bellongs to
	dJointID joint_id;

	dJointFeedback feedback; //used if checking forces
	dReal threshold; //if force on body exceeds threshold, eat buffer
	dReal buffer; //if buffer reaches zero, trigger event
	bool event; //if event triggered, run script
	script_struct *script; //the script to run

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct joint_data_struct *prev;
	struct joint_data_struct *next;
} joint_data;

extern joint_data *joint_data_head;

#include "car.hpp"
#include "camera.hpp"
#include "profile.hpp"
//#include "drag.hpp"

const struct data_index profile_index[] = {
	{"steer_speed",    'f' ,1 ,offsetof(struct profile_struct, steer_speed)},
	{"steer_max",      'f' ,1 ,offsetof(struct profile_struct, steer_max)},
	{"throttle_speed", 'f' ,1 ,offsetof(struct profile_struct, throttle_speed)},

	{"camera_default",   	   	'i' ,1 ,offsetof(struct profile_struct, camera)},

	{"camera1:target_offset",	'f' ,3 ,offsetof(struct profile_struct, cam[0].target)},
	{"camera1:anchor_offset",	'f' ,3 ,offsetof(struct profile_struct, cam[0].anchor)},
	{"camera1:anchor_distance",	'f' ,3 ,offsetof(struct profile_struct, cam[0].distance)},
	{"camera1:collision_radius",	'f' ,1 ,offsetof(struct profile_struct, cam[0].radius)},
	{"camera1:linear_stiffness",	'f' ,1 ,offsetof(struct profile_struct, cam[0].linear_stiffness)},
	{"camera1:angular_stiffness",	'f' ,1 ,offsetof(struct profile_struct, cam[0].angular_stiffness)},
	{"camera1:damping",		'f' ,1 ,offsetof(struct profile_struct, cam[0].damping)},
	{"camera1:relative_damping",	'b' ,1 ,offsetof(struct profile_struct, cam[0].relative_damping)},
	{"camera1:rotation_tightness",	'f' ,1 ,offsetof(struct profile_struct, cam[0].rotation_tightness)},
	{"camera1:target_tightness",	'f' ,1 ,offsetof(struct profile_struct, cam[0].target_tightness)},
	{"camera1:enable_reverse",	'b' ,1 ,offsetof(struct profile_struct, cam[0].reverse)},
	{"camera1:enable_in_air",	'b' ,1 ,offsetof(struct profile_struct, cam[0].in_air)},
	{"camera1:air_time",		'f', 1, offsetof(struct profile_struct, cam[0].air_time)},
	{"camera1:ground_time",		'f', 1, offsetof(struct profile_struct, cam[0].ground_time)},
	{"camera1:offset_scale_speed",	'f', 1, offsetof(struct profile_struct, cam[0].offset_scale_speed)},

	{"camera2:target_offset",	'f' ,3 ,offsetof(struct profile_struct, cam[1].target)},
	{"camera2:anchor_offset",	'f' ,3 ,offsetof(struct profile_struct, cam[1].anchor)},
	{"camera2:anchor_distance",	'f' ,3 ,offsetof(struct profile_struct, cam[1].distance)},
	{"camera2:collision_radius",	'f' ,1 ,offsetof(struct profile_struct, cam[1].radius)},
	{"camera2:linear_stiffness",	'f' ,1 ,offsetof(struct profile_struct, cam[1].linear_stiffness)},
	{"camera2:angular_stiffness",	'f' ,1 ,offsetof(struct profile_struct, cam[1].angular_stiffness)},
	{"camera2:damping",		'f' ,1 ,offsetof(struct profile_struct, cam[1].damping)},
	{"camera2:relative_damping",	'b' ,1 ,offsetof(struct profile_struct, cam[1].relative_damping)},
	{"camera2:rotation_tightness",	'f' ,1 ,offsetof(struct profile_struct, cam[1].rotation_tightness)},
	{"camera2:target_tightness",	'f' ,1 ,offsetof(struct profile_struct, cam[1].target_tightness)},
	{"camera2:enable_reverse",	'b' ,1 ,offsetof(struct profile_struct, cam[1].reverse)},
	{"camera2:enable_in_air",	'b' ,1 ,offsetof(struct profile_struct, cam[1].in_air)},
	{"camera2:air_time",		'f', 1, offsetof(struct profile_struct, cam[1].air_time)},
	{"camera2:ground_time",		'f', 1, offsetof(struct profile_struct, cam[1].ground_time)},
	{"camera2:offset_scale_speed",	'f', 1, offsetof(struct profile_struct, cam[1].offset_scale_speed)},

	{"camera3:target_offset",	'f' ,3 ,offsetof(struct profile_struct, cam[2].target)},
	{"camera3:anchor_offset",	'f' ,3 ,offsetof(struct profile_struct, cam[2].anchor)},
	{"camera3:anchor_distance",	'f' ,3 ,offsetof(struct profile_struct, cam[2].distance)},
	{"camera3:collision_radius",	'f' ,1 ,offsetof(struct profile_struct, cam[2].radius)},
	{"camera3:linear_stiffness",	'f' ,1 ,offsetof(struct profile_struct, cam[2].linear_stiffness)},
	{"camera3:angular_stiffness",	'f' ,1 ,offsetof(struct profile_struct, cam[2].angular_stiffness)},
	{"camera3:damping",		'f' ,1 ,offsetof(struct profile_struct, cam[2].damping)},
	{"camera3:relative_damping",	'b' ,1 ,offsetof(struct profile_struct, cam[2].relative_damping)},
	{"camera3:rotation_tightness",	'f' ,1 ,offsetof(struct profile_struct, cam[2].rotation_tightness)},
	{"camera3:target_tightness",	'f' ,1 ,offsetof(struct profile_struct, cam[2].target_tightness)},
	{"camera3:enable_reverse",	'b' ,1 ,offsetof(struct profile_struct, cam[2].reverse)},
	{"camera3:enable_in_air",	'b' ,1 ,offsetof(struct profile_struct, cam[2].in_air)},
	{"camera3:air_time",		'f', 1, offsetof(struct profile_struct, cam[2].air_time)},
	{"camera3:ground_time",		'f', 1, offsetof(struct profile_struct, cam[2].ground_time)},
	{"camera3:offset_scale_speed",	'f', 1, offsetof(struct profile_struct, cam[2].offset_scale_speed)},

	{"camera4:target_offset",	'f' ,3 ,offsetof(struct profile_struct, cam[3].target)},
	{"camera4:anchor_offset",	'f' ,3 ,offsetof(struct profile_struct, cam[3].anchor)},
	{"camera4:anchor_distance",	'f' ,3 ,offsetof(struct profile_struct, cam[3].distance)},
	{"camera4:collision_radius",	'f' ,1 ,offsetof(struct profile_struct, cam[3].radius)},
	{"camera4:linear_stiffness",	'f' ,1 ,offsetof(struct profile_struct, cam[3].linear_stiffness)},
	{"camera4:angular_stiffness",	'f' ,1 ,offsetof(struct profile_struct, cam[3].angular_stiffness)},
	{"camera4:damping",		'f' ,1 ,offsetof(struct profile_struct, cam[3].damping)},
	{"camera4:relative_damping",	'b' ,1 ,offsetof(struct profile_struct, cam[3].relative_damping)},
	{"camera4:rotation_tightness",	'f' ,1 ,offsetof(struct profile_struct, cam[3].rotation_tightness)},
	{"camera4:target_tightness",	'f' ,1 ,offsetof(struct profile_struct, cam[3].target_tightness)},
	{"camera4:enable_reverse",	'b' ,1 ,offsetof(struct profile_struct, cam[3].reverse)},
	{"camera4:enable_in_air",	'b' ,1 ,offsetof(struct profile_struct, cam[3].in_air)},
	{"camera4:air_time",		'f', 1, offsetof(struct profile_struct, cam[3].air_time)},
	{"camera4:ground_time",		'f', 1, offsetof(struct profile_struct, cam[3].ground_time)},
	{"camera4:offset_scale_speed",	'f', 1, offsetof(struct profile_struct, cam[3].offset_scale_speed)},
	{"",0,0}}; //end

//list of all buttons
const struct {
	const char *name;
	size_t offset;
} profile_key_list[] = {
	{"up",			offsetof(struct profile_struct, up)},
	{"down",			offsetof(struct profile_struct, down)},
	{"right",		offsetof(struct profile_struct, right)},
	{"left",			offsetof(struct profile_struct, left)},
	{"soft_break",		offsetof(struct profile_struct, soft_break)},
	{"drift_break",		offsetof(struct profile_struct, drift_break)},

	{"camera_x+",		offsetof(struct profile_struct, cam_x_pos)},
	{"camera_x-",		offsetof(struct profile_struct, cam_x_neg)},
	{"camera_y+",		offsetof(struct profile_struct, cam_y_pos)},
	{"camera_y-",		offsetof(struct profile_struct, cam_y_neg)},
	{"camera_z+",		offsetof(struct profile_struct, cam_z_pos)},
	{"camera_z-",		offsetof(struct profile_struct, cam_z_neg)},

	{"camera1",		offsetof(struct profile_struct, cam1)},
	{"camera2",		offsetof(struct profile_struct, cam2)},
	{"camera3",		offsetof(struct profile_struct, cam3)},
	{"camera4",		offsetof(struct profile_struct, cam4)},
	{"",0}}; //end
	

#include "track.hpp"

//TODO: weapons


//prototypes
void printlog (int, const char*, ...);
void free_joint_data (joint_data *target);
void free_all (void);
void set_camera_settings (camera_settings *settings);

//prototypes specific for shared data
void shared_init (void);
object_struct *allocate_object (bool adspace, bool adjointgroup);
joint_data *allocate_joint_data (dJointID joint, object_struct *obj, bool feedback);
profile *allocate_profile(void);
//void free_profile (profile *target);
//car_struct *allocate_car(void);
//file_3d_struct *allocate_file_3d (void);
void free_object(object_struct *target);
void free_joint_data (joint_data *target);
//void free_car (car_struct *target);
void free_all (void);

//global variables
extern dWorldID world;
extern dSpaceID space;
extern dJointGroupID contactgroup;

//extern car_struct *venom;
extern script_struct *box; //keep track of our loaded debug box
extern script_struct *sphere;

#endif
