//collide camera with track, generate acceleration on camera if collisding
void camera_physics_step(dReal step)
{
	//some values that are easy to deal with:
	dReal time = internal.stepsize;
	car_struct *car = camera.car;
	camera_settings *settings = camera.settings;

	//if camera got a targeted car and proper settings, simulate movment
	//
	//divided into 4 parts:
	//1) calculate velocity
	//2) check for collisions
	//3) add damping to velocity
	//4) move camera
	//
	if (car && settings)
	{
		//random values will come handy:


		//check for some exceptions
		if (settings->reverse) //enabled
		{
			if (car->throttle > 0.0) //wanting to go forward
				camera.reverse = false;
			else if (car->throttle < 0.0 && car->velocity < 0.0) //wanting and going backwards
				camera.reverse = true;
		}

		if (settings->in_air) //in air enabled
		{
			if (!(car->sensor1->event) && !(car->sensor2->event)) //in air
			{
				if (!camera.in_air) //camera not in "air mode"
				{
					if (camera.air_timer > settings->air_time)
					{
						camera.in_air = true; //go to air mode
						camera.air_timer = 0; //reset timer
					}
					else
						camera.air_timer += time;
				}
			}
			else //not in air
			{
				if (camera.in_air) //camera in "air mode"
				{
					if (camera.air_timer > settings->air_time)
					{
						camera.in_air = false; //leave air mode
						camera.air_timer = 0; //reset timer
					}
					else
						camera.air_timer += time;
				}
			}
		}


		//store old velocity
		dReal old_vel[3] = {camera.vel[0], camera.vel[1], camera.vel[2]};

		//wanted position of "target" - position on car that should be focused
		dVector3 t_pos;
		//wanted position of camera relative to anchor (translated to world coords)
		dVector3 pos_wanted;

		if (camera.in_air) //normal position, but target centre of car
		{
			const dReal * pos = dBodyGetPosition (car->bodyid);
			t_pos[0] = pos[0];
			t_pos[1] = pos[1];
			t_pos[2] = pos[2];

			dBodyVectorToWorld(car->bodyid, settings->distance[0], -settings->distance[1], settings->distance[2]*car->dir, pos_wanted);
		}
		else if (camera.reverse) //move target and position to opposite side
		{
			dBodyGetRelPointPos (car->bodyid, settings->target[0], -settings->target[1], settings->target[2]*car->dir, t_pos);
			dBodyVectorToWorld(car->bodyid, settings->distance[0], -settings->distance[1], settings->distance[2]*car->dir, pos_wanted);
		}
		else //normal
		{
			dBodyGetRelPointPos (car->bodyid, settings->target[0], settings->target[1], settings->target[2]*car->dir, t_pos);
			dBodyVectorToWorld(car->bodyid, settings->distance[0], settings->distance[1], settings->distance[2]*car->dir, pos_wanted);
		}

		//position and velocity of anchor
		dVector3 a_pos, a_vel;
		dBodyGetRelPointPos (car->bodyid, settings->anchor[0], settings->anchor[1], settings->anchor[2]*car->dir, a_pos);
		dBodyGetRelPointVel (car->bodyid, settings->anchor[0], settings->anchor[1], settings->anchor[2]*car->dir, a_vel);

		//relative pos and vel of camera (from anchor)
		dReal pos[3] = {camera.pos[0]-a_pos[0], camera.pos[1]-a_pos[1], camera.pos[2]-a_pos[2]};

		//vector lengths
		dReal pos_l = v_length(pos[0], pos[1], pos[2]);
		//how far from car we want to stay
		//(TODO: could be computed just once - only when changing camera)
		dReal pos_wanted_l = v_length(pos_wanted[0], pos_wanted[1], pos_wanted[2]);

		//unit vectors
		dReal pos_u[3] = {pos[0]/pos_l, pos[1]/pos_l, pos[2]/pos_l};
		dReal pos_wanted_u[3] = {pos_wanted[0]/pos_wanted_l, pos_wanted[1]/pos_wanted_l, pos_wanted[2]/pos_wanted_l};


		//
		// 1) spring physics for calculating acceleration
		//

		//"linear spring" between anchor and camera (based on distance)
		dReal dist = pos_l-pos_wanted_l;

		if (settings->linear_stiffness == 0) //disabled smooth movement, jump directly
		{
			//set position
			camera.pos[0]-=pos_u[0]*dist;
			camera.pos[1]-=pos_u[1]*dist;
			camera.pos[2]-=pos_u[2]*dist;

			//chanses are we have an anchor distance of 0, then vel=wanted
			if (pos_wanted_l == 0)
			{
				camera.vel[0]=a_vel[0];
				camera.vel[1]=a_vel[1];
				camera.vel[2]=a_vel[2];
			}
			else //velocity towards/from anchor = 0
			{
				//relative vel
				dReal vel[3] = {camera.vel[0]-a_vel[0], camera.vel[1]-a_vel[1], camera.vel[2]-a_vel[2]};
				//vel towards anchor
				dReal dot = (pos_u[0]*vel[0]+pos_u[1]*vel[1]+pos_u[2]*vel[2]);

				//remove vel towards anchor
				camera.vel[0]-=pos_u[0]*dot;
				camera.vel[1]-=pos_u[1]*dot;
				camera.vel[2]-=pos_u[2]*dot;
			}
		}
		else //smooth movement
		{
			//how much acceleration (based on distance from wanted distance)
			dReal acceleration = time*(camera.settings->linear_stiffness)*dist;

			camera.vel[0]-=pos_u[0]*acceleration;
			camera.vel[1]-=pos_u[1]*acceleration;
			camera.vel[2]-=pos_u[2]*acceleration;
		}

		//perpendicular "angular spring" to move camera behind car
		if (pos_wanted_l > 0 && !camera.in_air) //actually got distance, and camera not in "air mode"
		{
			//dot between wanted and current rotation
			dReal dot = (pos_wanted_u[0]*pos_u[0] + pos_wanted_u[1]*pos_u[1] + pos_wanted_u[2]*pos_u[2]);

			if (dot < 1.0) //if we aren't exactly at wanted position (and prevent possibility of acos a number bigger than 1.0)
			{
				//angle
				dReal angle = acos(dot);

				//how much acceleration
				dReal accel = time*angle*(settings->angular_stiffness);

				//direction of acceleration (remove part of wanted that's along current pos)
				dReal dir[3];
				dir[0]=pos_wanted_u[0]-dot*pos_u[0];
				dir[1]=pos_wanted_u[1]-dot*pos_u[1];
				dir[2]=pos_wanted_u[2]-dot*pos_u[2];

				//not unit, get length and modify accel to compensate for not unit
				accel /= v_length(dir[0], dir[1], dir[2]);

				camera.vel[0]+=(accel*dir[0]);
				camera.vel[1]+=(accel*dir[1]);
				camera.vel[2]+=(accel*dir[2]);
			}
		}


		//
		// 2) check for collision, and if so, remove possible movement into collision direction
		//

		if (camera.geom)
		{
			dGeomSetPosition(camera.geom, camera.pos[0], camera.pos[1], camera.pos[2]);

			dContactGeom contact[internal.contact_points];
			int count = dCollide ( (dGeomID)(track.object->space), camera.geom, internal.contact_points, &contact[0], sizeof(dContactGeom));

			int i;
			dReal depth;
			dReal V;
			for (i=0; i<count; ++i)
			{
				depth = contact[i].depth;
				camera.pos[0]-=contact[i].normal[0]*depth;
				camera.pos[1]-=contact[i].normal[1]*depth;
				camera.pos[2]-=contact[i].normal[2]*depth;

				//remove movement into colliding object
				//velocity along collision axis
				V = camera.vel[0]*contact[i].normal[0] + camera.vel[1]*contact[i].normal[1] + camera.vel[2]*contact[i].normal[2];
				if (V > 0) //right direction (not away from collision)?
				{
					//remove direction
					camera.vel[0]-=V*contact[i].normal[0];
					camera.vel[1]-=V*contact[i].normal[1];
					camera.vel[2]-=V*contact[i].normal[2];
				}
			}
		}

		//
		// 3) damping of current velocity
		//
	
		if (settings->relative_damping)
		{
			//damping (of relative movement)
			//(recalculate relative vel, to account for current velocity)
			dReal vel[3] = {camera.vel[0]-a_vel[0], camera.vel[1]-a_vel[1], camera.vel[2]-a_vel[2]};

			dReal damping = (time*settings->damping);
			if (damping > 1)
				damping=1;

			camera.vel[0]-=damping*vel[0];
			camera.vel[1]-=damping*vel[1];
			camera.vel[2]-=damping*vel[2];
		}
		else
		{
			//absolute damping
			dReal damping = 1-(time*settings->damping);

			if (damping < 0)
				damping=0;

			camera.vel[0]*=damping;
			camera.vel[1]*=damping;
			camera.vel[2]*=damping;
		}

		//
		// 4) movement
		//
	
		//during the step, camera will have linear acceleration from old velocity to new
		//avarge velocity over the step is between new and old velocity
		camera.pos[0]+=((camera.vel[0]+old_vel[0])/2)*time;
		camera.pos[1]+=((camera.vel[1]+old_vel[1])/2)*time;
		camera.pos[2]+=((camera.vel[2]+old_vel[2])/2)*time;



		//movement of camera done.




		//
		//the following is smooth rotation and focusing
		//

		//smooth rotation (if enabled)
		//(move partially from current "up" to car "up", and make unit)

		dReal target_up[3];

		if (camera.in_air) //if in air, use absolute up instead
		{
			target_up[0] = 0;
			target_up[1] = 0;
			target_up[2] = 1;
		}
		else //use car up
		{
			const dReal *rotation = dBodyGetRotation (car->bodyid);
			target_up[0] = rotation[2]*car->dir;
			target_up[1] = rotation[6]*car->dir;
			target_up[2] = rotation[10]*car->dir;
		}

		if (settings->rotation_tightness == 0) //disabled, rotate directly
		{
			camera.up[0]=target_up[0];
			camera.up[1]=target_up[1];
			camera.up[2]=target_up[2];
		}
		else
		{
			dReal diff[3]; //difference between
			diff[0]=target_up[0]-camera.up[0];
			diff[1]=target_up[1]-camera.up[1];
			diff[2]=target_up[2]-camera.up[2];
			
			dReal movement=time*(settings->rotation_tightness);

			if (movement > 1)
				movement=1;

			camera.up[0]+=diff[0]*movement;
			camera.up[1]+=diff[1]*movement;
			camera.up[2]+=diff[2]*movement;

			//gluLookAt wants up to be unit
			dReal length=v_length(camera.up[0], camera.up[1], camera.up[2]);
			camera.up[0]/=length;
			camera.up[1]/=length;
			camera.up[2]/=length;
		}

		//smooth movement of target focus (if enabled)
		if (settings->target_tightness == 0)
		{
			camera.t_pos[0] = t_pos[0];
			camera.t_pos[1] = t_pos[1];
			camera.t_pos[2] = t_pos[2];
		}
		else
		{
			dReal diff[3], movement;

			diff[0]=t_pos[0]-camera.t_pos[0];
			diff[1]=t_pos[1]-camera.t_pos[1];
			diff[2]=t_pos[2]-camera.t_pos[2];

			movement = time*(settings->target_tightness);

			if (movement>1)
				movement=1;

			camera.t_pos[0]+=diff[0]*movement;
			camera.t_pos[1]+=diff[1]*movement;
			camera.t_pos[2]+=diff[2]*movement;
		}

	}
}
