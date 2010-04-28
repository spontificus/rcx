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

#ifndef _RCX_CAMERA_H
#define _RCX_CAMERA_H
//TODO: make class
//
#include <ode/ode.h>
#include "car.hpp"

struct Camera_Settings {
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
};

class Camera
{
	public:
		Camera();
		void Set_Settings(Camera_Settings *settings);
		void Set_Pos(dReal p[3], dReal tp[3]);

		//movement (might change or be removed at some point)
		void Move(dReal x, dReal y, dReal z);

		//these should probably be static (for using more cameras), but this will do for now
		void Physics_Step();
		void Graphics_Step();

		//public for now...
		Car *car;
	private:
		struct Camera_Settings *settings;
		dReal pos[3];
		dReal t_pos[3];
		dReal vel[3];
		dReal up[3];
		dReal air_timer;
		dReal offset_scale; //0-1   0 in air, 1 on ground
		bool reverse;
		bool in_air;
};

extern Camera camera;
#endif
