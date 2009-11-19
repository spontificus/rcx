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
	dReal up[3] = {0,0,1};

	if (car && settings) //do some magic ;-)
	{
		//random values that might come handy

		//get position of target
		dVector3 target;
		dBodyGetRelPointPos (car->bodyid, settings->target[0], settings->target[1], settings->target[2]*car->dir, target);

		//position and velocity of wanted position
		dVector3 t_pos, t_vel;
		dBodyGetRelPointPos (car->bodyid, settings->position[0], settings->position[1], settings->position[2]*car->dir, t_pos);
		dBodyGetRelPointVel (car->bodyid, settings->position[0], settings->position[1], settings->position[2]*car->dir, t_vel);

		//relative vectors
		dReal pos[3] = {t_pos[0]-camera.pos[0], t_pos[1]-camera.pos[1], t_pos[2]-camera.pos[2]}; //rel to cam
		dReal vel[3] = {camera.vel[0]-t_vel[0], camera.vel[1]-t_vel[1], camera.vel[2]-t_vel[2]}; //rel to obj

		//vector lengths
		dReal pos_l = v_length(pos[0], pos[1], pos[2]);
		dReal vel_l = v_length(vel[0], vel[1], vel[2]);

		//unit vectors
		dReal pos_u[3] = {pos[0]/pos_l, pos[1]/pos_l, pos[2]/pos_l};
		dReal vel_u[3] = {vel[0]/vel_l, vel[1]/vel_l, vel[2]/vel_l};


		//calculations
		//NOTE: a_max will be used for both directions, making max acceleration vary between a_max and sqrt(2)*a_max

		if (pos_l || vel_l) //if pos and vel doesn't differ, we hove nothing more to do
		{
			//TODO: these tests should probably use thresholds, or they'll probably never be true
			if (!pos_l) //no relative position
			{
				printf("TODO: no relative position\n");
			}
			else if (!vel_l) //no relative velocity
			{
				printf("TODO: no relative velocity\n");
			}
			else //both relative position and velocity
			{
				//split position into two distances: one along velocity, one perpendicular (d1 and d2)
				//project position on velocity vector:
				dReal d1 = {vel_u[0]*pos[0]+vel_u[1]*pos[1]+vel_u[2]*pos[2]};
				dReal d1v[3] = {d1*vel_u[0], d1*vel_u[1], d1*vel_u[2]};
				//perpendicular (funny: could use z²=x²+y², but need vector later)
				dReal d2v[3] = {pos[0]-d1v[0], pos[1]-d1v[1], pos[2]-d1v[2]};
				dReal d2 = v_length(d2v[0], d2v[1], d2v[2]);
				dReal d2u[3] = {d2v[0]/d2, d2v[1]/d2, d2v[2]/d2}; //unit vector

				//acceleration/deceleration of velocity (d1)
				//if current velocity is towards d1 (it's positive), might overshoot position along this axis
				if (d1 > 0 && (vel_l*vel_l/(2*max_accel)) > d1 ) //yes, we will overshoot
				{
					//todo
				}
				else
				{
					//todo
				}

				//movement correction (d2)
				//t is time it takes to correct, between 0 and t/2 acceleration, between t/2 and t break
				dReal t = sqrt(4*d2/max_accel);
				if (time >= t) //will be able to move to wanted position in one step
				{
					camera.pos[0]+=d2v[0];
					camera.pos[1]+=d2v[1];
					camera.pos[2]+=d2v[2];
				}
				else if (time > t/2)  //acceleration and some breaking
				{
					//will move half way over first t/2 time
					dReal move = d2/2;

					//movement while breaking time_left time
					dReal time_left = time-t/2;
					move+=max_accel*time_left/2*(t-time_left);

					//move
					camera.pos[0]+=d2u[0]*move;
					camera.pos[1]+=d2u[1]*move;
					camera.pos[2]+=d2u[2]*move;

					//acceleration
					dReal accel = max_accel*(t-time);
					camera.vel[0]+=d2u[0]*accel;
					camera.vel[1]+=d2u[1]*accel;
					camera.vel[2]+=d2u[2]*accel;
				}
				else //just acceleration
				{
					dReal move = max_accel*time*time/2;
					dReal accel = max_accel*time;

					//move
					camera.pos[0]+=d2u[0]*move;
					camera.pos[1]+=d2u[1]*move;
					camera.pos[2]+=d2u[2]*move;

					//acceleration
					camera.vel[0]+=d2u[0]*accel;
					camera.vel[1]+=d2u[1]*accel;
					camera.vel[2]+=d2u[2]*accel;
				}
			}
		}





		/*//check if relative pos and vel can be set to 0 in this one step...
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
		}*/

		//TODO: smooth rotation

		

		//set camera
		gluLookAt(camera.pos[0], camera.pos[1], camera.pos[2], target[0], target[1], target[2], up[0], up[1], up[2]);
	}
	else
		gluLookAt (10, -10, 10, 0,0,0, 0,0,1);
}


