#ifndef _RCX_PROFILE_H
#define _RCX_PROFILE_H

#include "SDL/SDL_keyboard.h"
#define UNUSED_KEY SDLK_QUESTION //key that's not used during race ("safe" default)

#include "car.hpp"
#include "camera.hpp"
#include "joint.hpp"

//profile: stores the user's settings (including key list)
typedef struct profile_struct {
	//the car the user is controlling
	car_struct *car;
	struct profile_struct *next;
	struct profile_struct *prev;

	//settings (loaded from conf)
	dReal steer_speed;
	dReal steer_max;
	dReal throttle_speed;

	//keys (loaded from keys.lst)
	SDLKey up;
	SDLKey down;
	SDLKey right;
	SDLKey left;
	SDLKey soft_break;
	SDLKey drift_break;

	SDLKey cam_x_pos;
	SDLKey cam_x_neg;
	SDLKey cam_y_pos;
	SDLKey cam_y_neg;
	SDLKey cam_z_pos;
	SDLKey cam_z_neg;

	struct Camera_Settings cam[4];
	int camera;
	SDLKey cam1;
	SDLKey cam2;
	SDLKey cam3;
	SDLKey cam4;
} profile;

extern profile *profile_head;

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
	
profile *allocate_profile(void);
profile *load_profile (const char *path);
void free_profile (profile *target);

#endif
