#include "camera.hpp"

camera_struct camera = {NULL, NULL, //settings, car
		       {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, //pos, vel, up
			0,0,false,false}; //timer, scale, reverse, air

void set_camera_settings (camera_settings *settings)
{
	if (settings)
	{
		camera.settings = settings;

		//if this camera mode doesn't have reverse enabled, make sure camera isn't stuck in reverse
		if (!settings->reverse)
			camera.reverse = false;

		//if this camera mode has "air mode" disabled, make sure camera isn't stuck in "in air" mode from before
		if (!settings->in_air)
		{
			camera.in_air = false;
			camera.offset_scale = 1;
		}
	}
}

