#include "../shared/profile.hpp"

void Profile_Events_Step(Uint32 step)
{
	Profile *prof;
	Uint8 *keys;
	for (prof=profile_head; prof; prof=prof->next)
	{
		//get keys pressed
		keys = SDL_GetKeyState(NULL);


		//set camera settings
		if (keys[prof->cam1])
			camera.Set_Settings (&prof->cam[0]);
		else if (keys[prof->cam2])
			camera.Set_Settings (&prof->cam[1]);
		else if (keys[prof->cam3])
			camera.Set_Settings (&prof->cam[2]);
		else if (keys[prof->cam4])
			camera.Set_Settings (&prof->cam[3]);

		//move camera
		if (keys[prof->cam_x_pos]) //x
			camera.Move(+(step*0.03), 0, 0);
		if (keys[prof->cam_x_neg])
			camera.Move(-(step*0.03), 0, 0);

		if (keys[prof->cam_y_pos]) //y
			camera.Move(0, +(step*0.03), 0);
		if (keys[prof->cam_y_neg])
			camera.Move(0, -(step*0.03), 0);

		if (keys[prof->cam_z_pos]) //z
			camera.Move(0, 0, +(step*0.03));
		if (keys[prof->cam_z_neg])
			camera.Move(0, 0, -(step*0.03));

		//if selected car, read input
		if (prof->car)
		{
			Car *carp = prof->car;
			if (keys[prof->drift_break])
			{
				carp->drift_breaks = true;
				carp->breaks = false;
			}
			//Alt and Ctrl activates "softer" breaks...
			else if (keys[prof->soft_break])
			{
				carp->breaks = true;
				carp->drift_breaks = false;
			}
			else
			{
				carp->drift_breaks = false;
				carp->breaks = false;
			}

			dReal t_speed = prof->throttle_speed*step;
			if (keys[prof->up])
			{
				carp->throttle += t_speed;
				if (carp->throttle > 1.0)
					carp->throttle = 1.0;
			}
			else if (keys[SDLK_DOWN])
			{
				carp->throttle -= t_speed;
				if (carp->throttle < -1.0)
					carp->throttle = -1.0;
			}
			else
			{
				if (carp->throttle <= t_speed &&
					carp->throttle >= -(t_speed))
					carp->throttle = 0.0;

				else if (carp->throttle > 0.0)
				    carp->throttle -= t_speed;

				else
				    carp->throttle += t_speed;
			}

			t_speed = prof->steer_speed*step;
			dReal max = prof->steer_max;
			if (keys[prof->left]&&!keys[prof->right])
			{
				carp->steering -= t_speed;
				if (carp->steering < -max)
					carp->steering = -max;
			}
			else if (!keys[prof->left]&&keys[prof->right])
			{
				carp->steering += t_speed;
				if (carp->steering > max)
					carp->steering = max;
			}
			else //center
			{
				if (carp->steering <= t_speed &&
				    carp->steering >= -(t_speed))
					carp->steering = 0.0;

				else if (carp->steering > 0.0)
				    carp->steering -= t_speed;

				else
				    carp->steering += t_speed;
			}
		}
	}
}
