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
				dReal d1 = (vel_u[0]*pos[0]+vel_u[1]*pos[1]+vel_u[2]*pos[2]);
				dReal d1v[3] = {d1*vel_u[0], d1*vel_u[1], d1*vel_u[2]};
				//perpendicular (funny: could use z²=x²+y², but need vector later)
				dReal d2v[3] = {pos[0]-d1v[0], pos[1]-d1v[1], pos[2]-d1v[2]};
				dReal d2 = v_length(d2v[0], d2v[1], d2v[2]);
				dReal d2u[3] = {d2v[0]/d2, d2v[1]/d2, d2v[2]/d2}; //unit vector

				printf("> d1v: %f %f %f \n", d1v[0], d1v[1], d1v[2]);
				printf("> d2v: %f %f %f \n", d2v[0], d2v[1], d2v[2]);
				printf("> d1v*d2v: %f\n", d1v[0]*d2v[0]+d1v[1]*d2v[1]+d1v[2]*d2v[0]);
				//printf("> pos: %f %f %f\n", pos[0], pos[1], pos[2]);
				//
				//acceleration/deceleration of velocity (d1)
				//

				//default: will accelerate over all time
				dReal time_left=time;
				bool accelerate=true;

				//first: is moving in wrong direction or overshooting? Then break
				if (d1<0 || (vel_l*vel_l/(2*max_accel)) >d1) //moving in wrong direction
				{
					//time it takes to break
					dReal break_time = vel_l/max_accel;

					if (time <= break_time) //will only be able to break
					{
						//seems ok
						//printf("all break\n");
						dReal time_left_to_halt = (break_time-time); //time left needed to fully break (remove)

						//printf("> break in:%f    time:%f    left:%f\n", break_time, time, time_left_to_halt);
						//velocity
						dReal accel = max_accel*time; //acceleration/deceleration achieved
						camera.vel[0]-=vel_u[0]*accel;
						camera.vel[1]-=vel_u[1]*accel;
						camera.vel[2]-=vel_u[2]*accel;

						//movement
						dReal dist = max_accel*break_time*break_time/2; //how far if completely breaking
						dist -= max_accel*time_left_to_halt*time_left_to_halt/2; //remove time that's left
						camera.pos[0]+=vel_u[0]*dist;
						camera.pos[1]+=vel_u[1]*dist;
						camera.pos[2]+=vel_u[2]*dist;

						//no more time
						accelerate=false;
					}
					else
					{
						//something strange here
						//printf("some break\n");
						//velocity is 0 relative
						camera.vel[0]=t_vel[0];
						camera.vel[1]=t_vel[1];
						camera.vel[2]=t_vel[2];
						//standing still relative to target
						vel[0] = 0;
						vel[1] = 0;
						vel[2] = 0;

						//move
						dReal move = max_accel*break_time*break_time/2; //movement when breaking
						camera.pos[0]+=vel_u[0]*move;
						camera.pos[1]+=vel_u[1]*move;
						camera.pos[2]+=vel_u[2]*move;



						time_left=(time-break_time); //remove time spent breaking (rest will be used later)

						//since pos changed, some values need recalculation
						//printf("move=%f old=%f %f %f", move, pos[0], pos[1], pos[2]);
						pos[0] = t_pos[0]-camera.pos[0];
						pos[1] = t_pos[1]-camera.pos[1];
						pos[1] = t_pos[2]-camera.pos[2];
						pos_l = v_length(pos[0], pos[1], pos[2]);
						//printf("   new=%f %f %f\n", pos[0], pos[1], pos[2]);
						//velocity still got the same "direction", but now is 0
						vel_l = 0;
						//unit vector
						pos_u[0] = pos[0]/pos_l;
						pos_u[1] = pos[1]/pos_l;
						pos_u[2] = pos[2]/pos_l;
						//direction1 
						//d1 = (vel_u[0]*pos[0]+vel_u[1]*pos[1]+vel_u[2]*pos[2]);
						d1 -= move;

						d1v[0] = d1*vel_u[0];
						d1v[1] = d1*vel_u[1];
						d1v[2] = d1*vel_u[2];
					}
				}
				else
					printf("no break\n");

				//if we got time left (even if breaked earlier). guaranteed not needing to stop or reverse
				if (accelerate)
				{
					dReal time_accel, time_break; //time to break and then time to accelerate

					//do we have velocity?
					if (vel_l)
					{
						dReal time_passed = vel_l/max_accel; //time it would take to get current velocity
						//time it would take to move half way if starting at position with v=0
						dReal time_half = sqrt(d1/max_accel + time_passed*time_passed/2);

						time_accel=time_half-time_passed; //remove already passed
						time_break=time_half; //already removed passed from acceleration
							
					}
					else //simple
					{
						time_accel = time_break = sqrt(d1/max_accel);
					}


					if (time_left <= time_accel) //we can only accelerate
					{
						//printf("accel\n");
						//velocity
						dReal velocity = time_left*max_accel;
						camera.vel[0]+=vel_u[0]*velocity;
						camera.vel[1]+=vel_u[1]*velocity;
						camera.vel[2]+=vel_u[2]*velocity;

						//movement
						dReal move = (vel_l*time_left) + (max_accel*time_left*time_left/2);
						camera.pos[0]+=vel_u[0]*move;
						camera.pos[1]+=vel_u[1]*move;
						camera.pos[2]+=vel_u[2]*move;
					}
					else if (time_left < (time_accel+time_break)) //can accelerate and break
					{
						//printf("accel+break\n");
						//(move calculated from wanted position)
						dReal time_from_breaked = time_break+time_accel-time_left; //how much time do we miss from jumping to wanted pos?

						//velocity
						dReal velocity = max_accel*time_from_breaked;
						camera.vel[0]+=vel_u[0]*velocity;
						camera.vel[1]+=vel_u[1]*velocity;
						camera.vel[2]+=vel_u[2]*velocity;

						//movement
						dReal move = d1-(max_accel*time_from_breaked*time_from_breaked/2);
						camera.pos[0]+=vel_u[0]*move;
						camera.pos[1]+=vel_u[1]*move;
						camera.pos[2]+=vel_u[2]*move;
					}
					else //enough time to jump directly to wanted position
					{
						//printf("jump\n");
						//printf("TODO: something is wrong here?!\n");
						camera.pos[0]+=d1v[0];
						camera.pos[1]+=d1v[1];
						camera.pos[2]+=d1v[2];
						//camera.pos[0]+=pos[0];
						//camera.pos[1]+=pos[1];
						//camera.pos[2]+=pos[2];
						//camera.pos[0]=t_pos[0];
						//camera.pos[1]=t_pos[1];
						//camera.pos[2]=t_pos[2];
						camera.vel[0]=t_vel[0];
						camera.vel[1]=t_vel[1];
						camera.vel[2]=t_vel[2];
					}
				}
				else
					printf("(no accel)\n");

				//
				//movement correction (d2)
				//
				//t is time it takes to correct, between 0 and t/2 acceleration, between t/2 and t break
				dReal t = sqrt(4*d2/max_accel);
				if (1) //(time >= t) //will be able to move to wanted position in one step
				{
					printf("jump\n");
					//printf("> d1v: %f %f %f \n", d1v[0], d1v[1], d1v[2]);
					//printf("> d2v: %f %f %f \n", d2v[0], d2v[1], d2v[2]);
					//printf("> first camera: %f %f %f\n", camera.pos[0], camera.pos[1], camera.pos[2]);
					camera.pos[0]+=d2v[0];
					camera.pos[1]+=d2v[1];
					camera.pos[2]+=d2v[2];
					//printf("> now camera: %f %f %f\n", camera.pos[0], camera.pos[1], camera.pos[2]);
					//printf("> target: %f %f %f\n", t_pos[0], t_pos[1], t_pos[2]);
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



		//TODO: smooth rotation

		

		//set camera
		gluLookAt(camera.pos[0], camera.pos[1], camera.pos[2], target[0], target[1], target[2], up[0], up[1], up[2]);
	}
	else
		gluLookAt (10, -10, 10, 0,0,0, 0,0,1);
}


