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

#include "camera.hpp"

Camera camera;

//just make sure all variables are zeroed
Camera::Camera()
{
	settings=NULL;
	car = NULL;

	for (int i=0; i<3; ++i)
	{
		pos[i]=0;
		t_pos[i]=0;
		vel[i]=0;
		up[i]=0;
	}

	air_timer = 0;
	offset_scale = 0;
	reverse = false;
	in_air = false;
}

void Camera::Set_Settings (Camera_Settings *set)
{
	settings = set;

	if (settings)
	{

		//if this camera mode doesn't have reverse enabled, make sure camera isn't stuck in reverse
		if (!settings->reverse)
			reverse = false;

		//if this camera mode has "air mode" disabled, make sure camera isn't stuck in "in air" mode from before
		if (!settings->in_air)
		{
			in_air = false;
			offset_scale = 1;
		}
	}
}

void Camera::Set_Pos(dReal p[], dReal tp[])
{
	if (settings)
	{
		memcpy(pos, p, sizeof(dReal)*3);
		memcpy(t_pos, tp, sizeof(dReal)*3);
	}

	up[0]=0;
	up[1]=0;
	up[2]=1;
}

void Camera::Move(dReal x, dReal y, dReal z)
{
	if (settings)
	{
		settings->distance[0] += x;
		settings->distance[1] += y;
		settings->distance[2] += z;
	}
}

