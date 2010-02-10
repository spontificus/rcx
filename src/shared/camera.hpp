#ifndef _RCX_CAMERA_H
#define _RCX_CAMERA_H
//TODO: make class
//
#include <ode/ode.h>
#include "car.hpp"

typedef struct {
	dReal target[3];
	dReal anchor[3], distance[3];
	dReal radius;
	dReal linear_stiffness;
	dReal angular_stiffness;
	dReal damping;
	bool relative_damping;
	dReal rotation_tightness;
	dReal target_tightness;
	bool reverse, in_air;
	dReal air_time, ground_time;
	dReal offset_scale_speed;
} camera_settings;

typedef struct {
	camera_settings *settings;
	car_struct *car;
	dReal pos[3];
	dReal t_pos[3];
	dReal vel[3];
	dReal up[3];
	dReal air_timer;
	dReal offset_scale; //0-1   0 in air, 1 on ground
	bool reverse;
	bool in_air;
} camera_struct;

extern camera_struct camera;

void camera_graphics_step();
void set_camera_settings (camera_settings *settings);
void camera_physics_step();
void camera_graphics_step();
#endif
