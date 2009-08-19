void graphics_camera(Uint32 step)
{
	dReal time = step/1000.0;
	car_struct *car = camera.car;
	camera_settings *settings = camera.settings;

	if (car && settings) //do some magic ;-)
	{
		//get position of target, simple
		dVector3 target;
		dBodyGetRelPointPos (car->bodyid, settings->target[0], settings->target[1], settings->target[2]*car->dir, target);



		//smart/soft camera movement, fun
		dVector3 tmp;
		dReal accel[3];

		//relative position and velocity
		dBodyGetRelPointPos (car->bodyid, settings->position[0], settings->position[1], settings->position[2]*car->dir, tmp);
		dReal pos[3] = {tmp[0]-camera.pos[0], tmp[1]-camera.pos[1], tmp[2]-camera.pos[2]};
		dBodyGetRelPointVel (car->bodyid, settings->position[0], settings->position[1], settings->position[2]*car->dir, tmp);
		dReal vel[3] = {tmp[0]-camera.vel[0], tmp[1]-camera.vel[1], tmp[2]-camera.vel[2]};

		dReal pos_l = sqrt(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]); //length
		dReal pos_n[3] = {pos[0]/pos_l, pos[1]/pos_l, pos[2]/pos_l}; //normalized

		if (pos_l == 0)
			printf("TODO: add logic for handling keeping right position\n");

		//find point on length that gives projection by velocity on length
		dReal p = (pos_n[0]*vel[0]+pos_n[1]*vel[1]+pos_n[2]*vel[2]);
		dReal v[3] = {p*pos_n[0], p*pos_n[1], p*pos_n[2]}; //vector

		//remove the movement in the desired axis from total, and add breaking
		accel[0] = (vel[0]-v[0])*settings->accel_tweak;
		accel[1] = (vel[1]-v[1])*settings->accel_tweak;
		accel[2] = (vel[2]-v[2])*settings->accel_tweak;

		//deceleration over distance > current velocity (=keep on accelerating)
		dReal possible_change = settings->accel_max*time;
		if (sqrt(2*(pos_l-possible_change*time)*settings->accel_max) > -p+possible_change)
		{
			//printf("accel\n");
			accel[0] += pos_n[0]*settings->accel_max;
			accel[1] += pos_n[1]*settings->accel_max;
			accel[2] += pos_n[2]*settings->accel_max;
		}
		else //we need to break
		{
			//printf("break\n");
			dReal acceleration= (p*p)/(2*pos_l);
			accel[0] -= pos_n[0]*acceleration;
			accel[1] -= pos_n[1]*acceleration;
			accel[2] -= pos_n[2]*acceleration;
		}

		//see if wanted acceleration is above accepted max
		dReal accel_l = sqrt(accel[0]*accel[0]+accel[1]*accel[1]+accel[2]*accel[2]);

		dReal change = (settings->accel_max/accel_l);
		if (change < 1)
		{
			accel[0] *= change;
			accel[1] *= change;
			accel[2] *= change;
		}

		//now add acceleration
		camera.vel[0] += accel[0]*time;
		camera.vel[1] += accel[1]*time;
		camera.vel[2] += accel[2]*time;

		//move camera
		camera.pos[0] += camera.vel[0]*time;
		camera.pos[1] += camera.vel[1]*time;
		camera.pos[2] += camera.vel[2]*time;


		//set camera
		gluLookAt(camera.pos[0], camera.pos[1], camera.pos[2], target[0], target[1], target[2], 0,0,1);
	}
	else
		gluLookAt (10, -10, 10, 0,0,0, 0,0,1);
}


