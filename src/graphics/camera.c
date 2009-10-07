//length of vector
#define v_length(x, y, z) (dSqrt( (x)*(x) + (y*y)*(y*y) + (z)*(z) ))

void set_camera_settings (camera_settings *settings)
{
	if (settings)
	{
		camera.settings = settings;

		if (!camera.geom)
			camera.geom = dCreateSphere(0, settings->radius);
		else
			dGeomSphereSetRadius (camera.geom, settings->radius);
	}
}

void camera_graphics_step(Uint32 step)
{
	dReal time = step/1000.0;
	car_struct *car = camera.car;
	camera_settings *settings = camera.settings;
	dReal max_accel = settings->accel_max;

	if (car && settings) //do some magic ;-)
	{
		//random values that might come handy

		//get position of target, simple
		dVector3 target;
		dBodyGetRelPointPos (car->bodyid, settings->target[0], settings->target[1], settings->target[2]*car->dir, target);

		//position and velocity of wanted position
		dVector3 t_pos, t_vel;
		dBodyGetRelPointPos (car->bodyid, settings->position[0], settings->position[1], settings->position[2]*car->dir, t_pos);
		dBodyGetRelPointVel (car->bodyid, settings->position[0], settings->position[1], settings->position[2]*car->dir, t_vel);

		//relative vectors
		dReal pos[3] = {t_pos[0]-camera.pos[0], t_pos[1]-camera.pos[1], t_pos[2]-camera.pos[2]};
		dReal vel[3] = {t_vel[0]-camera.vel[0], t_vel[1]-camera.vel[1], t_vel[2]-camera.vel[2]};

		//vector lengths
		dReal pos_l = v_length(pos[0], pos[1], pos[2]);
		dReal vel_l = v_length(vel[0], vel[1], vel[2]);

		//unit vectors
		dReal pos_u[3] = {pos[0]/pos_l, pos[1]/pos_l, pos[2]/pos_l};
		dReal vel_u[3] = {vel[0]/vel_l, vel[1]/vel_l, vel[2]/vel_l};




		//check if relative pos and vel can be set to 0 in this one step...
		//1) accel for breaking to v=0
		dReal A1[3] = {-vel[0]/time, -vel[1]/time, -vel[2]/time};
		//2) accel for moving to p=0  (one time for accelerating, one time (negative) for breaking)
		dReal tmp = 4.0/(time*time);
		dReal A2[3] = {pos[0]*tmp, pos[1]*tmp, pos[2]*tmp};
		//3) can all accelerations be done in one step?
		if ((v_length(A1[0]+A2[0], A1[1]+A2[1], A1[2]+A2[2]) +
			v_length(A1[0]-A2[0], A1[1]-A2[1], A1[2]-A2[2]))/2.0 <= settings->accel_max)
		{
			camera.pos[0] = t_pos[0];
			camera.pos[1] = t_pos[1];
			camera.pos[2] = t_pos[2];
			
			camera.vel[0] = t_vel[0];
			camera.vel[1] = t_vel[1];
			camera.vel[2] = t_vel[2];
		}
		else //we can only approach wanted pos/vel in this step...
		{
			dReal accel[3] = {0,0,0};
			printf("TODO!!!\n");


			//ONLY DEMO, NOT REAL SOLUTION!
			//(just move against wanted position)
			accel[0] = pos_u[0]*max_accel;
			accel[1] = pos_u[1]*max_accel;
			accel[2] = pos_u[2]*max_accel;
			//!


			//done...
			dReal dV[3];
			dV[0] = accel[0]*time;
			dV[1] = accel[1]*time;
			dV[2] = accel[2]*time;

			//move camera
			camera.pos[0] += time*(camera.vel[0] + accel[0]/2);
			camera.pos[1] += time*(camera.vel[1] + accel[1]/2);
			camera.pos[2] += time*(camera.vel[2] + accel[2]/2);

			//now add acceleration
			camera.vel[0] += dV[0];
			camera.vel[1] += dV[1];
			camera.vel[2] += dV[2];
		}


		

		//set camera
		gluLookAt(camera.pos[0], camera.pos[1], camera.pos[2], target[0], target[1], target[2], 0,0,1);
	}
	else
		gluLookAt (10, -10, 10, 0,0,0, 0,0,1);
}


